﻿/*
BSD 2-Clause License

Copyright (c) 2024, badcoiq
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

#ifdef BQ_WITH_SOUND


#include "badcoiq/sound/bqSoundSystem.h"

#include "bqSoundEffectVolumeImpl.h"

#include "../framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;

bqSoundEffectVolumeImpl::bqSoundEffectVolumeImpl()
{
}

bqSoundEffectVolumeImpl::~bqSoundEffectVolumeImpl()
{
	
}

void bqSoundEffectVolumeImpl::SetVolume(float v)
{
	
	m_volume = v;
	if (m_volume < 0.f)
		m_volume = 0.f;
	if (m_volume > 1.f)
		m_volume = 1.f;
}

float bqSoundEffectVolumeImpl::GetVolume()
{
	return m_volume;
}

void bqSoundEffectVolumeImpl::Process(bqSoundMixer* mixer)
{
	BQ_ASSERT_ST(mixer);
	uint32_t noc = mixer->GetNumOfChannels();
	for (uint32_t i = 0; i < noc; ++i)
	{
		bqSoundBufferData* ch = mixer->GetChannel(i);
		bqSoundBufferInfo inf;
		mixer->GetSoundBufferInfo(inf);
		Process(ch, &inf);
	}
}

void bqSoundEffectVolumeImpl::Process(bqSoundBuffer* v)
{
	BQ_ASSERT_ST(v);
	Process(&v->m_bufferData, &v->m_bufferInfo);
}

void bqSoundEffectVolumeImpl::Process(bqSoundBufferData* data, bqSoundBufferInfo* info)
{
	BQ_ASSERT_ST(data);
	BQ_ASSERT_ST(info);

	if (info->m_format == bqSoundFormat::float32)
	{
		float32_t* data32 = (float32_t*)data->m_data;
		uint32_t dataSize = data->m_dataSize / sizeof(float32_t);

		for (uint32_t i = 0; i < dataSize; ++i)
		{
			*data32 *= m_volume;
			++data32;
		}
	}
}

#endif
