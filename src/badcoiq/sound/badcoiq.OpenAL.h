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
#ifndef _BQ_OPENAL_H_
#define _BQ_OPENAL_H_

#include "badcoiq/sound/bqSoundSystem.h"

#include "al.h"
#include "alc.h"

class bqSoundObjectOpenAL : public bqSoundObject
{
	ALuint m_source = 0;
	uint32_t m_loop = 0;
public:
	bqSoundObjectOpenAL();
	virtual ~bqSoundObjectOpenAL();
	BQ_PLACEMENT_ALLOCATOR(bqSoundObjectOpenAL);

	bool Init(ALuint buffer);

	virtual void Play() override;
	virtual void Stop() override;
	virtual void Pause() override;
	virtual void Loop(bool) override;
};

class bqSoundEngineOpenAL : public bqSoundEngine
{
	ALCdevice* m_device = 0;
	ALCcontext* m_context = 0;
	ALboolean m_eax = 0;

	struct OpenALBuffer
	{
		bqSound* m_sound = 0;
		ALuint m_buffer = 0;
	};
	bqArray<OpenALBuffer> m_buffers;

public:
	bqSoundEngineOpenAL();
	virtual ~bqSoundEngineOpenAL();
	BQ_PLACEMENT_ALLOCATOR(bqSoundEngineOpenAL);

	virtual const char* GetName() override;
	virtual bqSoundObject* SummonObject(bqSound*) override;

	static ALenum CheckOpenALError(ALenum);
};

#endif
