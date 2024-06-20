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

#include "../framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;

#include "bqSoundStreamFileWAV.h"

bqSoundStreamFileWAV::bqSoundStreamFileWAV()
{
}

bqSoundStreamFileWAV::~bqSoundStreamFileWAV()
{
	Close();
}


bool bqSoundStreamFileWAV::Open(const char* fn)
{
	if (!m_file)
	{
		fopen_s(&m_file, fn, "rb");
		if (m_file)
		{
			bool isGood = false;
			bqSoundFormat format = bqSoundFormat::unsupported;

			char riff[5] = { 0,0,0,0,0 };
			fread(riff, 1, 4, m_file);

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

					uint16_t TYPE = 0;
					fread(&TYPE, 1, 2, m_file);

					switch (TYPE)
					{
					case 1:
						format = bqSoundFormat::int16;
						break;
					case 3:
						format = bqSoundFormat::float32; // я пока ХЗ на счёт float64
						break;
					}

					uint16_t channels = 0;
					fread(&channels, 1, 2, m_file);
					uint32_t sampleRate = 0;
					fread(&sampleRate, 1, 4, m_file);
					uint32_t nAvgBytesPerSec = 0;
					fread(&nAvgBytesPerSec, 1, 4, m_file);
					uint16_t blockSize = 0;
					fread(&blockSize, 1, 2, m_file);
					uint16_t bitsPerSample = 0;
					fread(&bitsPerSample, 1, 2, m_file);
					if (format == bqSoundFormat::int16)
					{
						switch (bitsPerSample)
						{
						case 8:
							format = bqSoundFormat::uint8;
							break;
						case 24:
							format = bqSoundFormat::int24;
							break;
						case 32:
							format = bqSoundFormat::int32;
							break;
						}
					}
					else if (format == bqSoundFormat::float32)
					{
						// возможно можно определить так
						//if (bitsPerSample == 64)
						//	format = bqSoundFormat::float64;
					}
					if (FMTChunkSize > 16)
					{
						uint16_t extraFormatInfoSz = 0;
						fread(&extraFormatInfoSz, 1, 2, m_file);
						if (extraFormatInfoSz == 22)
						{
							uint16_t wValidBitsPerSample = 0;
							uint32_t dwChannelMask = 0;
							uint8_t SubFormat[16];
							fread(&wValidBitsPerSample, 1, 2, m_file);
							fread(&dwChannelMask, 1, 4, m_file);
							fread(SubFormat, 1, 16, m_file);
						}
					}

					while (!feof(m_file))
					{
						char chunkName[5] = { 0,0,0,0,0 };
						fread(chunkName, 1, 4, m_file);

						if (TYPE != 1)
						{
							if (strcmp(chunkName, "fact") == 0)
							{
								uint32_t factcksize = 0;
								fread(&factcksize, 1, 4, m_file);
								if (factcksize < 4)
									return false;

								for (uint32_t i = 0, sz = factcksize / 4; i < sz; ++i)
								{
									uint32_t factdwSampleLength = 0;
									fread(&factdwSampleLength, 1, 4, m_file);
								}
							}
						}

						if (strcmp(chunkName, "PEAK") == 0)
						{
							uint32_t peakchunkDataSize = 0;
							uint32_t peakversion = 0;
							uint32_t peaktimeStamp = 0;
							struct PositionPeak
							{
								float   value;    /* signed value of peak */
								uint32_t position; /* the sample frame for the peak */
							};

							fread(&peakchunkDataSize, 1, 4, m_file);
							fread(&peakversion, 1, 4, m_file);
							fread(&peaktimeStamp, 1, 4, m_file);

							if (peakchunkDataSize > 8)// надо ли это делать?
							{
								PositionPeak peak;
								for (uint32_t i = 0; i < channels; ++i)
								{
									fread(&peak.value, 1, 4, m_file);
									fread(&peak.position, 1, 4, m_file);
								}
							}
						}

						if (strcmp(chunkName, "data") == 0)
						{
							uint32_t dataSize = 0;
							fread(&dataSize, 1, 4, m_file);
							if (dataSize)
							{
								m_dataSize = dataSize;
								//	printf("DATA SIZE: %u\n", m_dataSize);
								if (format != bqSoundFormat::unsupported)
								{
									//тут проверка поддерживаемого формата
									// если всё ОК то isGood = true;
									m_info.m_format = format;
									if (format == bqSoundFormat::float32)
									{
										m_info.m_bitsPerSample = 32;
										m_info.m_bytesPerSample = 4;
										isGood = true;
									}
									else if (format == bqSoundFormat::int16)
									{
										m_info.m_bitsPerSample = 16;
										m_info.m_bytesPerSample = 2;
										isGood = true;
									}
									else if (format == bqSoundFormat::uint8)
									{
										m_info.m_bitsPerSample = 8;
										m_info.m_bytesPerSample = 1;
										isGood = true;
									}

									if (isGood)
									{
										m_info.m_channels = channels;
										m_info.m_sampleRate = sampleRate;
										m_info.m_blockSize = m_info.m_bytesPerSample * m_info.m_channels;
										m_info.m_numOfSamples = dataSize / m_info.m_blockSize;
										m_info.m_time = ceil((float)dataSize / (float)m_info.m_channels)
											/ (float)(m_info.m_sampleRate * m_info.m_bytesPerSample);

										m_firstDataBlock = ftell(m_file);
										m_currentDataBlock = m_firstDataBlock;
										m_lastDataBlock = m_firstDataBlock + m_dataSize;
									}

								}
								else
								{
									bqLog::PrintWarning("Unsupported sound file format\n");
								}
							}
						}

						if (isGood)
							break;
					}
				}
			}

			if (!isGood)
			{
				fclose(m_file);
				m_file = 0;
				return false;
			}
			return true;
		}
	}

	return false;
}

void bqSoundStreamFileWAV::Close()
{
	if (m_file)
	{
		fclose(m_file);
		m_file = 0;
	}
}

size_t bqSoundStreamFileWAV::Read(void* buffer, size_t size)
{
	BQ_ASSERT_ST(m_file);

	// при чтении может быть такое что в файле после звука идут
// ещё другие данные. 
// Необходимо не давать читать файл после звуковых данных
	size_t newPosition = m_currentDataBlock + size;
	if (newPosition >= m_lastDataBlock)
	{
		fseek(m_file, 0, SEEK_END);

		size = m_lastDataBlock - m_currentDataBlock;
		//	printf("SIZE %u\n", size);
		if (!size)
		{
			return 0;
		}
	}

	size_t rn = fread(buffer, 1, size, m_file);

	m_currentDataBlock = ftell(m_file);

	return rn;
}

void bqSoundStreamFileWAV::MoveToFirstDataBlock()
{
	BQ_ASSERT_ST(m_file);
	m_currentDataBlock = m_firstDataBlock;
	fseek(m_file, m_currentDataBlock, SEEK_SET);
}

long bqSoundStreamFileWAV::Tell()
{
	BQ_ASSERT_ST(m_file);
	return ftell(m_file);
}

void bqSoundStreamFileWAV::Seek(long v)
{
	BQ_ASSERT_ST(m_file);
	fseek(m_file, v, SEEK_SET);
}

bool bqSoundStreamFileWAV::eof()
{
	BQ_ASSERT_ST(m_file);
	auto f = feof(m_file);
	return (f != 0);
}

#endif
