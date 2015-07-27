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

#include "CStereoFFT.h"
#define _USE_MATH_DEFINES
#include <math.h>

#include "WindowFunction.h"

#define FILENAME L"CStereoFFT.cpp"
#define FFTWF_FREE(x) if (x != nullptr) { fftwf_free(x); x = nullptr; }

//Zero-out everything (except for refcount)
CStereoFFT::CStereoFFT() :
m_RefCount(1),
m_NumSamples(0),
m_ComplexBuffer(nullptr),
m_ProcessBuffer(nullptr),
m_Window(nullptr),
m_Plan(nullptr),
m_LeftMix(nullptr),
m_RightMix(nullptr),
m_MidMix(nullptr),
m_SideMix(nullptr),
m_LeftTransform(nullptr),
m_RightTransform(nullptr),
m_MidTransform(nullptr),
m_SideTransform(nullptr),
m_WindowFunction(STEREO_FFT_WINDOW_FUNCTION_RECTANGLE)
{ }

CStereoFFT::~CStereoFFT() {
	//Free everything
	FFTWF_FREE(m_HistoryBuffer);
	FFTWF_FREE(m_ComplexBuffer);
	FFTWF_FREE(m_ProcessBuffer);
	FFTWF_FREE(m_Window);

	if (m_Plan != nullptr) {
		fftwf_destroy_plan(m_Plan);
		m_Plan = nullptr;
	}

	FFTWF_FREE(m_LeftMix);
	FFTWF_FREE(m_RightMix);
	FFTWF_FREE(m_MidMix);
	FFTWF_FREE(m_SideMix);

	FFTWF_FREE(m_LeftTransform);
	FFTWF_FREE(m_RightTransform);
	FFTWF_FREE(m_MidTransform);
	FFTWF_FREE(m_SideTransform);
}

HRESULT CStereoFFT::Initialize(const STEREO_FFT_DESC& Desc) {
	m_NumSamples = Desc.NumSamples;
	m_WindowFunction = Desc.WindowFunction;

	//Allocate all buffers
	m_HistoryBuffer = fftwf_alloc_real(m_NumSamples * 2);
	m_ProcessBuffer = fftwf_alloc_real(m_NumSamples);
	m_ComplexBuffer = fftwf_alloc_complex(m_NumSamples / 2 + 1);
	m_Window = fftwf_alloc_real(m_NumSamples);

	m_Plan = fftwf_plan_dft_r2c_1d (
		m_NumSamples,
		m_ProcessBuffer,
		m_ComplexBuffer,
		0
	);

	m_LeftMix = fftwf_alloc_real(m_NumSamples);
	m_RightMix = fftwf_alloc_real(m_NumSamples);
	m_MidMix = fftwf_alloc_real(m_NumSamples);
	m_SideMix = fftwf_alloc_real(m_NumSamples);

	m_LeftTransform = fftwf_alloc_real(m_NumSamples / 2);
	m_RightTransform = fftwf_alloc_real(m_NumSamples / 2);
	m_SideTransform = fftwf_alloc_real(m_NumSamples / 2);
	m_MidTransform = fftwf_alloc_real(m_NumSamples / 2);

	WindowFunction(m_WindowFunction, m_Window, m_NumSamples);

	return S_OK;
}

VOID CStereoFFT::Post(PFLOAT Buffer, UINT BufferFrames) {
	//Move old data to the beginning of the buffer,
	//add new data to the end of the buffer
	INT NumOld = max(0, m_NumSamples - BufferFrames);
	INT NumNew = min(m_NumSamples, BufferFrames);
	
	if (NumOld > 0) {
		memcpy(m_HistoryBuffer, m_HistoryBuffer + NumNew * 2, NumOld * 2 * sizeof(FLOAT));
	}

	if (NumNew > 0) {
		memcpy(m_HistoryBuffer + NumOld * 2, Buffer, NumNew * 2 * sizeof(FLOAT));
	}
}

VOID CStereoFFT::Process() {
	//Generate all mixes
	GenerateLeft();
	GenerateRight();
	GenerateMid();
	GenerateSide();

	//Process all mixes
	ProcessMix(m_LeftMix, m_LeftTransform, m_LeftDC);
	ProcessMix(m_RightMix, m_RightTransform, m_RightDC);
	ProcessMix(m_MidMix, m_MidTransform, m_MidDC);
	ProcessMix(m_SideMix, m_SideTransform, m_SideDC);
}

VOID CStereoFFT::SetWindowFunction(STEREO_FFT_WINDOW_FUNCTION WindowFunction) {
	if (m_WindowFunction != WindowFunction) {
		m_WindowFunction = WindowFunction;
		::WindowFunction(m_WindowFunction, m_Window, m_NumSamples);
	}
}

VOID CStereoFFT::GenerateLeft() {
	//Grab the left channel
	for (UINT n = 0; n < m_NumSamples; n++) {
		m_LeftMix[n] = m_HistoryBuffer[n * 2];
	}
}

VOID CStereoFFT::GenerateRight() {
	//Grab the right channel
	for (UINT n = 0; n < m_NumSamples; n++) {
		m_RightMix[n] = m_HistoryBuffer[n * 2 + 1];
	}
}

VOID CStereoFFT::GenerateMid() {
	//Generate the mid by averaging the two channels
	for (UINT n = 0; n < m_NumSamples; n++) {
		m_MidMix[n] = m_HistoryBuffer[n * 2];
		m_MidMix[n] += m_HistoryBuffer[n * 2 + 1];
		m_MidMix[n] /= 2.0f;
	}
}

VOID CStereoFFT::GenerateSide() {
	//Generate the side by comparing the two channels
	for (UINT n = 0; n < m_NumSamples; n++) {
		m_SideMix[n] = m_HistoryBuffer[n * 2];
		m_SideMix[n] -= m_HistoryBuffer[n * 2 + 1];
		m_SideMix[n] /= 2.0f;
	}
}

VOID CStereoFFT::ProcessMix(PFLOAT Mix, PFLOAT Transform, FLOAT& DC) {
	//Multiply mix and window, add to process buffer
	for (UINT n = 0; n < m_NumSamples; n++) {
		m_ProcessBuffer[n] = Mix[n];
		m_ProcessBuffer[n] *= m_Window[n];
	}

	//Execute the fft
	fftwf_execute(m_Plan);

	//Calculate the DC offset power from index 0
	DC = pow(m_ComplexBuffer[0][0], 2) + pow(m_ComplexBuffer[0][1], 2);

	//Calculate the rest of the FFT powers
	for (UINT n = 0; n < m_NumSamples / 2; n++) {
		Transform[n] = pow(m_ComplexBuffer[n + 1][0], 2) + pow(m_ComplexBuffer[n + 1][1], 2);
	}
}