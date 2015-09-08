#include "CAudioTimeSandPitchS.h"
//#include <assert.h>
#include <stdlib.h>
#include <time.h>

const float oneKey=1.0595;

CAudioTimeSandPitchS::CAudioTimeSandPitchS()
{

}
CAudioTimeSandPitchS::~CAudioTimeSandPitchS()
{

}

float* CAudioTimeSandPitchS::WavReadFile(const char* filename)
{
	float* PCMOut;
	wav_struct WAV=m_wavread.ReadHead(filename);
	m_PCMSize=WAV.data_size/WAV.channel/WAV.sample_num_bit*8;
//	m_PCMSize=WAV.data_size/WAV.sample_num_bit*8;
	if (WAV.channel==2)
		PCMOut=m_wavread.ReadStereoData(WAV);
	else if(WAV.channel==1)
		PCMOut=m_wavread.ReadMonoData(WAV);

	return PCMOut;
}

float* CAudioTimeSandPitchS::WavReadBuffer(float* buffer,unsigned long bufferSize,int channel)
{
	//暂定32位的录音
	m_PCMSize = bufferSize/channel/32*8;
	if(channel==1)
		return buffer;
	
	//声道数为2，暂且没有处理
	if(channel==2)
	{
		
	}
	return 0;
}

float* CAudioTimeSandPitchS::TimeScaling(float* DataIn,int winSize,int hop,float scale)
{
	m_winSize=winSize;
	m_hop=hop;
	m_STFTOutRow=((m_PCMSize-winSize)/m_hop)+1;
	m_STFTOutCol=(winSize/2)+1;
	m_scale=scale;
	m_sampleRateScale=1;
	if( fabs(m_scale - 1.0) < 0.00001f )
	{
		m_timeScaleSize = m_PCMSize;
		return DataIn;
	}
	complex**	dataout1=STFT(DataIn);
	complex**	dataout2=PVsample(dataout1);
	for( int i = 0; i < m_STFTOutRow; ++i )
	{
		if( dataout1[i] != NULL )
		{
			delete [] dataout1[i];
			dataout1[i] = NULL;
		}
	}
	if( dataout1!= NULL )
	{
		delete [] dataout1;
		dataout1 = NULL;
	}
	float*		dataout3=ISTFT(dataout2);		
	for (int i = 0; i < (m_STFTOutRow - 2) / m_scale; ++i)
	{
		if (dataout2[i] != NULL)
		{
			delete[] dataout2[i];
			dataout2[i] = NULL;
		}
	}
	if (dataout2 != NULL)
	{
		delete[] dataout2;
		dataout2 = NULL;
	}
	return dataout3;

}

