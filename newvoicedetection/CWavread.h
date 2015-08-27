#pragma  once



#include <iostream> 
#include <fstream> 
#include <string.h> 
#include<math.h> 
#include<cmath> 
#include<stdlib.h> 
#include <bitset> 
#include <iomanip> 


struct wav_struct 
{ 
	const char* filename;
	unsigned long file_size;        //�ļ���С     
	unsigned short channel;         //ͨ����     
	unsigned long frequency;        //����Ƶ��     
	unsigned long Bps;              //Byte��     
	unsigned short sample_num_bit;  //һ��������λ��     
	unsigned long data_size;        //���ݴ�С     
	unsigned char *data;            //��Ƶ���� ,����Ҫ����ʲô�Ϳ�����λ���ˣ�������ֻ�ǵ����ĸ������� 
};


class CWavread
{
private:
	CWavread(const CWavread& cp){}
	CWavread& operator = (const CWavread& cp){}

	int hex_char_value(char c);
	int hex_to_decimal(char *szHex);
public:
	CWavread();
	~CWavread();
	wav_struct wavread_head(const char* filename);
	float* wavread_data1(wav_struct wav);
	float* wavread_data2(wav_struct wav);
	float** wavread_data3(wav_struct wav);
	float* wavresample(float * DataIn,int p,int q);
};