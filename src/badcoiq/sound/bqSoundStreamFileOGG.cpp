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
#include "badcoiq/common/bqFile.h"

#include "bqSoundStreamFileOGG.h"


BQ_LINK_LIBRARY("libogg");
BQ_LINK_LIBRARY("libvorbis");
size_t _oggvorbis_fread(void* buffer, size_t es, size_t ec, void* _f)
{
	bqSoundStreamFileOGG* sf = (bqSoundStreamFileOGG*)_f;
	if (sf)
	{
	//	FILE* f = sf->GetFILE();
	//	if (f)
	//		return fread(buffer, es, ec, f);
	}
	return 0;
}
int _oggvorbis_fseek(void* _f, ogg_int64_t o, int s)
{
	bqSoundStreamFileOGG* sf = (bqSoundStreamFileOGG*)_f;
	if (sf)
	{
	//	FILE* f = sf->GetFILE();
	//	if (f)
	//		return fseek(f, o, s);
	}
	return 0;
}
long _oggvorbis_ftell(void* _f)
{
	bqSoundStreamFileOGG* sf = (bqSoundStreamFileOGG*)_f;
	if (sf)
	{
	//	FILE* f = sf->GetFILE();
	//	if (f)
	//		return ftell(f);
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
	return ov_read(&m_vorbisFile, (char*)buffer, size, 0, 2, 1, &m_bitstreamCurrentSection);
}

void bqSoundStreamFileOGG::MoveToFirstDataBlock()
{
}

long bqSoundStreamFileOGG::Tell()
{
	return 0;
}

void bqSoundStreamFileOGG::Seek(long)
{
}

bool bqSoundStreamFileOGG::eof()
{
	return false;
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

			file.Seek(0, bqFile::_seek::set);

			return _OpenVorbis();
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

bool bqSoundStreamFileOGG::_OpenVorbis()
{
	printf("Open Vorbis\n");

	if (ov_open_callbacks((void*)this, &m_vorbisFile, 0, 0, m_callbacks) < 0)
	{
		bqLog::Print("%s %u\n", BQ_FUNCTION, BQ_LINE);
		return false;
	}

	return true;
}


#endif