//STFT、PVsample、ISTFT完成PV Phase Vocoder
complex** CAudioTimeSandPitchS::STFT(float* dataIn)
{
	complex **DataOut;
	DataOut=new complex* [m_STFTOutRow];
	for (int i=0;i<m_STFTOutRow;i++)
	{
		DataOut[i]=new complex[m_STFTOutCol];
	}

	float* window=new float[m_winSize];
	float* DataWin=new float[m_winSize];
	complex* DataWinC=new complex[m_winSize];
	int halfWinsize=m_winSize/2;

	//汉宁窗
	for (int i=0;i<m_winSize;i++)
	{
		window[i]=0.5*(1+cos(PI*(i-halfWinsize)/(halfWinsize-1)));
		//printf("%f\t",window[i]);
	}

	int c=0;
	
	for (int i=0;i<m_PCMSize-m_winSize;i+=m_hop)
	{	
		for (int j=0;j<m_winSize;j++)
		{
			DataWin[j]=dataIn[i+j]*window[j];
			DataWinC[j].real=DataWin[j];
			DataWinC[j].imag=0;
		}

		m_FFT.fft(m_winSize,DataWinC);
		memcpy(DataOut[c],DataWinC,sizeof(complex)*m_STFTOutCol);

		/*printf("-----%d--------\n",c);
		for (int k=0;k<OUT_STFT_LIE;k++)
		{
			printf("%d**%f+%fi\n",k,DataOut[c][k].real,DataOut[c][k].imag);
		}*/	
		c++;
	
	}	

	delete[] window;
	delete[] DataWin;
	delete[] DataWinC;

	return DataOut;

}
complex** CAudioTimeSandPitchS::PVsample(complex** dataIn)
{

	int N=2*(m_STFTOutCol-1);					//窗函数尺寸的大小(1024)
	int scalelen=(m_STFTOutRow-2)/m_scale+1;	//变换尺度数列的数组大小

	float *t=new float[scalelen];				//变换尺度数组

	//变换尺度数组赋初值
	for (int i=0;i<scalelen;i++)
	{
		t[i]=i*m_scale;							//时移序列，用于幅度谱interpolation
		//printf("%f\t",t[i]);
	}

	complex **DataOut;							//PV分析之后的输出
	DataOut=new complex* [scalelen];

	for (int i=0;i<scalelen;i++)
	{
		DataOut[i]=new complex[m_STFTOutCol];
	}
	//存放相邻的两帧数据
	//用于计算相位差
	complex **bcols=new complex *[2];
	for (int i=0;i<2;i++)
	{
		bcols[i]=new complex[m_STFTOutCol];
	}

	//
	float *dphi=new float[N/2+1];
	dphi[0]=0;
	for (int i=1;i<(N/2+1);i++)
	{
		dphi[i]=2*PI*m_hop*i/N;
		//	printf("%f\t",dphi[i]);

	}

	//存放phase
	float *ph=new float[m_STFTOutCol];
	for (int i=0;i<m_STFTOutCol;i++)
	{
		ph[i]=-1*atan2f(dataIn[0][i].imag,dataIn[0][i].real);//和matlab算出的符号相反
		//	printf("%f\t",ph[i]);
	}
	float tt,tf;
	int a;
	int ocol=0;
	//存放幅值
	float *bmag=new float[m_STFTOutCol];
	//存放相位差，用于估计真实频率
	float *dp=new float[m_STFTOutCol];
	
	
	for (int i=0;i<scalelen;i++)
	{
		tt=t[i];
		a=floor(tt);
		memcpy(bcols[0],dataIn[a],m_STFTOutCol*sizeof(complex));
		memcpy(bcols[1],dataIn[a+1],m_STFTOutCol*sizeof(complex));
		//	printf("%d\t",a);
		tf=tt-floor(tt);
		//	printf("----------------\n");
		//真实频率估计
		for (int j=0;j<m_STFTOutCol;j++)
		{
			//interpolated spectral magnitudes in the columns of input data 幅度谱内插
			bmag[j]=(1-tf)*sqrt(bcols[0][j].real*bcols[0][j].real+bcols[0][j].imag*bcols[0][j].imag)
				+tf*sqrt(bcols[1][j].real*bcols[1][j].real+bcols[1][j].imag*bcols[1][j].imag);

			//calculate phase advance, reduce to -pi:pi range 真实频率估计
			dp[j]=atan2f(bcols[1][j].imag,bcols[1][j].real)-atan2f(bcols[0][j].imag,bcols[0][j].real)-dphi[j];
			float adjust=2*PI*(int)((dp[j]/(2*PI)));
			dp[j]=dp[j]-adjust;
			//printf("%f\n",dp[j]);

			DataOut[ocol][j].real=bmag[j]*cos(ph[j]);
			DataOut[ocol][j].imag=bmag[j]*sin(ph[j]);
			//	printf("%f+%fi\n",DataOut[ocol][j].real,DataOut[ocol][j].imag);
		}
		//printf("%d------------\n",ocol);
		//cumulate phase, ready for next frame
		for (int k=0;k<m_STFTOutCol;k++)
		{
			ph[k]=ph[k]+dphi[k]+dp[k];
			//	printf("%f\t",ph[k]);
		}
		ocol++;		
	}

	delete []t;
	delete[] dphi;
	delete[] ph;

	for (int i=0;i<2;i++)
		delete[] bcols[i];
	delete[] bcols;

	return DataOut;

}
float* CAudioTimeSandPitchS::ISTFT(complex** dataIn)
{

	int ftsize=m_winSize;
	int cols=(m_STFTOutRow-2)/m_scale+1;
	int w=m_winSize;
	int halflen=m_winSize/2;
	int h=m_hop;
	float *window=new float[m_winSize];
	for (int i=0;i<m_winSize;i++)
	{
		window[i]=0.5*(1+cos(PI*(i-halflen)/(m_winSize/2-1)))*STH_ISTFT;
		//printf("%f\t",window[i]);
	}
	int xlen=ftsize+(cols-1)*h;
	m_timeScaleSize=xlen;
	float *Out=new float[xlen];
	memset(Out,0,xlen*sizeof(float));
	complex *ft=new complex [ftsize];
	float *px=new float[ftsize];
	for (int i=0;i<h*(cols-1);i+=h)
	{
		memcpy(ft,dataIn[i/h],sizeof(complex)*m_STFTOutCol);
		for (int l=ftsize-1;l>ftsize/2-1;l--)
		{
			ft[l].real=ft[ftsize-l-1].real;
			ft[l].imag=-ft[ftsize-l-1].imag;
		}

		//matlab ft 对称	
		m_FFT.ifft(ftsize,ft);

		for (int j=0;j<ftsize;j++)
		{
			px[j]=ft[j].real;		
		}
	
		for (int k=0;k<ftsize;k++)
		{

			Out[i+k]=Out[i+k]+px[k]*window[k];
		}

	}

	delete[] ft;
	delete[] px;
	delete[] window;
	
	return Out;
}

