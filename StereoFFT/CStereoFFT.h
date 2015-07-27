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

#include "StereoFFT.h"
#include "fftw3.h"
#include "QueryInterface.h"
#include <atlbase.h>

/* Final stereo fft implementation */
class CStereoFFT : public IStereoFFT {
public:
	CStereoFFT();

	~CStereoFFT();

	//IUnknown methods

	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject) final {
		QUERY_INTERFACE_CAST(IStereoFFT);
		QUERY_INTERFACE_CAST(IUnknown);
		QUERY_INTERFACE_FAIL();
	}

	ULONG STDMETHODCALLTYPE AddRef() {
		return ++m_RefCount;
	}

	ULONG STDMETHODCALLTYPE Release() {
		m_RefCount--;

		if (m_RefCount <= 0) {
			delete this;
			return 0;
		}

		return m_RefCount;
	}

	//IStereoFFT methods

	/* Adds new samples to the history buffer */
	VOID STDMETHODCALLTYPE Post(PFLOAT Buffer, UINT BufferFrames) final;

	/* Processes all FFTs */
	VOID STDMETHODCALLTYPE Process() final;

	/* Sets the window function used by the FFT. */
	VOID STDMETHODCALLTYPE SetWindowFunction(STEREO_FFT_WINDOW_FUNCTION WindowFunction) final;

	/* Returns the window function currently being used by the FFT. */
	STEREO_FFT_WINDOW_FUNCTION STDMETHODCALLTYPE GetWindowFunction() final {
		return m_WindowFunction;
	}

	/* Returns the number of samples in the history buffer */
	UINT STDMETHODCALLTYPE GetNumSamples() final {
		return m_NumSamples;
	}

	/* Returns the number of bins in an FFT */
	UINT STDMETHODCALLTYPE GetNumBins() final {
		return m_NumSamples / 2;
	}

	/* Returns the history buffer */
	CONST PFLOAT STDMETHODCALLTYPE GetHistoryBuffer() final {
		return m_HistoryBuffer;
	}

	/* Returns the left channel of the history buffer */
	CONST PFLOAT STDMETHODCALLTYPE GetLeftMix() final {
		return m_LeftMix;
	}

	/* Returns the right channel of the history buffer */
	CONST PFLOAT STDMETHODCALLTYPE GetRightMix() final {
		return m_RightMix;
	}

	/* Returns the mid of the history buffer */
	CONST PFLOAT STDMETHODCALLTYPE GetMidMix() final {
		return m_MidMix;
	}

	/* Returns the side of the history buffer */
	CONST PFLOAT STDMETHODCALLTYPE GetSideMix() final {
		return m_SideMix;
	}

	/* Returns the left channel transform */
	CONST PFLOAT STDMETHODCALLTYPE GetLeftTransform() final {
		return m_LeftTransform;
	}

	/* Returns the right channel transform */
	CONST PFLOAT STDMETHODCALLTYPE GetRightTransform() final {
		return m_RightTransform;
	}

	/* Returns the mid transform */
	CONST PFLOAT STDMETHODCALLTYPE GetMidTransform() final {
		return m_MidTransform;
	}

	/* Returns the side transform */
	CONST PFLOAT STDMETHODCALLTYPE GetSideTransform() final {
		return m_SideTransform;
	}

	/* Returns the left channel DC offset */
	FLOAT STDMETHODCALLTYPE GetLeftDCOffset() final {
		return m_LeftDC;
	}

	/* Returns the right channel DC offset */
	FLOAT STDMETHODCALLTYPE GetRightDCOffset() final {
		return m_RightDC;
	}

	/* Returns the mid DC offset */
	FLOAT STDMETHODCALLTYPE GetMidDCOffset() final {
		return m_MidDC;
	}

	/* Returns the side DC offset */
	FLOAT STDMETHODCALLTYPE GetSideDCOffset() final {
		return m_SideDC;
	}

	//New methods

	/* Initializes the plan and all buffers */
	HRESULT Initialize(const STEREO_FFT_DESC& Desc);

private:
	long m_RefCount; //Reference count of the object

	UINT m_NumSamples; //The number of samples to analyze

	PFLOAT m_HistoryBuffer; //The history buffer

	PFLOAT m_Window; //The pre-computed window function
	PFLOAT m_ProcessBuffer; //The input buffer used by fftw
	fftwf_complex* m_ComplexBuffer; //The output buffer used by fftw

	fftwf_plan m_Plan; //The fftw plan

	PFLOAT m_LeftMix; //The left channel
	PFLOAT m_RightMix; //The right channel
	PFLOAT m_MidMix; //The mid
	PFLOAT m_SideMix; //The side

	PFLOAT m_LeftTransform; //FFT of the left channel
	PFLOAT m_RightTransform; //FFT of the right channel
	PFLOAT m_MidTransform; //FFT of the mid
	PFLOAT m_SideTransform; //FFT of the side

	FLOAT m_LeftDC; //DC offset of the left channel
	FLOAT m_RightDC; //DC offset of the right channel
	FLOAT m_MidDC; //DC offset of the mid
	FLOAT m_SideDC; //DC offset of the side

	STEREO_FFT_WINDOW_FUNCTION m_WindowFunction;

	/* Generates the left mix from the history buffer. */
	VOID GenerateLeft();

	/* Generates the right mix from the history buffer. */
	VOID GenerateRight();

	/* Generates the mid mix from the history buffer. */
	VOID GenerateMid();

	/* Generates the side mix from the history buffer. */
	VOID GenerateSide();

	/* Processes the FFT of the given mix. */
	VOID ProcessMix(PFLOAT Mix, PFLOAT Transform, FLOAT& DC);
};