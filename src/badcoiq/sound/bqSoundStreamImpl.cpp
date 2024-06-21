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
#include "badcoiq/sound/bqSoundLab.h"

#include "bqSoundStreamImpl.h"



#include "../framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;

bqSoundStreamImpl::bqSoundStreamImpl(bqSoundStreamFile* sf)
{
	//m_file = new bqSoundFile;
	m_streamFile = sf;

	m_fileSampleRate = m_streamFile->GetBufferInfo().m_sampleRate;

	// Данные которые читаются из файла
	for (int i = 0; i < 2; ++i)
	{
		m_dataFromFile[i].m_size = m_fileSampleRate * m_streamFile->GetBufferInfo().m_blockSize;
		m_dataFromFile[i].reserve(m_dataFromFile[i].m_size);

		if (m_streamFile->GetBufferInfo().m_channels != 2)
		{
		//	// изменение каналов будет идти после конвертации
			m_dataAfterChannels[i].m_size = m_fileSampleRate;
			m_dataAfterChannels[i].m_size *= m_streamFile->GetBufferInfo().m_bitsPerSample / 8;
			m_dataAfterChannels[i].m_size *= 2;
			m_dataAfterChannels[i].reserve(m_dataAfterChannels[i].m_size);
		}
	}

		
	m_deviceInfo = bqFramework::GetSoundSystem()->GetDeviceInfo();

	// нужно сконвертировать это (m_dataFromFile) в формат миксера
	// формат миксера устанавливается в соответствии с bqSoundSystemDeviceInfo
	//    только используется bqSoundFormat::float32
	// 2 канала - прописано для главного миксера. нет желания реализовывать более.
	auto mixerChannels = 2;

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

//	m_dataSize = m_streamFile->GetDataSize();
	m_blockSize = m_streamFile->GetBufferInfo().m_blockSize;

	_convert = 0;
	switch (m_streamFile->GetBufferInfo().m_format)
	{
		case bqSoundFormat::uint8:
			_convert = &bqSoundStreamImpl::_convertUint8;
			break;
		case bqSoundFormat::int16:
			_convert = &bqSoundStreamImpl::_convertInt16;
			break;
	}

	m_thread = new std::thread(&bqSoundStreamImpl::_thread_function, this);
}

bqSoundStreamImpl::~bqSoundStreamImpl()
{
	for (int i = 0; i < 2; ++i)
	{
		m_dataFromFile[i].free_memory();
		m_dataAfterConvert[i].free_memory();
		m_dataAfterResample[i].free_memory();
		m_dataAfterChannels[i].free_memory();
	}

	// завершение thread тоже здесь
	m_threadContext.m_run = false;
	if (m_thread)
	{
		if (m_thread->joinable())
			m_thread->join();
		delete m_thread;
		m_thread = 0;
	}

	delete m_streamFile;
}


void bqSoundStreamImpl::PlaybackStart()
{
	m_state = state_playing;
//	printf("PLAY\n");
}

void bqSoundStreamImpl::PlaybackStop()
{
	m_state = state_notPlaying;
//	printf("STOP\n");
}

void bqSoundStreamImpl::PlaybackReset()
{
//	printf("RESET\n");
	m_state = state_notPlaying;

	

	this->ThreadCommand_SetPlaybackPosition(0.f);
}

//void bqSoundStreamImpl::PlaybackSet(uint32_t minutes, float seconds)
//{
//	if (seconds > 60.f)	seconds = 0.f;
//	PlaybackSet(seconds + ((float)minutes * 60.f));
//}
//
//void bqSoundStreamImpl::PlaybackSet(float secondsOnly)
//{
//	if (m_file)
//	{
//		m_file->MoveToFirstDataBlock();
//
//
//		//this->ThreadCommand_SetPlaybackPostion(v);
//	}
//}


void bqSoundStreamImpl::ThreadCommand_SetPlaybackPosition(float v)
{
	_thread_command cmd;
	cmd.m_ptr = 0;
	cmd._float32[0] = v;
	cmd.m_method = &bqSoundStreamImpl::ThreadCommandMethod_SetPlaybackPostion;
	this->m_threadContext.m_commands.Push(cmd);
}
void bqSoundStreamImpl::ThreadCommandMethod_SetPlaybackPostion(_thread_command* c)
{
	//printf("SPP %llu\n", c->_64bit);
	m_prepareBufferIndex = 0;
	m_numOfPreparedBuffers = 0;
	m_dataPosition = 0;
	m_activeBufferIndex = -1;

	for (int i = 0; i < 2; ++i)
	{
		memset(m_dataFromFile[i].m_data, 0, m_dataFromFile[i].m_size);
		memset(m_dataAfterChannels[i].m_data, 0, m_dataAfterChannels[i].m_size);
		memset(m_dataAfterConvert[i].m_data, 0, m_dataAfterConvert[i].m_size);
		memset(m_dataAfterResample[i].m_data, 0, m_dataAfterResample[i].m_size);
	}

	if(c->_float32[0] == 0.f)
		m_streamFile->MoveToFirstDataBlock();
}

