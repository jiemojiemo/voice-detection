#ifndef CFFT_H
#define CFFT_H

#include "math.h"

#define PI 3.1415926535897932384626433832795028841971

typedef struct complex //复数类型
{
	float real;		//实部
	float imag;		//虚部
}complex;

class CFFT
{
private:
	CFFT(const CFFT& cp){}
	CFFT& operator = (const CFFT& cp){}
public:
	void conjugate_complex(int n,complex in[],complex out[]);//取共轭
	void c_plus(complex a,complex b,complex *result);//复数加
	void c_mul(complex a,complex b,complex *result) ;//复数乘
	void c_sub(complex a,complex b,complex *result);	//复数减法
	void c_div(complex a,complex b,complex *result);	//复数除法
	void c_abs(complex f[],float out[],int size);//复数数组取模
	void Wn_i(int n,int i,complex *Wn,char flag);//
public:
	CFFT();
	~CFFT();
	void fft(int N,complex f[]);//傅立叶变换 输出也存在数组f中
	void ifft(int N,complex f[]); // 傅里叶逆变换	
	void ifft(int N, float in[]);
};

#endif

