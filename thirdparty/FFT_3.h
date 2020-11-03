#pragma once
/*
	Fast Fourier Transformation
	====================================================
	Coded by Miroslav Voinarovsky, 2002
	This source is freeware.
*/

struct Complex2;
struct ShortComplex;

/*
  Fast Fourier Transformation
  x: x - array of items
  N: N - number of items in array
  complement: false - normal (direct) transformation, true - reverse transformation
*/
extern void universal_fft(ShortComplex *x, int N, bool complement);

struct ShortComplex
{
	float re, im;
	inline void operator=(const Complex2 &y);
};

struct Complex2
{
	double re, im;
	inline void operator= (const Complex2 &y);
	inline void operator= (const ShortComplex &y);
};


inline void ShortComplex::operator=(const Complex2 &y) { re = (double)y.re; im = (double)y.im; }
inline void Complex2::operator= (const Complex2 &y) { re = y.re; im = y.im; }
inline void Complex2::operator= (const ShortComplex &y) { re = y.re; im = y.im; }

