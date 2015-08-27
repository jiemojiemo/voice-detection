#include "detection.h"
#include <memory>
#include <iostream>
#include <string.h>
using namespace std;
float** EnFrame( float* dataIn, int sampleSize, int winSize, int hop )
{
	int row = (sampleSize - winSize)/hop + 1;
	auto dataOut = new float*[ row ];
	//auto dataOut = std::shared_ptr<float>( new float*[row] );
	for( int i = 0; i < row; ++i )
	{
		dataOut[i] = new float[winSize];
		//dataOut[i] = std::shared_ptr<float>( new float[winSize] );
		memcpy( dataOut[i], (dataIn + i * hop), winSize*sizeof(float) );
	}
	return dataOut;
}
