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
	m_dataInfo.m_blockSize = 4; // не имеет значения, пусть будет как на 1 канал
	m_dataInfo.m_bytesPerSample = 4;
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

	bqSoundMixerNode node;
	node.m_sound = sound;
	m_sounds.push_back(node);
}

void bqSoundMixerImpl::RemoveSound(bqSound* sound)
{
	BQ_ASSERT_ST(sound);

	struct _compare
	{
		bool operator()(const bqSoundMixerNode& a, const bqSoundMixerNode& b) const
		{
			return a.m_sound == b.m_sound;
		}
	}compare;

	bqSoundMixerNode node;
	node.m_sound = sound;
	while (m_sounds.erase_first(node, compare));
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
		for (size_t ii = 0; ii < m_channels.m_data[i]->m_data.m_dataSize; ++ii)
		{
			m_channels.m_data[i]->m_data.m_data[ii] = 0;
		}
	}

	m_callback->OnStartProcess(this);

	for (size_t si = 0; si < m_sounds.m_size; ++si)
	{
		bqSoundMixerNode& soundNode = m_sounds.m_data[si];

		auto soundChannelsNum = soundNode.m_sound->m_soundBuffer->m_bufferInfo.m_channels;

		uint32_t sPos = soundNode.m_position; // в байтах

		bool isOnEnd = false;

		// Заполняю временные буферы m_channelsTmp
		size_t isz = soundNode.m_sound->m_soundBuffer->m_bufferData.m_dataSize;// / sizeof(bqFloat32);
		for (size_t i = 0, i_tmp = 0, last = isz-1; i < isz; ) // проход по звуку. 
		{
			// если заполнили m_channelsTmp то
			// * выход из цикла
			if (i >= m_bufferSizeForOneChannel)
			{
				break;
			}

			uint8_t* dataSound8 = soundNode.m_sound->m_soundBuffer->m_bufferData.m_data;
			bqFloat32* dataSound32 = (bqFloat32*)(&dataSound8[0]);

			// Сразу возьму звук. + умножу на значение громкости
			bqFloat32 ch1 = *dataSound32;
			bqFloat32 ch2 = ch1;
			// Если каналов больше то беру второй
			if (soundChannelsNum > 1)
			{
				++dataSound32;
				ch2 = *dataSound32 * soundNode.m_sound->m_volume;
			}

			// далее надо прокрутить sPos
			sPos += soundNode.m_sound->m_soundBuffer->m_bufferInfo.m_blockSize;

			// i тоже крутим
			i += soundNode.m_sound->m_soundBuffer->m_bufferInfo.m_blockSize;

			// теперь надо передать звук во временный буфер
			uint8_t* dataTmp8 = (uint8_t*)m_channelsTmp.m_data[0]->m_data.m_data;
			bqFloat32* dataTmp32 = (bqFloat32*)dataTmp8;
			dataTmp32 += i_tmp;
			*dataTmp32 = ch1;

			dataTmp8 = (uint8_t*)m_channelsTmp.m_data[1]->m_data.m_data;
			dataTmp32 = (bqFloat32*)dataTmp8;
			dataTmp32 += i_tmp;
			*dataTmp32 = ch2;

			++i_tmp;

			// Если позиция звука вышла за пределы массива звука
			if (sPos >= soundNode.m_sound->m_soundBuffer->m_bufferData.m_dataSize)
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
		}

		// Для pitch нужно будет по особому изменять значение soundNode.m_position
		// Для простоты буду использовать sPos
		// Типа, произвёл все действия, и присвоил это soundNode.m_position
		soundNode.m_position = sPos;

		// теперь смешиваю временные m_channelsTmp с каналами миксера
		for (size_t ti = 0; ti < m_channelsTmp.m_size; ++ti)
		{
			if (ti >= m_channels.m_size)
				break;

			auto src = m_channelsTmp.m_data[ti];
			auto dst = m_channels.m_data[ti];

			bqFloat32* src32 = (bqFloat32*)src->m_data.m_data;
			bqFloat32* dst32 = (bqFloat32*)dst->m_data.m_data;

			for (uint32_t i = 0, sz = src->m_data.m_dataSize/sizeof(bqFloat32); i < sz; ++i)
			{
				// тут звук нужно добавлять, и каким-то образом ограничивать
				// возростающую громкость

				bqFloat32 v = *src32;

				*dst32 += v;

				++dst32;
				++src32;
			}
		}
		
		if (isOnEnd)
		{
			m_callback->OnEndSound(this, soundNode.m_sound);
			isOnEnd = false;
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

		// Заполняю каналы миксера по семплам.
		// Так будет проще распределить каналы звука по каналам миксера.
		// А так-же сдвиг позиции soundNode.m_position без каких либо трудностей.
		//size_t isz = m_bufferSizeForOneChannel / 4; // sizeof(float32)
		//for (size_t i = 0; i < isz; ++i)
		//{
		//	for (size_t ci = 0; ci < m_channels.m_size; ++ci)
		//	{
		//		bool makeSilent = false;
		//		bqSoundBufferData* _channel = &m_channels.m_data[ci]->m_data;

		//		uint8_t* dataMixer8 = _channel->m_data;
		//		bqFloat32* dataMixer32 = (bqFloat32*)dataMixer8;

		//		uint8_t* dataSound8 = &soundNode.m_sound->m_soundBuffer->m_bufferData.m_data[soundNode.m_position];
		//		bqFloat32* dataSound32 = (bqFloat32*)dataSound8;

		//		// Надо установить указатель dataSound32 на нужный канал
		//		// если
		//		// канал миксера может быть например 2
		//		// звук одноканальный.
		//		// тогда ничего не делаем так как всё само заполниться

		//		// если 
		//		// миксер одноканальный, то надо все каналы звука добавлять в этот канал
		//		// в этом случае необходимо реализовать отдельный метод в виде указателя на метод

		//		// если миксер и звуки многоканальные
		//		// копируем звук из канала в канал
		//		// если каналов в миксере больше то каналы не соответствующие каналам звука будут пустыми
		//		// если каналов у звука больше то эти каналы будут пропущены
		//		// данный случай будет реализован по умолчанию 
		//		if (soundChannelsNum > 1)
		//		{
		//			int ch = 0;
		//			if (ci < soundChannelsNum)
		//				// например каналов в звуке 2
		//				// индекс ci равен 1 когда идёт 2й канал миксера
		//			{
		//				ch = ci;
		//			}
		//			else
		//			{
		//				makeSilent = true;
		//			}

		//			dataSound32 += ch;
		//		}
		//}




//				size_t isz = m_bufferSizeForOneChannel / 4; // sizeof(float32)
//				for (size_t i = 0; i < isz; ++i)
//				{
//					
//					if (makeSilent)
//					{
//						*dataMixer32 = 0;
//					}
//					else
//					{
//						*dataMixer32 += (*dataSound32) * soundNode.m_sound->m_volume;
//
//						// должно быть выравнивание звука
//						// примитивное решение - поделить на количество звуков
//					}
//					++dataMixer32;
//
//					//перемещение soundNode.m_position					
//					soundNode.m_position += soundNode.m_sound->m_soundBuffer->m_bufferInfo.m_blockSize;
//					
//					//printf("POS: %i | %i\n", soundNode.m_position, soundNode.m_sound->m_soundBuffer->m_bufferData.m_dataSize);
//
//			//		m_callback->OnEndMixSound(this, soundNode.m_sound);
//
//					//проверка на выход за пределы массива
//					if (soundNode.m_position >= (soundNode.m_sound->m_soundBuffer->m_bufferData.m_dataSize - 4))
//					{
//						//Если вышли то
//						//	завершаем обработку текущего звука
//
//						m_callback->OnEndSound(this, soundNode.m_sound, ci);
//
//						soundNode.m_position = 0;
//						goto end_sound;
//
//						//	или обрабатываем опять если указан repeat \ loop
//					}
//					dataSound8 = &soundNode.m_sound->m_soundBuffer->m_bufferData.m_data[soundNode.m_position];
//					dataSound32 = (bqFloat32*)dataSound8;
//				}
//
//				
//			//	_processEffects(ci);
//			//	m_callback->OnFullBuffer(this);
//			}
//		//}
//end_sound:;
//	}
	}

	//for (size_t ci = 0; ci < m_channels.m_size; ++ci)
	//{
	//	bool makeSilent = false;
	//	bqSoundBufferData* _channel = &m_channels.m_data[ci]->m_data;

	//	uint8_t* dataMixer8 = _channel->m_data;
	//	bqFloat32* dataMixer32 = (bqFloat32*)dataMixer8;
	//	if (!makeSilent)
	//	{
	//		dataMixer32 = (bqFloat32*)dataMixer8;
	//		size_t isz = m_bufferSizeForOneChannel / 4; // sizeof(float32)
	//		for (size_t i = 0; i < isz; ++i)
	//		{
	//			*dataMixer32 *= 0.5f;

	//			if (*dataMixer32 > 1.f)
	//				*dataMixer32 = 1.f;
	//			if (*dataMixer32 < -1.f)
	//				*dataMixer32 = -1.f;
	//			++dataMixer32;
	//		}
	//	}
	//}

	// добавить буферы из других миксеров будет проще
	// размеры у всех одинаковые
	// надо только будет распределять каналы
	for (size_t mi = 0; mi < m_mixers.m_size; ++mi)
	{
		auto mixer = m_mixers.m_data[mi];

		//mixer->
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
