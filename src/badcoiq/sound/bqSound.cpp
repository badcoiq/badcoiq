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

#ifdef BQ_WITH_SOUND

#include "badcoiq/sound/bqSoundSystem.h"
#include "badcoiq/math/bqMath.h"
#include "badcoiq/common/bqFileBuffer.h"

uint32_t bqSound_GetPlaybackPosition(bqFloat64 secondsOnly, const bqSoundBufferInfo& info)
{
	// Если m_sampleRate == 41000
	// То при 1 секунде значение должно стать 41000 
	//    если однобайтовый и одноканальный
	// f = SRate * Sec * BSize

	bqFloat64 f = (bqFloat64)info.m_sampleRate;
	f = f * (bqFloat64)secondsOnly;
	f = f * (bqFloat64)info.m_blockSize;
	return (uint32_t)floor(f);
}

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

void bqSound::PlaybackSet(uint32_t minutes, float seconds)
{
	if (seconds > 60.f)	seconds = 0.f;
	PlaybackSet(seconds + ((float)minutes * 60.f));
}

void bqSound::PlaybackSet(float secondsOnly)
{
	BQ_ASSERT_ST(m_soundBuffer);

	if (secondsOnly == 0.f)
	{
		m_playbackPosition = 0;
	}
	else
	{
		m_playbackPosition = bqSound_GetPlaybackPosition(secondsOnly, m_soundBuffer->m_bufferInfo);
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


void bqSound::SetRegion(uint32_t minutesStart, float secondsStart,
	uint32_t minutesStop, float secondsStop)
{
	if (secondsStart > 60.f)	secondsStart = 0.f;
	if (secondsStop > 60.f)	secondsStop = 0.f;
	SetRegion(secondsStart + ((float)minutesStart * 60.f),
		secondsStop + ((float)minutesStop * 60.f));
}

void bqSound::SetRegion(float secondsStart, float secondsStop)
{
	m_useRegion = true;
	m_regionBegin = m_regionEnd = 0;

	if (secondsStart < 0.f)
		secondsStart = 0.f;
	if (secondsStop < 0.f)
		secondsStop = 0.f;

	if (secondsStart >= secondsStop)
	{
		secondsStart = 0.f;
		secondsStop = 0.f;
	}

	if(secondsStart == 0.f && secondsStop == 0.f)
		m_useRegion = false;

	if (m_useRegion)
	{
		m_regionBegin = bqSound_GetPlaybackPosition(secondsStart, m_soundBuffer->m_bufferInfo);
		m_regionEnd = bqSound_GetPlaybackPosition(secondsStop, m_soundBuffer->m_bufferInfo);
		PlaybackSet(m_regionBegin);
	}
}

void bqSound::Update3D()
{
	m_volume3DLeft = 0.f;
	m_volume3DRight = 0.f;
	bqVec4 relPos = m_positionSound - m_listenerPosition;
	bqReal distance = bqMath::Distance(bqZeroVector, relPos);
	if (distance <= m_3DFar)
	{
		relPos.Normalize();
		m_listenerDirection.Normalize();

		auto dot = bqMath::Dot(relPos, m_listenerDirection);
		printf("DOT %f\n", dot);
	}
}

#endif
