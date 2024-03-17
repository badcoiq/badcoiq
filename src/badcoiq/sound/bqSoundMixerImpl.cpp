/*
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

#include "badcoiq/sound/bqSoundSystem.h"

#include "bqSoundMixerImpl.h"

#include "../framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;

class bqSoundMixerCallbackDefault : public bqSoundMixerCallback
{
public:
	bqSoundMixerCallbackDefault() {}
	virtual ~bqSoundMixerCallbackDefault() {}
};

static bqSoundMixerCallbackDefault g_defaultCallback;

bqSoundMixerImpl::bqSoundMixerImpl(uint32_t channels, const bqSoundSystemDeviceInfo& info)
{
	BQ_ASSERT_ST(channels > 0);
	BQ_ASSERT_ST(channels < 10);
	BQ_ASSERT_ST(info.m_bufferSize);
	BQ_ASSERT_ST(info.m_channels);

	if (channels == 0)
		channels = 1;
	if (channels > 10)
		channels = 10;

	uint32_t bufferSizeForOneChannel = info.m_bufferSize / info.m_channels;

	m_dataInfo.m_bitsPerSample = 32;
	m_dataInfo.m_blockSize = 4; // не имеет значения, пусть будет как на 1 канал
	m_dataInfo.m_bytesPerSample = 4;
	m_dataInfo.m_channels = channels;
	m_dataInfo.m_format = bqSoundFormat::float32; // всё миксуется единым форматом
	m_dataInfo.m_numOfSamples = bufferSizeForOneChannel / m_dataInfo.m_blockSize;
	m_dataInfo.m_sampleRate = info.m_sampleRate;
	m_dataInfo.m_time = bufferSizeForOneChannel / (m_dataInfo.m_sampleRate * m_dataInfo.m_bytesPerSample);;

	for (uint32_t i = 0; i < channels; ++i)
	{
		_channel* _new_channel = new _channel;

		_new_channel->m_data.m_dataSize = bufferSizeForOneChannel;
		_new_channel->m_data.m_data = (uint8_t*)bqMemory::malloc(_new_channel->m_data.m_dataSize);

		m_channels.push_back(_new_channel);
	}
}

bqSoundMixerImpl::~bqSoundMixerImpl()
{
	RemoveAllSounds();
	RemoveAllEffects();

	for (uint32_t i = 0; i < m_channels.m_size; ++i)
	{
		if (m_channels.m_data[i]->m_data.m_data)
		{
			bqMemory::free(m_channels.m_data[i]->m_data.m_data);
			m_channels.m_data[i]->m_data.m_data = 0;
		}

		delete m_channels.m_data[i];
		m_channels.m_data[i] = 0;
	}
}

void bqSoundMixerImpl::AddEffect(bqSoundEffect* effect)
{
	BQ_ASSERT_ST(effect);

	m_effects.push_back(effect);
}

void bqSoundMixerImpl::RemoveEffect(bqSoundEffect* effect)
{
	BQ_ASSERT_ST(effect);

	while (m_effects.erase_first(effect));
}

void bqSoundMixerImpl::RemoveAllEffects()
{
	m_effects.clear();
}

void bqSoundMixerImpl::AddSound(bqSound* sound)
{
	BQ_ASSERT_ST(sound);
	bqSoundMixerNode node;
	node.m_sound = sound;
	m_sounds.push_back(node);
}

void bqSoundMixerImpl::RemoveSound(bqSound* sound)
{
	BQ_ASSERT_ST(sound);

	struct
	{
		bool operator()(const bqSoundMixerNode& a, const bqSoundMixerNode& b) const
		{
			return a.m_sound == b.m_sound;
		}
	}compare;

	bqSoundMixerNode node;
	node.m_sound = sound;
	while (m_sounds.erase_first(node, compare));
}

void bqSoundMixerImpl::RemoveAllSounds()
{
	m_sounds.free_memory();
}

void bqSoundMixerImpl::Process()
{

}

void bqSoundMixerImpl::GetSoundBufferInfo(bqSoundBufferInfo& info)
{
	info = m_dataInfo;
}

uint32_t bqSoundMixerImpl::GetNumOfChannels()
{
	return (uint32_t)m_channels.m_size;
}

bqSoundBufferData* bqSoundMixerImpl::GetChannel(uint32_t i)
{
	BQ_ASSERT_ST(i < m_channels.m_size);

	if (i < m_channels.m_size)
	{
		return &m_channels.m_data[i]->m_data;
	}

	return 0;
}

void bqSoundMixerImpl::SetCallback(bqSoundMixerCallback* cb)
{
	if (cb)
	{
		m_callback = cb;
	}
	else
	{
		m_callback = &g_defaultCallback;
	}
}
