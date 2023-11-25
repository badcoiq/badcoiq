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

#include <Windows.h>
#include <AudioClient.h>
#include <AudioPolicy.h>
#include <functiondiscoverykeys.h>
#include <strsafe.h>


bqSoundSystemImpl::bqSoundSystemImpl()
{
}

bqSoundSystemImpl::~bqSoundSystemImpl()
{
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

    if (!retValue)
    {
        if (m_device) m_device->Release();
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

