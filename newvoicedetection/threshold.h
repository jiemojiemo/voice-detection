#pragma once
#include <vector>

class IThresholdCalc{
public:
	virtual double GetAmplitudesMin(const std::vector<double>& amp,
		const std::vector<int>&zcr )=0;
};

class CErgodicFindTheMin : public IThresholdCalc{
public:
	virtual double GetAmplitudesMin(const std::vector<double>& amp,
		const std::vector<int>&zcr );
};

class CNoiseAverageAmp : public IThresholdCalc{
private:
	bool IsNoise( int max, double zcr ){ 
		return zcr < double(max)/kZcrUp && zcr > double(max)/kZcrDown;
	}
	double FindMaxZrc(const std::vector<int>&zcr);
public:
	virtual double GetAmplitudesMin(const std::vector<double>& amp,
		const std::vector<int>&zcr );

private:
	static const double kZcrDown;
	static const double kZcrUp;
};