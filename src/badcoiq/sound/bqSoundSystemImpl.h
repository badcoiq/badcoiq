/*
BSD 2-Clause License

Copyright (c) 2024, badcoiq
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
#include <atomic>

#include "badcoiq/containers/bqThreadFIFO.h"
#include "badcoiq/containers/bqFixedFIFO.h"
#include "badcoiq/sound/bqSoundSystem.h"


#include <Windows.h>
#include <MMDeviceAPI.h>
#include <AudioClient.h>
#include <AudioPolicy.h>
#include <functiondiscoverykeys.h>

class bqSoundObjectImpl;
class bqSoundMixerImpl;

class bqWASAPIRenderer : IMMNotificationClient, IAudioSessionEvents
{
public:
	bqWASAPIRenderer();
	~bqWASAPIRenderer();
	bool Initialize(IMMDevice* Endpoint);
	void Shutdown();

	bqSoundFormat GetFormat();

	// эти вещи предположительно могут быть в 1м экземпляре
	IAudioClient* m_audioClient = 0;
	IAudioRenderClient* m_renderClient = 0;

	WAVEFORMATEX* m_mixFormat = 0;
	UINT32      m_frameSize = 0;
	UINT32      m_bufferSize = 0;

	enum RenderSampleType
	{
		SampleTypeFloat,
		SampleType16BitPCM,
	};
	RenderSampleType m_renderSampleType;

	// Когда надо что-то сделать в треде обработки звука
	// надо послать команду. Команды обрабатываются после
	// цикла обработки звука.
	struct _thread_command;
	using ThreadCommanMethodType = void(bqWASAPIRenderer::*)(_thread_command*);
	struct _thread_command
	{
		void* m_ptr = 0;

		ThreadCommanMethodType m_method = 0;
	};

	struct _thread_context
	{
		// Тут я не знаю как делать так как нет опыта.
		// Делать бесконечно растущую очередь как мне кажется - плохо.
		// Поэтому сделал контейнер фиксированного размера.
		// Какие-то команды должны иметь высокий приоритет.
		// Возможно для приоритетов достаточно будет ввести несколько контейнеров.
		// bqFixedThreadFIFO<_thread_command, 150> m_commands[3]; например так
		bqFixedThreadFIFO<_thread_command, 150> m_commands;

		//	bqArray<bqSoundObjectImpl*> m_sounds;
		bqArray<bqSoundMixerImpl*> m_mixers;
		bqSoundMixerImpl* m_mainMixer = 0;
		bool m_run = true;

		enum
		{
			threadState_play,
			threadState_notplay
		};
		uint32_t m_state = threadState_play;

		std::atomic_uchar m_atomic_onRemoveAllMixers = 0;

	}m_threadContext;

	bqSoundMixerImpl* m_mainMixer = 0;

	void ThreadCommand_AddMixer(bqSoundMixerImpl*);
	void ThreadCommandMethod_AddMixer(_thread_command*);
	void ThreadCommand_SetMainMixer(bqSoundMixerImpl*);
	void ThreadCommandMethod_SetMainMixer(_thread_command*);
	void ThreadCommand_RemoveAllMixers();
	void ThreadCommandMethod_RemoveAllMixers(_thread_command*);
	void ThreadCommand_CallMethod(ThreadCommanMethodType, _thread_command*);

	void _thread_function();

	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	IMMDevice* GetEndpoint() { return m_endpoint; }

private:
	LONG    _RefCount = 1;
	IMMDevice* m_endpoint = 0;
	//HANDLE      _RenderThread;
	//HANDLE      _ShutdownEvent;
	LONG        _EngineLatencyInMS;

	std::thread* m_thread = 0;

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
	bqSoundSystemDeviceInfo m_deviceInfo;

public:
	bqSoundSystemImpl();
	virtual ~bqSoundSystemImpl();

	bool Init();

	virtual bqSoundStream* SummonStream(const char*) override;
	virtual bqSoundSystemDeviceInfo GetDeviceInfo() override;
	virtual bqSoundMixer* SummonMixer(uint32_t channels) override;
	virtual void AddMixerToProcessing(bqSoundMixer*) override;
	virtual void RemoveAllMixersFromProcessing() override;
	virtual bqSoundEffectVolume* SummonEffectVolume() override;
	virtual bqSoundEffectDelay* SummonEffectDelay(uint32_t steps, uint32_t time) override;
};

#endif
