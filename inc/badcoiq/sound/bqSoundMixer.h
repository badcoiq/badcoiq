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
#ifndef __BQ_SOUNDMIXER_H__
#define __BQ_SOUNDMIXER_H__

class bqSoundMixerCallback;

// Класс который смешивает звуки в одно целое.
// Должны быть реализованы эффекты.
// Звуки должны добавляться, возможно в специальную оболочку, так как надо запоминать текущую позицию воспроизведения.
class bqSoundMixer
{
public:
	BQ_PLACEMENT_ALLOCATOR(bqSoundMixer);

	bqSoundMixer(){}
	virtual ~bqSoundMixer(){}

	//virtual void SetVolume(float) = 0;
	//virtual float GetVolume() = 0;
	// громкость должна быть реализована эффектом

	// Добавить эффект. Один эффект можно добавить множество раз.
	virtual void AddEffect(bqSoundEffect*) = 0;

	// Убрать эффект.
	virtual void RemoveEffect(bqSoundEffect*) = 0;

	// Убрать все эффекты
	virtual void RemoveAllEffects() = 0;

	virtual bqList<bqSoundEffect*>* GetEffects() = 0;

	// Добавить звук. Добавляется множество раз.
	// Звук должен быть формата float32
	virtual void AddSound(bqSound*) = 0;

	// Убрать звук.
	virtual void RemoveSound(bqSound*) = 0;

	// Убрать все звуки
	virtual void RemoveAllSounds() = 0;

	// Миксер имеет буферы. отдельный на каждый канал.
	// Эти буферы надо заполнить звуками и обработать эффектами.
	// Потом эти буферы будут смешаны с главным миксером, и
	// буферы из главного миксера будут скопированы в буфер
	// звукового движка.
	virtual void Process() = 0;

	// Получить информацию о буфере используемом в миксере
	virtual void GetSoundBufferInfo(bqSoundBufferInfo&) = 0;

	// Получить количество каналов
	virtual uint32_t GetNumOfChannels() = 0;

	// Получить канал.
	virtual bqSoundBufferData* GetChannel(uint32_t) = 0;

	virtual void SetCallback(bqSoundMixerCallback*) = 0;

	// Установить, будет ли работать метод Process.
	// По умолчанию уже установлено true.
	// Если false то метод Process будет игнорироваться,
	// а другие миксеры будут игнорировать использование этого миксера.
	// Воспроизведение движком будет тоже игнорироваться.
	virtual void UseProcessing(bool) = 0;

	// Если обработка не игнорируется то вернётся true
	virtual bool IsProcessing() = 0;
};

class bqSoundMixerCallback
{
public:
	bqSoundMixerCallback() {}
	virtual ~bqSoundMixerCallback() {}
	
	virtual void OnStartProcess(bqSoundMixer*) = 0;

	// Когда завершается смешивание звука в общую кучу.
	virtual void OnEndMixSound(bqSoundMixer*, bqSound*) = 0;
	
	// Когда прошлись по всем звукам.
	virtual void OnEndProcess(bqSoundMixer*) = 0;

	// Когда звук обработан полностью.
	virtual void OnEndSound(bqSoundMixer*, bqSound*) = 0;

	// Действия миксера примерно такие
	/*
		for(sounds)
		{
			mix...
			cb->OnEndMixSound(sound);

			if(sound->position == sound->end)
				cb->OnEndSound(sound);
		}

		cb->OnEndProcess();
	*/

	// Когда буфер миксера заполнен
	virtual void OnFullBuffer(bqSoundMixer*) = 0;
};

#endif
