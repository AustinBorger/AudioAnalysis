#include "AudioAnalysis.h"
#include "fftw3.h"
#include "QueryInterface.h"
#include <atlbase.h>

class CAudioAnalysis : public IAudioAnalysis {
public:
	CAudioAnalysis();

	~CAudioAnalysis();

	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject) final {
		QUERY_INTERFACE_CAST(IAudioAnalysis);
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

	VOID STDMETHODCALLTYPE Post(PFLOAT Buffer, UINT BufferFrames) final;

	VOID STDMETHODCALLTYPE Process() final;

	UINT STDMETHODCALLTYPE GetNumSamples() final {
		return m_NumSamples;
	}

	UINT STDMETHODCALLTYPE GetNumBins() final {
		return m_NumSamples;
	}

	CONST PFLOAT STDMETHODCALLTYPE GetLeftMix() final {
		return m_LeftMix;
	}

	CONST PFLOAT STDMETHODCALLTYPE GetRightMix() final {
		return m_RightMix;
	}

	CONST PFLOAT STDMETHODCALLTYPE GetMidMix() final {
		return m_MidMix;
	}

	CONST PFLOAT STDMETHODCALLTYPE GetSideMix() final {
		return m_SideMix;
	}

	CONST PFLOAT STDMETHODCALLTYPE GetLeftTransform() final {
		return m_LeftTransform;
	}

	CONST PFLOAT STDMETHODCALLTYPE GetRightTransform() final {
		return m_RightTransform;
	}

	CONST PFLOAT STDMETHODCALLTYPE GetMidTransform() final {
		return m_MidTransform;
	}

	CONST PFLOAT STDMETHODCALLTYPE GetSideTransform() final {
		return m_SideTransform;
	}

	FLOAT STDMETHODCALLTYPE GetLeftDCOffset() final {
		return m_LeftDC;
	}

	FLOAT STDMETHODCALLTYPE GetRightDCOffset() final {
		return m_RightDC;
	}

	FLOAT STDMETHODCALLTYPE GetMidDCOffset() final {
		return m_MidDC;
	}

	FLOAT STDMETHODCALLTYPE GetSideDCOffset() final {
		return m_SideDC;
	}

	HRESULT Initialize(const AUDIO_ANALYSIS_DESC& Desc, CComPtr<IAudioAnalysisCallback> Callback);

private:
	long m_RefCount;

	UINT m_NumSamples;

	CComPtr<IAudioAnalysisCallback> m_Callback;

	PFLOAT m_HistoryBuffer;

	PFLOAT m_Window;
	PFLOAT m_ProcessBuffer;
	fftwf_complex* m_ComplexBuffer;

	fftwf_plan m_Plan;

	PFLOAT m_LeftMix;
	PFLOAT m_RightMix;
	PFLOAT m_MidMix;
	PFLOAT m_SideMix;

	PFLOAT m_LeftTransform;
	PFLOAT m_RightTransform;
	PFLOAT m_MidTransform;
	PFLOAT m_SideTransform;

	FLOAT m_LeftDC;
	FLOAT m_RightDC;
	FLOAT m_MidDC;
	FLOAT m_SideDC;

	VOID GenerateLeft();

	VOID GenerateRight();

	VOID GenerateMid();

	VOID GenerateSide();

	VOID ProcessMix(PFLOAT Mix, PFLOAT Transform, FLOAT& DC);
};