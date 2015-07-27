/*
** Copyright (C) 2015 Austin Borger <aaborger@gmail.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
** API documentation is available here:
**		https://github.com/AustinBorger/StereoFFT
*/

#include "WindowFunction.h"
#define _USE_MATH_DEFINES
#include <math.h>

VOID Rectangle(PFLOAT W, UINT N);
VOID Bartlett(PFLOAT W, UINT N);
VOID Welch(PFLOAT W, UINT N);
VOID Hanning(PFLOAT W, UINT N);
VOID Hamming(PFLOAT W, UINT N);
VOID Blackman(PFLOAT W, UINT N);
VOID Nuttall(PFLOAT W, UINT N);
VOID BlackmanHarris(PFLOAT W, UINT N);
VOID BlackmanNuttall(PFLOAT W, UINT N);
VOID FlatTop(PFLOAT W, UINT N);
VOID Cosine(PFLOAT W, UINT N);
VOID Gaussian(PFLOAT W, UINT N);
VOID ConfinedGaussian(PFLOAT W, UINT N);
VOID Tukey(PFLOAT W, UINT N);
VOID PlanckTaper(PFLOAT W, UINT N);
VOID DolphChebyshev(PFLOAT W, UINT N);

VOID WindowFunction(STEREO_FFT_WINDOW_FUNCTION Function, PFLOAT Buffer, UINT N) {
	switch (Function) {
	case STEREO_FFT_WINDOW_FUNCTION_RECTANGLE:
		return Rectangle(Buffer, N);
	case STEREO_FFT_WINDOW_FUNCTION_BARTLETT:
		return Bartlett(Buffer, N);
	case STEREO_FFT_WINDOW_FUNCTION_WELCH:
		return Welch(Buffer, N);
	case STEREO_FFT_WINDOW_FUNCTION_HANNING:
		return Hanning(Buffer, N);
	case STEREO_FFT_WINDOW_FUNCTION_HAMMING:
		return Hamming(Buffer, N);
	case STEREO_FFT_WINDOW_FUNCTION_BLACKMAN:
		return Blackman(Buffer, N);
	case STEREO_FFT_WINDOW_FUNCTION_NUTTALL:
		return Nuttall(Buffer, N);
	case STEREO_FFT_WINDOW_FUNCTION_BLACKMAN_NUTTALL:
		return BlackmanNuttall(Buffer, N);
	case STEREO_FFT_WINDOW_FUNCTION_BLACKMAN_HARRIS:
		return BlackmanHarris(Buffer, N);
	case STEREO_FFT_WINDOW_FUNCTION_FLAT_TOP:
		return FlatTop(Buffer, N);
	case STEREO_FFT_WINDOW_FUNCTION_COSINE:
		return Cosine(Buffer, N);
	case STEREO_FFT_WINDOW_FUNCTION_GAUSSIAN:
		return Gaussian(Buffer, N);
	case STEREO_FFT_WINDOW_FUNCTION_CONFINED_GAUSSIAN:
		return ConfinedGaussian(Buffer, N);
	case STEREO_FFT_WINDOW_FUNCTION_TUKEY:
		return Tukey(Buffer, N);
	case STEREO_FFT_WINDOW_FUNCTION_PLANCK_TAPER:
		return PlanckTaper(Buffer, N);
	case STEREO_FFT_WINDOW_FUNCTION_DOLPH_CHEBYSHEV:
		return DolphChebyshev(Buffer, N);
	default:
		return Rectangle(Buffer, N);
	}
}

VOID Rectangle(PFLOAT W, UINT N) {
	for (UINT n = 0; n < N; n++) {
		W[n] = 1.0f;
	}
}

VOID Bartlett(PFLOAT W, UINT N) {
	float L = float(N - 1);

	for (UINT n = 0; n < N; n++) {
		W[n] = 1.0f - abs((float(n) - (L / 2)) / (L / 2));
	}
}

