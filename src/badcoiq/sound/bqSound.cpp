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


float bqSoundBuffer_sin_tri(float rads)
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

float bqSoundBuffer_sin_saw(float rads)
{
//	float v = -1.f;
	float r = 0.f;
	float pi = PIf;
	float halfPi = pi * 0.5f;
	float _270 = halfPi + halfPi + halfPi;
	float _180 = pi;
	float _90 = halfPi;
//	float halfHalfPi = pi * 0.25f;
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

bqSoundBuffer::bqSoundBuffer()
{
}

bqSoundBuffer::~bqSoundBuffer()
{
	if (m_bufferData.m_data)
	{
		bqMemory::free(m_bufferData.m_data);
		m_bufferData.m_data = 0;
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
	if (m_soundBuffer)
	{
		delete m_soundBuffer;
		m_soundBuffer = 0;
	}
}

// 
void bqSound::Create(float time,
	uint32_t channels,
	uint32_t sampleRate,
	uint32_t bitsPerSample)
{
	Clear();

	bqSoundBuffer* newSound = new bqSoundBuffer;
	newSound->m_bufferInfo.m_channels = channels;
	newSound->m_bufferInfo.m_sampleRate = sampleRate;
	newSound->m_bufferInfo.m_bitsPerSample = bitsPerSample;
	newSound->m_bufferInfo.m_bytesPerSample = newSound->m_bufferInfo.m_bitsPerSample / 8;
	newSound->m_bufferInfo.m_blockSize = newSound->m_bufferInfo.m_bytesPerSample * newSound->m_bufferInfo.m_channels;
	newSound->m_bufferInfo.m_time = time;

	newSound->m_bufferInfo.m_numOfSamples = (uint32_t)ceil((float)newSound->m_bufferInfo.m_sampleRate * time);
	// альтернативный способ получить m_numOfSamples
	// для данного метода не подходит так как создание происходит на основе времени
	//uint32_t numOfBlocks = m_soundBuffer->m_dataSize / m_soundBuffer->m_blockSize;
	//m_soundBuffer->m_numOfSamples = numOfBlocks;

	newSound->m_bufferData.m_dataSize = newSound->m_bufferInfo.m_numOfSamples * newSound->m_bufferInfo.m_bytesPerSample;
	newSound->m_bufferData.m_dataSize *= newSound->m_bufferInfo.m_channels;

	newSound->m_bufferData.m_data = (uint8_t*)bqMemory::malloc(newSound->m_bufferData.m_dataSize);

	newSound->m_format = bqSoundFormatFindFormat(newSound->m_bufferInfo);

	m_soundBuffer = newSound;
}

bqSoundFormat bqSoundFormatFindFormat(const bqSoundBufferInfo& info)
{
	bqSoundFormat r = bqSoundFormat::unsupported;

	if (info.m_channels == 1)
	{
		if (info.m_sampleRate == 44100)
		{
			if(info.m_bitsPerSample == 8)
				r = bqSoundFormat::uint8_mono_44100;
			else if (info.m_bitsPerSample == 16)
				r = bqSoundFormat::uint16_mono_44100;
		}
	}
	else if (info.m_channels == 2)
	{
		if (info.m_sampleRate == 44100)
		{
			if (info.m_bitsPerSample == 8)
				r = bqSoundFormat::uint8_stereo_44100;
			else if (info.m_bitsPerSample == 16)
				r = bqSoundFormat::uint16_stereo_44100;
		}
	}
	return r;
}

void bqSoundBuffer::MakeMono(uint32_t how)
{
	auto type = bqSoundFormatFindFormat(m_bufferInfo);

	switch (type)
	{
	case bqSoundFormat::uint8_stereo_44100:
	case bqSoundFormat::uint16_stereo_44100:
	{
		uint32_t _channels = 1;
		uint32_t _blockSize = m_bufferInfo.m_bytesPerSample * _channels;
		uint32_t _dataSize = m_bufferInfo.m_numOfSamples * m_bufferInfo.m_bytesPerSample * _channels;
		uint8_t* newData = (uint8_t*)bqMemory::calloc(_dataSize);
		
		uint8_t* dst = newData;
		uint8_t* src = m_bufferData.m_data;

		uint8_t* dst8 = dst;
		uint16_t* dst16 = (uint16_t*)dst;
		
		uint8_t* src8 = src;
		uint16_t* src16 = (uint16_t*)src;

		uint32_t numBlocks = _dataSize / _blockSize;
		for (uint32_t i = 0; i < numBlocks; ++i)
		{
			switch (type)
			{
			case bqSoundFormat::uint8_stereo_44100:
			{
				if (how)
				{
					*dst8 = *src8;
					++dst8;
					++src8;
					++src8;
				}
				else
				{
					++src8;
					*dst8 = *src8;
					++dst8;
					++src8;
				}
			}break;
			case bqSoundFormat::uint16_stereo_44100:
			{
				if (how)
				{
					*dst16 = *src16;
					++dst16;
					++src16;
					++src16;
				}
				else
				{
					++src16;
					*dst16 = *src16;
					++dst16;
					++src16;
				}
			}break;
			}
		}

		if (m_bufferData.m_data)
			bqMemory::free(m_bufferData.m_data);

		m_bufferInfo.m_channels = _channels;
		m_bufferInfo.m_blockSize = _blockSize;
		m_bufferData.m_data = newData;
		m_bufferData.m_dataSize = _dataSize;
	}break;
	default:
		break;
	}
	m_format = bqSoundFormatFindFormat(m_bufferInfo);
}

void bqSoundBuffer::MakeStereo()
{
	auto type = bqSoundFormatFindFormat(m_bufferInfo);
	
	switch (type)
	{
	case bqSoundFormat::uint8_mono_44100:
	case bqSoundFormat::uint16_mono_44100:
	{
		uint32_t _channels = 2;
		uint32_t _blockSize = m_bufferInfo.m_bytesPerSample * _channels;
		uint32_t _dataSize = m_bufferInfo.m_numOfSamples * m_bufferInfo.m_bytesPerSample * _channels;
		uint8_t* newData = (uint8_t*)bqMemory::calloc(_dataSize);

		uint8_t* dst = newData;
		uint8_t* src = m_bufferData.m_data;

		uint8_t* dst8 = dst;
		uint16_t* dst16 = (uint16_t*)dst;

		uint8_t* src8 = src;
		uint16_t* src16 = (uint16_t*)src;

		uint32_t numBlocks = _dataSize / _blockSize;
		for (uint32_t i = 0; i < numBlocks; ++i)
		{
			switch (type)
			{
			case bqSoundFormat::uint8_mono_44100:
			{
				*dst8 = *src8;
				++dst8;
				*dst8 = *src8;
				++dst8;
				++src8;
			}break;
			case bqSoundFormat::uint16_mono_44100:
			{
				*dst16 = *src16;
				++dst16;
				*dst16 = *src16;
				++dst16;
				++src16;
			}break;
			}
		}

		if (m_bufferData.m_data)
			bqMemory::free(m_bufferData.m_data);

		m_bufferInfo.m_channels = _channels;
		m_bufferInfo.m_blockSize = _blockSize;
		m_bufferData.m_data = newData;
		m_bufferData.m_dataSize = _dataSize;
	}break;
	default:
		break;
	}

	m_format = bqSoundFormatFindFormat(m_bufferInfo);
}

void bqSoundBuffer::Make8bits()
{
	auto type = bqSoundFormatFindFormat(m_bufferInfo);

	switch (type)
	{
	case bqSoundFormat::uint16_mono_44100:
	case bqSoundFormat::uint16_stereo_44100:
	{
		uint32_t _channels = 2;

		if(type == bqSoundFormat::uint16_mono_44100)
			_channels = 1;
		 
		uint32_t _bytesPerSample = 1;
		uint32_t _bitsPerSample = 8;
		uint32_t _blockSize = _bytesPerSample * _channels;
		uint32_t _dataSize = m_bufferInfo.m_numOfSamples * _bytesPerSample * _channels;
		uint8_t* newData = (uint8_t*)bqMemory::calloc(_dataSize);

		uint8_t* dst8 = (uint8_t*)newData;
		uint16_t* src16 = (uint16_t*)m_bufferData.m_data;
		uint32_t numBlocks = _dataSize / _blockSize;
		for (uint32_t i = 0; i < numBlocks; ++i)
		{
			switch (type)
			{
			case bqSoundFormat::uint16_mono_44100:
			{
				*dst8 = (*src16 + 32767) >> 8;
				++src16;
				++dst8;
			}break;
			case bqSoundFormat::uint16_stereo_44100:
			{
				*dst8 = (*src16 + 32767) >> 8;
				++src16;
				++dst8;
				*dst8 = (*src16 + 32767) >> 8;
				++src16;
				++dst8;
			}break;
			}
		}

		if (m_bufferData.m_data)
			bqMemory::free(m_bufferData.m_data);

		m_bufferInfo.m_channels = _channels;
		m_bufferInfo.m_blockSize = _blockSize;
		m_bufferInfo.m_bytesPerSample = _bytesPerSample;
		m_bufferInfo.m_bitsPerSample = _bitsPerSample;
		m_bufferData.m_data = newData;
		m_bufferData.m_dataSize = _dataSize;
	}break;
	default:
		break;
	}
	
	m_format = bqSoundFormatFindFormat(m_bufferInfo);
}

void bqSoundBuffer::Make16bits()
{
	auto type = bqSoundFormatFindFormat(m_bufferInfo);

	switch (type)
	{
	case bqSoundFormat::uint8_mono_44100:
	case bqSoundFormat::uint8_stereo_44100:
	{
		uint32_t _channels = 2;

		if (type == bqSoundFormat::uint8_mono_44100)
			_channels = 1;

		uint32_t _bytesPerSample = 2;
		uint32_t _bitsPerSample = 16;
		uint32_t _blockSize = _bytesPerSample * _channels;
		uint32_t _dataSize = m_bufferInfo.m_numOfSamples * _bytesPerSample * _channels;
		uint8_t* newData = (uint8_t*)bqMemory::calloc(_dataSize);

		uint16_t* dst16 = (uint16_t*)newData;
		uint8_t* src8 = (uint8_t*)m_bufferData.m_data;
		uint32_t numBlocks = _dataSize / _blockSize;
		for (uint32_t i = 0; i < numBlocks; ++i)
		{
			switch (type)
			{
			case bqSoundFormat::uint8_mono_44100:
			{
				*dst16 = ((uint16_t)*src8 << 8) - 32767;
				++src8;
				++dst16;
			}break;
			case bqSoundFormat::uint8_stereo_44100:
			{
				*dst16 = ((uint16_t)*src8 << 8) - 32767;
				++src8;
				++dst16;
				*dst16 = ((uint16_t)*src8 << 8) - 32767;
				++src8;
				++dst16;
			}break;
			}
		}

		if (m_bufferData.m_data)
			bqMemory::free(m_bufferData.m_data);

		m_bufferInfo.m_channels = _channels;
		m_bufferInfo.m_blockSize = _blockSize;
		m_bufferInfo.m_bytesPerSample = _bytesPerSample;
		m_bufferInfo.m_bitsPerSample = _bitsPerSample;
		m_bufferData.m_data = newData;
		m_bufferData.m_dataSize = _dataSize;
	}break;
	default:
		break;
	}

	m_format = bqSoundFormatFindFormat(m_bufferInfo);
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

		double samplesPerWave = (double)m_soundBuffer->m_bufferInfo.m_sampleRate / Hz;
		double angle = 0.f;
		double angleStep = 360.0 / samplesPerWave;

		float lM = 1.f / 32767.f;
		int16_t loudness = int16_t(_loudness / lM);
	//	printf("%f %f %i\n", _loudness, lM, loudness);

		// наверно лучше использовать это для сброса угла
		uint32_t samplesPerWavei = (uint32_t)floor(samplesPerWave);
		uint32_t samplesPerWaveindex = 0;


		for (uint32_t i = 0, index = 0; i < m_soundBuffer->m_bufferInfo.m_numOfSamples; ++i)
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
				auto sn2 = bqSoundBuffer_sin_tri((float)rad);
				v._16 = (int16_t)(sn2 * loudness);
			}
			else if (waveType == bqSoundWaveType::saw)
			{
				auto sn2 = bqSoundBuffer_sin_saw((float)rad);
				v._16 = (int16_t)(sn2 * loudness);
			}

			m_soundBuffer->m_bufferData.m_data[index++] = v._8[0];
			m_soundBuffer->m_bufferData.m_data[index++] = v._8[1];
		}
	}
	/*if (!m_soundBuffer)
	{
		int time = 1;

		m_soundBuffer = new bqSoundBuffer;
		m_soundBuffer->m_channels = 1;
		m_soundBuffer->m_sampleRate = 44100;
		m_soundBuffer->m_size = time * 2 * m_soundBuffer->m_sampleRate;
		m_soundBuffer->m_data = (uint8_t*)bqMemory::malloc(m_soundBuffer->m_size);

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

				m_soundBuffer->m_data[index++] = v._8[0];
				m_soundBuffer->m_data[index++] = v._8[1];
			}
		}
	}
*/
}

