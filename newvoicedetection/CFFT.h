#ifndef CFFT_H
#define CFFT_H

#include "math.h"

#define PI 3.1415926535897932384626433832795028841971

typedef struct complex //��������
{
	float real;		//ʵ��
	float imag;		//�鲿
}complex;

class CFFT
{
private:
	CFFT(const CFFT& cp){}
	CFFT& operator = (const CFFT& cp){}

	void conjugate_complex(int n,complex in[],complex out[]);
	void c_plus(complex a,complex b,complex *c);//������
	void c_mul(complex a,complex b,complex *c) ;//������
	void c_sub(complex a,complex b,complex *c);	//��������
	void c_div(complex a,complex b,complex *c);	//��������
	void c_abs(complex f[],float out[],int n);//��������ȡģ
	void Wn_i(int n,int i,complex *Wn,char flag);
public:
	CFFT();
	~CFFT();
	void fft(int N,complex f[]);//����Ҷ�任 ���Ҳ��������f��
	void ifft(int N,complex f[]); // ����Ҷ��任	

};

#endif

