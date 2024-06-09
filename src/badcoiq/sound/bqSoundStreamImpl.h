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
#ifndef _BQ_SNDSTREAMIMPL_H_
#define _BQ_SNDSTREAMIMPL_H_

#include "badcoiq/sound/bqSoundSystem.h"

class bqSoundStreamImpl : public bqSoundStream
{
	bqSoundFile* m_file = 0;
	std::thread* m_thread = 0;
	void _thread_function();
	struct _thread_context
	{
		bool m_run = true;
	}m_threadContext;
	
	// Нужно несколько буферов. Потому что:
	// Формат звука в файле может отличаться от формата миксера (миксер всегда float32)
	// Действия:
	//  - читаем из файла в буфер для `данных из файла`
	//  - конвертируем в float32, нужен для этого новый буфер 'после конвертации'
	//  - изменяем sample rate, так-же нужен будет отдельный буфер `после resample`
	// Всё это в случае если форматы разные и sample rate не совпадает
	// А может быть так что что-то совпадает, форматы разные, а sample rate одинаковый.
	// Или наоборот, или всё совпадает (тогда надо игнорировать буферы 'после конвертации'
	// `после resample`).
	 
	//uint8_t* m_fileData = 0;
	//uint32_t m_fileDataSize = 0;
	bqArray<uint8_t> m_dataFromFile;
	bqArray<uint8_t> m_dataAfterConvert;
	bqArray<uint8_t> m_dataAfterResample;

	// смотри код чтения, конвертирования в thread методе для ясности
	// типа, текущий указатель.
	// Он пойдёт в миксер.
	bqArray<uint8_t>* m_dataPointer = 0;

	// чтобы определить нужна ли конвертация использую указатель на метод
	// если указатель == 0 то конвертация не нужна
	// если != 0 то вызываем метод который и будет делать конвертацию
	// устанавливаться данный указатель должен в моменте открытия файла
	using _convertMethod = void(bqSoundStreamImpl::*)();
	_convertMethod _convert = 0;

	bqSoundSystemDeviceInfo m_deviceInfo;
	uint32_t m_fileSampleRate = 0;

public:
	bqSoundStreamImpl();
	virtual ~bqSoundStreamImpl();
	BQ_PLACEMENT_ALLOCATOR(bqSoundStreamImpl);

	virtual void PlaybackStart() override;
	virtual void PlaybackStop() override;
	virtual void PlaybackReset() override;
	virtual void PlaybackSet(uint32_t minutes, float seconds) override;
	virtual void PlaybackSet(float secondsOnly) override;
	virtual bool Open(const char*) override;
	virtual void Close() override;
	virtual bool IsOpened() override;
};

#endif