float* CAudioTimeSandPitchS::PitchShiftingFile(float* dataIn,int winSize,int hop,int shift)
{
	if(shift>19)
	{
		printf("音高调整尺度有误！\n");
		//assert(shift<20);
	}
	float scale=(float)(20-shift)/20;
	float* Out=TimeScaling(dataIn,winSize,hop,scale);
	m_sampleRateScale=1/scale;
	return Out;

}

float* CAudioTimeSandPitchS::PitchShifting(float* dataIn,int winSize,int hop,int shift)
{
	if(shift>20)
	{
		printf("音高调整尺度有误！\n");
		//assert(shift<20);
	}
	float scale=(float)(20-shift)/20;
	float* Out=TimeScaling(dataIn,winSize,hop,scale);
	Out=resample(Out,shift);
	return Out;

}

//用于变调的重采样
float* CAudioTimeSandPitchS::resample(float* dataIn, double scale)
{
	//采用线性内插的方法调整采样率
	m_resampleSize=m_timeScaleSize*scale;
	float* dataOut=new float[m_resampleSize];
	memset(dataOut,0,m_resampleSize*sizeof(float));

	int a=0;
	float x=0;
	int x1,x2;
	float y1,y2;
	if (scale!=1)
	{
		while(x<m_timeScaleSize && a<m_resampleSize)
		{
			x1=floor((double)x);
			x2=x1+1;
			y1=dataIn[x1];
			y2=dataIn[x2];		
			dataOut[a]=(x-x1)*(y2-y1)/(x2-x1)+y1;
			if (dataOut[a] > 1 || dataOut[a] < 1)
				dataOut[a] = 0;
			x+=1.0/scale;		
			a++;
		}		
	}
	else
		return dataIn;

	return dataOut;
}

//完成变速和变调
float* CAudioTimeSandPitchS::TimeScalingAndPitchShifting(int shift, float scale,float* dataIn,int winSize,int hop)
{
	printf("处理中...\n");
	double d=1.0/pow((double)oneKey,shift);
	float scaleTime=scale*d;
	float* out=TimeScaling(dataIn,winSize,hop,scaleTime);
	out=resample(out,d);
	return out;
}

float* CAudioTimeSandPitchS::TimeScalingAndPitchShifting(float freqshift, float scale,float* dataIn,int winSize,int hop)
{
	float scaleTime=scale*freqshift;
	auto out=TimeScaling(dataIn,winSize,hop,scaleTime);
	out=resample(out, freqshift);
	return out;
}