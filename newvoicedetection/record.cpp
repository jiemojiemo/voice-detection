#include "record.h"
#include <stdio.h>
#include <string>

RecordeInfo RecordABC::m_info;

RecordABC::RecordABC()
{
}
RecordABC::~RecordABC()
{
	if( m_data.recordedSamples != NULL )
	{
		free( m_data.recordedSamples );
		m_data.recordedSamples;
	}
}

int RecordABC::StartRecord()
{
	PaStreamParameters		inputParameters;
	PaError					err = paNoError;
	PaStream*				stream;
	printf( "Record Init\n" );fflush(stdout);

	//PortAudio Init
	err = Pa_Initialize();					//设备初始化
	if( err != paNoError ) return err;

	//设置输入设备参数
	inputParameters.device = Pa_GetDefaultInputDevice();	//获取默认的输入设备
	if( inputParameters.device == paNoDevice )
	{
		fprintf( stderr, "Error:No default input device.\n" );
		return -1;
	}

	inputParameters.channelCount = m_info.channels;
	inputParameters.sampleFormat = this->GetSampleFormat();
	inputParameters.suggestedLatency =
		Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
	inputParameters.hostApiSpecificStreamInfo = NULL;


	//打开音频流，进行准备录音
	err = Pa_OpenStream( &stream,
		&inputParameters,
		NULL,	//no output
		m_info.sampleRate,
		m_info.framesPerBuffer,
		paClipOff,
		m_pCallback,
		&m_data
		);

	//开始录音
	err = Pa_StartStream( stream );
	if( err != paNoError )
		return err;
	printf("\n=== Now %ds recording!! Please speak into the microphone. ===\n",m_info.seconds); fflush(stdout);

	//是否还在录音
	while( ( err = Pa_IsStreamActive( stream ) ) == 1 )
	{
		Pa_Sleep(1000);
		printf("index = %d\n", m_data.frameIndex ); fflush(stdout);
	}
	if( err < 0 )	return err;

	//结束录音
	err = Pa_StopStream( stream );
	if( err < 0 )	return err;
}
/*******************************
** FunctionName：GetSampleFormat
** Comment:     根据bits获得format
** return：     format
** Creator：    HW
** Date：		2015-8-14
** Modifier：	
** ModifyDate： 
** Version：    1.0.0
*******************************/
PaSampleFormat RecordABC::GetSampleFormat()
{
	switch( m_info.sampleType )
	{
	case SAMPLE_TYPE_FLOAT32:
		return paFloat32;
	case SAMPLE_TYPE_INT16:
		return paInt16;
	case SAMPLE_TYPE_INT8:
		return paInt8;
	case SAMPLE_TYPE_UINT8:
		return paUInt8;
	default:
		return paFloat32;
	}
}

//Float32
RecordFloat32::RecordFloat32( const RecordeInfo& info )
{
	m_info = info;

	m_data.recordedSamples = NULL;
	m_data.frameIndex = 0;
	m_data.maxFrameIndex = info.sampleRate * info.seconds;
	MallocForData();

	m_pCallback = this->recordCallback;

}
RecordFloat32::~RecordFloat32()
{

}

void RecordFloat32::MallocForData()
{
	if( m_data.recordedSamples != NULL )
	{
		free( m_data.recordedSamples );
		m_data.recordedSamples = NULL;
	}

	m_data.totalBytes = m_data.maxFrameIndex * m_info.channels * sizeof(float);
	m_data.recordedSamples = (float*)malloc( m_data.totalBytes );
	if( m_data.recordedSamples == NULL )
	{
		fprintf( stderr, "Eorror: malloc error.\n" );
		exit(-1);
	}
}

