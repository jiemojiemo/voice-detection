#include <string>
#include <stdio.h>
#include <assert.h>
#include <Windows.h>
#include "pcm2wav.h"

CPcm2Wav::CPcm2Wav()
{

}

int CPcm2Wav::Pcm2Wav( const float* buffer, unsigned long bufferSize,const Pcm2WavInfo& info )
{
	const string out( info.outFileName );
	int channel = info.channels;
	int sample_rate = info.sampleRate;
	int sample_bits = info.sampleBits;
	int format_tag = info.formatTag;	
	char word[5];
	DWORD DWORD_num;		
	WORD WORD_num;
	FILE *fpD;
	unsigned long dataSize = 0;

	fpD = fopen(out.c_str(),"wb");

	dataSize = bufferSize;

	//写RIFF
	fseek( fpD,RIFFID ,SEEK_SET );
	strcpy(word,"RIFF");
	fwrite( word, 1, 4, fpD );

	//.....写CHUNKE_SIZE = 音频实际大小 + 文件头（44个字节） - 8
	DWORD_num = dataSize + 0x2c - 8;
	fseek( fpD, CHUNKE_SIZE, SEEK_SET );
	fwrite( &DWORD_num, 1, sizeof(DWORD), fpD );

	//写WAVE
	strcpy(word,"WAVE");
	fseek( fpD, WAVE, SEEK_SET );
	fwrite( word, 1, 4, fpD );

	//写FMT
	strcpy(word,"fmt ");
	fseek( fpD, FMT, SEEK_SET );
	fwrite( word, 1, 4, fpD );

	//写size(16 or 18)通常16
	DWORD_num = 16;
	fseek( fpD, FMT_CKSIZE, SEEK_SET );
	fwrite( &DWORD_num, 1, sizeof(DWORD), fpD );

	//写AudioFormat
	WORD_num = format_tag;	//pcm数据
	fseek( fpD, AUDIO_FORMAT, SEEK_SET );
	fwrite( &WORD_num, 1, sizeof(WORD), fpD );

	//写声道数
	WORD_num = channel;
	fseek( fpD, CHANNELS, SEEK_SET );
	fwrite( &WORD_num, 1, sizeof(WORD), fpD );

	//写采样率
	DWORD_num = sample_rate;
	fseek( fpD, SAMPLE_RATE, SEEK_SET );
	fwrite( &DWORD_num, 1, sizeof(DWORD), fpD );

	//写byte率 = 采样频率*音频通道数*每次采样得到的样本位数/8
	DWORD_num = sample_rate * channel * sample_bits / 8;
	fseek( fpD, BYTE_RATE, SEEK_SET );
	fwrite( &DWORD_num, 1, sizeof(DWORD), fpD );

	//写块对齐 块对齐=通道数*每次采样得到的样本位数/8
	WORD_num = channel * sample_bits / 8;
	fseek( fpD, BLOCKALIGN, SEEK_SET );
	fwrite( &WORD_num, 1, sizeof(WORD), fpD );

	//写样本数据位
	WORD_num = sample_bits;
	fseek( fpD, BITS_PER_SAMPLE, SEEK_SET );
	fwrite( &WORD_num, 1, sizeof(WORD), fpD );

	//写"DATA"
	strcpy( word, "data" );
	fseek( fpD, DATAID, SEEK_SET );
	fwrite( word, 1, 4, fpD );


	//写实际的音频数据
	fseek( fpD, DATA_BEGIN, SEEK_SET );
	fwrite( buffer, 1, dataSize, fpD ); 

	fclose( fpD );

	return 0;
}
int CPcm2Wav::Pcm2Wav( const Pcm2WavInfo& info )
{
	const string in( info.inFileName );
	const string out( info.outFileName );
	int channel = info.channels;
	int sample_rate = info.sampleRate;
	int sample_bits = info.sampleBits;
	int format_tag = info.formatTag;	
	char word[5];
	DWORD DWORD_num;		
	WORD WORD_num;
	FILE *fpS;
	FILE *fpD;
	int dataSize = 0;
	float* dataBuffer = NULL;

	fpS = fopen(in.c_str(),"rb");
	fpD = fopen(out.c_str(),"wb");


	//将文件指针指向文件尾巴
	fseek( fpS, 0, SEEK_END );
	//获取音频数据的实际大小
	dataSize = ftell( fpS );
	//还原文件指针位置
	fseek( fpS, 0, SEEK_SET );

	//写RIFF
	fseek( fpD,RIFFID ,SEEK_SET );
	strcpy(word,"RIFF");
	fwrite( word, 1, 4, fpD );

	//.....写CHUNKE_SIZE = 音频实际大小 + 文件头（44个字节） - 8
	DWORD_num = dataSize + 0x2c - 8;
	fseek( fpD, CHUNKE_SIZE, SEEK_SET );
	fwrite( &DWORD_num, 1, sizeof(DWORD), fpD );

	//写WAVE
	strcpy(word,"WAVE");
	fseek( fpD, WAVE, SEEK_SET );
	fwrite( word, 1, 4, fpD );

	//写FMT
	strcpy(word,"fmt ");
	fseek( fpD, FMT, SEEK_SET );
	fwrite( word, 1, 4, fpD );

	//写size(16 or 18)通常16
	DWORD_num = 16;
	fseek( fpD, FMT_CKSIZE, SEEK_SET );
	fwrite( &DWORD_num, 1, sizeof(DWORD), fpD );

	//写AudioFormat
	WORD_num = format_tag;	//pcm数据
	fseek( fpD, AUDIO_FORMAT, SEEK_SET );
	fwrite( &WORD_num, 1, sizeof(WORD), fpD );

	//写声道数
	WORD_num = channel;
	fseek( fpD, CHANNELS, SEEK_SET );
	fwrite( &WORD_num, 1, sizeof(WORD), fpD );

	//写采样率
	DWORD_num = sample_rate;
	fseek( fpD, SAMPLE_RATE, SEEK_SET );
	fwrite( &DWORD_num, 1, sizeof(DWORD), fpD );

	//写byte率 = 采样频率*音频通道数*每次采样得到的样本位数/8
	DWORD_num = sample_rate * channel * sample_bits / 8;
	fseek( fpD, BYTE_RATE, SEEK_SET );
	fwrite( &DWORD_num, 1, sizeof(DWORD), fpD );

	//写块对齐 块对齐=通道数*每次采样得到的样本位数/8
	WORD_num = channel * sample_bits / 8;
	fseek( fpD, BLOCKALIGN, SEEK_SET );
	fwrite( &WORD_num, 1, sizeof(WORD), fpD );

	//写样本数据位
	WORD_num = sample_bits;
	fseek( fpD, BITS_PER_SAMPLE, SEEK_SET );
	fwrite( &WORD_num, 1, sizeof(WORD), fpD );

	//写"DATA"
	strcpy( word, "data" );
	fseek( fpD, DATAID, SEEK_SET );
	fwrite( word, 1, 4, fpD );

	//写实际音频数据大小
	DWORD_num = dataSize;
	fseek( fpD, DATA_SIZE, SEEK_SET );
	fwrite( &DWORD_num, 1, sizeof(DWORD), fpD );

	//写实际的音频数据

	//从文件读取音频数据
	dataBuffer = (float*)malloc( dataSize );
	assert( dataBuffer != NULL );
	fseek( fpS, 0, SEEK_SET );
	fread( dataBuffer, 1, dataSize, fpS );

	fseek( fpD, DATA_BEGIN, SEEK_SET );
	fwrite( dataBuffer, 1, dataSize, fpD ); 

	if( dataBuffer != NULL )
	{
		free( dataBuffer );
		dataBuffer = NULL;
	}

	fclose( fpS );
	fclose( fpD );

	return 0;
}

