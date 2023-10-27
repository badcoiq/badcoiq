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
uint32_t bitsPerSample,
)
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
		int16_t loudness = 32767;

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
				auto sn2 = bqSoundSource_sin_tri(rad);
				v._16 = (int16_t)(sn2 * loudness);
			}
			else if (waveType == bqSoundWaveType::saw)
			{
				auto sn2 = bqSoundSource_sin_saw(rad);
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
fopen_s(&f, "sound.wav", "wb");
	if(f)
	{
fwrite("RIFF", 4, 1, f);
			
			int32_t fileSz = m_dataSize + 44;
			fwrite(&fileSz, 4, 1, f);
			
			fwrite("WAVE", 4, 1, f);
			fwrite("fmt ", 4, 1, f);
			
bits per sample?
			int32_t chnkSz = 16;
			fwrite(&chnkSz, 4, 1, f);
			
			// pcm
			int16_t TYPE = 1;
			fwrite(&TYPE, 2, 1, f);
			
			fwrite(&m_channels, 2, 1, f);
			fwrite(&m_sampleRate, 4, 1, f);
			
	int nAvgBytesPerSec = (m_sampleRate *
		  m_bitsPerSample * m_channels)/8;
			// byterate
			fwrite(&nAvgBytesPerSec, 4, 1, f);
			
			fwrite(&m_blockSize, sizeof(m_blockSize), 1, f);
			fwrite(&m_bitsPerSample, sizeof(m_bitsPerSample), 1, f);
			
			fwrite("data", 4, 1, f);
			fwrite(&m_dataSize, 4, 1, f);
			fwrite(m_data, m_dataSize, 1, f)
fclose(f);
return true;
}

return false;
}
