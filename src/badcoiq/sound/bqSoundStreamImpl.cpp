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

#include "bqSoundStreamImpl.h"

#include "../framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;

bqSoundStreamImpl::bqSoundStreamImpl()
{
	m_file = new bqSoundFile;
}

bqSoundStreamImpl::~bqSoundStreamImpl()
{
	Close();
	delete m_file;
}


void bqSoundStreamImpl::PlaybackStart()
{
}

void bqSoundStreamImpl::PlaybackStop()
{
}

void bqSoundStreamImpl::PlaybackReset()
{
}

void bqSoundStreamImpl::PlaybackSet(uint32_t minutes, float seconds)
{
}

void bqSoundStreamImpl::PlaybackSet(float secondsOnly)
{
}

bool bqSoundStreamImpl::Open(const char* path)
{
	BQ_ASSERT_ST(path);
	if (IsOpened())
		return false;

	if (!path)
		return false;

	if (m_file->Open(path))
	{
		// 1 секунда
		//m_soundDataSize = m_file->GetBufferInfo().m_sampleRate
		//	* m_file->GetBufferInfo().m_blockSize;
		//m_soundData = new uint8_t[m_soundDataSize];
		
		// Данные которые читаются из файла
		m_dataFromFile.m_size = m_file->GetBufferInfo().m_sampleRate * m_file->GetBufferInfo().m_blockSize;
		m_dataFromFile.reserve(m_dataFromFile.m_size);
		
		bqSoundSystemDeviceInfo deviceInfo = bqFramework::GetSoundSystem()->GetDeviceInfo();

		// нужно сконвертировать это (m_dataFromFile) в формат миксера
		// формат миксера устанавливается в соответствии с bqSoundSystemDeviceInfo
		//    только используется bqSoundFormat::float32
		// 2 канала - прописано для главного миксера. нет желания реализовывать более.
		auto mixerChannels = deviceInfo.m_channels;
		if (mixerChannels > 2)
			mixerChannels = 2;
		m_dataAfterConvert.m_size = m_file->GetBufferInfo().m_sampleRate * sizeof(bqFloat32) * mixerChannels;
		m_dataAfterConvert.reserve(m_dataAfterConvert.m_size);
		
		// resample требует массива иного размера
		// как видно из m_dataAfterConvert там используется sample rate файла
		// здесь же используется значение из bqSoundSystemDeviceInfo
		m_dataAfterResample.m_size = deviceInfo.m_sampleRate * (deviceInfo.m_bitsPerSample / 8) * mixerChannels;
		m_dataAfterResample.reserve(m_dataAfterResample.m_size);

		m_thread = new std::thread(&bqSoundStreamImpl::_thread_function, this);

		if (m_thread)
			return true;

		Close();
	}

	return false;
}

void bqSoundStreamImpl::_thread_function()
{
	while (m_threadContext.m_run)
	{
		if (!m_file->eof())
		{
			m_file->Read(m_dataFromFile.m_data, m_dataFromFile.m_size);

			// Конвертация
			// Resample
			// Передача в миксер
			// Заполнение второго буфера
			// Если два буфера заполнены, то ожидаем когда завершитсч воспроизведение первого
		}
	}
}

void bqSoundStreamImpl::Close()
{
	//if (m_soundData)
	//{
	//	delete[]m_soundData;
	//	m_soundData = 0;
	//}
	m_dataFromFile.free_memory();
	m_dataAfterConvert.free_memory();
	m_dataAfterResample.free_memory();

	m_file->Close();
	// завершение thread тоже здесь
	m_threadContext.m_run = false;
	if (m_thread)
	{
		if (m_thread->joinable())
			m_thread->join();
		delete m_thread;
		m_thread = 0;
	}
}

bool bqSoundStreamImpl::IsOpened()
{
	return m_file->IsOpened();
}

#endif
