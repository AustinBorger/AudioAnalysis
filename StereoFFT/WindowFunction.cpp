#include "WindowFunction.h"
#define _USE_MATH_DEFINES
#include <math.h>

VOID Rectangle(PFLOAT W, UINT N);
VOID Triangle(PFLOAT W, UINT N);
VOID Welch(PFLOAT W, UINT N);
VOID Hanning(PFLOAT W, UINT N);
VOID Hamming(PFLOAT W, UINT N);
VOID Blackman(PFLOAT W, UINT N);
VOID Nuttall(PFLOAT W, UINT N);
VOID BlackmanHarris(PFLOAT W, UINT N);
VOID BlackmanNuttall(PFLOAT W, UINT N);
VOID FlatTop(PFLOAT W, UINT N);
VOID Cosine(PFLOAT W, UINT N);

VOID WindowFunction(STEREO_FFT_WINDOW_FUNCTION Function, PFLOAT Buffer, UINT N) {
	switch (Function) {
	case STEREO_FFT_WINDOW_FUNCTION_RECTANGLE:
		return Rectangle(Buffer, N);
	case STEREO_FFT_WINDOW_FUNCTION_TRIANGLE:
		return Triangle(Buffer, N);
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
	default:
		return Rectangle(Buffer, N);
	}
}

VOID Rectangle(PFLOAT W, UINT N) {
	for (UINT n = 0; n < N; n++) {
		W[n] = 1.0f;
	}
}

VOID Triangle(PFLOAT W, UINT N) {
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