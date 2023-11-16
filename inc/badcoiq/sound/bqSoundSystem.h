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
#ifndef __BQ_SS_H__
#define __BQ_SS_H__

#include "bqSound.h"
#include "bqSoundEngine.h"
#include "badcoiq/containers/bqQueue.h"

#include <functional>

// Если нужно воспроизвести звук напрямую вне bqSoundSystem::m_thread
// то надо использовать bqSoundEngineObject

namespace bq
{
	struct SoundInputThreadData;
}

class bqSoundEngineObject;
class bqSoundObject
{
	//friend void bqSoundSystem_thread(bqSoundSystem* ss);
	//friend struct bq::SoundInputThreadData;

	//bool m_inThread = false; // смотри комментарий в bqSoundSystem_thread где std::map
	//
	//float m_volume = 0.8f;

public:
	bqSoundObject() {}
	virtual ~bqSoundObject() {}
	BQ_PLACEMENT_ALLOCATOR(bqSoundObject);

	// значение от 0 до 1
	virtual void SetVolume(float) = 0;
	virtual float GetVolume() = 0;

	// 0 - нет повтора
	// -1 или 0xFFFFFFFF - бесконечный повтор
	// иное значение есть количество повторов.
	// Когда есть повторы но не бесконечные то значение уменьшается до 0.
	// Если указано более 0, то будет проигрываться на 1 больше.
	// Например если m_loopCount = 3; то проиграется 4 раза.
	virtual void SetLoop(uint32_t) = 0;
	virtual uint32_t GetLoop() = 0;

	////bqSoundSourceData m_sourceData[2];
	//// для эффектов нужно обрабатывать множество маленьких буферов
	//// надо реализовать очередь.
	//// если с очередью будет плохое воспроизведение то идею своих эффектов
	////  надо будет отложить.
	////bqQueue<>


	//bqSoundEngineObject* m_engineObject = 0;
	//bqSoundEngine* m_engine = 0;
	//bqSoundObjectCallback* m_userCallback = 0;
	virtual void SetCallback(bqSoundObjectCallback*) = 0;

	virtual void Start() = 0;
	virtual void Stop() = 0;
	virtual void Pause() = 0;

	// Вернёт указатель который был указан при создании этого объекта
	virtual bqSound* GetSound() = 0;
};

class bqSoundSystem
{
	friend void bqSoundSystem_thread(bqSoundSystem* ss);

	bqArray<bqSoundEngine*> m_engines;
	
public:
	bqSoundSystem();
	~bqSoundSystem();

	uint32_t GetNumOfEngines();

	bqSoundEngine* GetEngine(uint32_t, const char*);

	bqSoundObject* SummonSoundObject(bqSoundEngine*, bqSound*);

	// Нужна независимая система
	// Свои эффекты и т.д.
	// Пусть объект будет по типу IXAudio2SourceVoice
	// Все эти методы пусть будут у него.
//	void Play(bqSoundObject*);
//	void Stop(bqSoundObject*);
//	void Pause(bqSoundObject*);

//	void StopAll();

	// если активно воспроизведение то true
	// StopAll() должен всё остановить у поставить false
//	bool m_isActive = false;
};

#endif


