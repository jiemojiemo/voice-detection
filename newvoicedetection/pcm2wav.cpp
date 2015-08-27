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

	//дRIFF
	fseek( fpD,RIFFID ,SEEK_SET );
	strcpy(word,"RIFF");
	fwrite( word, 1, 4, fpD );

	//.....дCHUNKE_SIZE = ��Ƶʵ�ʴ�С + �ļ�ͷ��44���ֽڣ� - 8
	DWORD_num = dataSize + 0x2c - 8;
	fseek( fpD, CHUNKE_SIZE, SEEK_SET );
	fwrite( &DWORD_num, 1, sizeof(DWORD), fpD );

	//дWAVE
	strcpy(word,"WAVE");
	fseek( fpD, WAVE, SEEK_SET );
	fwrite( word, 1, 4, fpD );

	//дFMT
	strcpy(word,"fmt ");
	fseek( fpD, FMT, SEEK_SET );
	fwrite( word, 1, 4, fpD );

	//дsize(16 or 18)ͨ��16
	DWORD_num = 16;
	fseek( fpD, FMT_CKSIZE, SEEK_SET );
	fwrite( &DWORD_num, 1, sizeof(DWORD), fpD );

	//дAudioFormat
	WORD_num = format_tag;	//pcm����
	fseek( fpD, AUDIO_FORMAT, SEEK_SET );
	fwrite( &WORD_num, 1, sizeof(WORD), fpD );

	//д������
	WORD_num = channel;
	fseek( fpD, CHANNELS, SEEK_SET );
	fwrite( &WORD_num, 1, sizeof(WORD), fpD );

	//д������
	DWORD_num = sample_rate;
	fseek( fpD, SAMPLE_RATE, SEEK_SET );
	fwrite( &DWORD_num, 1, sizeof(DWORD), fpD );

	//дbyte�� = ����Ƶ��*��Ƶͨ����*ÿ�β����õ�������λ��/8
	DWORD_num = sample_rate * channel * sample_bits / 8;
	fseek( fpD, BYTE_RATE, SEEK_SET );
	fwrite( &DWORD_num, 1, sizeof(DWORD), fpD );

	//д����� �����=ͨ����*ÿ�β����õ�������λ��/8
	WORD_num = channel * sample_bits / 8;
	fseek( fpD, BLOCKALIGN, SEEK_SET );
	fwrite( &WORD_num, 1, sizeof(WORD), fpD );

	//д��������λ
	WORD_num = sample_bits;
	fseek( fpD, BITS_PER_SAMPLE, SEEK_SET );
	fwrite( &WORD_num, 1, sizeof(WORD), fpD );

	//д"DATA"
	strcpy( word, "data" );
	fseek( fpD, DATAID, SEEK_SET );
	fwrite( word, 1, 4, fpD );


	//дʵ�ʵ���Ƶ����
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


	//���ļ�ָ��ָ���ļ�β��
	fseek( fpS, 0, SEEK_END );
	//��ȡ��Ƶ���ݵ�ʵ�ʴ�С
	dataSize = ftell( fpS );
	//��ԭ�ļ�ָ��λ��
	fseek( fpS, 0, SEEK_SET );

	//дRIFF
	fseek( fpD,RIFFID ,SEEK_SET );
	strcpy(word,"RIFF");
	fwrite( word, 1, 4, fpD );

	//.....дCHUNKE_SIZE = ��Ƶʵ�ʴ�С + �ļ�ͷ��44���ֽڣ� - 8
	DWORD_num = dataSize + 0x2c - 8;
	fseek( fpD, CHUNKE_SIZE, SEEK_SET );
	fwrite( &DWORD_num, 1, sizeof(DWORD), fpD );

	//дWAVE
	strcpy(word,"WAVE");
	fseek( fpD, WAVE, SEEK_SET );
	fwrite( word, 1, 4, fpD );

	//дFMT
	strcpy(word,"fmt ");
	fseek( fpD, FMT, SEEK_SET );
	fwrite( word, 1, 4, fpD );

	//дsize(16 or 18)ͨ��16
	DWORD_num = 16;
	fseek( fpD, FMT_CKSIZE, SEEK_SET );
	fwrite( &DWORD_num, 1, sizeof(DWORD), fpD );

	//дAudioFormat
	WORD_num = format_tag;	//pcm����
	fseek( fpD, AUDIO_FORMAT, SEEK_SET );
	fwrite( &WORD_num, 1, sizeof(WORD), fpD );

	//д������
	WORD_num = channel;
	fseek( fpD, CHANNELS, SEEK_SET );
	fwrite( &WORD_num, 1, sizeof(WORD), fpD );

	//д������
	DWORD_num = sample_rate;
	fseek( fpD, SAMPLE_RATE, SEEK_SET );
	fwrite( &DWORD_num, 1, sizeof(DWORD), fpD );

	//дbyte�� = ����Ƶ��*��Ƶͨ����*ÿ�β����õ�������λ��/8
	DWORD_num = sample_rate * channel * sample_bits / 8;
	fseek( fpD, BYTE_RATE, SEEK_SET );
	fwrite( &DWORD_num, 1, sizeof(DWORD), fpD );

	//д����� �����=ͨ����*ÿ�β����õ�������λ��/8
	WORD_num = channel * sample_bits / 8;
	fseek( fpD, BLOCKALIGN, SEEK_SET );
	fwrite( &WORD_num, 1, sizeof(WORD), fpD );

	//д��������λ
	WORD_num = sample_bits;
	fseek( fpD, BITS_PER_SAMPLE, SEEK_SET );
	fwrite( &WORD_num, 1, sizeof(WORD), fpD );

	//д"DATA"
	strcpy( word, "data" );
	fseek( fpD, DATAID, SEEK_SET );
	fwrite( word, 1, 4, fpD );

	//дʵ����Ƶ���ݴ�С
	DWORD_num = dataSize;
	fseek( fpD, DATA_SIZE, SEEK_SET );
	fwrite( &DWORD_num, 1, sizeof(DWORD), fpD );

	//дʵ�ʵ���Ƶ����

	//���ļ���ȡ��Ƶ����
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


	//���ļ�ָ��ָ���ļ�β��
	fseek( fpS, 0, SEEK_END );
	//��ȡ��Ƶ���ݵ�ʵ�ʴ�С
	dataSize = ftell( fpS );
	//��ԭ�ļ�ָ��λ��
	fseek( fpS, 0, SEEK_SET );

	//дRIFF
	fseek( fpD,RIFFID ,SEEK_SET );
	strcpy(word,"RIFF");
	fwrite( word, 1, 4, fpD );

	//.....дCHUNKE_SIZE = ��Ƶʵ�ʴ�С + �ļ�ͷ��44���ֽڣ� - 8
	DWORD_num = dataSize + 0x2c - 8;
	fseek( fpD, CHUNKE_SIZE, SEEK_SET );
	fwrite( &DWORD_num, 1, sizeof(DWORD), fpD );

	//дWAVE
	strcpy(word,"WAVE");
	fseek( fpD, WAVE, SEEK_SET );
	fwrite( word, 1, 4, fpD );

	//дFMT
	strcpy(word,"fmt ");
	fseek( fpD, FMT, SEEK_SET );
	fwrite( word, 1, 4, fpD );

	//дsize(16 or 18)ͨ��16
	DWORD_num = 16;
	fseek( fpD, FMT_CKSIZE, SEEK_SET );
	fwrite( &DWORD_num, 1, sizeof(DWORD), fpD );

	//дAudioFormat
	WORD_num = format_tag;	//pcm����
	fseek( fpD, AUDIO_FORMAT, SEEK_SET );
	fwrite( &WORD_num, 1, sizeof(WORD), fpD );

	//д������
	WORD_num = channel;
	fseek( fpD, CHANNELS, SEEK_SET );
	fwrite( &WORD_num, 1, sizeof(WORD), fpD );

	//д������
	DWORD_num = sample_rate;
	fseek( fpD, SAMPLE_RATE, SEEK_SET );
	fwrite( &DWORD_num, 1, sizeof(DWORD), fpD );

	//дbyte�� = ����Ƶ��*��Ƶͨ����*ÿ�β����õ�������λ��/8
	DWORD_num = sample_rate * channel * sample_bits / 8;
	fseek( fpD, BYTE_RATE, SEEK_SET );
	fwrite( &DWORD_num, 1, sizeof(DWORD), fpD );

	//д����� �����=ͨ����*ÿ�β����õ�������λ��/8
	WORD_num = channel * sample_bits / 8;
	fseek( fpD, BLOCKALIGN, SEEK_SET );
	fwrite( &WORD_num, 1, sizeof(WORD), fpD );

	//д��������λ
	WORD_num = sample_bits;
	fseek( fpD, BITS_PER_SAMPLE, SEEK_SET );
	fwrite( &WORD_num, 1, sizeof(WORD), fpD );

	//д"DATA"
	strcpy( word, "data" );
	fseek( fpD, DATAID, SEEK_SET );
	fwrite( word, 1, 4, fpD );

	//дʵ����Ƶ���ݴ�С
	DWORD_num = dataSize;
	fseek( fpD, DATA_SIZE, SEEK_SET );
	fwrite( &DWORD_num, 1, sizeof(DWORD), fpD );

	//дʵ�ʵ���Ƶ����

	//���ļ���ȡ��Ƶ����
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

