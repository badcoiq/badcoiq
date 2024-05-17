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
#include "badcoiq/math/bqMath.h"
#include "badcoiq/common/bqFileBuffer.h"

bqSound::bqSound()
{
}

bqSound::~bqSound()
{
	if (m_soundBuffer && m_hasItsOwnSound)
	{
		delete m_soundBuffer;
		m_soundBuffer = 0;
	}
}

bqSound::bqSound(uint32_t channels, uint32_t sr, bqSoundFormat f, float t)
{
	if (t < 0.001f)
		t = 0.001f;
	m_soundBuffer = new bqSoundBuffer;
	m_soundBuffer->Create(t, channels, sr, f);
	m_hasItsOwnSound = true;
}

void bqSound::PlaybackStart()
{
	m_state = state_playing;
}

void bqSound::PlaybackStop()
{
	m_state = state_notPlaying;
}

void bqSound::PlaybackReset()
{
	m_state = state_notPlaying;
	PlaybackSet(0.f);
}

void bqSound::PlaybackSet(float minutes, float seconds)
{
}

void bqSound::PlaybackSet(float secondsOnly)
{
	if (secondsOnly == 0.f)
	{
		m_playbackPosition = 0;
	}
	else
	{

	}
}

void bqSound::SetLoop(uint32_t v)
{
	m_loop = v;
}

uint32_t bqSound::GetLoopNumber()
{
	return m_loop;
}


void bqSound::SetRegion(float minutesStart, float secondsStart,
	float minutesStop, float secondsStop)
{
}

void bqSound::SetRegion(float secondsStart, float secondsStop)
{
}


