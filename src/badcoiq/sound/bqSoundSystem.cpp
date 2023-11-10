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

#include "bqSoundSystemInternal.h"


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

		if (g_framework->m_threadSoundList->m_size)
		{
			auto soundNode = g_framework->m_threadSoundList->m_head;
			for (size_t i = 0; i < g_framework->m_threadSoundList->m_size; ++i)
			{
				auto so = soundNode->m_data.m_soundObject;
				
				// копирую часть звука
				uint8_t* src = so->m_engineObject->m_sourceData->m_data;
				uint8_t* dst = so->m_sourceData.m_data;
				for (size_t o = 0, srci = 0; o < so->m_sourceData.m_dataSize; ++o, ++srci)
				{
					if (srci >= so->m_engineObject->m_sourceData->m_dataSize)
						*dst = 0;
					else
						*dst = *src;

					++dst;
					++src;
				}

				so->m_engineObject->SetSource(so->m_sourceData.m_data,
					so->m_sourceData.m_dataSize);

				so->m_engineObject->PlaySource();
				std::this_thread::sleep_for(std::chrono::milliseconds(11110));

				soundNode = soundNode->m_right;
			}
		}


		if (g_framework->m_threadSoundInputQueue->Size())
		{
			//printf("GET\n");
			bq::SoundInputThreadData d;
			g_framework->_threadSoundInputQueue(false, &d);

			switch (d.m_command)
			{
			case bq::SoundInputThreadData::command_play:
			{
				if (g_framework->m_threadSoundList->size() < g_framework->m_threadSoundSoundLimit)
				{
					//printf("Add\n");
					g_framework->m_threadSoundList->push_back(d);
				}
			}break;
			case bq::SoundInputThreadData::command_stopAll:
			{
				g_framework->m_threadSoundList->clear();
			}break;
			}
		}
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

	// внутри m_threadSound
	//for (size_t i = 0; i < m_engines.m_size; ++i)
	//{
	//	m_engines.m_data[i]->Shutdown();
	//}
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
	so->m_engine = e;

	return so.Drop();
}

void bqSoundSystem::Play(bqSoundObject* so)
{
	BQ_ASSERT_ST(so);
	bq::SoundInputThreadData d;
	d.m_soundObject = so;
	d.m_command = d.command_play;
	g_framework->_threadSoundInputQueue(true, &d);
}

void bqFrameworkImpl::_threadSoundInputQueue(bool set, bq::SoundInputThreadData* d)
{
	std::lock_guard<std::mutex> guard(m_threadSoundInputQueueMutex);

	if (set)
	{
		m_threadSoundInputQueue->Push(*d);
	}
	else
	{
		if (m_threadSoundInputQueue->Size() && d)
		{
			*d = m_threadSoundInputQueue->Front();
			m_threadSoundInputQueue->Pop();
		}			
	}
}

void bqSoundSystem::StopAll()
{
	bq::SoundInputThreadData d;
	d.m_soundObject = 0;
	d.m_command = d.command_stopAll;
	g_framework->_threadSoundInputQueue(true, &d);
}