int RecordFloat32::recordCallback(const void *inputBuffer, 
	void *outputBuffer, unsigned long framesPerBuffer, 
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags, void *userData)
{
	RecordData* data = (RecordData*)userData;
	const float *rptr = (const float*)inputBuffer;
	float* buffer = (float*) data->recordedSamples;
	float *wptr = (float*)&buffer[data->frameIndex * m_info.channels];
	long framesToCalc;
	long i;
	int finished;
	unsigned long framesLeft = data->maxFrameIndex - data->frameIndex;

	(void) outputBuffer; /* Prevent unused variable warnings. */
	(void) timeInfo;
	(void) statusFlags;
	(void) userData;

	if( framesLeft < framesPerBuffer )
	{
		framesToCalc = framesLeft;
		finished = paComplete;
	}
	else
	{
		framesToCalc = framesPerBuffer;
		finished = paContinue;
	}

	if( inputBuffer == NULL )
	{
		for( i=0; i<framesToCalc; i++ )
		{
			*wptr++ = 0;  /* left */
			if( m_info.channels == 2 ) *wptr++ = 0;  /* right */
		}
	}
	else
	{
		for( i=0; i<framesToCalc; i++ )
		{
			*wptr++ = *rptr++;  /* left */
			if( m_info.channels == 2 ) *wptr++ = *rptr++;  /* right */
		}
	}
	data->frameIndex += framesToCalc;
	return finished;
}


//Int 16
RecordInt16::RecordInt16( const RecordeInfo& info )
{
	m_info = info;

	m_data.recordedSamples = NULL;
	m_data.frameIndex = 0;
	m_data.maxFrameIndex = info.sampleRate * info.seconds;
	MallocForData();

	m_pCallback = this->recordCallback;
}
RecordInt16::~RecordInt16()
{

}

void RecordInt16::MallocForData()
{
	if( m_data.recordedSamples != NULL )
	{
		free( m_data.recordedSamples );
		m_data.recordedSamples = NULL;
	}
	m_data.totalBytes = m_data.maxFrameIndex * m_info.channels * sizeof(short);
	m_data.recordedSamples = (short*)malloc( m_data.totalBytes );
	if( m_data.recordedSamples == NULL )
	{
		fprintf( stderr, "Eorror: malloc error.\n" );
		exit(-1);
	}
}

int RecordInt16::recordCallback(const void *inputBuffer, 
	void *outputBuffer, unsigned long framesPerBuffer, 
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData)
{
	RecordData* data = (RecordData*)userData;
	const short *rptr = (const short*)inputBuffer;
	short* buffer = (short*) data->recordedSamples;
	short *wptr = (short*)&buffer[data->frameIndex * m_info.channels];
	long framesToCalc;
	long i;
	int finished;
	unsigned long framesLeft = data->maxFrameIndex - data->frameIndex;

	(void) outputBuffer; /* Prevent unused variable warnings. */
	(void) timeInfo;
	(void) statusFlags;
	(void) userData;

	if( framesLeft < framesPerBuffer )
	{
		framesToCalc = framesLeft;
		finished = paComplete;
	}
	else
	{
		framesToCalc = framesPerBuffer;
		finished = paContinue;
	}

	if( inputBuffer == NULL )
	{
		for( i=0; i<framesToCalc; i++ )
		{
			*wptr++ = 0;  /* left */
			if( m_info.channels == 2 ) *wptr++ = 0;  /* right */
		}
	}
	else
	{
		for( i=0; i<framesToCalc; i++ )
		{
			*wptr++ = *rptr++;  /* left */
			if( m_info.channels == 2 ) *wptr++ = *rptr++;  /* right */
		}
	}
	data->frameIndex += framesToCalc;
	return finished;
}

//Int8

RecordInt8::RecordInt8( const RecordeInfo& info )
{
	m_info = info;

	m_data.recordedSamples = NULL;
	m_data.frameIndex = 0;
	m_data.maxFrameIndex = info.sampleRate * info.seconds;
	MallocForData();

	m_pCallback = this->recordCallback;
}
RecordInt8::~RecordInt8()
{

}

void RecordInt8::MallocForData()
{
	if( m_data.recordedSamples != NULL )
	{
		free( m_data.recordedSamples );
		m_data.recordedSamples = NULL;
	}
	m_data.totalBytes = m_data.maxFrameIndex * m_info.channels * sizeof(char);
	m_data.recordedSamples = (char*)malloc( m_data.totalBytes );
	if( m_data.recordedSamples == NULL )
	{
		fprintf( stderr, "Eorror: malloc error.\n" );
		exit(-1);
	}
}