bool bqSound::SaveToFile(bqSoundFileType ft, const bqStringA& fn)
{
	return SaveToFile(ft, fn.c_str());
}

bool bqSound::SaveToFile(bqSoundFileType ft, const char* fn)
{
	BQ_ASSERT_ST(fn);
	BQ_ASSERT_ST(m_soundBuffer);

	switch(ft)
	{
	default:
	case bqSoundFileType::wav:
	return _saveWav(fn);
	}
}

bool bqSound::_saveWav(const char* fn)
{
	FILE * f =0;
	fopen_s(&f, fn, "wb");
	if(f)
	{
		fwrite("RIFF", 4, 1, f);
			
		int32_t chunkSz = m_soundBuffer->m_bufferData.m_dataSize + 44 - 8;
		fwrite(&chunkSz, 4, 1, f);
			
		fwrite("WAVE", 4, 1, f);
		fwrite("fmt ", 4, 1, f);
			
//bits per sample?
		int32_t subchnkSz = 16;
		fwrite(&subchnkSz, 4, 1, f);
			
		// pcm
		int16_t TYPE = 1;
		fwrite(&TYPE, sizeof(TYPE), 1, f);
			
		fwrite(&m_soundBuffer->m_bufferInfo.m_channels, 2, 1, f);
		fwrite(&m_soundBuffer->m_bufferInfo.m_sampleRate, 4, 1, f);
			
		int nAvgBytesPerSec = (m_soundBuffer->m_bufferInfo.m_sampleRate *
			m_soundBuffer->m_bufferInfo.m_bitsPerSample * m_soundBuffer->m_bufferInfo.m_channels)/8;
		// byterate
		fwrite(&nAvgBytesPerSec, 4, 1, f);
			
		fwrite(&m_soundBuffer->m_bufferInfo.m_blockSize, 2, 1, f);
		fwrite(&m_soundBuffer->m_bufferInfo.m_bitsPerSample, 2, 1, f);
		
		// Где запись этого???
		//uint16_t extraFormatInfoSz = 0;
		// FMTChunkSize он же subchnkSz должен быть больше 16 для записи этого
		// подробности я не знаю.
		//fwrite(&extraFormatInfoSz, 2, 1, f);

		fwrite("data", 4, 1, f);
		fwrite(&m_soundBuffer->m_bufferData.m_dataSize, 4, 1, f);
		fwrite(m_soundBuffer->m_bufferData.m_data, m_soundBuffer->m_bufferData.m_dataSize, 1, f);
		fclose(f);
		return true;
	}

	return false;
}

