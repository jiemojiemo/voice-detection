#ifndef PCM2WAV_H
#define PCM2WAV_H

#include <string>
using std::string;
//----------------���¶���һЩ��ַ

//RIFF,��Դ������־
#define RIFFID			0x00
//����һ����ַ��ʼ���ļ�β�����ֽ���
#define CHUNKE_SIZE		0x04
//WAVE, ����WAV�ļ���ʽ
#define WAVE			0x08
//FMT,���θ�ʽ��־
#define FMT				0x0c
//16����18��18��ʾ�ж�����Ϣ
#define FMT_CKSIZE		0x10
//Ϊ1ʱ��ʾ����PCM���룬����1ʱ��ʾ��ѹ���ı���
//WAVE_FORMAT_IEEE_FLOATΪ32bit float׼��
#define AUDIO_FORMAT	0x14
//������
#define CHANNELS		0x16
//������
#define SAMPLE_RATE		0x18
//BYTE��=����Ƶ��*��Ƶͨ����*ÿ�β����õ�������λ��/8
#define BYTE_RATE		0x1c
//�����=ͨ����*ÿ�β����õ�������λ��/8
#define BLOCKALIGN		0x20
//��������λ��
#define BITS_PER_SAMPLE	0x22
//data, һ����ʾ����
#define DATAID			0x24
//Wav�ļ�ʵ����Ƶ������ռ��С
#define DATA_SIZE		0x28
//ʵ�����ݿ�ʼ��
#define DATA_BEGIN		0x2c

struct Pcm2WavInfo{
	unsigned int channels;		//������
	unsigned int sampleRate;		//������
	unsigned int sampleBits;		//������λ��
	unsigned int formatTag;		//��ʽ��־
									//3 Ϊ 32λfloat
									//1 λ 16λpcm
	string		 inFileName;
	string		 outFileName;
};


class CPcm2Wav{
private:
	CPcm2Wav( const CPcm2Wav& cp ){}
	CPcm2Wav& operator=( const CPcm2Wav& cp ){}
	int _Pcm2Wav( string inFile, string outFile, int channels, int sam_rate, int sam_bits, int fmt_tag );
public:
	CPcm2Wav();

	/**
	* @brief : ����infomation���з�װ 
	*/
	int Pcm2Wav( const Pcm2WavInfo& info );

	int Pcm2Wav( const float* buffer, unsigned long bufferSize,const Pcm2WavInfo& info );
};

#endif