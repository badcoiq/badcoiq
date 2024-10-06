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
#ifndef __BQ_SoundBuffer_H__
/// \cond
#define __BQ_SoundBuffer_H__
/// \endcond

#ifdef BQ_WITH_SOUND
#include "badcoiq/containers/bqArray.h"
#include "badcoiq/cryptography/bqCryptography.h"

/// В данной структуре хранятся звуковые данные
struct bqSoundBufferData
{
	/// массив данных
	uint8_t* m_data = 0;

	/// размер массива
	uint32_t m_dataSize = 0;
};

/// Информация о звуковом буфере
/// по умолчанию звук формата float
struct bqSoundBufferInfo
{

	/// формат
	bqSoundFormat m_format = bqSoundFormat::float32;

	// всё должно быть вычислено правильно.
	// используется в генерации, конвертации и эффектах

	/// Количество самплов в секунду
	uint32_t m_sampleRate = 44100;

	/// количество каналов.
	/// 1 - моно, 2 - стерео
	uint32_t m_channels = 1;

	/// количество бит на один сампл
	uint32_t m_bitsPerSample = 32;

	/// размер в виде времени
	float m_time = 0.f;
	
	/// количество самплов
	uint32_t m_numOfSamples = 0;// когда 2 канала это 2 сэмпла или 1?
	                            // ответ - у каждого канала свой сэмпл.
								// при проигрывании имеем столько-то сэмплов 
								// сколько и каналов, но переменная m_numOfSamples
								// отвечает только за 1 канал, как и m_sampleRate.
	// количество самплов есть количество блоков
	//uint32_t numOfBlocks = m_soundBuffer->m_bufferData.m_dataSize / m_soundBuffer->m_bufferInfo.m_blockSize;
	// на основе времени
	// newSound->m_bufferInfo.m_numOfSamples = (uint32_t)ceil((float)newSound->m_bufferInfo.m_sampleRate * time);

	/// Количество байт на один сампл
	uint32_t m_bytesPerSample = 0; // m_bitsPerSample / 8;
	
	/// nBlockAlign
	/// блоком называется текущий сэмпл с каждого канала
	/// размер в байтах
	uint32_t m_blockSize = 0;// m_bytesPerSample * m_channels;

	/// Если объект был создан из файла то bqMD5 вычислится на основании пути к файлу.
	/// Сделано для быстрого сравнения (поиска) загруженного звука.
	bqMD5 m_md5;
};

/// возможно лучше это сунуть в bqSound
/// думал что bqSound будет выступать в качестве управляющего объекта
/// но теперь есть bqSoundObject
class bqSoundBuffer
{
	void _reallocate(uint32_t);
	bool _saveWav(const char* fn);
	bool _loadWav(const char* fn);
	bool _loadWav(uint8_t* buffer, uint32_t bufferSz);

public:
	bqSoundBuffer();
	~bqSoundBuffer();
	BQ_PLACEMENT_ALLOCATOR(bqSoundBuffer);

	void Clear();

	// Создать пустой bqSoundSource
	// time - время, должно быть больше 0
	// channels - 1 моно, 2 стерео, но можно указывать и более, только тогда хрен это проиграть
	// sampleRate - желательно 44100. тоже как channels, можно указать иное значение, но наверно не проиграть
	// format - format
	// метод просто выделяет память и вычисляет остальные значения для bqSoundSource
	//       поэтому можно указывать почти любые значения, но для воспроизведения нужны правильные
	//       значения. channels 1/2; sampleRate 44100; format bqSoundFormat::float32
	void Create(float time, uint32_t channels, uint32_t sampleRate, bqSoundFormat format);

	// loudness - громкость, от 0 до 1
	void Generate(bqSoundWaveType, float time, uint32_t frequency, float loudness = 0.5f);

	// расширение надо указывать в fn
	bool SaveToFile(bqSoundFileType, const char* fn);
	bool SaveToFile(bqSoundFileType, const bqStringA& fn);

	bool LoadFromFile(const char* fn);
	bool LoadFromFile(const bqStringA& fn);

	bqSoundBufferData m_bufferData;
	bqSoundBufferInfo m_bufferInfo;
	
	// Всё не поддерживаемое должно конвертироваться в поддерживаемое
	//bqSoundFormat m_format = bqSoundFormat::uint16_mono_44100;

	void Convert(bqSoundFormat);

	// how:
	//  0 - использовать первый канал
	//  1 - использовать второй канал
	void MakeMono(uint32_t how);

	void Make8bits();
	void Make16bits();
	void Make32bitsFloat();

	void Resample(uint32_t newSampleRate);

	// Вычислить время по битрейту и прочим параметрам
	float CalculateTime();

	void Append(bqSoundBuffer*);
	void Append(bqSoundBufferData*, bqSoundBufferInfo*);
};

#endif
#endif


