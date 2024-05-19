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

#include "badcoiq.h"

#ifdef BQ_WITH_SOUND

#include "badcoiq/sound/bqSoundSystem.h"

#include "bqSoundObjectImpl.h"

#include "../framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;

bqSoundObjectImpl::bqSoundObjectImpl()
{
}

bqSoundObjectImpl::~bqSoundObjectImpl()
{
}

void bqSoundObjectImpl::Play()
{
	printf("%s\n", __FUNCTION__);
	if (m_state == bqSoundObject::state_notplaying)
	{
		//// в тред надо послать сигнал что нужно начать обработку
		//// возможно hr = m_audioClient->Start(); нужно вызывать там же
		//hr = m_audioClient->Start();
		//if (FAILED(hr))
		//{
		//	bqLog::PrintError("Unable to start render client: %x.\n", hr);
		//}
		//m_state = bqSoundObject::state_playing;
		m_threadCommand = ThreadCommand::ThreadCommand_start;
		printf("%s m_threadCommand\n", __FUNCTION__);
	}
}

void bqSoundObjectImpl::Stop()
{
	printf("%s\n", __FUNCTION__);
	if (m_state == bqSoundObject::state_playing)
	{
		m_threadCommand = ThreadCommand::ThreadCommand_stop;
		/*m_state = bqSoundObject::state_notplaying;

		HRESULT hr = m_audioClient->Stop();
		if (FAILED(hr))
		{
			bqLog::PrintError("Unable to stop render client: %x.\n", hr);
		}*/
	}
}

void bqSoundObjectImpl::Pause()
{
}

void bqSoundObjectImpl::Loop(bool v)
{
}

bool bqSoundObjectImpl::Init(IMMDevice* endpoint, bqSound* sound)
{
	m_bufferData = &sound->m_soundBuffer->m_bufferData;

	

	return true;
}

void bqSoundObjectImpl::_thread_fillRenderBuffer()
{
	// пока закрыл так как нужно убрать зависящие от WASAPI вещи внутрь bqWASAPIRenderer
	//BYTE* pData = 0;
	//HRESULT hr = m_renderClient->GetBuffer(m_bufferSize, &pData);
	//if (SUCCEEDED(hr))
	//{
	//	bqSoundBufferData* soundData = m_bufferData;

	//	memcpy(pData, &soundData->m_data[m_currentPosition], m_bufferSize);

	//	m_currentPosition += m_bufferSize;

	//	hr = m_renderClient->ReleaseBuffer(m_bufferSize, 0);
	//	printf("done %u\n", m_currentPosition);
	//	if (!SUCCEEDED(hr))
	//	{
	//		//AUDCLNT_E_BUFFER_ERROR
	//		printf("Unable to release buffer: %x\n", hr);
	//	//	stillPlaying = false;
	//	}
	//}
	//else
	//{
	//	printf("Unable to get buffer: %x bufferSize: %u\n", hr, m_bufferSize);
	////	stillPlaying = false;
	//}
}

#endif
