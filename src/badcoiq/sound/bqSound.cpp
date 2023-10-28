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
#include "badcoiq/math/bqMath.h"
#include "badcoiq/common/bqFileBuffer.h"


float bqSoundSource_sin_tri(float rads)
{
	float pi = PIf;
	float halfPi = pi * 0.5f;
	float _270 = halfPi + halfPi + halfPi;
	float _180 = pi;
	float _90 = halfPi;
	
	float M = 1.f / halfPi;
	float r = 0.f;

	if (rads >= _270)
	{
		r = (1.f - (-3.f + (rads * M))) * -1.f;
	}
	else if (rads >= _180)
	{
		r = 2.f - (rads * M);
	}
	else if (rads >= _90)
	{
		r = 2.f - (rads * M);
	//	printf("%f %f %f\n", rads, rads *M, r);
	}
	else
	{
		r = rads * M;
		//printf("%f %f %f\n", rads, rads *M, r);
	}
//	printf("\n");
	return r;
}

float bqSoundSource_sin_saw(float rads)
{
	float v = -1.f;
	float r = 0.f;
	float pi = PIf;
	float halfPi = pi * 0.5f;
	float _270 = halfPi + halfPi + halfPi;
	float _180 = pi;
	float _90 = halfPi;
	float halfHalfPi = pi * 0.25f;
	float M = 1.f / pi;

	if (rads >= _270)
	{
		// from -0.5 to 0
		r = (rads * M) - 2.f;
//		printf("%f %f %f\n", rads, rads *M, r);
	}
	else if (rads >= _180)
	{
		// from -1 to -0.5
		r = (rads * M) - 2.f;
	//	printf("%f %f %f\n", rads, rads *M, r);
	}
	else if (rads >= _90)
	{
		// from 0.5 to 1
		r = (rads * M);
	//	printf("%f %f %f\n", rads, rads *M, r);
	}
	else
	{
		// from 0 to 0.5
		r = rads * M;
		
		//printf("%f\n", r);
	}
//	printf("\n");
	return r;
}

bqSoundSource::bqSoundSource()
{
}

bqSoundSource::~bqSoundSource()
{
	if (m_data)
	{
		bqMemory::free(m_data);
		m_data = 0;
	}
}

bqSound::bqSound()
{
}

bqSound::~bqSound()
{
	Clear();
}

void bqSound::Clear()
{
	if (m_soundSource)
	{
		delete m_soundSource;
		m_soundSource = 0;
	}
}

// 
void bqSound::Create(float time,
	uint32_t channels,
	uint32_t sampleRate,
	uint32_t bitsPerSample)
{
	Clear();

	bqSoundSource* newSound = new bqSoundSource;
	newSound->m_channels = channels;
	newSound->m_sampleRate = sampleRate;
	newSound->m_bitsPerSample = bitsPerSample;
	newSound->m_bytesPerSample = newSound->m_bitsPerSample / 8;
	newSound->m_blockSize = newSound->m_bytesPerSample * newSound->m_channels;
	newSound->m_time = time;

	newSound->m_numOfSamples = (uint32_t)ceil((float)newSound->m_sampleRate * time);
	// альтернативный способ получить m_numOfSamples
	// для данного метода не подходит так как создание происходит на основе времени
	//uint32_t numOfBlocks = m_soundSource->m_dataSize / m_soundSource->m_blockSize;
	//m_soundSource->m_numOfSamples = numOfBlocks;

	newSound->m_dataSize = newSound->m_numOfSamples * newSound->m_bytesPerSample;
	newSound->m_dataSize *= newSound->m_channels;

	newSound->m_data = (uint8_t*)bqMemory::malloc(newSound->m_dataSize);

	m_soundSource = newSound;
}

