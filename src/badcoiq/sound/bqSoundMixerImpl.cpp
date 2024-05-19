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

#include "bqSoundMixerImpl.h"

#include "../framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;

class bqSoundMixerCallbackDefault : public bqSoundMixerCallback
{
public:
	bqSoundMixerCallbackDefault() {}
	virtual ~bqSoundMixerCallbackDefault() {}
	virtual void OnStartProcess(bqSoundMixer*) {};
	//virtual void OnEndMixSound(bqSoundMixer*, bqSound*) {}
	virtual void OnEndProcess(bqSoundMixer*) {}
	virtual void OnEndSound(bqSoundMixer*, bqSound*) {}
	//virtual void OnFullBuffer(bqSoundMixer*) {}
};

static bqSoundMixerCallbackDefault g_defaultCallback;

bqSoundMixerImpl::bqSoundMixerImpl(uint32_t channels, const bqSoundSystemDeviceInfo& info)
{
	BQ_ASSERT_ST(channels > 0);
	BQ_ASSERT_ST(channels < 10);
	BQ_ASSERT_ST(info.m_bufferSize);
	BQ_ASSERT_ST(info.m_channels);

	if (channels == 0)
		channels = 1;
	if (channels > 10)
		channels = 10;

	m_callback = &g_defaultCallback;

	m_bufferSizeForOneChannel = info.m_bufferSize / info.m_channels;

	m_dataInfo.m_bitsPerSample = 32;
	m_dataInfo.m_bytesPerSample = m_dataInfo.m_bitsPerSample / 8;
	m_dataInfo.m_blockSize = m_dataInfo.m_bytesPerSample * channels;
	m_dataInfo.m_channels = channels;
	m_dataInfo.m_format = bqSoundFormat::float32; // всё миксуется единым форматом
	m_dataInfo.m_numOfSamples = m_bufferSizeForOneChannel / m_dataInfo.m_blockSize;
	m_dataInfo.m_sampleRate = info.m_sampleRate;
	m_dataInfo.m_time = (float)m_bufferSizeForOneChannel / ((float)m_dataInfo.m_sampleRate * (float)m_dataInfo.m_bytesPerSample);;

	for (uint32_t i = 0; i < channels; ++i)
	{
		_channel* _new_channel = new _channel;

		_new_channel->m_data.m_dataSize = m_bufferSizeForOneChannel;
		_new_channel->m_data.m_data = (uint8_t*)bqMemory::malloc(_new_channel->m_data.m_dataSize);

		m_channels.push_back(_new_channel);
	}

	for (uint32_t i = 0; i < 2; ++i)
	{
		_channel* _new_channel = new _channel;

		_new_channel->m_data.m_dataSize = m_bufferSizeForOneChannel;
		_new_channel->m_data.m_data = (uint8_t*)bqMemory::malloc(_new_channel->m_data.m_dataSize);

		m_channelsTmp.push_back(_new_channel);
	}
}

bqSoundMixerImpl::~bqSoundMixerImpl()
{
	RemoveAllSounds();
	RemoveAllEffects();

	for (uint32_t i = 0; i < m_channelsTmp.m_size; ++i)
	{
		if (m_channelsTmp.m_data[i]->m_data.m_data)
		{
			bqMemory::free(m_channelsTmp.m_data[i]->m_data.m_data);
			m_channelsTmp.m_data[i]->m_data.m_data = 0;
		}

		delete m_channelsTmp.m_data[i];
		m_channelsTmp.m_data[i] = 0;
	}

	for (uint32_t i = 0; i < m_channels.m_size; ++i)
	{
		if (m_channels.m_data[i]->m_data.m_data)
		{
			bqMemory::free(m_channels.m_data[i]->m_data.m_data);
			m_channels.m_data[i]->m_data.m_data = 0;
		}

		delete m_channels.m_data[i];
		m_channels.m_data[i] = 0;
	}
}

void bqSoundMixerImpl::AddEffect(bqSoundEffect* effect)
{
	BQ_ASSERT_ST(effect);

	m_effects.push_back(effect);
}

void bqSoundMixerImpl::RemoveEffect(bqSoundEffect* effect)
{
	BQ_ASSERT_ST(effect);

	while (m_effects.erase_first(effect));
}

void bqSoundMixerImpl::RemoveAllEffects()
{
	m_effects.clear();
}

