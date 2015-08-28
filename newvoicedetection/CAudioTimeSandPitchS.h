//#include "wavread.h"
//#include "fft_complex.h"
#include "CFFT.h"
#include "CWavread.h"
#include <string.h>
#include <fstream>
using namespace std;

#define STH_ISTFT 2


class CAudioTimeSandPitchS
{
private:
	CAudioTimeSandPitchS(const CAudioTimeSandPitchS& cp){}
	CAudioTimeSandPitchS& operator = (const CAudioTimeSandPitchS& cp){}
	complex** STFT(float* dataIn);
	complex** PVsample(complex** dataIn);
	float* ISTFT(complex** dataIn);

	unsigned long	m_PCMSize;
	int				m_hop;
	unsigned long	m_STFTOutRow;
	unsigned long   m_STFTOutCol;
	int				m_winSize;
	float			m_scale;
	unsigned long   m_timeScaleSize;
	CFFT			m_FFT;
	CWavread		m_wavread;
	float			m_sampleRateScale;
	unsigned long	m_resampleSize;				

public:
	CAudioTimeSandPitchS();
	~CAudioTimeSandPitchS();
	float*	WavReadFile(const char* filename);
	float*  WavReadBuffer(float* buffer,unsigned long bufferSize,int channel);
	float*	TimeScaling(float* dataIn,int winSize,int hop,float scale);
	int		WavWrite(float* dataIn);	
	float*	wavresample(float * DataIn,int p,int q);
	unsigned long GetSize(){return m_timeScaleSize*sizeof(float);}
	unsigned long GetResampleSize(){ return m_resampleSize * sizeof(float); }
	float* PitchShiftingFile(float* dataIn,int winSize,int hop,int shift);
	float  GetSampleRateScale(){return m_sampleRateScale;}
	float* PitchShifting(float* dataIn,int winSize,int hop,int shift);
	float* TimeScalingAndPitchShifting(int shift, float scale,float* dataIn,int winSize,int hop);
	float* TimeScalingAndPitchShifting(float frequence, float scale,float* dataIn,int winSize,int hop);
	float* resample(float* dataIn, double scale);
};