void bqSoundStreamImpl::_OnEndBuffer()
{
//	printf("_OnEndBuffer\n");
	if (m_playbackInfo[m_activeBufferIndex].m_lastBuffer)
	{
//		printf("PlaybackStop\n");
		if (!m_loop)
			PlaybackStop();
		

		if (m_callback)
			m_callback->OnEndStream();
	}

	if (m_activeBufferIndex)
		m_activeBufferIndex = 0;
	else
		m_activeBufferIndex = 1;

	//m_activeBufferIndex = m_activeBufferIndexNext[m_activeBufferIndex];

	if (m_numOfPreparedBuffers)
		--m_numOfPreparedBuffers;
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

		//if (!m_streamFile->eof())
		{
			if (m_numOfPreparedBuffers < 2)
			{
			//	printf("prep %u %u\n", m_prepareBufferIndex, m_numOfPreparedBuffers);
				auto fileBufferInfo = m_streamFile->GetBufferInfo();

		//		printf("blockSize %u, fileBufferInfo.m_bytesPerSample %u, fileBufferInfo.m_blockSize %u \n", 
		//			m_blockSize, 
		//			fileBufferInfo.m_bytesPerSample,
		//			fileBufferInfo.m_blockSize);

				// сохраняю позицию перед чтением
				// потом сделаю проверку, сколько было прочитано байт
				auto beforeRead = m_streamFile->Tell();
				memset(m_dataFromFile[m_prepareBufferIndex].m_data, 0, m_dataFromFile[m_prepareBufferIndex].m_size);
				size_t numRead = m_streamFile->Read(m_dataFromFile[m_prepareBufferIndex].m_data, m_dataFromFile[m_prepareBufferIndex].m_size);
				
		//		printf("READ %u\n", numRead);
				m_playbackInfo[m_prepareBufferIndex].m_lastBuffer = false;
				if (m_streamFile->eof())
				{
		//			printf("eof\n");
					if (m_loop)
					{
						// если повтор то устанавливаем указатель на начало звука в файле
						// и заполняем остаток буфера чтобы небыло паузы
						m_streamFile->MoveToFirstDataBlock();
						m_streamFile->Read(&m_dataFromFile[m_prepareBufferIndex].m_data[numRead],
							m_dataFromFile[m_prepareBufferIndex].m_size - numRead);
					}
					else
					{
						// прочитали меньше, конец файла
						m_playbackInfo[m_prepareBufferIndex].m_lastBuffer = true;
					}
				}

				m_dataPointer = &m_dataFromFile[m_prepareBufferIndex];
				m_blockSize = m_streamFile->GetBufferInfo().m_blockSize;

				// делаю стерео
				if (fileBufferInfo.m_channels != 2)
				{
					auto dstBlock = m_dataAfterChannels[m_prepareBufferIndex].m_data;

					auto numOfBlocks = m_dataFromFile->m_size / fileBufferInfo.m_blockSize;
					uint8_t* srcBlock = &m_dataPointer->m_data[0];

					for (uint32_t di = 0; di < numOfBlocks; ++di)
					{
						if (fileBufferInfo.m_channels < 2)
						{
							for (uint32_t si = 0; si < fileBufferInfo.m_bytesPerSample; ++si)
							{
								dstBlock[si] = srcBlock[si];
								dstBlock[fileBufferInfo.m_bytesPerSample + si] = srcBlock[si];
							}
							dstBlock += fileBufferInfo.m_bytesPerSample * 2;
						}
						else
						{

						}

						srcBlock += fileBufferInfo.m_blockSize;
					}

					m_blockSize = fileBufferInfo.m_bytesPerSample * 2;
					m_dataPointer = &m_dataAfterChannels[m_prepareBufferIndex];
				}

				// в данном месте m_dataPointer уже должен быть 2х канальным

				// Конвертация
				if (_convert)
				{
					(this->*_convert)(); // конвертация будет сохранятся в m_dataAfterConvert
					m_dataPointer = &m_dataAfterConvert[m_prepareBufferIndex];
				}

				// Resample
				if (m_fileSampleRate != m_deviceInfo.m_sampleRate)
				{
					uint32_t newSampleRate = m_deviceInfo.m_sampleRate;
					uint32_t numOfChannels = 2;

					//m_dataAfterResample уже должен иметь нужный размер для звука
					// с новым sample rate
					uint32_t numOfBlocksOld = m_dataPointer->m_size / m_blockSize;
					uint32_t numOfBlocksNew = m_dataAfterResample[m_prepareBufferIndex].m_size / m_blockSize;

					double _m = (double)numOfBlocksOld / (double)numOfBlocksNew;

					for (uint32_t ic = 0; ic < numOfChannels; ++ic)
					{
						bqFloat32* ptrNew = (bqFloat32*)m_dataAfterResample[m_prepareBufferIndex].m_data;
						bqFloat32* ptrOld = (bqFloat32*)m_dataPointer->m_data;

						for (uint32_t ib = 0; ib < numOfBlocksNew; /*++ib*/)
						{

							uint32_t indexNewNoChannels = 0;
							uint32_t indexNew = 0;

							uint32_t _1_second = newSampleRate;
							if ((numOfBlocksNew - ib) < _1_second)
								_1_second = numOfBlocksNew - ib;

							for (uint32_t i = 0; i < _1_second; ++i)
							{
								double _v = (double)indexNewNoChannels * _m;
								uint32_t indexOld = (uint32_t)floor(_v) * numOfChannels;
								indexOld += ic;

								ptrNew[indexNew + ic] = ptrOld[indexOld];
								indexNew += numOfChannels;
								++indexNewNoChannels;
							}

							ib += _1_second;
							ptrNew += newSampleRate * numOfChannels;
							ptrOld += m_deviceInfo.m_sampleRate * numOfChannels;
						}
					}

					m_dataPointer = &m_dataAfterResample[m_prepareBufferIndex];
				}

				m_dataActiveBufferPointer[m_prepareBufferIndex] = m_dataPointer;
				m_blockSizeCurrent[m_prepareBufferIndex] = m_blockSize;

				if(m_activeBufferIndex == -1)
					m_activeBufferIndex = m_prepareBufferIndex;


				auto old_m_prepareBufferIndex = m_prepareBufferIndex;

				++m_numOfPreparedBuffers;

				++m_prepareBufferIndex;
				if (m_prepareBufferIndex > 1)
					m_prepareBufferIndex = 0;
				
				//m_activeBufferIndexNext[old_m_prepareBufferIndex] = m_prepareBufferIndex;

			}
		}
		//else
		{
			//m_file->MoveToFirstDataBlock();
			//printf("END\n");
		}
	}
}


