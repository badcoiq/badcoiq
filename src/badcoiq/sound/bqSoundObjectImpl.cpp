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

//#include "bqSoundSystemInternal.h"
#include <thread>

#include "../framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;

//void bqSoundObjectImpl_thread(bqSoundObjectImpl* so)
//{
//	while (so->m_threadRun)
//	{
//		std::this_thread::sleep_for(std::chrono::milliseconds(1));
//	}
//}

bqSoundObjectImpl::bqSoundObjectImpl(bqSound* s)
{
	m_sound = s;
	m_soundSource = s->m_soundSource;

	//m_listNodeInThreadListSounds = g_framework->m_threadSounds->push_back(this);
//	Init();
}

bqSoundObjectImpl::~bqSoundObjectImpl()
{
//	Shutdown();
}

//void bqSoundObjectImpl::Init()
//{
//	if (!m_thread)
//	{
//		m_threadRun = true;
//		m_thread = new std::thread(bqSoundObjectImpl_thread, this);
//	}
//}
//
//void bqSoundObjectImpl::Shutdown()
//{
//	if (m_thread)
//	{
//		m_threadRun = false;
//		m_thread->join();
//		delete m_thread;
//		m_thread = 0;
//	}
//
//	//if (m_threadObject.m_threadReady)
//	//{
//
//	//	m_threadObject.m_delete = m_threadObject.Delete_request;
//	//	while (m_threadObject.m_delete == m_threadObject.Delete_request)
//	//	{
//	//	}
//	//	//g_framework->m_threadSounds->erase_by_node(m_listNodeInThreadListSounds);
//	//}
//}


void bqSoundObjectImpl::SetVolume(float v)
{
	if (v < 0.f)
		v = 0.f;
	if (v > 1.f)
		v = 1.f;

	m_volume = v;
}

float bqSoundObjectImpl::GetVolume()
{
	return m_volume;
}

void bqSoundObjectImpl::SetLoop(uint32_t v)
{
	m_loop = v;
}

uint32_t bqSoundObjectImpl::GetLoop()
{
	return m_loop;
}

void bqSoundObjectImpl::SetCallback(bqSoundObjectCallback* v)
{
	m_callback = v;
}

void bqSoundObjectImpl::Start()
{
	m_state = State::Start;
}

void bqSoundObjectImpl::Stop()
{
	m_state = State::Stop;
}

void bqSoundObjectImpl::Pause()
{
	m_state = State::Pause;
}

bqSound* bqSoundObjectImpl::GetSound()
{
	return m_sound;
}
