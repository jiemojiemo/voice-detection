#ifndef VOICEDETECTION_H_INCLUDED
#define VOICEDETECTION_H_INCLUDED
#include "calcthreshold.h"
#include <map>
#include <vector>
using std::map;
using std::vector;
using std::pair;

#define MIN_VOICE_FREQUENCY 80.0
#define MAX_VOICE_FREQUENCY 500
#define MAX(x,y) ( (x)>(y)?(x):(y) )
#define MIN(x,y) ( (x)<(y)?(x):(y) )
#define AMP_MIN_MUL 30

struct SpeechSegment{
	int frequence;
	unsigned int start;
	unsigned int end;
	float beat;
	SpeechSegment( int fre, int s, int e, float b ):
	frequence(fre), start(s), end(e), beat(b)
	{

	}
};

class CVoiceDetection{
private:
    CVoiceDetection( const CVoiceDetection& ){}
    void operator=( const CVoiceDetection& ){}

	bool IsVoice( double amp, int zcr );
	bool IsMaybeVoice( double amp, int zcr );
    void CalcZeroCrossRate(  );
    void CalcAmplitude(  );
    void CalcAmpThreshold(  );
    double GetAmplitudesMax( );
    double GetAmplitudesMin(  );
	void EnFrame( const float* dataIn, int sampleSize, int winSize, int hop );
    void StartEndPointDetection();
	vector<SpeechSegment> FindSpeechSegment( const float* buffer, int sampleRate );
	vector<float> AMDFCalc( const vector<float>& amdfData );
	int VoiceFrequenceCalc( const vector<float>& amdfResult, int sampleRate );


public:
    CVoiceDetection();
	~CVoiceDetection();
    vector<SpeechSegment> Detection( const float* buffer, int sampleCount, int sampleRate );

public:
    vector<int> m_zeroCrossRate;
    vector<double> m_amplitude;
	vector<SpeechSegment> m_speechSegment;
	map<int, int> m_startEndMap;
    int m_winSize;
	int m_hop;
	float m_ampMaxThreshold;
	float m_ampMinThreshold;
	int m_zcMaxThreshold;
	int m_zcMinThreshold;

	int m_minSilence;
	int m_minVoice;
	int m_voiceCount;
	int m_silenceCount;

	int m_status;
	int m_start;
	int m_end;

	int m_frameCount;
	float** m_frameData;
};


#endif // VOICEDETECTION_H_INCLUDED
