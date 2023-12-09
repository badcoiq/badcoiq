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

#include "bqSoundObjectImpl.h"

#include "../framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;

bqSoundObjectImpl::bqSoundObjectImpl()
{
}

bqSoundObjectImpl::~bqSoundObjectImpl()
{
	if (m_audioClient)
	{
		m_audioClient->Release();
		m_audioClient = 0;
	}

	if (m_renderClient)
	{
		m_renderClient->Release();
		m_renderClient = 0;
	}
}

void bqSoundObjectImpl::Play()
{
	if (m_state == bqSoundObject::state_notplaying)
	{
		HRESULT hr = S_OK;

		// очистка буфера у m_renderClient
		// чтобы при старте не проигрывался `мусор`
		{
			BYTE* pData;
			hr = m_renderClient->GetBuffer(m_bufferSize, &pData);
			if (FAILED(hr))
			{
				bqLog::PrintError("Failed to get buffer: %x.\n", hr);
				return;
			}
			hr = m_renderClient->ReleaseBuffer(m_bufferSize, AUDCLNT_BUFFERFLAGS_SILENT);
			if (FAILED(hr))
			{
				bqLog::PrintError("Failed to release buffer: %x.\n", hr);
				return;
			}
		}

		// в тред надо послать сигнал что нужно начать обработку
		// возможно hr = m_audioClient->Start(); нужно вызывать там же
		hr = m_audioClient->Start();
		if (FAILED(hr))
		{
			bqLog::PrintError("Unable to start render client: %x.\n", hr);
		}

		m_state = bqSoundObject::state_playing;
	}
}

void bqSoundObjectImpl::Stop()
{
	if (m_state == bqSoundObject::state_playing)
	{
		m_state = bqSoundObject::state_notplaying;

		HRESULT hr = m_audioClient->Stop();
		if (FAILED(hr))
		{
			bqLog::PrintError("Unable to stop render client: %x.\n", hr);
		}
	}
}

void bqSoundObjectImpl::Pause()
{
}

void bqSoundObjectImpl::Loop(bool v)
{
}

bool bqSoundObjectImpl::Init(IMMDevice* endpoint, bqSound* sound, uint32_t EngineLatency)
{
	m_bufferData = &sound->m_soundBuffer->m_bufferData;

	HRESULT hr = endpoint->Activate(__uuidof(IAudioClient),
		CLSCTX_INPROC_SERVER, NULL,
		reinterpret_cast<void**>(&m_audioClient));
	if (FAILED(hr))
	{
		bqLog::PrintError("Unable to activate audio client: %x.\n", hr);
		return false;
	}

	hr = m_audioClient->GetMixFormat(&m_mixFormat);
	if (FAILED(hr))
	{
		bqLog::PrintError("Unable to get mix format on audio client: %x.\n", hr);
		return false;
	}
	m_frameSize = m_mixFormat->nBlockAlign;

	//  If the mix format is a float format, just try to convert the format to PCM.
	if (m_mixFormat->wFormatTag == WAVE_FORMAT_PCM ||
		m_mixFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
		reinterpret_cast<WAVEFORMATEXTENSIBLE*>(m_mixFormat)->SubFormat == KSDATAFORMAT_SUBTYPE_PCM)
	{
		if (m_mixFormat->wBitsPerSample == 16)
		{
			m_renderSampleType = SampleType16BitPCM;
		}
		else
		{
			bqLog::PrintError("Unknown PCM integer sample type\n");
			return false;
		}
	}
	else if (m_mixFormat->wFormatTag == WAVE_FORMAT_IEEE_FLOAT ||
		(m_mixFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
			reinterpret_cast<WAVEFORMATEXTENSIBLE*>(m_mixFormat)->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT))
	{
		m_renderSampleType = SampleTypeFloat;
	}
	else
	{
		bqLog::PrintError("unrecognized device format.\n");
		return false;
	}

	int PERIODS_PER_BUFFER = 4;
	REFERENCE_TIME bufferDuration = EngineLatency * 10000 * PERIODS_PER_BUFFER;
	REFERENCE_TIME periodicity = EngineLatency * 10000;

	hr = m_audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
		AUDCLNT_STREAMFLAGS_NOPERSIST,
		bufferDuration,
		periodicity,
		m_mixFormat,
		NULL);
	if (FAILED(hr))
	{
		bqLog::PrintError("Unable to initialize audio client: %x.\n", hr);
		return false;
	}

	//  Retrieve the buffer size for the audio client.
	hr = m_audioClient->GetBufferSize(&m_bufferSize);
	if (FAILED(hr))
	{
		bqLog::PrintError("Unable to get audio client buffer: %x. \n", hr);
		return false;
	}

	hr = m_audioClient->GetService(IID_PPV_ARGS(&m_renderClient));
	if (FAILED(hr))
	{
		bqLog::PrintError("Unable to get new render client: %x.\n", hr);
		return false;
	}

	return true;
}
