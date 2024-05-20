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
#ifndef __BQ_Sound_H__
#define __BQ_Sound_H__

#ifdef BQ_WITH_SOUND

#include "badcoiq/containers/bqArray.h"
#include "badcoiq/math/bqMath.h"

enum class bqSoundFileType
{
	wav
};

enum class bqSoundWaveType
{
	sin,
	square,
	triangle,
	saw
};

enum class bqSoundFormat
{
	uint8,
	int16,
	int24,
	int32,
	float32,
	//float64,
	unsupported
};

#include "badcoiq/sound/bqSoundBuffer.h"

// По умолчанию bqSound не удаляет m_soundBuffer
// так как предполагается что звук из m_soundBuffer
// может быть использован во множестве bqSound.
// Если bqSound создан с использованием спец. конструктора
// то деструктор уничтожит m_soundBuffer. Этот спец.конструктор
// создан для упрощённого тестирования функциональности. 
//
class bqSound
{

// В основе воспроизведения стоит миксер, который использует
// m_soundBuffer. Воспроизведение будет бесконечным.
// Для управления воспроизведением дополнительные нужны данные,
// например,чтобы миксер знал, нужно ли "играть" данный звук.
	enum
	{
		state_notPlaying,
		state_playing,
	};
	uint32_t m_state = state_notPlaying;

	// Указывает на байт находящийся в начале блока.
	uint32_t m_playbackPosition = 0;

	// если m_loop == -1
	// то повторение бесконечное
	uint32_t m_loop = 0;

	bool m_useRegion = false;
	uint32_t m_regionBegin = 0;
	uint32_t m_regionEnd = 0;
public:
	bqSound();
	bqSound(uint32_t channels, uint32_t samplerate, bqSoundFormat, float time = 0.001f);
	virtual ~bqSound();
	BQ_PLACEMENT_ALLOCATOR(bqSound);

	bool IsPlaying() { return m_state == state_playing; }
	uint32_t GetPlaybackPosition() { return m_playbackPosition; }
	void SetPlaybackPosition(uint32_t v) { m_playbackPosition = v; }

	void PlaybackStart();
	void PlaybackStop();
	void PlaybackReset();
	void PlaybackSet(uint32_t minutes, float seconds);
	void PlaybackSet(float secondsOnly);

	void SetLoop(uint32_t);
	uint32_t GetLoopNumber();

	void SetRegion(uint32_t minutesStart, float secondsStart,
		uint32_t minutesStop, float secondsStop);
	void SetRegion(float secondsStart, float secondsStop);
	bool IsRegion() { return m_useRegion; }
	uint32_t GetRegionBegin() { return m_regionBegin; }
	uint32_t GetRegionEnd() { return m_regionEnd; }

	bqSoundBuffer* m_soundBuffer = 0;
	bool m_hasItsOwnSound = false;

	float m_volume = 0.5f;
	
	float m_pitch = 1.0f;
	float m_pitchLimitUp = 3.f;
	float m_pitchLimitDown = 3.f;

	// 3D
	bool m_use3D = false;
	bqVec4 m_positionSound;
	bqVec4 m_listenerPosition;
	bqVec4 m_listenerDirection;
	bool m_useDoppler = false;
	float m_Doppler = 1.f;
	// Far - с какого расстояния начинает слышится звук
	// Near - с какого расстояния звук слишится на полную громкость
	float m_3DFar = 10.f;
	float m_3DNear = 1.f;
	// эти значения вычисляются в Update3D();
	float m_volume3DLeft = 1.f;
	float m_volume3DRight = 1.f;
	void Update3D();

public:

};

#endif

#endif
