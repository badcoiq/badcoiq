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

#include "badcoiq.h"

#ifdef BQ_WITH_SOUND
#include "badcoiq/sound/bqSoundSystem.h"
#include "badcoiq/common/bqFile.h"

#include "bqSoundStreamFileOGG.h"


BQ_LINK_LIBRARY("libogg");
BQ_LINK_LIBRARY("libvorbis");
size_t _oggvorbis_fread(void* buffer, size_t es, size_t ec, void* _f)
{
	bqSoundStreamFileOGG* sf = (bqSoundStreamFileOGG*)_f;
	if (sf)
	{
		FILE* f = sf->m_file;
		if (f)
			return fread(buffer, es, ec, f);
	}
	return 0;
}
int _oggvorbis_fseek(void* _f, ogg_int64_t o, int s)
{
	bqSoundStreamFileOGG* sf = (bqSoundStreamFileOGG*)_f;
	if (sf)
	{
		FILE* f = sf->m_file;
		if (f)
			return fseek(f, o, s);
	}
	return 0;
}
long _oggvorbis_ftell(void* _f)
{
	bqSoundStreamFileOGG* sf = (bqSoundStreamFileOGG*)_f;
	if (sf)
	{
		FILE* f = sf->m_file;
		if (f)
			return ftell(f);
	}
	return 0;
}


#include <opus.h>
BQ_LINK_LIBRARY("opus");


#include "../framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;

bqSoundStreamFileOGG::bqSoundStreamFileOGG()
{
}

bqSoundStreamFileOGG::~bqSoundStreamFileOGG()
{
	Close();
}

size_t bqSoundStreamFileOGG::Read(void* buffer, size_t size)
{
	// ov_read вернёт не size а значение например 4096
	// надо заполнить буфер buffer и выходить из функции
	// нужно иметь запасной буфер, куда будем засовывать
	// излишки. Потом при чтении сначала вытаскиваем излишки
	// а потом читаем новую порцию.

	uint8_t* dst = (uint8_t*)buffer;

	long allReadNum = 0;

	// если есть что-то в буфере с излишками то копируем данные
	if (m_outBufferSize)
	{
		memcpy(&dst[allReadNum], m_outBuffer, m_outBufferSize);
		allReadNum += m_outBufferSize;
		m_outBufferSize = 0;
	//	printf("m_outBufferSize\n");
	}

	while (true)
	{
		m_ovReadBufferSize = ov_read(&m_vorbisFile, (char*)m_ovReadBuffer, 4096, 0, 2, 1, &m_bitstreamCurrentSection);

		if (m_ovReadBufferSize <= 0)
		{
			if (m_ovReadBufferSize == 0) 
				m_eof = true;

			break;
		}

		long newReadNum = allReadNum + m_ovReadBufferSize;

		if (newReadNum > size)
		{
			//                                    vvv_size
			// *===================================*                     buffer/dst
			// *---------|-----------------------------------|           m_ovReadBuffer
			//                                              ^^^newReadNum / m_ovReadBufferSize
			// 
			//                                     |---------| s = newReadNum - size;
			//                                                 размер чтобы оставить на потом
			//                                                 надо скопировать в m_outBuffer
			// 
			//           |-------------------------|           sizeForDst = m_ovReadBufferSize - s;
			//                                                 это надо скопировать в buffer / dst
			// 
			// 
			//
			auto s = newReadNum - size;
			auto sizeForDst = m_ovReadBufferSize - s;
			memcpy(&dst[allReadNum], m_ovReadBuffer, sizeForDst);
			// заполнили dst

			// теперь надо сохранить лишнее

			// вышли за пределы buffer
		//	printf("OUT %u, %u > %u (%u)\n", m_ovReadBufferSize, newReadNum, size, newReadNum - size);
			printf("sizeForDst %u s %u\n", sizeForDst, s);

			memset(m_outBuffer, 0, 4096);
			m_outBufferSize = s;
			memcpy(m_outBuffer, &m_ovReadBuffer[sizeForDst], s);
			// но чтото не работает

			break;
		}
		else
		{
			memcpy(&dst[allReadNum], m_ovReadBuffer, m_ovReadBufferSize);
			
			if (newReadNum == size)
				break;
		}


		allReadNum = newReadNum;
	}

	return allReadNum;
}