void bqSoundMixerImpl::AddSound(bqSound* sound)
{
	BQ_ASSERT_ST(sound);
	BQ_ASSERT_ST(sound->m_soundBuffer);
	
	bqSoundSystem* ss = (bqSoundSystem*)g_framework->m_soundSystem;

	auto deviceInfo = ss->GetDeviceInfo();

	if (sound->m_soundBuffer->m_bufferInfo.m_format != deviceInfo.m_format)
	{
		bqLog::PrintWarning("SoundMixer: unable to add sound. Wrong format.\n");
		return;
	}

	if (sound->m_soundBuffer->m_bufferInfo.m_sampleRate != deviceInfo.m_sampleRate)
	{
		bqLog::PrintWarning("SoundMixer: unable to add sound. Wrong sample rate.\n");
		return;
	}

	m_sounds.push_back(sound);
}

void bqSoundMixerImpl::RemoveSound(bqSound* sound)
{
	BQ_ASSERT_ST(sound);

	struct _compare
	{
		bool operator()(bqSound* a, bqSound* b) const
		{
			return a == b;
		}
	}compare;

	while (m_sounds.erase_first(sound, compare));
}

void bqSoundMixerImpl::RemoveAllSounds()
{
	m_sounds.free_memory();
}

void bqSoundMixerImpl::Process()
{
	if (!m_useProcessing)
		return;

	// НАДО ВСЁ ОЧИСТИТЬ
	// возможно можно будет убрать или вставить условие чтобы зря не проходиться циклом
	// так как ниже в большинстве случаев буфер и так будет заполнен значениями
	for (size_t i = 0; i < m_channels.m_size; ++i)
	{
		bqSoundBufferData* _channel = &(m_channels.m_data[i])->m_data;
		
		uint8_t* dataMixer8 = _channel->m_data;
		bqFloat32* dataMixer32 = (bqFloat32*)dataMixer8;

		size_t isz = m_bufferSizeForOneChannel / sizeof(bqFloat32);

		for (size_t ii = 0; ii < isz; ++ii)
		{
			dataMixer32[ii] = 0.f;
		}
	}

	m_callback->OnStartProcess(this);

	for (size_t si = 0; si < m_sounds.m_size; ++si)
	{
		bqSound* sound = m_sounds.m_data[si];

		if (!sound->IsPlaying())
			continue;

		auto soundChannelsNum = sound->m_soundBuffer->m_bufferInfo.m_channels;

		uint32_t sPos = sound->GetPlaybackPosition(); // в байтах

		bool isOnEnd = false;


		// Заполняю временные буферы m_channelsTmp
		size_t isz = sound->m_soundBuffer->m_bufferData.m_dataSize;// / sizeof(bqFloat32);


		// Надо знать какой по счёту идёт блок из sound->m_soundBuffer
		uint32_t blockCounter = 0;
		float pitchCounter = 0.f;
		
		

		for (size_t i = 0, i_tmp = 0, last = isz - 1; i < isz; ) // проход по звуку. 
		{
			// если заполнили m_channelsTmp то
			// * выход из цикла
			//
			// !!! Размер звука может быть больше размера одного канала
			// поэтому выход из цикла скорее всего будет здесь
			if (i >= m_bufferSizeForOneChannel)
			{
				break;
			}

			uint8_t* dataSound8 = sound->m_soundBuffer->m_bufferData.m_data;
			bqFloat32* dataSound32 = (bqFloat32*)(&dataSound8[sPos]);

			// Сразу возьму звук. + умножу на значение громкости
			bqFloat32 ch1 = *dataSound32 * sound->m_volume;
			bqFloat32 ch2 = ch1;
			// Если каналов больше то беру второй
			if (soundChannelsNum > 1)
			{
				++dataSound32;
				ch2 = *dataSound32 * sound->m_volume;
			}

			// далее надо прокрутить sPos
			// прибавляем m_blockSize потому что 1 или 2 канала взяли, нужно
			// перейти на след. сампл.
			sPos += sound->m_soundBuffer->m_bufferInfo.m_blockSize;

			// для pitch скорее всего достаточно модифицировать sPos
			if (sound->m_pitch != 1.f)
			{
				if (sound->m_pitch < 0.001f)
					sound->m_pitch = 0.001f;
				if (sound->m_pitch > 10.00f)
					sound->m_pitch = 10.00f;

				if (sound->m_pitch > 1.f)
				{
					if (pitchCounter > sound->m_pitchLimitUp)
					{
						sPos += sound->m_soundBuffer->m_bufferInfo.m_blockSize;
						pitchCounter = 0;
					}
				//	i += m_dataInfo.m_bytesPerSample;
					pitchCounter += sound->m_pitch - 1.f;
				}
				else
				{
					if (pitchCounter > sound->m_pitchLimitDown)
					{
						sPos -= sound->m_soundBuffer->m_bufferInfo.m_blockSize;
						pitchCounter = 0;
					}
					//	i += m_dataInfo.m_bytesPerSample;
					pitchCounter += 1.f - sound->m_pitch;
				}
			}

			// i тоже крутим
			// тут прибавляем m_bytesPerSample потому что i отвечает за 1 канал
			// вот такой вот хаос здесь. возможно нужно будет добавить отдельный индекс
			// пока i играет 2 роли (проход по звуку целиком и выход из цикла при заполнении m_channelsTmp)
			i += m_dataInfo.m_bytesPerSample;

			// теперь надо передать звук во временный буфер
			uint8_t* dataTmp8 = (uint8_t*)m_channelsTmp.m_data[0]->m_data.m_data;
			bqFloat32* dataTmp32 = (bqFloat32*)dataTmp8;
			dataTmp32[i_tmp] = ch1;

			dataTmp8 = (uint8_t*)m_channelsTmp.m_data[1]->m_data.m_data;
			dataTmp32 = (bqFloat32*)dataTmp8;
			dataTmp32[i_tmp] = ch2;

			++i_tmp;

			// Если позиция звука вышла за пределы массива звука
			if (sPos >= sound->m_soundBuffer->m_bufferData.m_dataSize)
			{
				// если вышла за пределы то обнуление
				// при pitch нужно ли что-то иное пока хз
				sPos = 0;

				isOnEnd = true;

				break;
			}

			if (i >= last)
			{
				isOnEnd = true;
				break;
			}

			++blockCounter;
		}

		

		// Для pitch нужно будет по особому изменять значение soundNode.m_position
		// Для простоты буду использовать sPos
		// Типа, произвёл все действия, и присвоил это soundNode.m_position
		sound->SetPlaybackPosition(sPos);

		// теперь смешиваю временные m_channelsTmp с каналами миксера
		for (uint32_t bi = 0; bi < m_bufferSizeForOneChannel; )
		{
			for (uint32_t ci = 0; ci < m_channels.m_size; ++ci)
			{
				uint8_t* dst8 = (uint8_t*)m_channels.m_data[ci]->m_data.m_data;
				dst8 += bi;
				bqFloat32* dst32 = (bqFloat32*)dst8;

				if (ci < m_channelsTmp.m_size)
				{
					uint8_t* src8 = (uint8_t*)m_channelsTmp.m_data[ci]->m_data.m_data;
					src8 += bi;
					bqFloat32* src32 = (bqFloat32*)src8;

					bqFloat32 v = *src32;

					// Если миксер имеет 1 канал а звук более 1го то нужно
					// присваивать сумму значений.
					// В данном случае используется временные каналы, их 2 штуки.
					if (m_channels.m_size == 1)
					{
						src8 = (uint8_t*)m_channelsTmp.m_data[ci+1]->m_data.m_data;
						src8 += bi;
						src32 = (bqFloat32*)src8;
						v += *src32;
					}

					*dst32 += v * m_mixerVolume;

					if (*dst32 > 1.f) *dst32 = 1.f;
					if (*dst32 < -1.f) *dst32 = -1.f;
				}
			}

			bi += m_dataInfo.m_bytesPerSample;
		}

		if (sound->IsRegion())
		{
			if (sound->GetPlaybackPosition() >= sound->GetRegionEnd())
			{
				sound->PlaybackSet(sound->GetRegionBegin());
			}
		}

		if (isOnEnd)
		{
			auto loop = sound->GetLoopNumber();
			if (loop)
			{
				if (loop != (uint32_t)-1)
				{
					sound->SetLoop(--loop);
				}
			}
			else
			{
				sound->PlaybackStop();
			}

			if (sound->IsRegion())
			{
				sound->PlaybackSet(sound->GetRegionBegin());
			}

			m_callback->OnEndSound(this, sound);
			isOnEnd = false;
		}
	}
		// Вот такая ситуация может быть:
		/*
		* Изобразим графически
		*
		* Есть буфер канала
		*                 |-------------------------------|
		*
		* Есть звук,      |===| размером он меньше буфера канала
		*
		* Если надо сделать непрерывное воспроизведение звука
		* то буфер канала надо будет заполнять полностью
		*                 |-------------------------------|
		*                 |===||===||===||===||===||===||==
		*
		* Потом, звук может быть больше буфера канала.
		* В этом случае остаток так-же должен быть скопирован.
		*
		* *                 |-------------------------------| первый проход
		*                   |================================
		*
		* *                 |-------------------------------| второй проход
		*                   =======||======================== конец звука скопировали, начали копировать опять.
		*
		* Правда ли что это должно быть реализовано? Для чего это нужно?
		* Если имеем звук который надо воспроизвести 1 раз, то проблем не будет.
		* Он будет обработан от начала и до конца.
		* Но если нужно воспроизводить звук постоянно, то, при повторном воспроизведении,
		* если ничего не предпринимать, будут микропаузы.
		* Возможно, из-за маленького размера буфера канала, эти микропаузы будут незаметны.

		*/

	// добавить буферы из других миксеров будет проще
	// размеры у всех одинаковые (должно быть)
	// надо только будет распределять каналы
	for (size_t mi = 0; mi < m_mixers.m_size; ++mi)
	{
		auto input_mixer = reinterpret_cast<bqSoundMixerImpl*>(m_mixers.m_data[mi]);

		// по сути надо делать конвертацию если какие-то параметрыне совпадают
		// Пока без конвертации.
		// Потом, чтобы не плодить if будет вызываться метод по указателю.
		
		if (input_mixer->m_dataInfo.m_format == m_dataInfo.m_format)
		{
			if (input_mixer->m_dataInfo.m_sampleRate == m_dataInfo.m_sampleRate)
			{
				// далее дублируется код взятый с места смешивания m_channelsTmp с m_channels
				for (size_t ti = 0; ti < input_mixer->m_channels.m_size; ++ti)
				{
					if (ti >= m_channels.m_size)
						break;
					auto src = input_mixer->m_channels.m_data[ti];
					auto dst = m_channels.m_data[ti];
					bqFloat32* src32 = (bqFloat32*)src->m_data.m_data;
					bqFloat32* dst32 = (bqFloat32*)dst->m_data.m_data;
					for (uint32_t i = 0, sz = src->m_data.m_dataSize / sizeof(bqFloat32); i < sz; ++i)
					{
						bqFloat32 v = *src32;
						*dst32 += v * m_mixerVolume;
						if (*dst32 > 1.f) *dst32 = 1.f;
						if (*dst32 < -1.f) *dst32 = -1.f;
						++dst32;
						++src32;
					}
				}
			}
		}
	}

	m_callback->OnEndProcess(this);
}