int RecordInt8::recordCallback(const void *inputBuffer,
	void *outputBuffer, unsigned long framesPerBuffer, 
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags, void *userData)
{
	RecordData* data = (RecordData*)userData;
	const char *rptr = (const char*)inputBuffer;
	char* buffer = (char*) data->recordedSamples;
	char *wptr = (char*)&buffer[data->frameIndex * m_info.channels];
	long framesToCalc;
	long i;
	int finished;
	unsigned long framesLeft = data->maxFrameIndex - data->frameIndex;

	(void) outputBuffer; /* Prevent unused variable warnings. */
	(void) timeInfo;
	(void) statusFlags;
	(void) userData;

	if( framesLeft < framesPerBuffer )
	{
		framesToCalc = framesLeft;
		finished = paComplete;
	}
	else
	{
		framesToCalc = framesPerBuffer;
		finished = paContinue;
	}

	if( inputBuffer == NULL )
	{
		for( i=0; i<framesToCalc; i++ )
		{
			*wptr++ = 0;  /* left */
			if( m_info.channels == 2 ) *wptr++ = 0;  /* right */
		}
	}
	else
	{
		for( i=0; i<framesToCalc; i++ )
		{
			*wptr++ = *rptr++;  /* left */
			if( m_info.channels == 2 ) *wptr++ = *rptr++;  /* right */
		}
	}
	data->frameIndex += framesToCalc;
	return finished;
}

//UInt 8
RecordUInt8::RecordUInt8( const RecordeInfo& info )
{
	m_info = info;

	m_data.recordedSamples = NULL;
	m_data.frameIndex = 0;
	m_data.maxFrameIndex = info.sampleRate * info.seconds;
	MallocForData();

	m_pCallback = this->recordCallback;
}
RecordUInt8::~RecordUInt8()
{

}

void RecordUInt8::MallocForData()
{
	if( m_data.recordedSamples != NULL )
	{
		free( m_data.recordedSamples );
		m_data.recordedSamples = NULL;
	}
	m_data.totalBytes = m_data.maxFrameIndex * m_info.channels * sizeof(unsigned char);
	m_data.recordedSamples = (unsigned char*)malloc( m_data.totalBytes );
	if( m_data.recordedSamples == NULL )
	{
		fprintf( stderr, "Eorror: malloc error.\n" );
		exit(-1);
	}
}

int RecordUInt8::recordCallback(const void *inputBuffer, 
	void *outputBuffer, unsigned long framesPerBuffer, 
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData)
{
	RecordData* data = (RecordData*)userData;
	const unsigned char *rptr = (const unsigned char*)inputBuffer;
	unsigned char* buffer = (unsigned char*) data->recordedSamples;
	unsigned char *wptr = (unsigned char*)&buffer[data->frameIndex * m_info.channels];
	long framesToCalc;
	long i;
	int finished;
	unsigned long framesLeft = data->maxFrameIndex - data->frameIndex;

	(void) outputBuffer; /* Prevent unused variable warnings. */
	(void) timeInfo;
	(void) statusFlags;
	(void) userData;

	if( framesLeft < framesPerBuffer )
	{
		framesToCalc = framesLeft;
		finished = paComplete;
	}
	else
	{
		framesToCalc = framesPerBuffer;
		finished = paContinue;
	}

	if( inputBuffer == NULL )
	{
		for( i=0; i<framesToCalc; i++ )
		{
			*wptr++ = 0;  /* left */
			if( m_info.channels == 2 ) *wptr++ = 0;  /* right */
		}
	}
	else
	{
		for( i=0; i<framesToCalc; i++ )
		{
			*wptr++ = *rptr++;  /* left */
			if( m_info.channels == 2 ) *wptr++ = *rptr++;  /* right */
		}
	}
	data->frameIndex += framesToCalc;
	return finished;
}

