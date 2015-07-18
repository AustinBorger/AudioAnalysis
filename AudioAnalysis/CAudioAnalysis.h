#include "AudioAnalysis.h"
#include "fftw3.h"
#include "QueryInterface.h"

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
		return InterlockedIncrement(&m_RefCount);
	}

	ULONG STDMETHODCALLTYPE Release() {
		long refcount = InterlockedDecrement(&m_RefCount);

		if (refcount <= 0) {
			delete this;
			return 0;
		}

		return refcount;
	}

	HRESULT Initialize(const AUDIO_ANALYSIS_DESC& Desc);

private:
	long m_RefCount;
};