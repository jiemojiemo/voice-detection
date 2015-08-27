#ifndef VOICEDETECTION_H_INCLUDED
#define VOICEDETECTION_H_INCLUDED
#include <map>
#include <vector>
using std::map;
using std::vector;
using std::pair;

struct SpeechSegment{
	int frequence;
	unsigned int start;
	unsigned int end;
	SpeechSegment( int fre, int s, int e ):
	frequence(fre), start(s), end(e)
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
	vector<SpeechSegment> FindSpeechSegment();


public:
    CVoiceDetection();
	~CVoiceDetection();
    vector<SpeechSegment> Detection( const float* buffer, int sampleCount );

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
