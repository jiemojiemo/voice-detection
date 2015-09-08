#include <iostream >
#include <vector>
#include "detection.h"
#include "voicedetection.h"
#include "CAudioTimeSandPitchS.h"
#include "CWavread.h"
#include "pcm2wav.h"
#include "CPitchShift.h"
#include <cmath>
#include <fstream>
#include <memory>
#include <map>
using namespace std;
#define MY_INT16 2



const char* url = "iii.wav";
void WriteResultToFile( const float* buffer, const vector<SpeechSegment>& sgm, int hop );
vector<Solmization> getMelody();
void PrintSgm( const vector<SpeechSegment>& sgms );
int main()
{
	CWavread wavReader;
	
	CpitchShiftwithHop pitchShifttest;
	CAudioTimeSandPitchS pitch;
	wav_struct wavhead = wavReader.ReadHead( url );
	float* buffer = wavReader.ReadMonoData( wavhead );
	int sampleSize = wavhead.data_size/MY_INT16;
	int sampleRate = wavhead.frequency;
	int amdfSize = sampleRate / MIN_VOICE_FREQUENCY * 2;	//取 分析人声频率 的范围

	CVoiceDetection detection;
	auto result( detection.Detection( buffer, sampleSize, sampleRate ) );
	float shift = 0;
	float dest = 260;
	float scale = 0.0;
	FILE* file = fopen( "222.raw", "wb" );
	//int melody[] = { C3,C3,G3,G3,A3,A3,G3,F3,F3,E3,E3,D3,D3,C3 };
	//float thythm[] = { 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2  };
	int melody[] = { C3,D3,E3,C3,C3,D3,E3,C3,E3,F3,G3,E3,F3,G3,G3,A3, G3, F3, E3,C3,G3, A3, G3, F3, E3,C3,D3,G2,C3 };
	float thythm[]={ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2,0.5,0.5,0.5,0.5,1, 1, 0.5,0.5,0.5,0.5,1, 1, 1, 1, 2  };
	int melodySize=sizeof(melody)/sizeof(melody[0]);
	PrintSgm( result );
	for (int i=0; i<melodySize;)
	{
		for( auto it = result.begin(); it != result.end(); ++it )
		{
			if( i >= melodySize )
				break;
			dest=melody[i];
			int distance = it->start * detection.m_hop;
			int length = (it->end - it->start) * sizeof(float) * detection.m_hop;
			auto datain = pitch.WavReadBuffer( buffer+distance, length,1 );
			shift = it->frequence / dest;
			scale = it->beat/thythm[i] * 4;
			//scale=1;
			//shift = 1;
			//auto dataReslut(pitch.TimeScalingAndPitchShifting( shift, scale, datain, 1024, 256 ));
			auto dataReslut1(pitchShifttest.pitchShift(dest, datain, length, 1024));
			auto dataReslut2 = pitch.WavReadBuffer(dataReslut1, length, 1);
			auto dataReslut(pitch.TimeScaling(dataReslut2, 1024, 256, scale));
			fwrite( dataReslut, 1, pitch.GetSize(), file );
			if( dataReslut != NULL )
			{
				delete [] dataReslut;
				dataReslut = NULL;
			}
			i++;
		}

	}
	
	fclose( file );
	WriteResultToFile( buffer, result, detection.m_hop );
}

void WriteResultToFile( const float* buffer, const vector<SpeechSegment>& sgm, int hop )
{

	FILE* file = fopen( "result.raw", "wb" );
	for( auto it = sgm.begin(); it != sgm.end(); ++it )
	{
		int distance = it->start * hop;
		fwrite( buffer + distance, 1,(it->end - it->start)*hop*sizeof(float) , file );
	}
	fclose( file );
}

void PrintSgm( const vector<SpeechSegment>& sgms )
{
	for( auto it = sgms.begin(); it != sgms.end(); ++it  )
	{
		cout << it->start << " " << it->end << " freq:" << it->frequence << " beat:" << it->beat << endl;
	}
}