void bqSoundStreamFileOGG::MoveToFirstDataBlock()
{
	ov_pcm_seek(&m_vorbisFile, 0);
	m_eof = false;
}

long bqSoundStreamFileOGG::Tell()
{
	return 0;
}

void bqSoundStreamFileOGG::Seek(long v)
{
	ov_pcm_seek(&m_vorbisFile, v);
	m_eof = false;
}

bool bqSoundStreamFileOGG::eof()
{
	return m_eof;
}

bool bqSoundStreamFileOGG::Open(const char* fn)
{
	bqFile file;
	
	if (file.Open(bqFile::_open::Open, fn))
	{
		memset(&m_info, 0, sizeof(m_info));

		file.Seek(26, bqFile::_seek::set);

		uint8_t numOfSegments = 0;
		if (!file.Read(&numOfSegments, 1))
			return false;

		if (numOfSegments != 1)
			return false;

		uint8_t sizeOfSegment = 0;
		if (!file.Read(&sizeOfSegment, 1))
			return false;

		if (!sizeOfSegment)
			return false;

		uint8_t segment[256];
		memset(segment, 0, 256);
		if (file.Read(segment, sizeOfSegment) != sizeOfSegment)
			return false;

		// " the identification header is type 1, the comment header type 3 and 
		// the setup header type 5 (these types are all odd as a packet with a 
		// leading single bit of ’0’ is an audio packet)."
		// "The packets must occur in the order of identification, comment, setup."
		if (segment[0] != 1)
			return false;

		if (memcmp(&segment[1], "vorbis", 6) == 0)
		{
			uint32_t vorbisVersion = *((uint32_t*)&segment[7]);
			if (vorbisVersion != 0) // должно быть 0
				return false;

			uint32_t channels = *((uint8_t*)&segment[11]);
			uint32_t samplerate = *((uint32_t*)&segment[12]);

			int32_t bitrate_maximum = *((int32_t*)&segment[16]);
			int32_t bitrate_nominal = *((int32_t*)&segment[20]);
			int32_t bitrate_minimum = *((int32_t*)&segment[24]);

			uint32_t blocksize_0 = (*((uint8_t*)&segment[28])) & 15;
			uint32_t blocksize_1 = ((*((uint32_t*)&segment[28])) & 240) >> 4;

			if (blocksize_0 > blocksize_1)
				return false;

			uint8_t framing_flag = *((uint8_t*)&segment[29]);
			if (!framing_flag)
				return false;


			m_info.m_format = bqSoundFormat::int16;
			m_info.m_bitsPerSample = 16;
			m_info.m_bytesPerSample = 2;
			m_info.m_channels = channels;
			m_info.m_sampleRate = samplerate;
			m_info.m_blockSize = m_info.m_bytesPerSample * m_info.m_channels;

			//file.Seek(0, bqFile::_seek::set);

			return _OpenVorbis(fn);
		}
		else if (memcmp(&segment[1], "OpusHead", 9) == 0)
		{
			//	OpusDecoder* decoder = 0;
			//	opus_decoder_create();
		}
		else
		{
			return false;
		}


			//if (ov_open_callbacks((void*)this, &vf, 0, 0, callbacks) < 0)
			//{
			//	// try opus


			//	return false;
			//}
			//else
			//{
			//	printf("VORBIS\n");
			//	m_readMethod = &bqSoundFile::_ReadOGG;
			//	m_setPlaybackPosition_method = &bqSoundFile::_SetPlaybackPositionNull;
			//}
	}
	return false;
}

void bqSoundStreamFileOGG::Close()
{
}

bool bqSoundStreamFileOGG::_OpenVorbis(const char* fn)
{
	printf("Open Vorbis\n");
	if (m_file)
		fclose(m_file);

	fopen_s(&m_file, fn, "rb");
	if (!m_file)
		return false;

	if (ov_open_callbacks((void*)this, &m_vorbisFile, 0, 0, m_callbacks) < 0)
	{
		bqLog::Print("%s %u\n", BQ_FUNCTION, BQ_LINE);
		fclose(m_file);
		return false;
	}

	return true;
}


#endif