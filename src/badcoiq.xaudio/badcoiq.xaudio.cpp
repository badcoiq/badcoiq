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
	m_so->m_state = m_so->state_notplaying;
	if(m_so->m_callback)
		m_so->m_callback->OnStop();
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
	wfx.nChannels = s->m_soundSource->m_channels;
	wfx.cbSize = 0;

	wfx.nSamplesPerSec = s->m_soundSource->m_sampleRate;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * 2;
	wfx.nBlockAlign = 2;
	wfx.wBitsPerSample = s->m_soundSource->m_bits;

	IXAudio2SourceVoice* SourceVoice = 0;
	bqIXAudio2VoiceCallback* callback = new bqIXAudio2VoiceCallback;

	if (FAILED(hr = m_XAudio->CreateSourceVoice(&SourceVoice, &wfx, 0, 2.f, callback)))
	{
		delete callback;
		bqLog::PrintError(L"Error %#X creating source voice\n", hr);
		return 0;
	}

	bqSoundObjectXAudio* so = new bqSoundObjectXAudio;
	so->m_SourceVoice = SourceVoice;
	so->m_source = s;
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
			return false;
		}

		if (FAILED(hr = m_XAudio->CreateMasteringVoice(&m_MasteringVoice)))
		{
			bqLog::PrintError(L"Failed creating mastering voice: %#X\n", hr);
			SAFE_RELEASE(m_XAudio);
			return false;
		}

		

		m_isInit = true;
		bqLog::PrintInfo(L"XAudio2 is ready\n");

		return true;
	}
	return false;
}

void bqSoundEngineXAudio::Shutdown()
{
	BQ_ASSERT_ST(m_isInit == true);
	if (m_isInit)
	{
		m_isInit = false;

		if (m_MasteringVoice)
		{
			m_MasteringVoice->DestroyVoice();
			m_MasteringVoice = 0;
		}
		SAFE_RELEASE(m_XAudio);
	}
}

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
		XAUDIO2_BUFFER buffer = { 0 };
		buffer.pAudioData = m_source->m_soundSource->m_data;
		buffer.Flags = XAUDIO2_END_OF_STREAM;  // tell the source voice not to expect any data after this buffer
		buffer.AudioBytes = m_source->m_soundSource->m_dataSize;

		HRESULT hr = S_OK;
		if (FAILED(hr = m_SourceVoice->SubmitSourceBuffer(&buffer)))
		{
			bqLog::PrintError(L"Error %#X submitting source buffer\n", hr);
			return;
		}

		if(m_callback)
			m_callback->OnStart();

		hr = m_SourceVoice->Start(0);
		m_state = state_playing;
	}
}

void bqSoundObjectXAudio::Stop()
{
	if (m_state == state_playing)
	{
		m_SourceVoice->Stop();
		m_state = state_notplaying;
	}
}
