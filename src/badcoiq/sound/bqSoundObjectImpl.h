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
#ifndef _BQ_SNDOBJIMPL_H_
#define _BQ_SNDOBJIMPL_H_

#include "badcoiq/sound/bqSoundSystem.h"

class bqSoundObjectImpl : public bqSoundObject
{
	friend class bqSoundSystem;

	float m_volume = 0.7f;
	uint32_t m_loop = 0;
	bqSoundObjectCallback* m_callback = 0;

	enum class State { Null, Start, Stop, Pause } m_state = State::Null;

	bqSound* m_sound = 0;
	bqSoundSource* m_soundSource = 0;
	bqSoundEngineObject* m_soundEngineObject = 0;
	bqSoundEngine* m_soundEngine = 0;
public:
	bqSoundObjectImpl(bqSound*);
	virtual ~bqSoundObjectImpl();
	BQ_PLACEMENT_ALLOCATOR(bqSoundObjectImpl);

	virtual void SetVolume(float) override;
	virtual float GetVolume() override;

	virtual void SetLoop(uint32_t) override;
	virtual uint32_t GetLoop() override;

	virtual void SetCallback(bqSoundObjectCallback*) override;

	virtual void Start() override;
	virtual void Stop() override;
	virtual void Pause() override;

	virtual bqSound* GetSound() override;

	// Указатель но объект данной структуры будет передан в нить
	// Нужно сделать так чтобы удаление bqSoundObjectImpl было
	// быстрым и не вызывало ошибок.
	// Вполне возможно что здесь bqSoundObjectImpl в деструкторе
	// надо обнулить данный указатель в определённый момент,
	// потом дождаться определённого момента в ните, и продолжить
	// выполнение программы.
	struct ThreadObject
	{
		bqSoundObjectImpl* m_so = 0;


		enum
		{
			Delete_null,
			Delete_request,
			Delete_ok
		};
		// вызывается в деструкторе. нужно указать Delete_request и дождаться
		// когда будет Delete_ok
		uint8_t m_delete = Delete_null;
		bool m_threadReady = false;
	}
	m_threadObject;
};

#endif