uint32_t bqSoundStreamImpl::GetNumOfChannels()
{
	return 2;
}

uint32_t bqSoundStreamImpl::GetBlockSize()
{
	return m_blockSizeCurrent[m_activeBufferIndex];
}

void bqSoundStreamImpl::_convertUint8()
{
	auto fileBufferInfo = m_streamFile->GetBufferInfo();
	auto numBlocks = m_dataFromFile->m_size / fileBufferInfo.m_blockSize;

	uint8_t* dst_block = m_dataAfterConvert[m_prepareBufferIndex].m_data;
	uint8_t* src_block = m_dataPointer->m_data;

	for (uint32_t i = 0; i < numBlocks; ++i)
	{
		bqSoundSample_32bitFloat* dst_32bit = (bqSoundSample_32bitFloat*)dst_block;
		bqSoundSample_8bit* _8bit = (bqSoundSample_8bit*)src_block;

		dst_32bit[0] = bqSoundLab::SampleTo32bitFloat(_8bit[0]);
		dst_32bit[1] = bqSoundLab::SampleTo32bitFloat(_8bit[1]);

		dst_block += sizeof(bqFloat32) * 2;
		src_block += m_blockSize;
	}

	m_blockSize = sizeof(bqFloat32) * 2;
}

void bqSoundStreamImpl::_convertInt16()
{
	auto fileBufferInfo = m_streamFile->GetBufferInfo();
	auto numBlocks = m_dataFromFile->m_size / fileBufferInfo.m_blockSize;

	uint8_t* dst_block = m_dataAfterConvert[m_prepareBufferIndex].m_data;
	uint8_t* src_block = m_dataPointer->m_data;
	
	for (uint32_t i = 0; i < numBlocks; ++i)
	{
		bqSoundSample_32bitFloat* dst_32bit = (bqSoundSample_32bitFloat*)dst_block;
		bqSoundSample_16bit* _16bit = (bqSoundSample_16bit*)src_block;

		dst_32bit[0] = bqSoundLab::SampleTo32bitFloat(_16bit[0]);
		dst_32bit[1] = bqSoundLab::SampleTo32bitFloat(_16bit[1]);

		dst_block += sizeof(bqFloat32) * 2;
		src_block += m_blockSize;
	}

	m_blockSize = sizeof(bqFloat32) * 2;
}

bqArray<uint8_t>* bqSoundStreamImpl::GetActiveBuffer()
{
	if(m_activeBufferIndex != -1)
		return m_dataActiveBufferPointer[m_activeBufferIndex]; 
	return 0;
}

void bqSoundStreamImpl::SetCallback(bqSoundStreamCallback* cb)
{
	m_callback = cb;
}

#endif