void bqSound::Generate(
	bqSoundWaveType waveType,
	float time, 
	uint32_t Hz, 
	float _loudness)
{
	Clear();
	
	if (time > 0.f)
	{
		Create(time, 1, 44100, 16);

		double samplesPerWave = (double)m_soundSource->m_sampleRate / Hz;
		double angle = 0.f;
		double angleStep = 360.0 / samplesPerWave;

		float lM = 1.f / 32767.f;
		int16_t loudness = int16_t(_loudness / lM);
	//	printf("%f %f %i\n", _loudness, lM, loudness);

		// наверно лучше использовать это для сброса угла
		uint32_t samplesPerWavei = (uint32_t)floor(samplesPerWave);
		uint32_t samplesPerWaveindex = 0;


		for (uint32_t i = 0, index = 0; i < m_soundSource->m_numOfSamples; ++i)
		{
			auto rad = bqMath::DegToRad(angle);
			auto sn = sin(rad);

			++samplesPerWaveindex;
			if(samplesPerWaveindex >= samplesPerWavei)
			{ 
				samplesPerWaveindex = 0;
				angle = 0.;
			}

			angle += angleStep;
			if (angle >= 360.)
			{
				angle = 0.;
			}

			union _short
			{
				int8_t _8[2];
				int16_t _16;
			};
			_short v;

			if (waveType == bqSoundWaveType::sin)
			{

				v._16 = (int16_t)(sn * loudness);
				//		printf("%f %f %f %i\n", angle, rad, sn, v._16);
			}
			else if (waveType == bqSoundWaveType::square)
			{
				if (sn >= 0.f)
				{
					v._16 = loudness;
				}
				else
				{
					v._16 = -loudness;
				}
			}
			else if (waveType == bqSoundWaveType::triangle)
			{
				auto sn2 = bqSoundSource_sin_tri((float)rad);
				v._16 = (int16_t)(sn2 * loudness);
			}
			else if (waveType == bqSoundWaveType::saw)
			{
				auto sn2 = bqSoundSource_sin_saw((float)rad);
				v._16 = (int16_t)(sn2 * loudness);
			}

			m_soundSource->m_data[index++] = v._8[0];
			m_soundSource->m_data[index++] = v._8[1];
		}
	}
	/*if (!m_soundSource)
	{
		int time = 1;

		m_soundSource = new bqSoundSource;
		m_soundSource->m_channels = 1;
		m_soundSource->m_sampleRate = 44100;
		m_soundSource->m_size = time * 2 * m_soundSource->m_sampleRate;
		m_soundSource->m_data = (uint8_t*)bqMemory::malloc(m_soundSource->m_size);

		union _short
		{
			int8_t _8[2];
			int16_t _16;
		};

		// Объяснение:
		//   Значение sample rate 44100 это сколько герц проигрывается за 1 секунду.
		//                                         сэмплов
		// 
		//   Чтобы проиграть звук определённой частоты, делим 44100 на эту частоту.
		//   Получается значение, типа, одна волна проходит на столько-то единиц в 44100 значении
		//                                          использует столько-то сэмплов
		//   То есть, 
		//      Hz = 440
		//      hz = 44100 / Hz = 100.2272727
		//      Синусоида должна сделать круг типа за 100 шагов.
		//   Но шаги циклом не делаем, а вычисляем angleStep, и просто 
		//   делаем приращение угла на это значение.
		//   
		//   Нам известно что из 44100 на создание одной волны нужно 100 сэмплов (при Hz = 441)
		//   Получаем значение которым будет прибавлять к углу
		//      angleStep = 360 градусов / hz
		//   
		//   Данный код надо будет задокументировать и оставить чтобы не потерялся.
		for (int index = 0, second = 0; second < time; second++)
		{
			float Hz = 365.f;
			float hz = 44100.f / Hz;
			printf("hz %f\n", hz);

			float angle = 0.f;
			float angleStep = 360.f / hz;
			printf("angleStep %f\n", angleStep);


			for (int cycle = 0; cycle < 44100; cycle++)
			{
				auto rad = bqMath::DegToRad(angle);
				auto sn = sin(rad);

				angle += angleStep;
				if (angle > 360.f)
				{
					angle = 0.f;
				}

				_short v;

				// амплитуда\громкость.
				// дефолтное значение 32767.f. можно считать максимальным значением.
				//  выше уже типа перегруз.
				v._16 = (int16_t)(sn * 32767.f);
			//		printf("%f %f %f %i\n", angle, rad, sn, v._16);

				m_soundSource->m_data[index++] = v._8[0];
				m_soundSource->m_data[index++] = v._8[1];
			}
		}
	}
*/
}

bool bqSound::SaveToFile(bqSoundFileType ft, const char* fn)
{
	BQ_ASSERT_ST(fn);
	BQ_ASSERT_ST(m_soundSource);

	switch(ft)
	{
	default:
	case bqSoundFileType::wav:
	return _saveWav(fn);
	}
	return false;
}