void bqSoundMixerImpl::_processEffects(int channel)
{
	auto channelData = GetChannel(channel);

	auto curr = m_effects.m_head;
	auto end = m_effects.m_head->m_left;
	while(true)
	{
		curr->m_data->Process(channelData, &this->m_dataInfo);
		curr = curr->m_right;
		if (curr == end)
			break;
	}
}

void bqSoundMixerImpl::GetSoundBufferInfo(bqSoundBufferInfo& info)
{
	info = m_dataInfo;
}

uint32_t bqSoundMixerImpl::GetBlockSize()
{
	return m_dataInfo.m_blockSize;
}

uint32_t bqSoundMixerImpl::GetNumOfChannels()
{
	return (uint32_t)m_channels.m_size;
}

bqSoundBufferData* bqSoundMixerImpl::GetChannel(uint32_t i)
{
	BQ_ASSERT_ST(i < m_channels.m_size);

	if (i < m_channels.m_size)
	{
		return &m_channels.m_data[i]->m_data;
	}

	return 0;
}

void bqSoundMixerImpl::SetCallback(bqSoundMixerCallback* cb)
{
	if (cb)
	{
		m_callback = cb;
	}
	else
	{
		m_callback = &g_defaultCallback;
	}
}

bqList<bqSoundEffect*>* bqSoundMixerImpl::GetEffects()
{
	return &m_effects;
}

void bqSoundMixerImpl::UseProcessing(bool v)
{
	m_useProcessing = v;
}

bool bqSoundMixerImpl::IsProcessing()
{
	return m_useProcessing;
}

void bqSoundMixerImpl::AddMixer(bqSoundMixer* mixer)
{
	BQ_ASSERT_ST(mixer);
	m_mixers.push_back(mixer);
}

void bqSoundMixerImpl::RemoveMixer(bqSoundMixer* mixer)
{
	BQ_ASSERT_ST(mixer);
	while (m_mixers.erase_first(mixer));
}

void bqSoundMixerImpl::RemoveAllMixers()
{
	m_mixers.clear();
}

void bqSoundMixerImpl::SetVolume(float v)
{
	m_mixerVolume = v;
}

float bqSoundMixerImpl::GetVolume()
{
	return m_mixerVolume;
}

#endif
