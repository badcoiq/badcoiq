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
#ifndef __BQ_SOUNDOBJECT_H__
#define __BQ_SOUNDOBJECT_H__

class bqSoundObjectCallback
{
public:
	bqSoundObjectCallback() {}
	virtual ~bqSoundObjectCallback() {}

	virtual void OnStart() {}
	virtual void OnStop() {}

	void* m_context = 0;
};

class bqSoundObject
{
public:
	bqSoundObject() {}
	virtual ~bqSoundObject() {}
	BQ_PLACEMENT_ALLOCATOR(bqSoundObject);

	virtual void Start() = 0;
	virtual void Stop() = 0;
	//virtual void Pause() = 0;
	//virtual void SetVolume(float) = 0;

	// придётся перезапустить проигрывание
	// loops - 0 без повторение
	//         -1 (0xFFFFFFFF) бесконечное повторение
	//         остальные значения это количество повторов.
	//virtual void EnableLoop(uint32_t loops) = 0;
	//virtual void DisableLoop() = 0;

	// Воспроизводить будет без учёта играет ли уже или нет.
	// Нет проверок.
	// Нет вызовов коллбэка.
	//virtual void SetSource(void* data, uint32_t dataSize) = 0;
	//virtual void PlaySource() = 0;
	//virtual void StopSource() = 0;

	//virtual void Update() = 0;

	bqSoundObjectCallback* m_callback = 0;

	enum
	{
		state_notplaying,
		state_playing,
	};
	uint32_t m_state = state_notplaying;

	bqSoundSourceData* m_sourceData = 0;
};


#endif


