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
#ifndef __BQ_SS_H__
#define __BQ_SS_H__

#include "bqSound.h"
#include "bqSoundObject.h"
#include "bqSoundFile.h"

struct bqSoundSystemDeviceInfo
{
	bqSoundFormat m_format = bqSoundFormat::unsupported;
	uint32_t m_sampleRate = 0;
	uint32_t m_channels = 0;
};

// * спрячу реализацию в наследнике
// Перед использованием нужно инициализировать.
class bqSoundSystem
{
public:
	bqSoundSystem() {};
	virtual ~bqSoundSystem() {};
	
	// Создать объект из имеющегося звука
	virtual bqSoundObject* SummonObject(bqSound*) = 0;

	// Создать объект для воспроизведения звука в отдельной нитке.
	// Файл будет открыт для чтения и потихоньку будет происходить
	// чтение.
	virtual bqSoundStreamObject* SummonStreamObject(const char*) = 0;
	virtual bqSoundStreamObject* SummonStreamObject(const bqStringA&) = 0;

	virtual bqSoundSystemDeviceInfo GetDeviceInfo() = 0;
};

#endif


