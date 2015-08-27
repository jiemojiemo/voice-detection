#pragma once
#include "portaudio.h"
typedef unsigned long SampleType;
#define SAMPLE_TYPE_FLOAT32 ((SampleType) 0x00000001)
#define SAMPLE_TYPE_INT32	((SampleType) 0x00000002)
#define SAMPLE_TYPE_INT24	((SampleType) 0x00000004)
#define SAMPLE_TYPE_INT16   ((SampleType) 0x00000008)
#define SAMPLE_TYPE_INT8    ((SampleType) 0x00000010)
#define SAMPLE_TYPE_UINT8   ((SampleType) 0x00000020)

//¼��ʱ��Ҫ�Ĳ���
struct RecordeInfo
{
	unsigned int seconds;
	unsigned int channels;
	unsigned int sampleRate;
	SampleType sampleType;
	unsigned int framesPerBuffer;
};

//����¼��ʱ��Ҫ�Ĳ���
//seconds�����ϸ�<=ʵ�ʵ�¼���ĳ���
//RecordAudioPlayer������ָ��buffer��bufferSize��ֱ�Ӳ��ţ����ʱ�򣬻����secondsֵ
//��ֱ�Ӱ���bufferSize������
struct RecordPlayInfo{
	unsigned int seconds;
	unsigned int sampleRate;
	unsigned int channels;
	SampleType sampleType;
	unsigned int framePerBuffer;
};

//¼�����ݵĴ洢�ṹ
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