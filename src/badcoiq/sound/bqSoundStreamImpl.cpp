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
	m_state = state_playing;
}

void bqSoundStreamImpl::PlaybackStop()
{
	m_state = state_notPlaying;
}

void bqSoundStreamImpl::PlaybackReset()
{
	m_state = state_notPlaying;
	PlaybackSet(0.f);
}

void bqSoundStreamImpl::PlaybackSet(uint32_t minutes, float seconds)
{
	if (seconds > 60.f)	seconds = 0.f;
	PlaybackSet(seconds + ((float)minutes * 60.f));
}

void bqSoundStreamImpl::PlaybackSet(float secondsOnly)
{
	if (m_file)
	{
		m_file->MoveToFirstDataBlock();
	}
	/*if (secondsOnly == 0.f)
	{
		m_playbackPosition = 0;
	}
	else
	{
		m_playbackPosition = bqSound_GetPlaybackPosition(secondsOnly, m_soundBuffer->m_bufferInfo);
	}*/
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
		
		m_fileSampleRate = m_file->GetBufferInfo().m_sampleRate;

		// Данные которые читаются из файла
		for (int i = 0; i < 2; ++i)
		{
			m_dataFromFile[i].m_size = m_fileSampleRate * m_file->GetBufferInfo().m_blockSize;
			m_dataFromFile[i].reserve(m_dataFromFile[i].m_size);
		}

		
		m_deviceInfo = bqFramework::GetSoundSystem()->GetDeviceInfo();

		// нужно сконвертировать это (m_dataFromFile) в формат миксера
		// формат миксера устанавливается в соответствии с bqSoundSystemDeviceInfo
		//    только используется bqSoundFormat::float32
		// 2 канала - прописано для главного миксера. нет желания реализовывать более.
		auto mixerChannels = m_deviceInfo.m_channels;
		if (mixerChannels > 2)
			mixerChannels = 2;

		for (int i = 0; i < 2; ++i)
		{
			m_dataAfterConvert[i].m_size = m_fileSampleRate * sizeof(bqFloat32) * mixerChannels;
			m_dataAfterConvert[i].reserve(m_dataAfterConvert[i].m_size);
		}
		
		// resample требует массива иного размера
		// как видно из m_dataAfterConvert там используется sample rate файла
		// здесь же используется значение из bqSoundSystemDeviceInfo
		for (int i = 0; i < 2; ++i)
		{
			m_dataAfterResample[i].m_size = m_deviceInfo.m_sampleRate * (m_deviceInfo.m_bitsPerSample / 8) * mixerChannels;
			m_dataAfterResample[i].reserve(m_dataAfterResample[i].m_size);
		}

		m_dataSize = m_file->GetDataSize();

		// конвертации пока нет
		// читаем float32
		_convert = 0;

		m_thread = new std::thread(&bqSoundStreamImpl::_thread_function, this);

		if (m_thread)
			return true;

		Close();
	}

	return false;
}

void bqSoundStreamImpl::ThreadCommand_SetPlaybackPostion(uint64_t v)
{
	_thread_command cmd;
	cmd.m_ptr = 0;
	cmd._64bit = v;
	cmd.m_method = &bqSoundStreamImpl::ThreadCommandMethod_SetPlaybackPostion;
	this->m_threadContext.m_commands.Push(cmd);
}
void bqSoundStreamImpl::ThreadCommandMethod_SetPlaybackPostion(_thread_command* c)
{
	//printf("SPP %llu\n", c->_64bit);

	m_playbackPosition = c->_64bit;
	m_file->SetPlaybackPosition(c->_64bit);
}

void bqSoundStreamImpl::_OnEndBuffer()
{
	if(m_numOfPreparedBuffers)
		--m_numOfPreparedBuffers;

	/*SetPlaybackPosition(sPos + m_dataPositionInFile[stream->m_activeBufferIndex]);
	if (stream->GetPlaybackPosition() >= stream->GetDataSize())
	{
		stream->SetPlaybackPosition(stream->GetDataSize() - 1);
	}*/
}

void bqSoundStreamImpl::_thread_function()
{
	while (m_threadContext.m_run)
	{
		while (m_threadContext.m_commands.IsEmpty() == false)
		{
			auto command = m_threadContext.m_commands.Front();
			ThreadCommand_CallMethod(command.m_method, &command);
			m_threadContext.m_commands.Pop();
		}

		if (!IsPlaying())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			continue;
		}

		if (!m_file->eof())
		{
			if (m_numOfPreparedBuffers < 2)
			{
			//	printf("prep %u %u\n", m_prepareBufferIndex, m_numOfPreparedBuffers);

				m_dataPositionInFile[m_prepareBufferIndex] = m_file->Tell();
				m_file->Read(m_dataFromFile[m_prepareBufferIndex].m_data, m_dataFromFile[m_prepareBufferIndex].m_size);


				m_dataPointer = &m_dataFromFile[m_prepareBufferIndex];

				// Конвертация
				if (_convert)
				{
					(this->*_convert)(); // конвертация будет сохранятся в m_dataAfterConvert
					m_dataPointer = &m_dataAfterConvert[m_prepareBufferIndex];
				}

				// Resample
				if (m_fileSampleRate != m_deviceInfo.m_sampleRate)
				{
					//m_dataAfterResample уже должен иметь нужный размер для звука
					// с новым sample rate


					m_dataPointer = &m_dataAfterResample[m_prepareBufferIndex];
				}

				m_dataActiveBufferPointer[m_prepareBufferIndex] = m_dataPointer;


				m_activeBufferIndex = m_prepareBufferIndex;

				++m_numOfPreparedBuffers;

				++m_prepareBufferIndex;
				if (m_prepareBufferIndex > 1)
					m_prepareBufferIndex = 0;

			}
		}
	}
}

void bqSoundStreamImpl::Close()
{
	_convert = 0;
	//if (m_soundData)
	//{
	//	delete[]m_soundData;
	//	m_soundData = 0;
	//}
	for (int i = 0; i < 2; ++i)
	{
		m_dataFromFile[i].free_memory();
		m_dataAfterConvert[i].free_memory();
		m_dataAfterResample[i].free_memory();
	}

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

uint32_t bqSoundStreamImpl::GetNumOfChannels()
{
	return 2;
}

const bqSoundBufferInfo& bqSoundStreamImpl::GetBufferInfo()
{
	if (m_file)
		return m_file->GetBufferInfo();

	static bqSoundBufferInfo info;
	if (info.m_format == bqSoundFormat::float32)
	{
		memset(&info, 0, sizeof(bqSoundBufferInfo));
		info.m_format = bqSoundFormat::unsupported;
	}
	return info;
}

uint64_t bqSoundStreamImpl::GetDataSize()
{
	return m_dataSize;
}

void bqSoundStreamImpl::SetPlaybackPosition(uint64_t v)
{
	printf("SPP %llu\n", v);

	if (m_file)
		this->ThreadCommand_SetPlaybackPostion(v);
}

uint64_t bqSoundStreamImpl::GetPlaybackPosition()
{
	return m_playbackPosition;
}

bqArray<uint8_t>* bqSoundStreamImpl::GetActiveBuffer()
{
	if(m_activeBufferIndex != -1)
		return m_dataActiveBufferPointer[m_activeBufferIndex]; 
	return 0;
}


#endif
