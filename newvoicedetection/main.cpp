#include <iostream>
#include <vector>
#include "detection.h"
#include "voicedetection.h"
#include "CWavread.h"
#include "pcm2wav.h"
#include <cmath>
#include <fstream>
#include <map>
using namespace std;
#define MY_INT16 2

const char* url = "555.wav";

#define MAX(x,y) ( (x)>(y)?(x):(y) )
#define MIN(x,y) ( (x)<(y)?(x):(y) )

void SaveVoiceToWav( const char* outfile,int channels, int sampleRate, int bits);
vector<float> AMDFCalc( const vector<float>& amdfData );
int VoiceFrequenceCalc( const vector<float>& amdfResult, int sampleRate );


int main()
{
	CWavread wavReader;
	wav_struct wavhead = wavReader.wavread_head( url );
	float* buffer = wavReader.wavread_data1( wavhead );
	int sampleSize = wavhead.data_size/MY_INT16/wavhead.channel;
	int sampleRate = wavhead.frequency;
	int amdfSize = sampleRate / MIN_VOICE_FREQUENCY * 2;	//取 分析人声频率 的范围

	CVoiceDetection detection;
	auto result( detection.Detection( buffer, sampleSize, sampleRate ) );

	for( auto it = result.begin(); it != result.end(); ++it )
	{
		cout << it->start << " " << it->end << " fre:" << it->frequence << endl;
	}
}