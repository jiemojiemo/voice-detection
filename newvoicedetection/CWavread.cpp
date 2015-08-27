#include "CWavread.h"
using namespace std;

CWavread::CWavread(){}
CWavread::~CWavread(){}

wav_struct CWavread::wavread_head(const char* filename)
{
	fstream fs;
	wav_struct WAV;
	WAV.filename=filename;
	fs.open(filename, ios::binary | ios::in); 

	fs.seekg(0, ios::end);       
	WAV.file_size = fs.tellg(); 

	fs.seekg(0x16); 
	fs.read((char*)&WAV.channel, sizeof(WAV.channel)); 

	fs.seekg(0x18); 
	fs.read((char*)&WAV.frequency, sizeof(WAV.frequency)); 

	fs.seekg(0x1c); 
	fs.read((char*)&WAV.Bps, sizeof(WAV.Bps)); 

	fs.seekg(0x22); 
	fs.read((char*)&WAV.sample_num_bit, sizeof(WAV.sample_num_bit)); 

	fs.seekg(0x28); 
	fs.read((char*)&WAV.data_size, sizeof(WAV.data_size)); 

	WAV.data = new unsigned char[WAV.data_size]; 

	fs.seekg(0x2c); 
	fs.read((char *)WAV.data, sizeof(char)*WAV.data_size); 

	cout << "文件大小为  ：" << WAV.file_size << endl; 
	cout << "音频通道数  ：" << WAV.channel << endl; 
	cout << "采样频率    ：" << WAV.frequency << endl; 
	cout << "Byte率      ：" << WAV.Bps << endl; 
	cout << "样本位数    ：" << WAV.sample_num_bit << endl; 
	cout << "音频数据大小：" << WAV.data_size << endl; 

	return WAV;
}

float* CWavread::wavread_data1(wav_struct wav)
{

	float *Out=new float[wav.data_size];

	 for (unsigned long i =0; i<wav.data_size; i = i + 2) 
    {        
        //右边为大端         
		unsigned long data_low = wav.data[i]; 
        unsigned long data_high = wav.data[i + 1]; 
		float float_data;
        double data_true = data_high * 256 + data_low; 
        //printf("%d ",data_true);         
		long data_complement = 0;        
        //取大端的最高位（符号位）         
		int my_sign = (int)(data_high / 128); 
        //printf("%d ", my_sign);         
		if (my_sign == 1) 
        { 
            data_complement = data_true - 65536; 
        } 
        else 
        { 
            data_complement = data_true; 
        } 
        //printf("%d ", data_complement);         
		setprecision(4); 
        float_data = (float)(data_complement/(double)32768); 
		Out[i/2]=float_data;
	}
  
	/*for (int i=0;i<WAV.data_size/2;i++)
	{
		printf("%d*%f\t",i,Out[i]);
	}*/

    delete[] wav.data; 
	return Out;

}

float* CWavread::wavread_data2(wav_struct wav)
{
	float* DataOut=new float[wav.data_size/2];
	float **Out=new float*[2];
	for (int i=0;i<2;i++)
	{
		Out[i]=new float[wav.data_size/4];
	}

	 for (unsigned long i =0; i<wav.data_size; i = i + 4) 
    {        
        //右边为大端         
		unsigned long data_low1 = wav.data[i]; 
        unsigned long data_high1 = wav.data[i + 1]; 
		unsigned long data_low2 = wav.data[i+2]; 
		unsigned long data_high2 = wav.data[i + 3]; 
		float float_data1,float_data2;
        double data_true1 = data_high1 * 256 + data_low1; 
		double data_true2 = data_high2 * 256 + data_low2;        
		long data_complement1 = 0;   
		long data_complement2 = 0;  
        //取大端的最高位（符号位）         
		int my_sign1 = (int)(data_high1 / 128); 
		int my_sign2 = (int)(data_high2 / 128);  
        //printf("%d ", my_sign);         
		if (my_sign1 == 1) 
            data_complement1 = data_true1 - 65536; 
        else 
            data_complement1 = data_true1; 

		if (my_sign2 == 1) 
			data_complement2 = data_true2 - 65536; 
		else 
			data_complement2 = data_true2; 
        //printf("%d ", data_complement);         
		setprecision(4); 
        float_data1 = (float)(data_complement1/(double)32768); 
		float_data2 = (float)(data_complement2/(double)32768); 
		Out[0][i/4]=float_data1;
		Out[1][i/4]=float_data2;
	}
	 memcpy(DataOut,Out[0],sizeof(float)*wav.data_size/4);
	 memcpy(DataOut+wav.data_size/4,Out[1],sizeof(float)*wav.data_size/4);
  
	/*for (int i=0;i<WAV.data_size/2;i++)
	{
		printf("%d*%f\t",i,Out[i]);
	}*/

    delete[] wav.data; 
	return DataOut;

}

float** CWavread::wavread_data3(wav_struct wav)
{
	float **Out=new float*[2];
	for (int i=0;i<2;i++)
	{
		Out[i]=new float[wav.data_size/4];
	}

	 for (unsigned long i =0; i<wav.data_size; i = i + 4) 
    {        
        //右边为大端         
		unsigned long data_low1 = wav.data[i]; 
        unsigned long data_high1 = wav.data[i + 1]; 
		unsigned long data_low2 = wav.data[i+2]; 
		unsigned long data_high2 = wav.data[i + 3]; 
		float float_data1,float_data2;
        double data_true1 = data_high1 * 256 + data_low1; 
		double data_true2 = data_high2 * 256 + data_low2;        
		long data_complement1 = 0;   
		long data_complement2 = 0;  
        //取大端的最高位（符号位）         
		int my_sign1 = (int)(data_high1 / 128); 
		int my_sign2 = (int)(data_high2 / 128);  
        //printf("%d ", my_sign);         
		if (my_sign1 == 1) 
            data_complement1 = data_true1 - 65536; 
        else 
            data_complement1 = data_true1; 

		if (my_sign2 == 1) 
			data_complement2 = data_true2 - 65536; 
		else 
			data_complement2 = data_true2; 
        //printf("%d ", data_complement);         
		setprecision(4); 
        float_data1 = (float)(data_complement1/(double)32768); 
		float_data2 = (float)(data_complement2/(double)32768); 
		Out[0][i/4]=float_data1;
		Out[1][i/4]=float_data2;
	}
//	 memcpy(DataOut,Out[0],sizeof(float)*wav.data_size/4);
//	 memcpy(DataOut+wav.data_size/4,Out[1],sizeof(float)*wav.data_size/4);
  
	/*for (int i=0;i<WAV.data_size/2;i++)
	{
		printf("%d*%f\t",i,Out[i]);
	}*/

    delete[] wav.data; 
	return Out;

}

int CWavread::hex_char_value(char c) 
{ 
    if (c >= '0' && c <= '9') 
        return c - '0'; 
    else if (c >= 'a' && c <= 'f') 
        return (c - 'a' + 10); 
    else if (c >= 'A' && c <= 'F') 
        return (c - 'A' + 10); 
    //assert(0);     
	return 0; 
} 

int CWavread::hex_to_decimal(char* szHex) 
{ 
    int len = 2; 
    int result = 0; 
    for (int i = 0; i < len; i++) 
    { 
        result += (int)pow((float)16, (int)len - i - 1) * hex_char_value(szHex[i]); 
    } 
    return result; 
} 
