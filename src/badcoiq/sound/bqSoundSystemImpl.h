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
#pragma once
#ifndef _BQ_SNDSSTMIMPL_H_
#define _BQ_SNDSSTMIMPL_H_

#include <thread>
#include <mutex>

#include "badcoiq/sound/bqSoundSystem.h"


#include <Windows.h>
#include <MMDeviceAPI.h>
#include <AudioClient.h>
#include <AudioPolicy.h>
#include <functiondiscoverykeys.h>


class bqWASAPIRenderer : IMMNotificationClient, IAudioSessionEvents
{
public:
	bqWASAPIRenderer(IMMDevice* Endpoint);
	~bqWASAPIRenderer();
	bool Initialize(UINT32 EngineLatency);
	void Shutdown();

	enum RenderSampleType
	{
		SampleTypeFloat,
		SampleType16BitPCM,
	};

	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

private:
	LONG    _RefCount = 1;
	IMMDevice* m_endpoint = 0;
	HANDLE      _RenderThread;
	HANDLE      _ShutdownEvent;
	WAVEFORMATEX* _MixFormat;
	RenderSampleType _RenderSampleType;
	LONG        _EngineLatencyInMS;
	UINT32      _FrameSize =0;
	UINT32      _BufferSize=0;
	IAudioClient* _AudioClient = 0;
	IAudioRenderClient* _RenderClient=0;

	//  IUnknown
	STDMETHOD(QueryInterface)(REFIID iid, void** pvObject);

	STDMETHOD(OnDisplayNameChanged) (LPCWSTR /*NewDisplayName*/, LPCGUID /*EventContext*/) { return S_OK; };
	STDMETHOD(OnIconPathChanged) (LPCWSTR /*NewIconPath*/, LPCGUID /*EventContext*/) { return S_OK; };
	STDMETHOD(OnSimpleVolumeChanged) (float /*NewSimpleVolume*/, BOOL /*NewMute*/, LPCGUID /*EventContext*/) { return S_OK; }
	STDMETHOD(OnChannelVolumeChanged) (DWORD /*ChannelCount*/, float /*NewChannelVolumes*/[], DWORD /*ChangedChannel*/, LPCGUID /*EventContext*/) { return S_OK; };
	STDMETHOD(OnGroupingParamChanged) (LPCGUID /*NewGroupingParam*/, LPCGUID /*EventContext*/) { return S_OK; };
	STDMETHOD(OnStateChanged) (AudioSessionState /*NewState*/) { return S_OK; };
	STDMETHOD(OnSessionDisconnected) (AudioSessionDisconnectReason DisconnectReason);
	STDMETHOD(OnDeviceStateChanged) (LPCWSTR /*DeviceId*/, DWORD /*NewState*/) { return S_OK; }
	STDMETHOD(OnDeviceAdded) (LPCWSTR /*DeviceId*/) { return S_OK; };
	STDMETHOD(OnDeviceRemoved) (LPCWSTR /*DeviceId(*/) { return S_OK; };
	STDMETHOD(OnDefaultDeviceChanged) (EDataFlow Flow, ERole Role, LPCWSTR NewDefaultDeviceId);
	STDMETHOD(OnPropertyValueChanged) (LPCWSTR /*DeviceId*/, const PROPERTYKEY /*Key*/) { return S_OK; };
};

class bqSoundSystemImpl : public bqSoundSystem
{
	IMMDevice* m_device = 0;
	bqWASAPIRenderer* m_WASAPIrenderer = 0;
public:
	bqSoundSystemImpl();
	virtual ~bqSoundSystemImpl();

	bool Init();

	virtual bqSoundObject* SummonObject(const char*) override;
	virtual bqSoundObject* SummonObject(bqSound*) override;
	virtual bqSoundStreamObject* SummonStreamObject(const char*) override;
	virtual bqSoundStreamObject* SummonStreamObject(const bqStringA&) override;
};

#endif
