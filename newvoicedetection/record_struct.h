#pragma once
#include "portaudio.h"
typedef unsigned long SampleType;
#define SAMPLE_TYPE_FLOAT32 ((SampleType) 0x00000001)
#define SAMPLE_TYPE_INT32	((SampleType) 0x00000002)
#define SAMPLE_TYPE_INT24	((SampleType) 0x00000004)
#define SAMPLE_TYPE_INT16   ((SampleType) 0x00000008)
#define SAMPLE_TYPE_INT8    ((SampleType) 0x00000010)
#define SAMPLE_TYPE_UINT8   ((SampleType) 0x00000020)

//录音时需要的参数
struct RecordeInfo
{
	unsigned int seconds;
	unsigned int channels;
	unsigned int sampleRate;
	SampleType sampleType;
	unsigned int framesPerBuffer;
};

//播放录音时需要的参数
//seconds必须严格<=实际的录音的长度
//RecordAudioPlayer，可以指定buffer和bufferSize来直接播放，这个时候，会忽略seconds值
//而直接按照bufferSize来处理
struct RecordPlayInfo{
	unsigned int seconds;
	unsigned int sampleRate;
	unsigned int channels;
	SampleType sampleType;
	unsigned int framePerBuffer;
};

//录音数据的存储结构
struct RecordData{
	int          frameIndex;	/* Index into sample array. */
	int          maxFrameIndex;
	unsigned long totalBytes;
	void	     *recordedSamples;
};

typedef int (*callbackPointer)( const void*, void*, unsigned long,
	const PaStreamCallbackTimeInfo*,
	PaStreamCallbackFlags,
	void*);