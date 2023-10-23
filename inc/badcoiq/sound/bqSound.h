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
#ifndef __BQ_Sound_H__
#define __BQ_Sound_H__

#include "badcoiq/containers/bqArray.h"

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

// возможно лучше это сунуть в bqSound
// думал что bqSound будет выступать в качестве управляющего объекта
// но теперь есть bqSoundObject
class bqSoundSource
{
public:
	bqSoundSource();
	~bqSoundSource();
	BQ_PLACEMENT_ALLOCATOR(bqSoundSource);

	uint8_t* m_data = 0;
	uint32_t m_size = 0;
	uint32_t m_sampleRate = 44100;
	uint32_t m_channels = 1;
	uint32_t m_bits = 16;
};

// звук загружается сюда
class bqSound
{
public:
	bqSound();
	virtual ~bqSound();
	BQ_PLACEMENT_ALLOCATOR(bqSound);

// loudness - громкость, от 0 до 1
	void Generate(bqSoundWaveType, float time, uint32_t frequency, float loudness = 1.f);

// расширение надо указывать в fn
bool SaveToFile(bqSoundFileType, const char* fn);

void Clear();

	bqSoundSource* m_soundSource = 0;
};

#endif


