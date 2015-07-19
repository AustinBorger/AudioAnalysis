#include "AudioAnalysis.h"
#include "fftw3.h"
#include "QueryInterface.h"
#include <atlbase.h>

#define IDEAL_BUFFER_SIZE 2048

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
		return IDEAL_BUFFER_SIZE;
	}

	UINT STDMETHODCALLTYPE GetNumBins() final {
		return IDEAL_BUFFER_SIZE / 2;
	}

	CONST PFLOAT STDMETHODCALLTYPE GetMonoMix() final {
		return m_MonoMix;
	}

	CONST PFLOAT STDMETHODCALLTYPE GetLeftMix() final {
		return m_LeftMix;
	}

	CONST PFLOAT STDMETHODCALLTYPE GetRightMix() final {
		return m_RightMix;
	}

	CONST PFLOAT STDMETHODCALLTYPE GetMiddleMix() final {
		return m_MiddleMix;
	}

	CONST PFLOAT STDMETHODCALLTYPE GetSideMix() final {
		return m_SideMix;
	}

	CONST PFLOAT STDMETHODCALLTYPE GetMonoTransform() final {
		return m_MonoTransform;
	}

	CONST PFLOAT STDMETHODCALLTYPE GetLeftTransform() final {
		return m_LeftTransform;
	}

	CONST PFLOAT STDMETHODCALLTYPE GetRightTransform() final {
		return m_RightTransform;
	}

	CONST PFLOAT STDMETHODCALLTYPE GetMiddleTransform() final {
		return m_MiddleTransform;
	}

	CONST PFLOAT STDMETHODCALLTYPE GetSideTransform() final {
		return m_SideTransform;
	}

	HRESULT Initialize(const AUDIO_ANALYSIS_DESC& Desc, CComPtr<IAudioAnalysisCallback> Callback);

private:
	long m_RefCount;

	CComPtr<IAudioAnalysisCallback> m_Callback;

	PFLOAT m_HistoryBuffer;

	PFLOAT m_Window;
	PFLOAT m_ProcessBuffer;
	fftwf_complex* m_ComplexBuffer;

	fftwf_plan m_Plan;

	PFLOAT m_MonoMix;
	PFLOAT m_LeftMix;
	PFLOAT m_RightMix;
	PFLOAT m_MiddleMix;
	PFLOAT m_SideMix;

	PFLOAT m_MonoTransform;
	PFLOAT m_LeftTransform;
	PFLOAT m_RightTransform;
	PFLOAT m_MiddleTransform;
	PFLOAT m_SideTransform;

	VOID GenerateMono();

	VOID GenerateLeft();

	VOID GenerateRight();

	VOID GenerateMiddle();

	VOID GenerateSide();

	VOID ProcessMix(PFLOAT Mix, PFLOAT Transform);
};