#include "calcthreshold.h"
#include <assert.h>
#include <algorithm>
#include <numeric>

const double CThreshodCalculator::kZcrDown = 1.5;
const double CThreshodCalculator::kZcrUp = 1.2;

CThreshodCalculator::CThreshodCalculator(const std::vector<double>& amp,
	const std::vector<int>&zcr) :
	m_amp(amp), m_zcr(zcr)
{
}

double CThreshodCalculator::GetMaxAmp()const
{
	return *max_element(m_amp.begin(), m_amp.end());
}

double CThreshodCalculator::GetAveAmp()const
{
	return accumulate(m_amp.begin(), m_amp.end(), 0.0) / m_amp.size();
}

double CThreshodCalculator::GetNoiseAmp()const
{
	int maxZcr = *max_element(m_zcr.begin(), m_zcr.end());
	int count = 0;
	double noiseAverageAmp = 0.0;
	for (int i = 0; i < m_zcr.size(); ++i)
	{
		if (IsNoise(maxZcr, m_zcr[i]))
		{
			++count;
			noiseAverageAmp += m_amp[i];
		}
	}
	assert(count > 0);
	noiseAverageAmp /= count;
	return noiseAverageAmp;
}

double CThreshodCalculator::GetThreshold()const
{
	double maxAmp( this->GetMaxAmp() );
	double aveAmp( this->GetAveAmp() );
	double noiseAmp( this->GetNoiseAmp() );
	double threshold = std::min( std::min(maxAmp/10, aveAmp/2), 5.0*noiseAmp );
	return threshold;
}