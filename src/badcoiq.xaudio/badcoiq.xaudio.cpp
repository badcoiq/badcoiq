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

#include "badcoiq.xaudio.h"

#include <windows.h>

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

#pragma comment(lib, "XAudio2.lib")

extern "C"
{
	bqSoundEngine* BQ_CDECL bqSoundEngine_createXAudio()
	{
		bqSoundEngineXAudio* o = bqCreate<bqSoundEngineXAudio>();
		return o;
	}
}

template<class T>
bqSoundObjectXAudio* _GetXAudioSoundObject(T* so)
{
	return (bqSoundObjectXAudio*)so;
}

bqSoundEngineXAudio::bqSoundEngineXAudio()
{
}

bqSoundEngineXAudio::~bqSoundEngineXAudio()
{
}

void bqIXAudio2VoiceCallback::OnVoiceProcessingPassStart(THIS_ UINT32 BytesRequired)
{
}

void bqIXAudio2VoiceCallback::OnVoiceProcessingPassEnd()
{
}

void bqIXAudio2VoiceCallback::OnStreamEnd()
{
}

void bqIXAudio2VoiceCallback::OnBufferStart (THIS_ void* pBufferContext)
{
}

void bqIXAudio2VoiceCallback::OnBufferEnd (THIS_ void* pBufferContext)
{
	IXAudio2SourceVoice* voice = (IXAudio2SourceVoice*)pBufferContext;
	voice->Stop();
	voice->FlushSourceBuffers();

	if (m_so->m_state == m_so->state_playing)
	{
		m_so->m_state = m_so->state_notplaying;
		if (m_so->m_callback)
		{
			m_so->m_callback->OnStop();
		}
	}
}

void bqIXAudio2VoiceCallback::OnLoopEnd (THIS_ void* pBufferContext)
{
}

void bqIXAudio2VoiceCallback::OnVoiceError (THIS_ void* pBufferContext, HRESULT Error)
{
	bqLog::PrintError("XAudio: voice error %u\n", Error);
}


bqSoundObject* bqSoundEngineXAudio::SummonSoundObject(bqSound* s)
{
	HRESULT hr = S_OK;

	WAVEFORMATEX wfx;
	memset(&wfx, 0, sizeof(wfx));
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = s->m_soundSource->m_sourceInfo.m_channels;
	wfx.cbSize = 0;

	wfx.nSamplesPerSec = s->m_soundSource->m_sourceInfo.m_sampleRate;
	wfx.wBitsPerSample = s->m_soundSource->m_sourceInfo.m_bitsPerSample;
	wfx.nBlockAlign = (wfx.wBitsPerSample / 8) * wfx.nChannels;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

	IXAudio2SourceVoice* SourceVoice = 0;
	bqIXAudio2VoiceCallback* callback = new bqIXAudio2VoiceCallback;

	XAUDIO2_SEND_DESCRIPTOR sendDescriptors[2];
	sendDescriptors[0].Flags = XAUDIO2_SEND_USEFILTER; // LPF direct-path
	sendDescriptors[0].pOutputVoice = m_MasteringVoice; 
	sendDescriptors[1].Flags = XAUDIO2_SEND_USEFILTER; // LPF reverb-path -- omit for better performance at the cost of less realistic occlusion
	sendDescriptors[1].pOutputVoice = m_ReverbVoice;
	const XAUDIO2_VOICE_SENDS sendList = { 2, sendDescriptors };

	if (FAILED(hr = m_XAudio->CreateSourceVoice(&SourceVoice, &wfx, 0, 2.f, callback, 0)))
	{
		delete callback;
		bqLog::PrintError(L"Error %#X creating source voice\n", hr);
		return 0;
	}
	
	bqSoundObjectXAudio* so = new bqSoundObjectXAudio;
	so->m_SourceVoice = SourceVoice;
	so->m_sourceData = &s->m_soundSource->m_sourceData;
	so->m_xaudioCallback = callback;
	callback->m_so = so;

	return so;
}

const char* bqSoundEngineXAudio::Name()
{
	return "xaudio";
}

