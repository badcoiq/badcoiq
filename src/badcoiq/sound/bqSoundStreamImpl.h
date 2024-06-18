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

#include "badcoiq/containers/bqFixedFIFO.h"
#include "badcoiq/sound/bqSoundSystem.h"

class bqSoundStreamImpl : public bqSoundStream
{
	bqSoundFile* m_file = 0;
	std::thread* m_thread = 0;
	void _thread_function();
	struct _thread_command;
	using ThreadCommanMethodType = void(bqSoundStreamImpl::*)(_thread_command*);
	struct _thread_command
	{
		void* m_ptr = 0;

		union
		{
			uint64_t _64bit;
			uint32_t _32bit[2];
			bqFloat32 _float32[2];
			uint16_t _16bit[4];
			uint8_t _8bit[8];
		};

		ThreadCommanMethodType m_method = 0;
	};
	struct _thread_context
	{
		bool m_run = true;
		bqFixedThreadFIFO<_thread_command, 150> m_commands;
	}m_threadContext;
	
	// Нужно несколько буферов. Потому что:
	// Формат звука в файле может отличаться от формата миксера (миксер всегда float32)
	// Действия:
	//  - читаем из файла в буфер для `данных из файла`
	//  - делаем звук двуканальным (засовываем в m_dataAfterChannels)
	//  - конвертируем в float32, нужен для этого новый буфер 'после конвертации'
	//  - изменяем sample rate, так-же нужен будет отдельный буфер `после resample`
	// Всё это в случае если форматы разные и sample rate не совпадает
	// А может быть так что что-то совпадает, форматы разные, а sample rate одинаковый.
	// Или наоборот, или всё совпадает (тогда надо игнорировать буферы 'после конвертации'
	// `после resample`).
	 
	// Почему по 2 штуки?
	// Чтение происходит по одной секунде.
	// Пока одна секунда будет играть, можно заранее прочитать ещё одну.
	// Необходимо определять сколько буферов готово для воспроизведения
	bqArray<uint8_t> m_dataFromFile[2];
	bqArray<uint8_t> m_dataAfterChannels[2];
	bqArray<uint8_t> m_dataAfterConvert[2];
	bqArray<uint8_t> m_dataAfterResample[2];
	
	// читаем и готовим буфер используя этот индекс
	// 0 или 1 так как использую 2 буфера
	int m_prepareBufferIndex = 0;
	// Когда готовим буфер, увеличиваем на единицу.
	// если значение = 2, то 2 буфера готово, больше готовить не надо.
	// Значение должно уменьшаться где-то, в моменте когда завершится
	// воспроизведение одного из буферов.
	int m_numOfPreparedBuffers = 0;

	// чтобы определить нужна ли конвертация использую указатель на метод
	// если указатель == 0 то конвертация не нужна
	// если != 0 то вызываем метод который и будет делать конвертацию
	// устанавливаться данный указатель должен в моменте открытия файла
	using _convertMethod = void(bqSoundStreamImpl::*)();
	_convertMethod _convert = 0;
	void _convertUint8();
	void _convertInt16();

	bqSoundSystemDeviceInfo m_deviceInfo;
	uint32_t m_fileSampleRate = 0;

	uint64_t m_dataSize = 0;

	bqArray<uint8_t>* m_dataActiveBufferPointer[2];

	bqSoundStreamCallback* m_callback = 0;

public:
	bqSoundStreamImpl();
	virtual ~bqSoundStreamImpl();
	BQ_PLACEMENT_ALLOCATOR(bqSoundStreamImpl);
	void ThreadCommand_CallMethod(ThreadCommanMethodType m, _thread_command* c)
	{
		(this->*m)(c);
	}
	void ThreadCommand_SetPlaybackPosition(float);
	void ThreadCommandMethod_SetPlaybackPostion(_thread_command*);

	virtual uint64_t GetDataSize() override;
	virtual void PlaybackStart() override;
	virtual void PlaybackStop() override;
	virtual void PlaybackReset() override;
	//virtual void PlaybackSet(uint32_t minutes, float seconds) override;
	//virtual void PlaybackSet(float secondsOnly) override;
	virtual bool Open(const char*) override;
	virtual void Close() override;
	virtual bool IsOpened() override;
	virtual uint32_t GetNumOfChannels() override;
	//virtual const bqSoundBufferInfo& GetBufferInfo() override;
	virtual uint32_t GetBlockSize() override;
	virtual void SetCallback(bqSoundStreamCallback*) override;

	// для гибкости при конвертировании и resample
	bqArray<uint8_t>* m_dataPointer = 0;
	uint32_t m_blockSize = 0;

	uint32_t m_blockSizeCurrent[2];

	bqArray<uint8_t>* GetActiveBuffer();// { return m_dataActiveBufferPointer[m_activeBufferIndex]; }

	// позиция текущего воспроизведения
	// в байтах. в массивах m_data____
	// если в миксере позиция ушла за пределы значит завершили обработку
	uint32_t m_dataPosition = 0;
	
	/// DELETE
	// позиция в файле перед началом воспроизведения секунды
	//uint64_t m_dataPositionInFile[2];
	//uint64_t m_currentDataPositionInFile = 0;
	
	// надо бы что-то сделать с этими массивами по 2 штуки которые
	//bool m_lastBuffer = false;
	// будет структура
	struct _playbackInfo
	{
	//	uint64_t m_dataPositionInFile = 0;
		bool m_lastBuffer = false;
	};
	_playbackInfo m_playbackInfo[2];
	
	// Тот буфер который будет использован в миксере.
	// Сначала 0, потом 1 потом опять 0 потом опять 1 и т.д.
	int m_activeBufferIndex = -1;
	// Чтобы реализовать смену m_activeBufferIndex 0->1...1->0
	// Потому что я сразу заготавливаю 2 буфера
	// Значения надо где-то сохранять.
	int m_activeBufferIndexNext[2] = {0,0};


	void _OnEndBuffer();
};

#endif
