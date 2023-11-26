/*
BSD 2-Clause License

Copyright (c) 2023, badcoiq
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "badcoiq.h"

#include "badcoiq/sound/bqSoundSystem.h"

#include "bqSoundSystemImpl.h"

#include "../framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;


#include <strsafe.h>


bqSoundSystemImpl::bqSoundSystemImpl()
{
}

bqSoundSystemImpl::~bqSoundSystemImpl()
{
    bqLog::PrintInfo("Shutdown Sound System\n");
    if (m_WASAPIrenderer) delete m_WASAPIrenderer;
    if (m_device) m_device->Release();
}

//
//  Retrieves the device friendly name for a particular device in a device collection.
//
LPWSTR GetDeviceName(IMMDeviceCollection* DeviceCollection, UINT DeviceIndex)
{
    IMMDevice* device;
    LPWSTR deviceId;
    HRESULT hr;

    hr = DeviceCollection->Item(DeviceIndex, &device);
    if (FAILED(hr))
    {
        printf("Unable to get device %d: %x\n", DeviceIndex, hr);
        return NULL;
    }
    hr = device->GetId(&deviceId);
    if (FAILED(hr))
    {
        printf("Unable to get device %d id: %x\n", DeviceIndex, hr);
        return NULL;
    }

    IPropertyStore* propertyStore;
    hr = device->OpenPropertyStore(STGM_READ, &propertyStore);
    if (device)
    {
        device->Release();
        device = 0;
    }
    if (FAILED(hr))
    {
        printf("Unable to open device %d property store: %x\n", DeviceIndex, hr);
        return NULL;
    }

    PROPVARIANT friendlyName;
    PropVariantInit(&friendlyName);
    hr = propertyStore->GetValue(PKEY_Device_FriendlyName, &friendlyName);
    if (propertyStore)
    {
        propertyStore->Release();
        propertyStore = 0;
    }

    if (FAILED(hr))
    {
        printf("Unable to retrieve friendly name for device %d : %x\n", DeviceIndex, hr);
        return NULL;
    }

    wchar_t deviceName[128];
    hr = StringCbPrintf(deviceName, sizeof(deviceName), L"%s (%s)", friendlyName.vt != VT_LPWSTR ? L"Unknown" : friendlyName.pwszVal, deviceId);
    if (FAILED(hr))
    {
        printf("Unable to format friendly name for device %d : %x\n", DeviceIndex, hr);
        return NULL;
    }

    PropVariantClear(&friendlyName);
    CoTaskMemFree(deviceId);

    wchar_t* returnValue = _wcsdup(deviceName);
    if (returnValue == NULL)
    {
        printf("Unable to allocate buffer for return\n");
        return NULL;
    }
    return returnValue;
}

bool bqSoundSystemImpl::Init()
{
    bool retValue = true;
    IMMDeviceEnumerator* deviceEnumerator = NULL;
    IMMDeviceCollection* deviceCollection = NULL;
    bqLog::PrintInfo("Init sound system...\n");

    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));
    if (FAILED(hr))
    {
        bqLog::PrintError("Unable to instantiate device enumerator: %x\n", hr);
        retValue = false;
    }


    hr = deviceEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &deviceCollection);
    if (FAILED(hr))
    {
        bqLog::PrintError("Unable to retrieve device collection: %x\n", hr);
        retValue = false;
    }

    UINT deviceCount;
    hr = deviceCollection->GetCount(&deviceCount);
    if (FAILED(hr))
    {
        bqLog::PrintError("Unable to get device collection length: %x\n", hr);
        retValue = false;
    }

    bqLog::PrintInfo("Output devices:\n");
    for (UINT i = 0; i < deviceCount; i += 1)
    {
        LPWSTR deviceName;

        deviceName = GetDeviceName(deviceCollection, i);
        if (deviceName == NULL)
        {
            retValue = false;
        }
        else
        {
            bqLog::PrintInfo("    %d:  %S\n", i + 1, deviceName);
            free(deviceName);
        }
    }

    hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &m_device);
    if (FAILED(hr))
    {
        retValue = false;
        bqLog::PrintInfo("GetDefaultAudioEndpoint\n");
    }

    if (deviceCollection) deviceCollection->Release();
    if (deviceEnumerator) deviceEnumerator->Release();

    if (retValue)
    {
        m_WASAPIrenderer = new bqWASAPIRenderer(m_device);
        retValue = m_WASAPIrenderer->Initialize(10);
        if (!retValue)
        {
            delete m_WASAPIrenderer;
            m_WASAPIrenderer = 0;
        }
    }

    if (!retValue)
    {
        if (m_device)
        {
            m_device->Release();
            m_device = 0;
        }
    }

	return retValue;
}

bqSoundObject* bqSoundSystemImpl::SummonObject(const char* fn)
{
	BQ_ASSERT_ST(fn);
	return 0;
}

bqSoundObject* bqSoundSystemImpl::SummonObject(bqSound* sound)
{
	BQ_ASSERT_ST(sound);
	return 0;
}

bqSoundStreamObject* bqSoundSystemImpl::SummonStreamObject(const char* fn)
{
	BQ_ASSERT_ST(fn);
	return 0;
}

bqSoundStreamObject* bqSoundSystemImpl::SummonStreamObject(const bqStringA& str)
{
	return SummonStreamObject(str.c_str());
}

bqWASAPIRenderer::bqWASAPIRenderer(IMMDevice* Endpoint)
{
    m_endpoint = Endpoint;
    m_endpoint->AddRef();
}

bqWASAPIRenderer::~bqWASAPIRenderer()
{
    Shutdown();
}

bool bqWASAPIRenderer::Initialize(UINT32 EngineLatency)
{
    HRESULT hr = m_endpoint->Activate(__uuidof(IAudioClient), 
        CLSCTX_INPROC_SERVER, NULL, reinterpret_cast<void**>(&_AudioClient));
    if (FAILED(hr))
    {
        bqLog::PrintError("Unable to activate audio client: %x.\n", hr);
        return false;
    }
    hr = _AudioClient->GetMixFormat(&_MixFormat);
    if (FAILED(hr))
    {
        bqLog::PrintError("Unable to get mix format on audio client: %x.\n", hr);
        return false;
    }
    _FrameSize = _MixFormat->nBlockAlign;
 
        //
        //  If the mix format is a float format, just try to convert the format to PCM.
        //
    if (_MixFormat->wFormatTag == WAVE_FORMAT_PCM ||
        _MixFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
        reinterpret_cast<WAVEFORMATEXTENSIBLE*>(_MixFormat)->SubFormat == KSDATAFORMAT_SUBTYPE_PCM)
    {
        if (_MixFormat->wBitsPerSample == 16)
        {
            _RenderSampleType = SampleType16BitPCM;
        }
        else
        {
            bqLog::PrintError("Unknown PCM integer sample type\n");
            return false;
        }
    }
    else if (_MixFormat->wFormatTag == WAVE_FORMAT_IEEE_FLOAT ||
        (_MixFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
            reinterpret_cast<WAVEFORMATEXTENSIBLE*>(_MixFormat)->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT))
    {
        _RenderSampleType = SampleTypeFloat;
    }
    else
    {
        bqLog::PrintError("unrecognized device format.\n");
        return false;
    }

    int PERIODS_PER_BUFFER = 4;
    REFERENCE_TIME bufferDuration = EngineLatency * 10000 * PERIODS_PER_BUFFER;
    REFERENCE_TIME periodicity = EngineLatency * 10000;

    hr = _AudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
        AUDCLNT_STREAMFLAGS_NOPERSIST,
        bufferDuration,
        periodicity,
        _MixFormat,
        NULL);
    if (FAILED(hr))
    {
        bqLog::PrintError("Unable to initialize audio client: %x.\n", hr);
        return false;
    }

    //
    //  Retrieve the buffer size for the audio client.
    //
    hr = _AudioClient->GetBufferSize(&_BufferSize);
    if (FAILED(hr))
    {
        bqLog::PrintError("Unable to get audio client buffer: %x. \n", hr);
        return false;
    }

    hr = _AudioClient->GetService(IID_PPV_ARGS(&_RenderClient));
    if (FAILED(hr))
    {
        bqLog::PrintError("Unable to get new render client: %x.\n", hr);
        return false;
    }

    return true;
}

void bqWASAPIRenderer::Shutdown()
{
    bqLog::PrintInfo("Shutdown WASAPI\n");
    if (m_endpoint)
    {
        m_endpoint->Release();
        m_endpoint = 0;
    }

    if (_AudioClient)
    {
        _AudioClient->Release();
        _AudioClient = 0;
    }

    if (_RenderClient)
    {
        _RenderClient->Release();
        _RenderClient = 0;
    }
}