bool bqSoundEngineXAudio::Init()
{
	BQ_ASSERT_ST(m_isInit == false);

	if (!m_isInit)
	{
		uint32_t flags = 0;
		HRESULT hr = S_OK;
		
		if (FAILED(hr = XAudio2Create(&m_XAudio, flags)))
		{
			bqLog::PrintError(L"Failed to init XAudio2 engine: %#X\n", hr);
			Shutdown();
			return false;
		}

		if (FAILED(hr = m_XAudio->CreateMasteringVoice(&m_MasteringVoice)))
		{
			bqLog::PrintError(L"Failed creating mastering voice: %#X\n", hr);
			Shutdown();
			return false;
		}
		

		const float SPEEDOFSOUND = X3DAUDIO_SPEED_OF_SOUND;
		if (FAILED(hr = X3DAudioInitialize(3, SPEEDOFSOUND, m_X3DAudio)))
		{
			bqLog::PrintError(L"X3DAudioInitialize: %#X\n", hr);
			Shutdown();
			return false;
		}

		if (FAILED(hr = XAudio2CreateReverb(&m_ReverbEffect, 0)))
		{
			bqLog::PrintError(L"XAudio2CreateReverb: %#X\n", hr);
			Shutdown();
			return hr;
		}

		XAUDIO2_EFFECT_DESCRIPTOR effects[] = { { m_ReverbEffect, TRUE, 1 } };
		XAUDIO2_EFFECT_CHAIN effectChain = { 1, effects };

		if (FAILED(hr = m_XAudio->CreateSubmixVoice(&m_ReverbVoice, 2,
			44100, 0, 0,
			NULL, &effectChain)))
		{
			bqLog::PrintError(L"CreateSubmixVoice(&m_ReverbVoice: %#X\n", hr);
			Shutdown();
			return hr;
		}

		XAUDIO2FX_REVERB_PARAMETERS native;
		ReverbConvertI3DL2ToNative(&m_ReverbPresets[0], &native);
		m_ReverbVoice->SetEffectParameters(10, &native, sizeof(native));

		m_isInit = true;
		bqLog::PrintInfo(L"XAudio2 is ready\n");

		return true;
	}
	return false;
}

void bqSoundEngineXAudio::Shutdown()
{
	m_isInit = false;

	if (m_MasteringVoice)
	{
		m_MasteringVoice->DestroyVoice();
		m_MasteringVoice = 0;
	}
	if (m_XAudio)
		m_XAudio->StopEngine();
	SAFE_RELEASE(m_XAudio);
	SAFE_RELEASE(m_ReverbEffect);
}

bool bqSoundEngineXAudio::IsReady()
{
	return m_isInit;
}

// =====================================================
// 
// 
// 
// 
// =====================================================
bqSoundObjectXAudio::bqSoundObjectXAudio()
{
}

bqSoundObjectXAudio::~bqSoundObjectXAudio()
{
	if (m_SourceVoice)
	{
		m_SourceVoice->DestroyVoice();
		m_SourceVoice = 0;
	}

	if (m_xaudioCallback)
	{
		delete m_xaudioCallback;
		m_xaudioCallback = 0;
	}
}

void bqSoundObjectXAudio::Start()
{
	if (m_state == state_notplaying)
	{
		SetSource(m_sourceData->m_data, m_sourceData->m_dataSize);
		if(m_callback)
			m_callback->OnStart();

		PlaySource();

		m_state = state_playing;
	}
}

void bqSoundObjectXAudio::Stop()
{
	switch (m_state)
	{
	case state_playing:
		StopSource();
		m_state = state_notplaying;
		break;
	}
}

void bqSoundObjectXAudio::SetVolume(float v)
{
	m_SourceVoice->SetVolume(v);
}

void bqSoundObjectXAudio::EnableLoop(uint32_t loops)
{
	m_loopCount = loops;
	if(loops == 0xFFFFFFFF)
		m_loopCount = XAUDIO2_LOOP_INFINITE;
}

void bqSoundObjectXAudio::DisableLoop()
{
	m_loopCount = 0;
}

void bqSoundObjectXAudio::SetSource(void* data, uint32_t dataSize)
{
//	printf("%s\n", __FUNCTION__);

	XAUDIO2_BUFFER buffer = { 0 };
	buffer.pAudioData = (uint8_t*)data;
	buffer.Flags = XAUDIO2_END_OF_STREAM;  // tell the source voice not to expect any data after this buffer
	buffer.AudioBytes = dataSize;
	buffer.LoopCount = m_loopCount;
	buffer.pContext = m_SourceVoice;

	HRESULT hr = S_OK;
	if (FAILED(hr = m_SourceVoice->SubmitSourceBuffer(&buffer)))
		bqLog::PrintError(L"Error %#X submitting source buffer\n", hr);
}

void bqSoundObjectXAudio::PlaySource()
{
	m_SourceVoice->Start(0);
}

void bqSoundObjectXAudio::StopSource()
{
	//printf("STOPPED\n");
	m_SourceVoice->Stop();
	m_SourceVoice->FlushSourceBuffers();
}

void bqSoundObjectXAudio::Pause()
{
	m_SourceVoice->Stop();
}

void bqSoundObjectXAudio::Use3D()
{
}
