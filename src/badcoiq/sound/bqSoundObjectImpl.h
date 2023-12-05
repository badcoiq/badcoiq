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

#include <Windows.h>
#include <MMDeviceAPI.h>
#include <AudioClient.h>
#include <AudioPolicy.h>
#include <functiondiscoverykeys.h>

#include "badcoiq/sound/bqSoundSystem.h"

class bqSoundObjectImpl : bqSoundObject
{
	IAudioClient* m_audioClient = 0;
	IAudioRenderClient* m_renderClient=0;
	WAVEFORMATEX* m_mixFormat = 0;
	UINT32      m_frameSize = 0;
	UINT32      m_bufferSize = 0;

	enum RenderSampleType
	{
		SampleTypeFloat,
		SampleType16BitPCM,
	};
	RenderSampleType m_renderSampleType;

public:
	bqSoundObjectImpl();
	virtual ~bqSoundObjectImpl();
	BQ_PLACEMENT_ALLOCATOR(bqSoundObjectImpl);

	virtual void Play() override;
	virtual void Stop() override;
	virtual void Pause() override;
	virtual void Loop(bool) override;

	bool Init(uint32_t EngineLatency);
};

#endif