int CPcm2Wav::_Pcm2Wav( string inFile, string outFile, int channels, int sam_rate, int sam_bits, int fmt_tag )
{
	const string in( inFile );
	const string out( outFile );
	int channel = channels;
	int sample_rate = sam_rate;
	int sample_bits = sam_bits;
	int format_tag = fmt_tag;	
	char word[5];
	DWORD DWORD_num;		
	WORD WORD_num;
	FILE *fpS;
	FILE *fpD;
	int dataSize = 0;
	float* dataBuffer = NULL;

	fpS = fopen(in.c_str(),"rb");
	fpD = fopen(out.c_str(),"wb");


	//将文件指针指向文件尾巴
	fseek( fpS, 0, SEEK_END );
	//获取音频数据的实际大小
	dataSize = ftell( fpS );
	//还原文件指针位置
	fseek( fpS, 0, SEEK_SET );

	//写RIFF
	fseek( fpD,RIFFID ,SEEK_SET );
	strcpy(word,"RIFF");
	fwrite( word, 1, 4, fpD );

	//.....写CHUNKE_SIZE = 音频实际大小 + 文件头（44个字节） - 8
	DWORD_num = dataSize + 0x2c - 8;
	fseek( fpD, CHUNKE_SIZE, SEEK_SET );
	fwrite( &DWORD_num, 1, sizeof(DWORD), fpD );

	//写WAVE
	strcpy(word,"WAVE");
	fseek( fpD, WAVE, SEEK_SET );
	fwrite( word, 1, 4, fpD );

	//写FMT
	strcpy(word,"fmt ");
	fseek( fpD, FMT, SEEK_SET );
	fwrite( word, 1, 4, fpD );

	//写size(16 or 18)通常16
	DWORD_num = 16;
	fseek( fpD, FMT_CKSIZE, SEEK_SET );
	fwrite( &DWORD_num, 1, sizeof(DWORD), fpD );

	//写AudioFormat
	WORD_num = format_tag;	//pcm数据
	fseek( fpD, AUDIO_FORMAT, SEEK_SET );
	fwrite( &WORD_num, 1, sizeof(WORD), fpD );

	//写声道数
	WORD_num = channel;
	fseek( fpD, CHANNELS, SEEK_SET );
	fwrite( &WORD_num, 1, sizeof(WORD), fpD );

	//写采样率
	DWORD_num = sample_rate;
	fseek( fpD, SAMPLE_RATE, SEEK_SET );
	fwrite( &DWORD_num, 1, sizeof(DWORD), fpD );

	//写byte率 = 采样频率*音频通道数*每次采样得到的样本位数/8
	DWORD_num = sample_rate * channel * sample_bits / 8;
	fseek( fpD, BYTE_RATE, SEEK_SET );
	fwrite( &DWORD_num, 1, sizeof(DWORD), fpD );

	//写块对齐 块对齐=通道数*每次采样得到的样本位数/8
	WORD_num = channel * sample_bits / 8;
	fseek( fpD, BLOCKALIGN, SEEK_SET );
	fwrite( &WORD_num, 1, sizeof(WORD), fpD );

	//写样本数据位
	WORD_num = sample_bits;
	fseek( fpD, BITS_PER_SAMPLE, SEEK_SET );
	fwrite( &WORD_num, 1, sizeof(WORD), fpD );

	//写"DATA"
	strcpy( word, "data" );
	fseek( fpD, DATAID, SEEK_SET );
	fwrite( word, 1, 4, fpD );

	//写实际音频数据大小
	DWORD_num = dataSize;
	fseek( fpD, DATA_SIZE, SEEK_SET );
	fwrite( &DWORD_num, 1, sizeof(DWORD), fpD );

	//写实际的音频数据

	//从文件读取音频数据
	dataBuffer = (float*)malloc( dataSize );
	assert( dataBuffer != NULL );
	fseek( fpS, 0, SEEK_SET );
	fread( dataBuffer, 1, dataSize, fpS );

	fseek( fpD, DATA_BEGIN, SEEK_SET );
	fwrite( dataBuffer, 1, dataSize, fpD ); 

	if( dataBuffer != NULL )
	{
		free( dataBuffer );
		dataBuffer = NULL;
	}

	fclose( fpS );
	fclose( fpD );

	return 0;
}

