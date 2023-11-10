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
#ifndef __BQ_XAUDIO_H__
#define __BQ_XAUDIO_H__

#include "badcoiq.h"
#include "badcoiq/containers/bqArray.h"
#include "badcoiq/sound/bqSoundSystem.h"

#include <xaudio2.h>
#undef PlaySound

class bqSoundObjectXAudio;
class bqIXAudio2VoiceCallback : public IXAudio2VoiceCallback
{
public:
	bqIXAudio2VoiceCallback() {}
	virtual ~bqIXAudio2VoiceCallback() {}

	STDMETHOD_(void, OnVoiceProcessingPassStart) (THIS_ UINT32 BytesRequired);
	STDMETHOD_(void, OnVoiceProcessingPassEnd) (THIS);
	STDMETHOD_(void, OnStreamEnd) (THIS);
	STDMETHOD_(void, OnBufferStart) (THIS_ void* pBufferContext);
	STDMETHOD_(void, OnBufferEnd) (THIS_ void* pBufferContext);
	STDMETHOD_(void, OnLoopEnd) (THIS_ void* pBufferContext);
	STDMETHOD_(void, OnVoiceError) (THIS_ void* pBufferContext, HRESULT Error);

	bqSoundObjectXAudio* m_so = 0;
};

class bqSoundObjectXAudio : public bqSoundEngineObject
{
public:
	bqSoundObjectXAudio();
	virtual ~bqSoundObjectXAudio();
	BQ_PLACEMENT_ALLOCATOR(bqSoundObjectXAudio);

	IXAudio2SourceVoice* m_SourceVoice = 0;

	bqIXAudio2VoiceCallback* m_xaudioCallback = 0;

	UINT32 m_loopCount = 0;

	virtual void Start() override;
	virtual void Stop() override;
	virtual void SetVolume(float) override;
	virtual void EnableLoop() override;
	virtual void DisableLoop() override;
};

class bqSoundEngineXAudio : public bqSoundEngine
{
	bool m_isInit = false;
	IXAudio2* m_XAudio = 0;
	IXAudio2MasteringVoice* m_MasteringVoice = 0;

public:
	bqSoundEngineXAudio();
	virtual ~bqSoundEngineXAudio();

	virtual bqSoundEngineObject* SummonSoundObject(bqSound*) override;
	virtual const char* Name() override;

	virtual bool Init() override;
	virtual void Shutdown() override;
	virtual bool IsReady() override;
};

#endif
