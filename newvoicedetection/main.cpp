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
#define MIN_VOICE_FREQUENCY 80.0
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
	auto result( detection.Detection( buffer, sampleSize ) );

	//FILE* file = fopen( "result.raw", "wb" );
	//for( auto it = result.begin(); it != result.end(); ++it )
	//{
	//	cout << it->first << " " << it->second << endl;
	//	int distance = it->first * detection.m_hop;
	//	fwrite( buffer + distance, 1,(it->second - it->first)*detection.m_hop*sizeof(float) , file );
	//}
	//fclose( file );

	vector<SpeechSegment> speechSegment;
	for( auto it = result.begin(); it != result.end(); ++it )
	{
		int middleFrameIndex = (it->first + it->second)/2;
		int middleSample = middleFrameIndex * detection.m_hop;

		vector<float> amdfData( buffer+middleSample, buffer+middleSample+amdfSize );
		auto amdfResult = AMDFCalc( amdfData );
		auto voiceFrequence = VoiceFrequenceCalc( amdfResult, sampleRate );
		if( 0 != voiceFrequence )
		{
			SpeechSegment smg(voiceFrequence, it->first, it->second);
			speechSegment.push_back( smg );
		}
			
		cout << voiceFrequence << endl;
	}

	for( auto it = speechSegment.begin(); it != speechSegment.end(); ++it )
	{
		cout << it->start << " " << it->end << " fre:" << it->frequence << endl;
	}

//	int winSize = 256;
//	int hop = 90;
//	float ampMax = 10;
//	float ampMin = 2;
//	int zeroCrossMax = 10;
//	int zeroCrossMin = 5;
//
//	int minSilence = 30;
//	int minVoice = 100;
//	int voiceCount = 0;
//	int silenceCount = 0;
//
//	int status=0;
//	int start = 0;
//	int end = 0;
//
//	map<int, int> startEndMap;
//
//	auto result = EnFrame( buffer, sampleSize, winSize, hop );
//
//	int row = (sampleSize - winSize)/hop + 1;
//	cout << "row:" <<row << endl;
//	cout <<row << endl;
//	vector<int> zeroCrossVector;
//	vector<double> amplitudes;
//	double maxAmp = 0.0;
//	double minAmp = 1000.0;
//	for( int i = 0; i < row; ++i )
//	{
//		int count = 0;
//		double ampSum = 0;
//		for( int j = 0; j < winSize -1; ++j )
//		{
//			int sign = 0;
//			int diff = 0;
//			if( result[i][j] * result[i][j + 1] < 0)
//			{
//				sign = 1;
//			}
//			if( result[i][j] - result[i][j + 1] > 0.0002 )
//			{
//				diff = 1;
//			}
//			if( sign * diff == 1 )
//				count++;
//			ampSum += result[i][j] * result[i][j];
//		}
//     //   cout << ampSum << endl;
//		zeroCrossVector.push_back( count );
//		amplitudes.push_back( ampSum );
//		if( maxAmp < ampSum )
//			maxAmp = ampSum;
//        if( ampSum < minAmp )
//            minAmp = ampSum;
//	}
////	for( auto it = amplitudes.begin(); it != amplitudes.end(); ++it )
////		cout << *it << " ";
////	for( auto it = zeroCrossVector.begin(); it != zeroCrossVector.end(); ++it )
////		cout << *it << " ";
//
//	//ampMax = MIN( (maxAmp/4.0), ampMax );
//	//ampMin = MIN( (maxAmp/8.0), ampMin );
//
//	ampMax = maxAmp/8.0;
//	ampMin = minAmp*100;
//
//    cout << "ampmax:" << maxAmp <<endl;
//    cout << "ampMin:" << minAmp <<endl;
//	for( int i = 0; i < row; ++i )
//	{
//		switch (status)
//		{
//		case 0:
//		case 1:
//			if (amplitudes[i] > ampMax || (amplitudes[i] > ampMin && zeroCrossVector[i] < zeroCrossMin))
//			{
//				start = MAX( ( i - voiceCount -1 ),1 );
//				status = 2;
//				silenceCount = 0;
//				voiceCount++;
//			}
//			else if( amplitudes[i] > ampMin || zeroCrossVector[i] < zeroCrossMax )  //可能是语音
//			{
//				status = 1;
//				voiceCount++;
//			}
//			else
//			{
//				status = 0;
//				voiceCount = 0;
//			}
//			break;
//		case 2:
//			if( amplitudes[i] > ampMin || zeroCrossVector[i] < zeroCrossMin )
//				voiceCount++;
//			else
//			{
//				silenceCount++;
//				if( silenceCount < minSilence )
//					voiceCount++;
//				else if( voiceCount < minVoice )
//				{
//					status = 0;
//					silenceCount = 0;
//					voiceCount = 0;
//				}
//				else
//					status = 3;
//			}
//			break;
//		case 3:
//			voiceCount = voiceCount - silenceCount/2;
//			end = start + voiceCount - 1;
//			startEndMap.insert( pair<int,int>( start, end ) );
//			status = 0;
//			silenceCount = 0;
//			voiceCount = 0;
//			break;
//		}
//	}
//	for( auto it = startEndMap.begin(); it != startEndMap.end(); ++it )
//	{
//		cout << it->first << " " << it->second <<endl;
//	}
    //for( auto &it : startEndMap )
    //{
    //    cout << it.first << " " << it.second << endl;
    //}
    ////
    //FILE* file = fopen( "result.raw", "wb" );

    //for( auto &it : startEndMap )
    //{
    //    int distance = it.first * hop;
    //    fwrite( buffer + distance, 1,(it.second - it.first)*hop*sizeof(float) , file );
    //}
    //fclose( file );

	return 0;
}

vector<float> AMDFCalc( const vector<float>& amdfData )
{
	vector<float> amdfResult;
	double maxamdfResult = 0.0;
	for( int i = 0;  i < amdfData.size(); ++i )
	{
		int k=0;
		double sum = 0;
		for( int j = i; j < amdfData.size(); ++j)
		{
			sum += fabs( amdfData[j] - amdfData[k++] );
		}
		amdfResult.push_back( sum );
		if( maxamdfResult < sum )
			maxamdfResult = sum;
	}
	amdfResult.push_back( maxamdfResult );
	return amdfResult;
}
int VoiceFrequenceCalc( const vector<float>& amdfResult, int sampleRate )
{
	double maxamdfResult = amdfResult.back();
	int index = 0;

	for( int i = 1; i < amdfResult.size() - 1; ++i )
	{
		if( amdfResult[i] < amdfResult[i-1] && amdfResult[i]<amdfResult[i+1] )
			if( amdfResult[i] < maxamdfResult/3.0)
				if( (sampleRate / i) < 1000 && (sampleRate / i) > 80 )//人声范围 [1000,80]
			{
				index = i;
				break;
			}
	}
	if( 0 == index )
		return 0;
	return sampleRate / index;
}