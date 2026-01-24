#include "DSAudioPlayer.h"
#include <windows.h>
#include <dshow.h>

#pragma comment(lib, "strmiids.lib")


class DSAudioPlayer::DSAudioPlayerPrivate
{
private:
    IGraphBuilder* pGraph = NULL;
    IMediaControl* pControl = NULL;
    IMediaSeeking* pSeek = NULL;

public:
    DSAudioPlayerPrivate()
    {
        CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    }

    ~DSAudioPlayerPrivate()
    {
        Cleanup();
        CoUninitialize();
    }

    HRESULT LoadFile(LPCWSTR filePath)
    {
        Cleanup();
        HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&pGraph);
        if (FAILED(hr)) return hr;
        hr = pGraph->QueryInterface(IID_IMediaControl, (void**)&pControl);
        if (FAILED(hr)) return hr;
        hr = pGraph->QueryInterface(IID_IMediaSeeking, (void**)&pSeek);
        return pGraph->RenderFile(filePath, NULL);
    }

    HRESULT Play()
    {
        if (!pControl) return E_POINTER;
        return pControl->Run();
    }

    HRESULT Pause()
    {
        if (!pControl) return E_POINTER;
        return pControl->Pause();
    }

    HRESULT Stop()
    {
        if (!pControl) return E_POINTER;
        return pControl->Stop();
    }

    HRESULT GetDuration(LONGLONG* pDuration)
    {
        if (!pSeek) return E_POINTER;
        return pSeek->GetDuration(pDuration);
    }

    void Cleanup()
    {
        if (pControl) pControl->Release();
        if (pSeek) pSeek->Release();
        if (pGraph) pGraph->Release();

        pControl = NULL;
        pSeek = NULL;
        pGraph = NULL;
    }
};

DSAudioPlayer::DSAudioPlayer() :
    pimpl(new DSAudioPlayerPrivate)
{

}

DSAudioPlayer::~DSAudioPlayer()
{
    delete pimpl, pimpl = nullptr;
}

bool DSAudioPlayer::LoadFile(const wchar_t *filePath)
{
    return SUCCEEDED(pimpl->LoadFile(filePath));
}

bool DSAudioPlayer::GetDuration(long long *pDuration)
{
    return SUCCEEDED(pimpl->GetDuration(pDuration));
}

bool DSAudioPlayer::Play()
{
    return SUCCEEDED(pimpl->Play());
}

bool DSAudioPlayer::Pause()
{
    return SUCCEEDED(pimpl->Pause());
}

bool DSAudioPlayer::Stop()
{
    return SUCCEEDED(pimpl->Stop());
}

