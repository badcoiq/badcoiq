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
#ifndef _BQ_WAVEOUT_H_
#define _BQ_WAVEOUT_H_

#include <windows.h>

#include "badcoiq/sound/bqSoundSystem.h"

class bqSoundObjectWaveOut : public bqSoundObject
{
public:
	bqSoundObjectWaveOut();
	virtual ~bqSoundObjectWaveOut();
	BQ_PLACEMENT_ALLOCATOR(bqSoundObjectWaveOut);

	virtual void Start() override;
	virtual void Stop() override;
	virtual void Pause() override;
	virtual void SetVolume(float) override;
	virtual void EnableLoop(uint32_t loops) override;
	virtual void DisableLoop() override;

	virtual void SetSource(void* data, uint32_t dataSize) override;
	virtual void PlaySource() override;
	virtual void StopSource() override;
	virtual void Use3D(bool) override;
	virtual void Set3DUpdateEachNFrame(uint32_t N) override;
	virtual void Update3D() override;
	virtual void SetListenerPosition(const bqVec3&) override;
	virtual void SetEmitterPosition(const bqVec3&) override;

	HWAVEOUT m_device = 0;
	WAVEHDR m_waveHeader;
	bool m_bufferPrepared = false;
};

class bqSoundEngineWaveOut : public bqSoundEngine
{
	HWAVEOUT m_device = 0;

	bool m_isInit = false;
public:
	bqSoundEngineWaveOut();
	virtual ~bqSoundEngineWaveOut();

	virtual bqSoundObject* SummonSoundObject(bqSound*) override;
	virtual const char* Name() override;

	virtual bool Init() override;
	virtual void Shutdown() override;
	virtual bool IsReady() override;
};

#endif