VOID Welch(PFLOAT W, UINT N) {
	float L = float(N - 1);

	for (UINT n = 0; n < N; n++) {
		W[n] = 1.0f - pow((float(n) - (L / 2)) / (L / 2), 2);
	}
}

VOID Hanning(PFLOAT W, UINT N) {
	float L = float(N - 1);

	for (UINT n = 0; n < N; n++) {
		W[n] = 0.5f * (1.0f - cos((2.0f * float(M_PI) * n) / L));
	}
}

VOID Hamming(PFLOAT W, UINT N) {
	float L = float(N - 1);
	float a = 0.54f;
	float B = 1.0f - a;

	for (UINT n = 0; n < N; n++) {
		W[n] = a - B * cos((2.0f * float(M_PI) * n) / L);
	}
}

VOID Blackman(PFLOAT W, UINT N) {
	float a0 = 0.426591f;
	float a1 = 0.496561f;
	float a2 = 0.076849f;
	float L = float(N - 1);

	for (UINT n = 0; n < N; n++) {
		W[n] = a0;
		W[n] -= a1 * cos((1.0f * 2.0f * float(M_PI) * n) / L);
		W[n] += a2 * cos((2.0f * 2.0f * float(M_PI) * n) / L);
	}
}

VOID Nuttall(PFLOAT W, UINT N) {
	float a0 = 0.355768f;
	float a1 = 0.487396f;
	float a2 = 0.144232f;
	float a3 = 0.012604f;
	float L = float(N - 1);

	for (UINT n = 0; n < N; n++) {
		W[n] = a0;
		W[n] -= a1 * cos((1.0f * 2.0f * float(M_PI) * n) / L);
		W[n] += a2 * cos((2.0f * 2.0f * float(M_PI) * n) / L);
		W[n] -= a3 * cos((3.0f * 2.0f * float(M_PI) * n) / L);
	}
}

VOID BlackmanHarris(PFLOAT W, UINT N) {
	float a0 = 0.35875f;
	float a1 = 0.48829f;
	float a2 = 0.14128f;
	float a3 = 0.01168f;
	float L = float(N - 1);

	for (UINT n = 0; n < N; n++) {
		W[n] = a0;
		W[n] -= a1 * cos((1.0f * 2.0f * float(M_PI) * n) / L);
		W[n] += a2 * cos((2.0f * 2.0f * float(M_PI) * n) / L);
		W[n] -= a3 * cos((3.0f * 2.0f * float(M_PI) * n) / L);
	}
}

VOID BlackmanNuttall(PFLOAT W, UINT N) {
	float a0 = 0.3635819f;
	float a1 = 0.4891775f;
	float a2 = 0.1365995f;
	float a3 = 0.0105411f;
	float L = float(N - 1);

	for (UINT n = 0; n < N; n++) {
		W[n] = a0;
		W[n] -= a1 * cos((1.0f * 2.0f * float(M_PI) * n) / L);
		W[n] += a2 * cos((2.0f * 2.0f * float(M_PI) * n) / L);
		W[n] -= a3 * cos((3.0f * 2.0f * float(M_PI) * n) / L);
	}
}

VOID FlatTop(PFLOAT W, UINT N) {
	float a0 = 1.000f;
	float a1 = 1.930f;
	float a2 = 1.290f;
	float a3 = 0.388f;
	float a4 = 0.028f;
	float L = float(N - 1);

	for (UINT n = 0; n < N; n++) {
		W[n] = a0;
		W[n] -= a1 * cos((1.0f * 2.0f * float(M_PI) * n) / L);
		W[n] += a2 * cos((2.0f * 2.0f * float(M_PI) * n) / L);
		W[n] -= a3 * cos((3.0f * 2.0f * float(M_PI) * n) / L);
		W[n] += a4 * cos((4.0f * 2.0f * float(M_PI) * n) / L);
	}
}

VOID Cosine(PFLOAT W, UINT N) {
	float L = float(N - 1);

	for (UINT n = 0; n < N; n++) {
		W[n] = sin(float(M_PI) * n / L);
	}
}

