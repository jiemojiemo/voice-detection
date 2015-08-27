#include "voicedetection.h"

#define MAX(x,y) ( (x)>(y)?(x):(y) )
#define MIN(x,y) ( (x)<(y)?(x):(y) )
#define AMP_MIN_MUL 20

CVoiceDetection::CVoiceDetection()
{
	m_winSize = 256;
	m_hop = 90;
	m_ampMaxThreshold = 10;
	m_ampMinThreshold = 2;
	m_zcMaxThreshold = 10;
	m_zcMinThreshold = 5;

	m_minSilence = 30;
	m_minVoice = 100;
	m_voiceCount = 0;
	m_silenceCount = 0;

	m_status=0;
	m_start = 0;
	m_end = 0;

	m_frameCount = 0;
	m_frameData = NULL;
}
CVoiceDetection::~CVoiceDetection()
{
	if( m_frameData != NULL )
		for( int i = 0; i < m_frameCount; ++i )
		{
			if( m_frameData[i] != NULL )
				delete [] m_frameData[i];
		}
}
vector<SpeechSegment> CVoiceDetection::Detection( const float* buffer, int sampleCount )
{
    //map<int, int> startEndMap;
    EnFrame( buffer, sampleCount, m_winSize, m_hop );
    CalcZeroCrossRate();
    CalcAmplitude();
	CalcAmpThreshold();
    StartEndPointDetection();
	return FindSpeechSegment();
}
void CVoiceDetection::EnFrame( const float* dataIn, int sampleSize, int winSize, int hop )
{
	m_frameCount = (sampleSize - winSize)/hop + 1;
	m_frameData = new float*[ m_frameCount ];
	//auto dataOut = std::shared_ptr<float>( new float*[row] );
	for( int i = 0; i < m_frameCount; ++i )
	{
		m_frameData[i] = new float[winSize];
		if( m_frameData[i] != NULL )
			memcpy( m_frameData[i], (dataIn + i * hop), winSize*sizeof(float) );
	}
}
void CVoiceDetection::CalcZeroCrossRate()
{

    for( int i = 0; i < m_frameCount; ++i )
	{
		int count = 0;
		for( int j = 0; j < m_winSize -1; ++j )
		{
			if( m_frameData[i][j] * m_frameData[i][j + 1] < 0 && m_frameData[i][j] - m_frameData[i][j + 1] > 0.0002)
				count++;
		}
		m_zeroCrossRate.push_back( count );
	}
}
void CVoiceDetection::CalcAmplitude()
{
	for( int i = 0; i < m_frameCount; ++i )
	{
		double ampSum = 0;
		for( int j = 0; j < m_winSize -1; ++j )
		{
			ampSum += m_frameData[i][j] * m_frameData[i][j];
		}
		m_amplitude.push_back( ampSum );
	}
}
void CVoiceDetection::CalcAmpThreshold()
{
    double ampMax = GetAmplitudesMax();
    double ampMin = GetAmplitudesMin();
    m_ampMaxThreshold = ampMax/8.0;
    m_ampMinThreshold = ampMin*AMP_MIN_MUL;
}

double CVoiceDetection::GetAmplitudesMax()
{
    double maxAmp = 0.0;
    for( int i = 0; i < m_frameCount; ++i )
	{
		int count = 0;
		double ampSum = 0;
		for( int j = 0; j < m_winSize -1; ++j )
		{
			ampSum += m_frameData[i][j] * m_frameData[i][j];
		}
		if( ampSum > maxAmp )
			maxAmp = ampSum;
	}
	return maxAmp;
}
double CVoiceDetection::GetAmplitudesMin()
{
    double minAmp = 10000.0;
    for( int i = 0; i < m_frameCount; ++i )
	{
		int count = 0;
		double ampSum = 0;
		for( int j = 0; j < m_winSize -1; ++j )
		{
			ampSum += m_frameData[i][j] * m_frameData[i][j];
		}
		if( ampSum < minAmp )
			minAmp = ampSum;
	}
	return minAmp;
}
void CVoiceDetection::StartEndPointDetection()
{
    int status = 0;
    int start = 0;
    int end = 0;
    int voiceCount = 0;
    int silenceCount = 0;
    for( int i = 0; i < m_frameCount; ++i )
	{
		switch (status)
		{
		case 0:
		case 1:
			if ( IsVoice( m_amplitude[i], m_zeroCrossRate[i] ) )
			{
				start = MAX( ( i - voiceCount -1 ),1 );
				status = 2;
				silenceCount = 0;
				voiceCount++;
			}
			else if( IsMaybeVoice( m_amplitude[i], m_zeroCrossRate[i] ))  //¿ÉÄÜÊÇÓïÒô
			{
				status = 1;
				voiceCount++;
			}
			else
			{
				status = 0;
				voiceCount = 0;
			}
			break;
		case 2:
			if( IsVoice( m_amplitude[i], m_zeroCrossRate[i] ) )
				voiceCount++;
			else
			{
				silenceCount++;
				if( silenceCount < m_minSilence )
					voiceCount++;
				else if( voiceCount < m_minVoice )
				{
					status = 0;
					silenceCount = 0;
					voiceCount = 0;
				}
				else
					status = 3;
			}
			break;
		case 3:
			voiceCount = voiceCount - silenceCount/2;
			end = start + voiceCount - 1;
			m_startEndMap.insert( pair<int,int>( start, end ) );
			status = 0;
			silenceCount = 0;
			voiceCount = 0;
			break;
		}
	}
}
bool CVoiceDetection::IsVoice( double amp, int zcr )
{
	return amp > m_ampMinThreshold;
}

bool CVoiceDetection::IsMaybeVoice( double amp, int zcr )
{
	return amp > m_ampMinThreshold || zcr < m_zcMaxThreshold;
}

vector<SpeechSegment> CVoiceDetection::FindSpeechSegment()
{
	for( auto it = m_startEndMap.begin(); it != m_startEndMap.end(); ++it )
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
}