bool bqSound::LoadFromFile(const bqStringA& fn, bool convertTo16bitStereo)
{
	return LoadFromFile(fn.c_str(), convertTo16bitStereo);
}

bool bqSound::LoadFromFile(const char* fn, bool convertTo16bitStereo)
{
	Clear();

	bqString path(fn);
	bool r = false;
	if (path.extension(".wav"))
	{
		r = _loadWav(fn);

		if (r && convertTo16bitStereo)
		{
			this->m_soundBuffer->Make16bits();
			this->m_soundBuffer->MakeStereo();
		}
	}

	return r;
}

bool bqSound::_loadWav(const char* fn)
{
	uint32_t file_size = 0;
	uint8_t* ptr = bqFramework::SummonFileBuffer(fn, &file_size, false);
	if (ptr)
	{
		bool b = _loadWav(ptr, (uint32_t)file_size);
		bqDestroy(ptr);
		return b;
	}
	return false;
}

void bqSound::Convert(bqSoundFormat type)
{
	BQ_ASSERT_ST(type != bqSoundFormat::unsupported);

	if (m_soundBuffer && (type != bqSoundFormat::unsupported))
	{
		if (m_soundBuffer->m_format != type)
		{
			switch (type)
			{
				case bqSoundFormat::uint8_mono_44100:
				{
					m_soundBuffer->Make8bits();
					m_soundBuffer->MakeMono(0);
				}break;
				case bqSoundFormat::uint8_stereo_44100:
				{
					m_soundBuffer->Make8bits();
					m_soundBuffer->MakeStereo();
				}break;
				case bqSoundFormat::uint16_mono_44100:
				{
					m_soundBuffer->Make16bits();
					m_soundBuffer->MakeMono(0);
				}break;
				case bqSoundFormat::uint16_stereo_44100:
				{
					m_soundBuffer->Make16bits();
					m_soundBuffer->MakeStereo();
				}break;
			}
		}
	}
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
				file.Read(&format, 2);

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

				if (FMTChunkSize > 16)
				{
					uint16_t extraFormatInfoSz = 0;
					file.Read(&extraFormatInfoSz, 2);
				}

				char data[5] = { 0,0,0,0,0 };
				file.Read(data, 4);
				if (strcmp(data, "data") == 0)
				{
					uint32_t dataSize = 0;
					file.Read(&dataSize, 4);
					if (dataSize)
					{
						Create(0.1f, channels, sampleRate, bitsPerSample);
						if (m_soundBuffer->m_bufferData.m_dataSize < dataSize)
							_reallocate(dataSize);

						file.Read(m_soundBuffer->m_bufferData.m_data, m_soundBuffer->m_bufferData.m_dataSize);

						//Convert();

						CalculateTime();
						return true;
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
	if (m_soundBuffer)
	{
		BQ_ASSERT_ST(m_soundBuffer->m_bufferInfo.m_sampleRate > 0);

		if (m_soundBuffer->m_bufferInfo.m_sampleRate && m_soundBuffer->m_bufferData.m_dataSize)
		{
			uint32_t numOfBlocks = m_soundBuffer->m_bufferData.m_dataSize / m_soundBuffer->m_bufferInfo.m_blockSize;
			
			// правильно ли то что количество сэмплов == количеству блоков?
			m_soundBuffer->m_bufferInfo.m_numOfSamples = numOfBlocks;
			
			// может быть блок может состоять из 2х и более сэмплов?
			//m_soundBuffer->m_bufferInfo.m_numOfSamples = numOfBlocks * m_soundBuffer->m_bufferInfo.m_channels;
			
			m_soundBuffer->m_bufferInfo.m_time = 1.f / m_soundBuffer->m_bufferInfo.m_sampleRate;
			m_soundBuffer->m_bufferInfo.m_time *= m_soundBuffer->m_bufferInfo.m_numOfSamples;
		}
	}

	return time;
}

// если при чтении файла нужен будет буфер бОльшего размера
// надо его увеличить.
void bqSound::_reallocate(uint32_t newSz)
{
	uint8_t* newBuf = (uint8_t*)bqMemory::malloc(newSz);
	memcpy(newBuf, m_soundBuffer->m_bufferData.m_data, m_soundBuffer->m_bufferData.m_dataSize);
	bqMemory::free(m_soundBuffer->m_bufferData.m_data);
	m_soundBuffer->m_bufferData.m_data = newBuf;
	m_soundBuffer->m_bufferData.m_dataSize = newSz;
}

//bqSoundBuffer* bqSound::DropSource()
//{
//	bqSoundBuffer* tmp = m_soundBuffer;
//	m_soundBuffer = 0;
//	return tmp;
//}
