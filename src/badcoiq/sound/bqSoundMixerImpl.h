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
#pragma once
#ifndef _BQ_SNDMIXIMPL_H_
#define _BQ_SNDMIXIMPL_H_

#include "badcoiq/sound/bqSoundSystem.h"

#include <map>

// один звук может находится во множестве миксерах
// каждый миксер будет воспроизводить его по своему
// надо знать определённые вещи у звука
// например позиция воспроизведения
struct bqSoundMixerNode
{
	bqSound* m_sound = 0;
	uint32_t m_position = 0;

	bqSoundMixerNode& operator=(const bqSoundMixerNode& other) 
	{
		m_sound = other.m_sound;
		m_position = other.m_position;
		return *this; 
	}

	bqSoundMixerNode& operator=(int) 
	{
		zero();
		return *this; 
	}

	void zero()
	{
		m_sound = 0;
		m_position = 0;
	}
};

class bqSoundMixerImpl : public bqSoundMixer
{
	bqArray<bqSoundMixerNode> m_sounds;
	bqList<bqSoundEffect*> m_effects;

	uint32_t m_bufferSizeForOneChannel = 0;
	bqSoundBufferInfo m_dataInfo;
	struct _channel
	{
		bqSoundBufferData m_data;
	};
	bqArray<_channel*> m_channels;

	bqSoundMixerCallback* m_callback = 0;

	void _processEffects(int channel);
public:
	BQ_PLACEMENT_ALLOCATOR(bqSoundMixer);
	
	bqSoundMixerImpl(uint32_t channels, const bqSoundSystemDeviceInfo& );
	virtual ~bqSoundMixerImpl();

	virtual void AddEffect(bqSoundEffect*) override;
	virtual void RemoveEffect(bqSoundEffect*) override;
	virtual void RemoveAllEffects() override;
	virtual bqList<bqSoundEffect*>* GetEffects() override;

	virtual void AddSound(bqSound*) override;
	virtual void RemoveSound(bqSound*) override;
	virtual void RemoveAllSounds() override;

	virtual void Process() override;

	virtual void GetSoundBufferInfo(bqSoundBufferInfo&) override;
	virtual uint32_t GetNumOfChannels() override;
	virtual bqSoundBufferData* GetChannel(uint32_t) override;
	virtual void SetCallback(bqSoundMixerCallback*) override;

};
#endif
