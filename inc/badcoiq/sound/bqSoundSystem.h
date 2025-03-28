﻿/*
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
#ifndef __BQ_SS_H__
/// \cond
#define __BQ_SS_H__
/// \endcond


#ifdef BQ_WITH_SOUND

#include "bqSound.h"
#include "bqSoundStream.h"
#include "bqSoundEffect.h"
#include "bqSoundMixer.h"
#include "bqSoundStreamFile.h"

/// информация о типе звука который воспроизводит аудио девайс
struct bqSoundSystemDeviceInfo
{
	/// Формат звука
	bqSoundFormat m_format = bqSoundFormat::unsupported;

	/// количество самплов в секунду
	uint32_t m_sampleRate = 0;

	/// количество каналов
	uint32_t m_channels = 0;

	/// размер того буфера который передаётся в звуковой драйвер
	/// Со всеми каналами
	uint32_t m_bufferSize = 0;

	/// количество битов на один сампл
	uint32_t m_bitsPerSample = 0;
};

/// * спрячу реализацию в наследнике
/// Перед использованием нужно инициализировать.
/// Перед завершением работы нужно убрать все миксеры
///  вызвав RemoveAllMixersFromProcessing()
class bqSoundSystem
{
public:
	bqSoundSystem() {};
	virtual ~bqSoundSystem() {};

	/// Создать объект для воспроизведения звука в отдельной нитке.
	/// Файл будет открыт для чтения и потихоньку будет происходить
	/// чтение.
	/// чтото пока не реализовано
	virtual bqSoundStream* SummonStream(const char*) = 0;

	/// Получить информацию о девайсе
	virtual bqSoundSystemDeviceInfo GetDeviceInfo() = 0;

	/// Создать миксер. 
	/// Параметры будут как GetDeviceInfo()
	/// Нужно только указать количество каналов.
	virtual bqSoundMixer* SummonMixer(uint32_t channels) = 0;

	/// Добавить миксер в аудиодвижок.
	/// Для воспроизведения звука нужно будет добавить.
	virtual void AddMixerToProcessing(bqSoundMixer*) = 0;

	/// Перед освобождением ресурсов нужно убрать миксеры из звукового движка
	virtual void RemoveAllMixersFromProcessing() = 0;

	virtual bqSoundEffectVolume* SummonEffectVolume() = 0;

	/// time - какая-то величина, не связана с реальным отсчётом времени
	/// чем выше тем дольше задержка между получившимся `эхо`
	/// steps - количество `эхо`. Ограничен 10 максимум.
	virtual bqSoundEffectDelay* SummonEffectDelay(uint32_t steps, uint32_t time) = 0;

};

#endif
#endif


