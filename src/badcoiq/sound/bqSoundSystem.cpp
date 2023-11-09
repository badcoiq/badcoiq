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

#include <thread>

#include "../framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;

extern "C"
{
	bqSoundEngine* BQ_CDECL bqSoundEngine_createXAudio();
}

BQ_LINK_LIBRARY("badcoiq.xaudio");

void bqSoundSystem_thread(bqSoundSystem* ss)
{
	bqLog::PrintInfo("Init sound thread\n");

	// добавить engines
	ss->m_engines.push_back(bqSoundEngine_createXAudio());

	g_framework->m_threadSoundRun = true;
	while (g_framework->m_threadSoundRun)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	for (size_t i = 0; i < ss->m_engines.m_size; ++i)
	{
		ss->m_engines.m_data[i]->Shutdown();
	}
}

bqSoundSystem::bqSoundSystem()
{
	g_framework->m_threadSound = new std::thread(bqSoundSystem_thread, this);
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		if (g_framework->m_threadSoundRun)
			break;
	}
}

bqSoundSystem::~bqSoundSystem()
{
	if (g_framework->m_threadSoundRun)
	{
		g_framework->m_threadSoundRun = false;
		g_framework->m_threadSound->join();
		delete g_framework->m_threadSound;
		g_framework->m_threadSound = 0;
	}
}

uint32_t bqSoundSystem::GetNumOfEngines()
{
	return (uint32_t)m_engines.m_size;
}

bqSoundEngine* bqSoundSystem::GetEngine(uint32_t in, const char* n)
{
	BQ_ASSERT_ST(m_engines.m_size);
	if (n)
	{
		for (size_t i = 0; i < m_engines.m_size; ++i)
		{
			if (strcmp(m_engines.m_data[i]->Name(), n) == 0)
			{
				return m_engines.m_data[i];
			}
		}

		// если не нашлось то вернётся самый первый bqSoundEngine
		in = 0;
	}

	return m_engines.m_data[in];
}

bqSoundObject* bqSoundSystem::SummonSoundObject(bqSoundEngine* e, bqSound* s)
{
	BQ_ASSERT_ST(e);
	BQ_ASSERT_ST(s);

	BQ_PTR_D(bqSoundObject, so, new bqSoundObject());
	
	so->m_engineObject = e->SummonSoundObject(s);
	if (!so->m_engineObject)
	{
		bqLog::PrintError("Can't create sound engine object\n");
		return 0;
	}

	// Engine Object должен иметь указатель на source data из sound object
	//  
	so->m_engineObject->m_sourceData = &so->m_sourceData;	
	so->m_engine = e;

	return so.Drop();
}

void bqSoundSystem::Play(bqSoundObject* so)
{
	BQ_ASSERT_ST(so);

}
