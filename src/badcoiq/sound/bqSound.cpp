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


float sin_tri(float rads)
{
	float pi = M_PI;
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
	printf("\n");
	return r;
}

float sin_saw(float rads)
{
	float v = -1.f;
	float r = 0.f;
	float pi = M_PI;
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
bqSoundSource* bqSound_createNew(float time)
{
	bqSoundSource* newSound = new bqSoundSource;
	newSound->m_channels = 1;
	newSound->m_sampleRate = 44100;

int16_t bitsPerSample = 16;
int bytesPerSample = bitsPerSample / 8;

// nBlockAlign
		int16_t blockSize = bytesPerSample * newSound->m_channels;
		
int numOfSamples = (double)newSound->m_sampleRate * time;

int dataSize = numOfSamples * bytesPerSample;
		dataSize *= newSound->m_channels;

	newSound->m_size = dataSize;


	newSound->m_data = (uint8_t*)bqMemory::malloc(newSound->m_size);

	return newSound;
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
		m_soundSource = bqSound_createNew(time);

// некоторые значения надо добавить в класс
	int numOfSamples = (double)m_soundSource->m_sampleRate * time;
		
float samplesPerWave = (float)m_soundSource->m_sampleRate / Hz;
	float angle = 0.f;
		float angleStep = 360.f / samplesPerWave;
		float loudness = 32767.f;
		
		
			for(int i = 0, index = 0; i < numOfSamples; ++i)
			{
				auto rad = DegToRad(angle);
				auto sn = sin(rad);

				angle += angleStep;
				if (angle > 360.f)
				{
					angle = 0.f;
				}

				_short v;
				
				if(waveType == WaveType_sin)
				{
				
					v._16 = (int16_t)(sn * loudness);
			//		printf("%f %f %f %i\n", angle, rad, sn, v._16);
				}
				else if(waveType == WaveType_square)
				{
					if(sn >= 0.f)
					{
						v._16 = loudness;
					}
					else
					{
						v._16 = -loudness;
					}
				}
				else if(waveType == WaveType_triangle)
				{
					auto sn2 = sin_tri(rad);
					v._16 = (int16_t)(sn2 * loudness);
				}
				else if(waveType == WaveType_saw)
				{
					auto sn2 = sin_saw(rad);
					v._16 = (int16_t)(sn2 * loudness);
				}

				data[index++] = v._8[0];
				data[index++] = v._8[1];
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