VOID Gaussian(PFLOAT W, UINT N) {
	float L = float(N - 1);
	float sigma = 0.5f;

	for (UINT n = 0; n < N; n++) {
		W[n] = exp(-0.5f * pow((float(n) - L / 2) / (sigma * L / 2), 2));
	}
}

VOID ConfinedGaussian(PFLOAT W, UINT N) {
	float fN = float(N);
	float sigma = fN * 0.1f;
	float L = float(N - 1);

	static const auto G = [&](float x) {
		return exp(-pow((x - L / 2) / (2 * sigma), 2));
	};

	float numerator = 0.0f;
	float denominator = 0.0f;
	float fn = 0.0f;

	for (UINT n = 0; n < N; n++) {
		fn = float(n);
		numerator = G(-0.5f) * (G(fn + fN) + G(fn - fN));
		denominator = G(-0.5f + fN) + G(-0.5f - fN);
		W[n] = G(fn) - numerator / denominator;
	}
}

VOID Tukey(PFLOAT W, UINT N) {
	float a = 0.5f;
	float L = float(N - 1);

	for (UINT n = 0; n < N; n++) {
		if (n <= a * L / 2.0f) {
			W[n] = 0.5f * (1.0f + cos(float(M_PI) * (2.0f * float(n) / (a * L) - 1.0f)));
		} else if (n <= L * (1.0f - a / 2.0f)) {
			W[n] = 1.0f;
		} else {
			W[n] = 0.5f * (1.0f + cos(float(M_PI) * (2.0f * float(n) / (a * L) - (2.0f / a) + 1.0f)));
		}
	}
}

VOID PlanckTaper(PFLOAT W, UINT N) {
	float E = 0.1f;
	float L = float(N - 1);

	static const auto Z = [&](float n, float sign) {
		return 2.0f * E * ((1.0f / (1.0f + sign * 2.0f * n / L)) + (1.0f / (1.0f - 2.0f * E + sign * 2.0f * n / L)));
	};

	for (UINT n = 0; n < N; n++) {
		if (n <= E * L) {
			W[n] = 1.0f / (exp(Z(float(n), 1.0f)) + 1.0f);
		} else if (n < (1.0f - E) * L) {
			W[n] = 1.0f;
		} else {
			W[n] = 1.0f / (exp(Z(float(N), -1.0f)) + 1.0f);
		}
	}
}

//The code below was taken from http://practicalcryptography.com/miscellaneous/machine-learning/implementing-dolph-chebyshev-window/

/**************************************************************************
This function computes the chebyshev polyomial T_n(x)
***************************************************************************/
double cheby_poly(int n, double x) {
	double res;
	if (fabs(x) <= 1) res = cos(n*acos(x));
	else              res = cosh(n*acosh(x));
	return res;
}

/***************************************************************************
calculate a chebyshev window of size N, store coeffs in out as in Antoniou
-out should be array of size N
-atten is the required sidelobe attenuation (e.g. if you want -60dB atten, use '60')
***************************************************************************/
void cheby_win(float *out, int N, float atten) {
	int nn, i;
	double M, n, sum = 0, max = 0;
	double tg = pow(10, atten / 20);  /* 1/r term [2], 10^gamma [2] */
	double x0 = cosh((1.0 / (N - 1))*acosh(tg));
	M = (N - 1) / 2;
	if (N % 2 == 0) M = M + 0.5; /* handle even length windows */
	for (nn = 0; nn<(N / 2 + 1); nn++) {
		n = nn - M;
		sum = 0;
		for (i = 1; i <= M; i++) {
			sum += cheby_poly(N - 1, x0*cos(M_PI*i / N))*cos(2.0*n*M_PI*i / N);
		}
		out[nn] = float(tg + 2 * sum);
		out[N - nn - 1] = out[nn];
		if (out[nn]>max)max = out[nn];
	}
	for (nn = 0; nn<N; nn++) out[nn] /= float(max); /* normalise everything */
	return;
}

VOID DolphChebyshev(PFLOAT W, UINT N) {
	cheby_win(W, N, 120.0f);
}