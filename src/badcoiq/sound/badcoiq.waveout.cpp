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
#include "badcoiq.waveout.h"

#pragma comment(lib, "Winmm.lib")

extern "C"
{
	bqSoundEngine* BQ_CDECL bqSoundEngine_createWaveOut()
	{
		bqSoundEngineWaveOut* o = bqCreate<bqSoundEngineWaveOut>();
		return o;
	}
}

bqSoundEngineWaveOut::bqSoundEngineWaveOut()
{
}

bqSoundEngineWaveOut::~bqSoundEngineWaveOut()
{
	Shutdown();
}


bqSoundObject* bqSoundEngineWaveOut::SummonSoundObject(bqSound* s)
{
	BQ_ASSERT_ST(s);

	bqSoundObjectWaveOut* o = new bqSoundObjectWaveOut;
	o->m_sourceData = &s->m_soundSource->m_sourceData;
	o->m_device = m_device;
	return o;
}

const char* bqSoundEngineWaveOut::Name()
{
	return "WaveOut";
}

// Возможно девайс должен быть уникальным для каждого звука

bool bqSoundEngineWaveOut::Init()
{
	if (!m_isInit)
	{
		WAVEFORMATEX wfx;
		memset(&wfx, 0, sizeof(wfx));
		wfx.wFormatTag = WAVE_FORMAT_PCM;
		wfx.cbSize = 0;
		wfx.nChannels = 2;
		wfx.nSamplesPerSec = 44100;
		wfx.wBitsPerSample = 16;
		wfx.nBlockAlign = (wfx.wBitsPerSample / 8) * wfx.nChannels;
		wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;// *wfx.nChannels;

		MMRESULT mmr = waveOutOpen(&m_device, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);

		if (MMSYSERR_NOERROR != mmr)
			bqLog::PrintError("Failed to initialize WaveOut\n");
		else
			m_isInit = true;
	}

	return m_isInit;
}

void bqSoundEngineWaveOut::Shutdown()
{
	if (m_isInit)
	{
		m_isInit = false;
	}
}

bool bqSoundEngineWaveOut::IsReady()
{
	return m_isInit;
}

bqSoundObjectWaveOut::bqSoundObjectWaveOut()
{
	memset(&m_waveHeader, 0, sizeof(m_waveHeader));
}

bqSoundObjectWaveOut::~bqSoundObjectWaveOut()
{
}

void bqSoundObjectWaveOut::Start()
{
	SetSource(m_sourceData->m_data, m_sourceData->m_dataSize);
	PlaySource();
}

void bqSoundObjectWaveOut::Stop()
{
	StopSource();
}

void bqSoundObjectWaveOut::Pause()
{
	waveOutPause(m_device);
}

void bqSoundObjectWaveOut::SetVolume(float v)
{
	if (v > 0.f && v < 1.f)
	{
		union _DWORD
		{
			DWORD _32;
			WORD _16[2];
		};
		_DWORD __V;
		__V._16[0] = (WORD)floor(v / (1.f / 65535.f));
		__V._16[1] = __V._16[0];
		waveOutSetVolume(m_device, __V._32);
	}
}

void bqSoundObjectWaveOut::EnableLoop(uint32_t loops)
{
	waveOutPause(m_device);
	m_waveHeader.dwLoops = loops;
	MMRESULT r = waveOutPrepareHeader(m_device, &m_waveHeader, sizeof(m_waveHeader));
	if (r != MMSYSERR_NOERROR)
		bqLog::PrintError("waveOutPrepareHeader [%u]\n", r);
	else
		m_bufferPrepared = true;
	waveOutRestart(m_device);
}

void bqSoundObjectWaveOut::DisableLoop()
{
}

void bqSoundObjectWaveOut::SetSource(void* data, uint32_t dataSize)
{
	waveOutPause(m_device);
	m_bufferPrepared = false;
	m_waveHeader.lpData = (LPSTR)data;
	m_waveHeader.dwBufferLength = dataSize;
	m_waveHeader.dwBytesRecorded = 0;
	m_waveHeader.dwUser = 0;
	m_waveHeader.dwFlags = 0;
	m_waveHeader.dwLoops = 0;
	m_waveHeader.lpNext = 0;
	m_waveHeader.reserved = 0;
	MMRESULT r = waveOutPrepareHeader(m_device, &m_waveHeader, sizeof(m_waveHeader));
	if (r != MMSYSERR_NOERROR)
		bqLog::PrintError("waveOutPrepareHeader [%u]\n", r);
	else
		m_bufferPrepared = true;
	waveOutRestart(m_device);
}

void bqSoundObjectWaveOut::PlaySource()
{
	if (m_bufferPrepared)
	{
		MMRESULT r = waveOutWrite(m_device, &m_waveHeader, sizeof(m_waveHeader));
		if (r != MMSYSERR_NOERROR)
		{
			bqLog::PrintError("waveOutWrite [%u]\n", r);
		}
	}
}

void bqSoundObjectWaveOut::StopSource()
{
	waveOutReset(m_device);
}

void bqSoundObjectWaveOut::Use3D(bool)
{
}

void bqSoundObjectWaveOut::SetListenerPosition(const bqVec3&) {}
void bqSoundObjectWaveOut::SetEmitterPosition(const bqVec3&) {}
void bqSoundObjectWaveOut::Set3DUpdateEachNFrame(uint32_t N) {}
void bqSoundObjectWaveOut::Update3D() {}
