/*
BSD 2-Clause License

Copyright (c) 2023, badcoiq
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

#include "../framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;

bqSoundFile::bqSoundFile()
{
	m_readMethod = &bqSoundFile::_ReadWav;
}

bqSoundFile::~bqSoundFile()
{
	if (m_file)
	{
		fclose(m_file);
	}
}

bool bqSoundFile::_openWav(const char* fn)
{
	fopen_s(&m_file, fn, "rb");

	bool good = false;

	if (m_file)
	{
		char riff[5] = { 0,0,0,0,0 };
		fread(riff, 1, 4, m_file);

		if (strcmp(riff, "RIFF") == 0)
		{
			uint32_t RIFFChunkSize = 0;
			fread(&RIFFChunkSize, 1, 4, m_file);

			char wave[5] = { 0,0,0,0,0 };
			fread(wave, 1, 4, m_file);
			if (strcmp(wave, "WAVE") == 0)
			{
				char fmt[5] = { 0,0,0,0,0 };
				fread(fmt, 1, 4, m_file);
				if (strcmp(fmt, "fmt ") == 0)
				{
					uint32_t FMTChunkSize = 0;
					fread(&FMTChunkSize, 1, 4, m_file);

					uint16_t format = 0;
					fread(&format, 1, 2, m_file);

					uint16_t channels = 0;
					fread(&channels, 1, 2, m_file);

					uint32_t sampleRate = 0;
					fread(&sampleRate, 1, 4, m_file);

					uint32_t byteRate = 0;
					fread(&byteRate, 1, 4, m_file);

					uint16_t blockAlign = 0;
					fread(&blockAlign, 1, 2, m_file);

					uint16_t bitsPerSample = 0;
					fread(&bitsPerSample, 1, 2, m_file);

					if (FMTChunkSize > 16)
					{
						uint16_t extraFormatInfoSz = 0;
						fread(&extraFormatInfoSz, 1, 2, m_file);
					}

					char data[5] = { 0,0,0,0,0 };
					fread(data, 1, 4, m_file);
					if (strcmp(data, "data") == 0)
					{
						m_dataSize = 0;

						uint32_t dataSize = 0;
						fread(&dataSize, 1, 4, m_file);
						if (dataSize)
						{
							m_dataSize = dataSize;
							//Create(0.1f, channels, sampleRate, bitsPerSample);
							//if (m_soundSource->m_sourceData.m_dataSize < dataSize)
								//_reallocate(dataSize);

							//fread(.m_data, 1, m_dataSize, m_file);

							m_info.m_bitsPerSample = bitsPerSample;
							m_info.m_blockSize = blockAlign;
							m_info.m_bytesPerSample = bitsPerSample / 8;
							m_info.m_channels = channels;
							m_info.m_numOfSamples = dataSize / blockAlign;
							m_info.m_sampleRate = sampleRate;
							m_info.m_time = float(m_info.m_numOfSamples) / float(sampleRate);

							m_firstDataBlock = ftell(m_file);
							m_currentDataBlock = m_firstDataBlock;

						//	printf("TIME: %f\n", m_info.m_time);

							good = true;
						}
					}
				}
			}
		}
	}

	if (!good && m_file)
	{
		fclose(m_file);
		m_file = 0;
	}

	return good;
}

bool bqSoundFile::Open(const bqStringA& stra)
{
	return Open(stra.c_str());
}

bool bqSoundFile::Open(const char* fn)
{
	BQ_ASSERT_ST(fn);
	BQ_ASSERT_ST(!m_file);
	if (!m_file)
	{
		fopen_s(&m_file, fn, "rb");

		if (m_file)
		{
			fclose(m_file);
			m_file = 0;

			bqStringA stra;
			stra = fn;
			if (stra.extension(".wav"))
				return _openWav(fn);
		}
		else
		{
			bqLog::PrintError("Can't open file %s\n", fn);
		}
	}

	return false;
}

size_t bqSoundFile::_ReadWav(void* buffer, size_t size)
{
	size_t rn = fread(buffer, 1, size, m_file);

	m_currentDataBlock = ftell(m_file);

	return rn;
}

size_t bqSoundFile::Read(void* buffer, size_t size)
{
	BQ_ASSERT_ST(buffer);
	BQ_ASSERT_ST(size);
	return (this->*m_readMethod)(buffer, size);
}

const bqSoundBufferInfo& bqSoundFile::GetBufferInfo()
{
	return m_info;
}

bqSoundFormat bqSoundFile::GetFormat()
{
	bqSoundFormat r = bqSoundFormat::unsupported;
	
	r = bqSoundFormatFindFormat(m_info);

	return r;
}

void bqSoundFile::MoveToFirstDataBlock()
{
	m_currentDataBlock = m_firstDataBlock;
	fseek(m_file, m_currentDataBlock, SEEK_SET);
}

long bqSoundFile::Tell()
{
	return ftell(m_file);
}

void bqSoundFile::Seek(long v)
{
	fseek(m_file, v, SEEK_SET);
}
