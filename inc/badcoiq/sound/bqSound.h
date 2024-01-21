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

struct bqSoundBufferData
{
	uint8_t* m_data = 0;
	uint32_t m_dataSize = 0;
};

enum class bqSoundFormat
{
	uint8,
	int16,
	int24,
	int32,
	float32,
	float64,
	unsupported
};

//bqSoundFormat bqSoundFormatFindFormat(const bqSoundBufferInfo&);


struct bqSoundBufferInfo
{
	bqSoundFormat m_format = bqSoundFormat::int16;

	// всё должно быть вычислено правильно.
	// используется в генерации, конвертации и эффектах

	uint32_t m_sampleRate = 44100;
	uint32_t m_channels = 1;
	uint32_t m_bitsPerSample = 16;

	float m_time = 0.f;
	
	uint32_t m_numOfSamples = 0;// когда 2 канала это 2 сэмпла или 1?
	                            // ответ - у каждого канала свой сэмпл.
								// при проигрывании имеем столько-то сэмплов 
								// сколько и каналов, но переменная m_numOfSamples
								// отвечает только за 1 канал, как и m_sampleRate.
								
	// Тогда количество сэмплов это размер данных делёная на размер 1го сэмпла
	//m_info.m_numOfSamples = dataSize / m_blockSize;

	uint32_t m_bytesPerSample = 0; // m_bitsPerSample / 8;
	// nBlockAlign
	uint32_t m_blockSize = 0;// m_bytesPerSample * m_channels;
};

// возможно лучше это сунуть в bqSound
// думал что bqSound будет выступать в качестве управляющего объекта
// но теперь есть bqSoundObject
class bqSoundBuffer
{
public:
	bqSoundBuffer();
	~bqSoundBuffer();
	BQ_PLACEMENT_ALLOCATOR(bqSoundBuffer);

	bqSoundBufferData m_bufferData;
	bqSoundBufferInfo m_bufferInfo;
	
	// Всё не поддерживаемое должно конвертироваться в поддерживаемое
	//bqSoundFormat m_format = bqSoundFormat::uint16_mono_44100;

	
	// how:
	//  0 - использовать первый канал
	//  1 - использовать второй канал
	void MakeMono(uint32_t how);

	void Make8bits();
	void Make16bits();
	void Make32bitsFloat();

	void Resample(uint32_t newSampleRate);
};

// звук загружается сюда
class bqSound
{
	void _reallocate(uint32_t);
	bool _saveWav(const char* fn);
	bool _loadWav(const char* fn);
	bool _loadWav(uint8_t* buffer, uint32_t bufferSz);
public:
	bqSound();
	virtual ~bqSound();
	BQ_PLACEMENT_ALLOCATOR(bqSound);

	// Создать пустой bqSoundSource
	// time - время, должно быть больше 0
	// channels - 1 моно, 2 стерео, но можно указывать и более, только тогда хрен это проиграть
	// sampleRate - желательно 44100. тоже как channels, можно указать иное значение, но наверно не проиграть
	// bitsPerSample - количество бит на сэмпл. 8, 16. можно указать иное значение
	// метод просто выделяет память и вычисляет остальные значения для bqSoundSource
	//       поэтому можно указывать почти любые значения, но для воспроизведения нужны правильные
	//       значения. channels 1/2; sampleRate 44100; bitsPerSample 16
	void Create(float time, uint32_t channels, uint32_t sampleRate, uint32_t bitsPerSample);

	// loudness - громкость, от 0 до 1
	void Generate(bqSoundWaveType, float time, uint32_t frequency, float loudness = 0.5f);

	// расширение надо указывать в fn
	bool SaveToFile(bqSoundFileType, const char* fn);
	bool SaveToFile(bqSoundFileType, const bqStringA& fn);
	
	bool LoadFromFile(const char* fn);
	bool LoadFromFile(const bqStringA& fn);

	void Clear();

	void Convert(bqSoundFormat);

	// Вычислить время по битрейту и прочим параметрам
	float CalculateTime();

	bqSoundBuffer* m_soundBuffer = 0;
};

#endif


