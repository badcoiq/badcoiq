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
#ifndef __BQ_SE_H__
#define __BQ_SE_H__

class bqSoundObjectCallback
{
public:
	bqSoundObjectCallback() {}
	virtual ~bqSoundObjectCallback() {}

	virtual void OnStart() {}
	virtual void OnStop() {}
};



class bqSoundEngineObject
{
public:
	bqSoundEngineObject() {}
	virtual ~bqSoundEngineObject() {}
	BQ_PLACEMENT_ALLOCATOR(bqSoundEngineObject);

	bqSoundObjectCallback* m_callback = 0;

	enum
	{
		state_notplaying,
		state_playing,
	};
	uint32_t m_state = state_notplaying;
	
	//bqSound* m_source = 0;
	bqSoundSourceData* m_sourceData = 0;

	virtual void Start() = 0;
	virtual void Stop() = 0;
	virtual void SetVolume(float) = 0;
};



// то через что будет проигрываться звук
// типа xaudio
// реализация должна находиться в плагинах

class bqSoundEngine
{
public:
	bqSoundEngine(){}
	virtual ~bqSoundEngine(){}

	virtual bqSoundEngineObject* SummonSoundObject(bqSound*) = 0;

	virtual const char* Name() = 0;

	virtual bool Init() = 0;
	virtual void Shutdown() = 0;
};

#endif