bool bqSound::_saveWav(const char* fn)
{
	FILE * f =0;
	fopen_s(&f, fn, "wb");
	if(f)
	{
		fwrite("RIFF", 4, 1, f);
			
		int32_t chunkSz = m_soundSource->m_dataSize + 44 - 8;
		fwrite(&chunkSz, 4, 1, f);
			
		fwrite("WAVE", 4, 1, f);
		fwrite("fmt ", 4, 1, f);
			
//bits per sample?
		int32_t subchnkSz = 16;
		fwrite(&subchnkSz, 4, 1, f);
			
		// pcm
		int16_t TYPE = 1;
		fwrite(&TYPE, sizeof(TYPE), 1, f);
			
		fwrite(&m_soundSource->m_channels, 2, 1, f);
		fwrite(&m_soundSource->m_sampleRate, 4, 1, f);
			
		int nAvgBytesPerSec = (m_soundSource->m_sampleRate *
			m_soundSource->m_bitsPerSample * m_soundSource->m_channels)/8;
		// byterate
		fwrite(&nAvgBytesPerSec, 4, 1, f);
			
		fwrite(&m_soundSource->m_blockSize, 2, 1, f);
		fwrite(&m_soundSource->m_bitsPerSample, 2, 1, f);
			
		fwrite("data", 4, 1, f);
		fwrite(&m_soundSource->m_dataSize, 4, 1, f);
		fwrite(m_soundSource->m_data, m_soundSource->m_dataSize, 1, f);
		fclose(f);
		return true;
	}

	return false;
}

bool bqSound::LoadFromFile(const char* fn)
{
	Clear();

	bqString path(fn);

	if (path.extension(".wav"))
	{
		return _loadWav(fn);
	}

	/*bqImage* img = 0;
	uint32_t file_size = 0;
	uint8_t* ptr = bqFramework::SummonFileBuffer(path, &file_size, false);
	if (ptr)
	{
		img = LoadBMP(path, ptr, (uint32_t)file_size);
		bqDestroy(ptr);
	}
	return img;*/

	return false;
}

bool bqSound::_loadWav(const char* fn)
{
	uint32_t file_size = 0;
	uint8_t* ptr = bqFramework::SummonFileBuffer(fn, &file_size, false);
	if (ptr)
	{
		return _loadWav(ptr, (uint32_t)file_size);
		bqDestroy(ptr);
	}
	return false;
}

bool bqSound::_loadWav(uint8_t* buffer, uint32_t bufferSz)
{
	bqFileBuffer file(buffer, bufferSz);

	char riff[5] = { 0,0,0,0,0 };
	file.Read(riff, 4);
	if (strcmp(riff, "RIFF") == 0)
	{
		uint32_t RIFFChunkSize = 0;
		file.Read(&RIFFChunkSize, 4);

		char wave[5] = { 0,0,0,0,0 };
		file.Read(wave, 4);
		if (strcmp(wave, "WAVE") == 0)
		{
			char fmt[5] = { 0,0,0,0,0 };
			file.Read(fmt, 4);
			if (strcmp(fmt, "fmt ") == 0)
			{
				uint32_t FMTChunkSize = 0;
				file.Read(&FMTChunkSize, 4);

				uint16_t format = 0;
				file.Read(&FMTChunkSize, 2);

				uint16_t channels = 0;
				file.Read(&channels, 2);

				uint32_t sampleRate = 0;
				file.Read(&sampleRate, 4);

				uint32_t byteRate = 0;
				file.Read(&byteRate, 4);

				uint16_t blockAlign = 0;
				file.Read(&blockAlign, 2);

				uint16_t bitsPerSample = 0;
				file.Read(&bitsPerSample, 2);

				char data[5] = { 0,0,0,0,0 };
				file.Read(data, 4);
				if (strcmp(data, "data") == 0)
				{
					uint32_t dataSize = 0;
					file.Read(&dataSize, 4);
					if (dataSize)
					{
						Create(0.1f, channels, sampleRate, bitsPerSample);
						if (m_soundSource->m_dataSize < dataSize)
							_reallocate(dataSize);

						file.Read(m_soundSource->m_data, m_soundSource->m_dataSize);
						CalculateTime();
					}
				}
			}
		}
	}

	return false;
}

float bqSound::CalculateTime()
{
	float time = 0.f;
	if (m_soundSource)
	{
		BQ_ASSERT_ST(m_soundSource->m_sampleRate > 0);

		if (m_soundSource->m_sampleRate && m_soundSource->m_dataSize)
		{
			uint32_t numOfBlocks = m_soundSource->m_dataSize / m_soundSource->m_blockSize;
			m_soundSource->m_numOfSamples = numOfBlocks;
			m_soundSource->m_time = 1.f / m_soundSource->m_sampleRate;
			m_soundSource->m_time *= m_soundSource->m_numOfSamples;
		}
	}

	return time;
}

// если при чтении файла нужен будет буфер бОльшего размера
// надо его увеличить.
void bqSound::_reallocate(uint32_t newSz)
{
	uint8_t* newBuf = (uint8_t*)bqMemory::malloc(newSz);
	memcpy(newBuf, m_soundSource->m_data, m_soundSource->m_dataSize);
	bqMemory::free(m_soundSource->m_data);
	m_soundSource->m_data = newBuf;
	m_soundSource->m_dataSize = newSz;
}
