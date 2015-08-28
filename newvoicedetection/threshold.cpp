#include "threshold.h"
#include <assert.h>


const double CNoiseAverageAmp::kZcrDown = 1.5;
const double CNoiseAverageAmp::kZcrUp = 1.2;


double CErgodicFindTheMin::GetAmplitudesMin(const std::vector<double>& amp,
	const std::vector<int>&zcr )
{
	double minAmp = 100000.0;
	for( int i = 0; i < amp.size(); ++i )
	{
		if( amp[i] < minAmp )
			minAmp = amp[i];
	}
	assert( minAmp < 100000 );
	return minAmp;
}

double CNoiseAverageAmp::GetAmplitudesMin(const std::vector<double>& amp, const std::vector<int>&zcr )
{
	double maxZcr = FindMaxZrc( zcr );

	int count = 0;
	double noiseAverageAmp = 0.0;
	for( int i = 0; i < zcr.size(); ++i )
	{
		if( IsNoise( maxZcr, zcr[i] ) )
		{
			++count;
			noiseAverageAmp += amp[i];
		}
	}
	assert( count > 0 );
	noiseAverageAmp /= count;
	return noiseAverageAmp;
}
//找 最大 过零率
double CNoiseAverageAmp::FindMaxZrc(const std::vector<int>&zcr)
{
	double max = 0.0;
	for( int i = 0; i < zcr.size(); ++i )
	{
		if( zcr[i]  > max)
			max = zcr[i];
	}
	assert( max > 0.0 );
	return max;
}