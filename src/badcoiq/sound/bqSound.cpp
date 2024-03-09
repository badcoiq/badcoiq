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

//class bqSoundBlock
//{
//	float * m_data = 0;
//	uint32_t m_channels = 1;
//public:
//	bqSoundBlock(uint32_t channels)
//	{
//		if (channels == 0)
//			channels = 1;
//		m_channels = channels;
//
//		m_data = (float*)bqMemory::calloc(sizeof(float) * channels);
//	}
//
//	~bqSoundBlock()
//	{
//		if (m_data)
//			bqMemory::free(m_data);
//	}
//
//	void MakeZero()
//	{
//		for (uint32_t i = 0; i < m_channels; ++i)
//		{
//			m_data[i] = 0;
//		}
//	}
//
//	void FromUint8(uint8_t* other, uint32_t ch)
//	{
//	}
//};

template <typename _type>
class bqSoundSample
{
public:
	_type m_data;

	using sample_type = _type;
};

#include "badcoiq/common/bqPack.h"
struct bqSoundSample24Base { int8_t m_byte[3]; };
#include "badcoiq/common/bqUnpack.h"

using bqSoundSample_8bit = bqSoundSample<uint8_t>;
using bqSoundSample_16bit = bqSoundSample<int16_t>;
using bqSoundSample_24bit = bqSoundSample<bqSoundSample24Base>;
using bqSoundSample_32bit = bqSoundSample<int32_t>;
using bqSoundSample_32bitFloat = bqSoundSample<bqFloat32>;
using bqSoundSample_64bitFloat = bqSoundSample<bqFloat64>;

union bqInteger4
{
	int32_t m_int;
	int8_t m_bytes[4];
};

class bqSoundLab
{
public:
	bqSoundLab() {}
	~bqSoundLab() {}

	static bqSoundSample_32bitFloat SampleTo32bitFloat(bqSoundSample_8bit);
	static bqSoundSample_32bitFloat SampleTo32bitFloat(bqSoundSample_16bit);
	static bqSoundSample_32bitFloat SampleTo32bitFloat(bqSoundSample_24bit);
	static bqSoundSample_32bitFloat SampleTo32bitFloat(bqSoundSample_32bit);
	static bqSoundSample_32bitFloat SampleTo32bitFloat(bqSoundSample_64bitFloat);

	static bqSoundSample_8bit SampleTo8bit(bqSoundSample_16bit);
	static bqSoundSample_8bit SampleTo8bit(bqSoundSample_24bit);
	static bqSoundSample_8bit SampleTo8bit(bqSoundSample_32bit);
	static bqSoundSample_8bit SampleTo8bit(bqSoundSample_32bitFloat);
	static bqSoundSample_8bit SampleTo8bit(bqSoundSample_64bitFloat);

	static bqSoundSample_16bit SampleTo16bit(bqSoundSample_8bit);
	static bqSoundSample_16bit SampleTo16bit(bqSoundSample_24bit);
	static bqSoundSample_16bit SampleTo16bit(bqSoundSample_32bit);
	static bqSoundSample_16bit SampleTo16bit(bqSoundSample_32bitFloat);
	static bqSoundSample_16bit SampleTo16bit(bqSoundSample_64bitFloat);

	/*static bqSoundSample_24bit SampleTo24bit(bqSoundSample_8bit);
	static bqSoundSample_24bit SampleTo24bit(bqSoundSample_16bit);
	static bqSoundSample_24bit SampleTo24bit(bqSoundSample_32bit);
	static bqSoundSample_24bit SampleTo24bit(bqSoundSample_32bitFloat);
	static bqSoundSample_24bit SampleTo24bit(bqSoundSample_64bitFloat);*/

	static bqSoundSample_32bit SampleTo32bit(bqSoundSample_8bit);
	static bqSoundSample_32bit SampleTo32bit(bqSoundSample_16bit);
	static bqSoundSample_32bit SampleTo32bit(bqSoundSample_24bit);
	static bqSoundSample_32bit SampleTo32bit(bqSoundSample_32bitFloat);
	static bqSoundSample_32bit SampleTo32bit(bqSoundSample_64bitFloat);

	/*static bqSoundSample_64bitFloat SampleTo64bitFloat(bqSoundSample_8bit);
	static bqSoundSample_64bitFloat SampleTo64bitFloat(bqSoundSample_16bit);
	static bqSoundSample_64bitFloat SampleTo64bitFloat(bqSoundSample_24bit);
	static bqSoundSample_64bitFloat SampleTo64bitFloat(bqSoundSample_32bit);
	static bqSoundSample_64bitFloat SampleTo64bitFloat(bqSoundSample_32bitFloat);*/

	static uint8_t _32_to_8(float v);
	static int16_t _32_to_16(float v);
	static float _8_to_32(uint8_t v);
	static float _16_to_32(int16_t v);
	static int32_t _24_to_32i(bqSoundSample_24bit* in_sample);
};

bqSoundSample_32bitFloat bqSoundLab::SampleTo32bitFloat(bqSoundSample_8bit in_sample)
{
	bqSoundSample_32bitFloat out_sample;
	out_sample.m_data = 0.f;

	const float m = 0.00787401574;// 1:127
	out_sample.m_data = (in_sample.m_data * m) - 1.f;
	if (out_sample.m_data < -1.f)
		out_sample.m_data = -1.f;
	if (out_sample.m_data > 1.f)
		out_sample.m_data = 1.f;

	return out_sample;
}

bqSoundSample_32bitFloat bqSoundLab::SampleTo32bitFloat(bqSoundSample_16bit in_sample)
{
	bqSoundSample_32bitFloat out_sample;
	out_sample.m_data = 0.f;

	const double m = 0.0000305185094759971922971282;// 1:32767
	out_sample.m_data = ((double)in_sample.m_data * m);
	if (out_sample.m_data < -1.f)
		out_sample.m_data = -1.f;
	if (out_sample.m_data > 1.f)
		out_sample.m_data = 1.f;

	return out_sample;
}

bqSoundSample_32bitFloat bqSoundLab::SampleTo32bitFloat(bqSoundSample_24bit in_sample)
{
	bqSoundSample_32bitFloat out_sample;
	out_sample.m_data = 0;

	int32_t sample = _24_to_32i(&in_sample);

	// 3 байта signed int = максимум 8388607
	//1 / 8388607 = 1,192093037616377e-7
	const double m = 1.192093037616377e-7;
	
	// ХЗ правильно ли

	out_sample.m_data = (bqSoundSample_32bitFloat::sample_type)(floor((double)sample * m));
	return out_sample;
}

bqSoundSample_32bitFloat bqSoundLab::SampleTo32bitFloat(bqSoundSample_32bit in_sample)
{
	bqSoundSample_32bitFloat out_sample;
	out_sample.m_data = 0.f;

	// 4 байта signed int = максимум 2147483647 (хз я просто поделил 4294967295 на 2)
	//1 / 2147483647 = 4,656612875245797e-10
	const double m = 4.656612875245797e-10;
	out_sample.m_data = (bqSoundSample_32bitFloat::sample_type)(floor((double)in_sample.m_data * m));
	return out_sample;
}

bqSoundSample_32bitFloat bqSoundLab::SampleTo32bitFloat(bqSoundSample_64bitFloat in_sample)
{
	bqSoundSample_32bitFloat out_sample;
	out_sample.m_data = (bqSoundSample_32bitFloat::sample_type)in_sample.m_data;
	return out_sample;
}


bqSoundSample_8bit bqSoundLab::SampleTo8bit(bqSoundSample_32bitFloat in_sample)
{
	bqSoundSample_8bit out_sample;
	out_sample.m_data = 0;

	if (in_sample.m_data < -1.f)
		in_sample.m_data = -1.f;
	in_sample.m_data += 1.f;
	in_sample.m_data *= 127.f;
	in_sample.m_data = floorf(in_sample.m_data);
	if (in_sample.m_data > 255.f)
		in_sample.m_data = 255.f;
	out_sample.m_data = (bqSoundSample_8bit::sample_type)in_sample.m_data;
	return out_sample;
}

bqSoundSample_8bit bqSoundLab::SampleTo8bit(bqSoundSample_64bitFloat in_sample)
{
	bqSoundSample_8bit out_sample;
	out_sample.m_data = 0;

	if (in_sample.m_data < -1.0)
		in_sample.m_data = -1.0;
	in_sample.m_data += 1.0;
	in_sample.m_data *= 127.0;
	in_sample.m_data = floor(in_sample.m_data);
	if (in_sample.m_data > 255.0)
		in_sample.m_data = 255.0;
	out_sample.m_data = (bqSoundSample_8bit::sample_type)in_sample.m_data;
	return out_sample;
}


bqSoundSample_8bit bqSoundLab::SampleTo8bit(bqSoundSample_16bit in_sample)
{
	bqSoundSample_8bit out_sample;
	out_sample.m_data = 0;
	out_sample.m_data = (bqSoundSample_8bit::sample_type)((in_sample.m_data + 32767) >> 8);
	return out_sample;
}

bqSoundSample_8bit bqSoundLab::SampleTo8bit(bqSoundSample_24bit in_sample)
{
	bqSoundSample_8bit out_sample;
	out_sample.m_data = 0;

	int32_t sample = _24_to_32i(&in_sample);

	//1 / 4194304
	//const double m = 2.384185791015625e-7;

	//127 / 8388607 = 1,513958157772798e-5
	const double m = 1.513958157772798e-5;

	out_sample.m_data = (bqSoundSample_8bit::sample_type)(floor((double)sample * m)) + 127;
	return out_sample;
}

bqSoundSample_8bit bqSoundLab::SampleTo8bit(bqSoundSample_32bit in_sample)
{
	bqSoundSample_8bit out_sample;
	out_sample.m_data = 0;

	//127 / 2147483647 = 5,913898351562162e-8
	const double m = 5.913898351562162e-8;
	out_sample.m_data = (bqSoundSample_8bit::sample_type)(floor((double)in_sample.m_data * m)) + 127;

	return out_sample;
}

bqSoundSample_16bit bqSoundLab::SampleTo16bit(bqSoundSample_8bit in_sample)
{
	bqSoundSample_16bit out_sample;
	out_sample.m_data = 0;
	// должно быть
	// 0 ->   -32767
	// 127 ->  0
	// 255 ->  +32767
	// можно перевести в диапазон от 0 до 0xffff
	// потом вычесть 32767

	// 65534 / 255 = 256,9960784313725
	const double m = 256.9960784313725;

	if (in_sample.m_data == 127)
		out_sample.m_data = 0;
	else
		out_sample.m_data = (bqSoundSample_16bit::sample_type)(floor((double)in_sample.m_data * m)) - 32767;

	return out_sample;
}

bqSoundSample_16bit bqSoundLab::SampleTo16bit(bqSoundSample_24bit in_sample)
{
	bqSoundSample_16bit out_sample;
	out_sample.m_data = 0;

	int32_t sample = _24_to_32i(&in_sample);

	// должно быть
	// -4194304 ->  -32767
	// 0 ->  0
	// +4194304 ->  +32767
	
	// 65534 / 8388608 = 0,0078122615814209
	const double m = 0.0078122615814209;
	
	out_sample.m_data = (bqSoundSample_16bit::sample_type)(floor((double)sample * m));

	return out_sample;
}

bqSoundSample_16bit bqSoundLab::SampleTo16bit(bqSoundSample_32bit in_sample)
{
	bqSoundSample_16bit out_sample;
	out_sample.m_data = 0;

	// 2147483647
	// половина от 4294967295

	// должно быть
	// -2147483647 ->  -32767
	// 0 ->  0
	// +2147483647 ->  +32767

	// 65534 / 4294967295 = 1,52583234047653e-5
	const double m = 1.52583234047653e-5;

	out_sample.m_data = (bqSoundSample_16bit::sample_type)(floor((double)in_sample.m_data * m));

	return out_sample;
}

bqSoundSample_16bit bqSoundLab::SampleTo16bit(bqSoundSample_32bitFloat in_sample)
{
	bqSoundSample_16bit out_sample;
	out_sample.m_data = _32_to_16(in_sample.m_data);
	return out_sample;
}

bqSoundSample_16bit bqSoundLab::SampleTo16bit(bqSoundSample_64bitFloat in_sample)
{
	bqSoundSample_16bit out_sample;
	out_sample.m_data = _32_to_16((float)in_sample.m_data);
	return out_sample;
}

bqSoundSample_32bit bqSoundLab::SampleTo32bit(bqSoundSample_8bit in_sample)
{
	bqSoundSample_32bit out_sample;
	out_sample.m_data = 0;

	// 2147483647
	// половина от 4294967295

	// должно быть
	// 0   -> -2147483647
	// 127 ->  0
	// 255 -> +2147483647

	// 4294967295 / 255 = 16 843 009
	const double m = 16843009.0;

	// надо будет вычитать 2 139 062 143
	// так как 127 * 16843009.0 = 2139062143
	// а в итоге должен быть 0

	out_sample.m_data = (bqSoundSample_32bit::sample_type)(floor((double)in_sample.m_data * m)) - 2139062143;
	return out_sample;
}

bqSoundSample_32bit bqSoundLab::SampleTo32bit(bqSoundSample_16bit in_sample)
{
	bqSoundSample_32bit out_sample;
	out_sample.m_data = 0;
	// 2147483647
	// половина от 4294967295

	// должно быть
	// -32767   -> -2147483647
	// 0        ->  0
	// +32767   -> +2147483647

	// 4294967295 / 65534 = 65 538,00004577776
	const double m = 65538.00004577776;

	out_sample.m_data = (bqSoundSample_32bit::sample_type)(floor((double)in_sample.m_data * m));
	return out_sample;
}

bqSoundSample_32bit bqSoundLab::SampleTo32bit(bqSoundSample_24bit in_sample)
{
	bqSoundSample_32bit out_sample;
	out_sample.m_data = 0;

	int32_t sample = _24_to_32i(&in_sample);

	// 2147483647
	// половина от 4294967295

	// должно быть
	// -4194304 ->  -2147483647
	// 0 ->  0
	// +4194304 ->  +2147483647

	// 4294967295 / 8388608 = 511,9999998807907
	const double m = 511.9999998807907;

	out_sample.m_data = (bqSoundSample_32bit::sample_type)(floor((double)sample * m));

	return out_sample;
}

bqSoundSample_32bit bqSoundLab::SampleTo32bit(bqSoundSample_32bitFloat in_sample)
{
	bqSoundSample_32bit out_sample;
	out_sample.m_data = 0;

	// 2147483647
	// половина от 4294967295

	// должно быть
	// -1 ->  -2147483647
	// 0  ->  0
	// +1 ->  +2147483647

	// 1 / 2147483647 = 4,656612875245797e-10
	const double d = 4.656612875245797e-10;

	if (in_sample.m_data != 0.f)
	{
		out_sample.m_data = (bqSoundSample_32bit::sample_type)(floor((double)in_sample.m_data / d));
	}


	return out_sample;
}

bqSoundSample_32bit bqSoundLab::SampleTo32bit(bqSoundSample_64bitFloat in_sample)
{
	bqSoundSample_32bit out_sample;
	out_sample.m_data = 0;

	// 2147483647
	// половина от 4294967295

	// должно быть
	// -1 ->  -2147483647
	// 0  ->  0
	// +1 ->  +2147483647

	// 1 / 2147483647 = 4,656612875245797e-10
	const double d = 4.656612875245797e-10;

	if (in_sample.m_data != 0.f)
	{
		out_sample.m_data = (bqSoundSample_32bit::sample_type)(floor((double)in_sample.m_data / d));
	}

	return out_sample;
}


uint8_t bqSoundLab::_32_to_8(float v)
{
	if (v < -1.f)
		v = -1.f;
	v += 1.f;
	v *= 127.f;
	v = floorf(v);
	if (v > 255.f)
		v = 255.f;
	return (uint8_t)v;
}

int16_t bqSoundLab::_32_to_16(float v)
{
	if (v < -1.f)
		v = -1.f;
	v *= 32767.f;
	v = floorf(v);
	if (v > 0xffff)
		v = 0xffff;
	return (int16_t)v;
}

// 0   0
// 127 1
// 255 2
float bqSoundLab::_8_to_32(uint8_t v)
{
	const float m = 0.00787401574;// 1:127
	float r = (v * m) - 1.f;
	if (r < -1.f)
		r = -1.f;
	if (r > 1.f)
		r = 1.f;
	return r;
}


// -32767   0
// 0        1
// 32767    2
float bqSoundLab::_16_to_32(int16_t v)
{
	const double m = 0.0000305185094759971922971282;// 1:32767
	float r = ((double)v * m);
	if (r < -1.f)
		r = -1.f;
	if (r > 1.f)
		r = 1.f;
	return r;
}

// 24bit max = 8388607
int32_t bqSoundLab::_24_to_32i(bqSoundSample_24bit* in_sample)
{
	int32_t ret = in_sample->m_data.m_byte[0];
	ret += in_sample->m_data.m_byte[1] << 8;
	ret += in_sample->m_data.m_byte[2] << 16;
	return ret;
}

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

//	newSound->m_format = bqSoundFormatFindFormat(newSound->m_bufferInfo);

	m_soundBuffer = newSound;
}

void bqSoundBuffer::MakeMono(uint32_t how)
{
	if (m_bufferInfo.m_channels > 1)
	{
		uint32_t new_channels = 1;
		uint32_t new_blockSize = m_bufferInfo.m_bytesPerSample * new_channels;
		uint32_t new_dataSize = m_bufferInfo.m_numOfSamples * new_blockSize;
		uint8_t* new_data = (uint8_t*)bqMemory::calloc(new_dataSize);

		// Блок содержит по одному сэмплу с каждого канала.
		// Если 16бит моно то блок равен 2 байта
		// Если 32бит стерео то равен 4байта*2канала=8байт

		uint32_t old_blockSize = m_bufferInfo.m_blockSize;
		// numBlocks должен быть у обоих буферов одинаковым
		uint32_t numBlocks = new_dataSize / new_blockSize;
		
		uint8_t* dst_block = new_data;
		uint8_t* src_block = m_bufferData.m_data;
		
		// должен быть такого же размера как src block
	//	uint8_t* tmp_block = (uint8_t*)bqMemory::calloc(old_blockSize);

		for (uint32_t i = 0; i < numBlocks; ++i)
		{			
			// Данный способ будет брать только первый канал.
			// Нужно сделать так чтобы можно было выбрать какой канал
			// копировать. Для этого используется парамет `how`
			// Значение 0 у how должен означать использовать все каналы.
			/*for (uint32_t o = 0; o < new_blockSize; ++o)
			{
				dst_block[o] = src_block[o];
			}
			dst_block += new_blockSize;
			src_block += old_blockSize;*/

			// Пусть пока будет реализован how==0
			
			// Надо получить сумму со всех каналов и поделить на количество каналов
			int64_t summInt = 0;
			bqFloat64 summReal = 0.0;
			for (uint32_t o = 0; o < m_bufferInfo.m_channels; ++o)
			{
				switch (m_bufferInfo.m_format)
				{
				case bqSoundFormat::uint8:
				{
					uint8_t* int_data = (uint8_t*)src_block;
					summInt += int_data[o];
				}break;
				case bqSoundFormat::int16:
				{
					int16_t* int_data = (int16_t*)src_block;
					summInt += int_data[o];
				}break;
				case bqSoundFormat::int24:
				{
					bqSoundSample_24bit* int_data = (bqSoundSample_24bit*)src_block;
					int64_t sample = bqSoundLab::_24_to_32i(int_data);

					summInt += sample;
				}break;
				case bqSoundFormat::int32:
				{
					int32_t* int_data = (int32_t*)src_block;
					summInt += int_data[o];
				}break;

				case bqSoundFormat::float32:
				{
					bqFloat32* float_data = (bqFloat32*)src_block;
					summReal += float_data[o];
				}break;

				case bqSoundFormat::float64:
				{
					bqFloat64* float_data = (bqFloat64*)src_block;
					summReal += float_data[o];
				}break;
				default:
					bqLog::PrintError("Need implementation! %s %i\n", BQ_FUNCTION, BQ_LINE);
					break;
				}
			}

			int64_t avgInt = 0;
			bqFloat64 avgReal = 0.0;

			if (summReal != 0.0)
				avgReal = summReal / m_bufferInfo.m_channels;
			if (summInt != 0)
				avgInt = summInt / m_bufferInfo.m_channels;

			// так как пока реализую how==0
			// нужно использовать avgReal или avgInt
			// ниже
			for (uint32_t o = 0; o < new_blockSize; ++o)
			{
	//			dst_block[o] = tmp_block[o];
				switch (m_bufferInfo.m_format)
				{
				case bqSoundFormat::uint8:
				{
					uint8_t* int_data = (uint8_t*)dst_block;
					*int_data = (uint8_t)avgInt;
				}break;
				case bqSoundFormat::int16:
				{
					int16_t* int_data = (int16_t*)dst_block;
					*int_data = (int16_t)avgInt;
				}break;
				case bqSoundFormat::int24:
				{
					bqSoundSample_24bit* int_data = (bqSoundSample_24bit*)dst_block;
					bqInteger4 bint;
					bint.m_int = avgInt;

					if (bint.m_int > 4194304)
						bint.m_int = 4194304;
					else if (bint.m_int < -4194304)
						bint.m_int = -4194304;

					int_data[0].m_data.m_byte[0] = bint.m_bytes[0];
					int_data[0].m_data.m_byte[1] = bint.m_bytes[1];
					int_data[0].m_data.m_byte[2] = bint.m_bytes[2];
				}break;

				case bqSoundFormat::int32:
				{
					int32_t* int_data = (int32_t*)dst_block;
					*int_data = (int32_t)avgInt;
				}break;

				case bqSoundFormat::float32:
				{
					bqFloat32* float_data = (bqFloat32*)dst_block;
					*float_data = (bqFloat32)avgReal;
				}break;

				case bqSoundFormat::float64:
				{
					bqFloat64* float_data = (bqFloat64*)dst_block;
					*float_data = (bqFloat64)avgReal;
				}break;

				default:
					bqLog::PrintError("Need implementation! %s %i\n", BQ_FUNCTION, BQ_LINE);
					break;
				}
			}
			dst_block += new_blockSize;
			src_block += old_blockSize;
		}

		if (m_bufferData.m_data)
			bqMemory::free(m_bufferData.m_data);

		m_bufferInfo.m_channels = new_channels;
		m_bufferInfo.m_blockSize = new_blockSize;
		m_bufferData.m_data = new_data;
		m_bufferData.m_dataSize = new_dataSize;
	}
}

void bqSoundBuffer::Make8bits()
{
	if (m_bufferInfo.m_format != bqSoundFormat::uint8)
	{
		uint32_t newBytesPerSample = 1;
		uint32_t newBitsPerSample = 8;
		uint32_t newBlockSize = newBytesPerSample * m_bufferInfo.m_channels;
		uint32_t newDataSize = m_bufferInfo.m_numOfSamples * newBytesPerSample * m_bufferInfo.m_channels;
		uint8_t* newData = (uint8_t*)bqMemory::calloc(newDataSize);

		uint32_t numBlocks = newDataSize / newBlockSize;
		uint32_t oldBlockSize = m_bufferInfo.m_blockSize;

		uint8_t* dst_block = newData;
		uint8_t* src_block = m_bufferData.m_data;
		for (uint32_t i = 0; i < numBlocks; ++i)
		{
			bqSoundSample_8bit* dst_8bit = (bqSoundSample_8bit*)dst_block;
			bqSoundSample_8bit* _8bit = (bqSoundSample_8bit*)src_block;
			bqSoundSample_16bit* _16bit = (bqSoundSample_16bit*)src_block;
			bqSoundSample_24bit* _24bit = (bqSoundSample_24bit*)src_block;
			bqSoundSample_32bit* _32bit = (bqSoundSample_32bit*)src_block;
			bqSoundSample_32bitFloat* _32bitFloat = (bqSoundSample_32bitFloat*)src_block;
			bqSoundSample_64bitFloat* _64bitFloat = (bqSoundSample_64bitFloat*)src_block;

			for (uint32_t o = 0; o < m_bufferInfo.m_channels; ++o)
			{
				switch (m_bufferInfo.m_format)
				{
				case bqSoundFormat::uint8:
					dst_8bit[o] = _8bit[o];
					break;
				
				case bqSoundFormat::int16:
					dst_8bit[o] = bqSoundLab::SampleTo8bit(_16bit[o]);
				break;

				case bqSoundFormat::int24:
					dst_8bit[o] = bqSoundLab::SampleTo8bit(_24bit[o]);
				break;

				case bqSoundFormat::int32:
					dst_8bit[o] = bqSoundLab::SampleTo8bit(_32bit[o]);
					break;

				case bqSoundFormat::float32:
					dst_8bit[o] = bqSoundLab::SampleTo8bit(_32bitFloat[o]);
					break;

				case bqSoundFormat::float64:
					dst_8bit[o] = bqSoundLab::SampleTo8bit(_64bitFloat[o]);
					break;

				default:
					bqLog::PrintError("Need implementation! %s %i\n", BQ_FUNCTION, BQ_LINE);
					break;
				}
			}

			dst_block += newBlockSize;
			src_block += oldBlockSize;
		}

		if (m_bufferData.m_data)
			bqMemory::free(m_bufferData.m_data);

		m_bufferInfo.m_blockSize = newBlockSize;
		m_bufferInfo.m_bytesPerSample = newBytesPerSample;
		m_bufferInfo.m_bitsPerSample = newBitsPerSample;
		m_bufferData.m_data = newData;
		m_bufferData.m_dataSize = newDataSize;

		m_bufferInfo.m_format = bqSoundFormat::uint8;
	}
}

void bqSoundBuffer::Make16bits()
{
	if (m_bufferInfo.m_format != bqSoundFormat::int16)
	{
		uint32_t newBytesPerSample = 2;
		uint32_t newBitsPerSample = 16;
		uint32_t newBlockSize = newBytesPerSample * m_bufferInfo.m_channels;
		uint32_t newDataSize = m_bufferInfo.m_numOfSamples * newBytesPerSample * m_bufferInfo.m_channels;
		uint8_t* newData = (uint8_t*)bqMemory::calloc(newDataSize);

		uint32_t numBlocks = newDataSize / newBlockSize;
		uint32_t oldBlockSize = m_bufferInfo.m_blockSize;

		uint8_t* dst_block = newData;
		uint8_t* src_block = m_bufferData.m_data;
		for (uint32_t i = 0; i < numBlocks; ++i)
		{
			bqSoundSample_16bit* dst_16bit = (bqSoundSample_16bit*)dst_block;

			bqSoundSample_8bit* _8bit = (bqSoundSample_8bit*)src_block;
			bqSoundSample_16bit* _16bit = (bqSoundSample_16bit*)src_block;
			bqSoundSample_24bit* _24bit = (bqSoundSample_24bit*)src_block;
			bqSoundSample_32bit* _32bit = (bqSoundSample_32bit*)src_block;
			bqSoundSample_32bitFloat* _32bitFloat = (bqSoundSample_32bitFloat*)src_block;
			bqSoundSample_64bitFloat* _64bitFloat = (bqSoundSample_64bitFloat*)src_block;

			for (uint32_t o = 0; o < m_bufferInfo.m_channels; ++o)
			{
				switch (m_bufferInfo.m_format)
				{
				case bqSoundFormat::uint8:
					dst_16bit[o] = bqSoundLab::SampleTo16bit(_8bit[o]);
					break;

				case bqSoundFormat::int16:
					dst_16bit[o] = _16bit[o];
					break;

				case bqSoundFormat::int24:
					dst_16bit[o] = bqSoundLab::SampleTo16bit(_24bit[o]);
					break;

				case bqSoundFormat::int32:
					dst_16bit[o] = bqSoundLab::SampleTo16bit(_32bit[o]);
					break;

				case bqSoundFormat::float32:
					dst_16bit[o] = bqSoundLab::SampleTo16bit(_32bitFloat[o]);
					break;

				case bqSoundFormat::float64:
					dst_16bit[o] = bqSoundLab::SampleTo16bit(_64bitFloat[o]);
					break;

				default:
					bqLog::PrintError("Need implementation! %s %i\n", BQ_FUNCTION, BQ_LINE);
					break;
				}
			}

			dst_block += newBlockSize;
			src_block += oldBlockSize;
		}

		if (m_bufferData.m_data)
			bqMemory::free(m_bufferData.m_data);

		m_bufferInfo.m_blockSize = newBlockSize;
		m_bufferInfo.m_bytesPerSample = newBytesPerSample;
		m_bufferInfo.m_bitsPerSample = newBitsPerSample;
		m_bufferData.m_data = newData;
		m_bufferData.m_dataSize = newDataSize;

		m_bufferInfo.m_format = bqSoundFormat::int16;
	}
}

void bqSoundBuffer::Make32bitsFloat()
{
	if (m_bufferInfo.m_format != bqSoundFormat::float32)
	{
		uint32_t newBytesPerSample = 4;
		uint32_t newBitsPerSample = 32;
		uint32_t newBlockSize = newBytesPerSample * m_bufferInfo.m_channels;
		uint32_t newDataSize = m_bufferInfo.m_numOfSamples * newBytesPerSample * m_bufferInfo.m_channels;
		uint8_t* newData = (uint8_t*)bqMemory::calloc(newDataSize);

		uint32_t numBlocks = newDataSize / newBlockSize;
		uint32_t oldBlockSize = m_bufferInfo.m_blockSize;

		uint8_t* dst_block = newData;
		uint8_t* src_block = m_bufferData.m_data;
		for (uint32_t i = 0; i < numBlocks; ++i)
		{
			bqSoundSample_32bitFloat* dst_32bit = (bqSoundSample_32bitFloat*)dst_block;

			bqSoundSample_8bit* _8bit = (bqSoundSample_8bit*)src_block;
			bqSoundSample_16bit* _16bit = (bqSoundSample_16bit*)src_block;
			bqSoundSample_24bit* _24bit = (bqSoundSample_24bit*)src_block;
			bqSoundSample_32bit* _32bit = (bqSoundSample_32bit*)src_block;
			bqSoundSample_32bitFloat* _32bitFloat = (bqSoundSample_32bitFloat*)src_block;
			bqSoundSample_64bitFloat* _64bitFloat = (bqSoundSample_64bitFloat*)src_block;

			for (uint32_t o = 0; o < m_bufferInfo.m_channels; ++o)
			{
				switch (m_bufferInfo.m_format)
				{
				case bqSoundFormat::uint8:
					dst_32bit[o] = bqSoundLab::SampleTo32bitFloat(_8bit[o]);
					break;

				case bqSoundFormat::int16:
					dst_32bit[o] = bqSoundLab::SampleTo32bitFloat(_16bit[o]);
					break;

				case bqSoundFormat::int24:
					dst_32bit[o] = bqSoundLab::SampleTo32bitFloat(_24bit[o]);
					break;

				case bqSoundFormat::int32:
					dst_32bit[o] = bqSoundLab::SampleTo32bitFloat(_32bit[o]);
					break;

				case bqSoundFormat::float32:
					dst_32bit[o] = _32bitFloat[o];
					break;

				case bqSoundFormat::float64:
					dst_32bit[o] = bqSoundLab::SampleTo32bitFloat(_64bitFloat[o]);
					break;

				default:
					bqLog::PrintError("Need implementation! %s %i\n", BQ_FUNCTION, BQ_LINE);
					break;
				}
			}

			dst_block += newBlockSize;
			src_block += oldBlockSize;
		}

		if (m_bufferData.m_data)
			bqMemory::free(m_bufferData.m_data);

		m_bufferInfo.m_blockSize = newBlockSize;
		m_bufferInfo.m_bytesPerSample = newBytesPerSample;
		m_bufferInfo.m_bitsPerSample = newBitsPerSample;
		m_bufferData.m_data = newData;
		m_bufferData.m_dataSize = newDataSize;

		m_bufferInfo.m_format = bqSoundFormat::float32;
	}
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

		if (m_soundBuffer->m_bufferInfo.m_format == bqSoundFormat::float32)
			chunkSz += 4 + 4 + 4;

		fwrite(&chunkSz, 4, 1, f);
			
		fwrite("WAVE", 4, 1, f);
		fwrite("fmt ", 4, 1, f);
		
		// 16 - минимальный размер
		// 18 - extraFormatInfoSz
		// если 18 и extraFormatInfoSz == 22 то
		//     subchnkSz = 40
		int32_t subchnkSz = 16;
		fwrite(&subchnkSz, 4, 1, f);
			
		// pcm = 1
		// IEEE float = 3
		int16_t TYPE = 1;

		if (m_soundBuffer->m_bufferInfo.m_format == bqSoundFormat::float32)
				TYPE = 3;

		fwrite(&TYPE, 2, 1, f);
			
		fwrite(&m_soundBuffer->m_bufferInfo.m_channels, 2, 1, f);
		fwrite(&m_soundBuffer->m_bufferInfo.m_sampleRate, 4, 1, f);
			
		int nAvgBytesPerSec = (m_soundBuffer->m_bufferInfo.m_sampleRate *
			m_soundBuffer->m_bufferInfo.m_bitsPerSample * m_soundBuffer->m_bufferInfo.m_channels)/8;
		// byterate
		fwrite(&nAvgBytesPerSec, 4, 1, f);
			
		fwrite(&m_soundBuffer->m_bufferInfo.m_blockSize, 2, 1, f);
		fwrite(&m_soundBuffer->m_bufferInfo.m_bitsPerSample, 2, 1, f);
		
		// Если 
		//uint16_t extraFormatInfoSz = 0;
		//fwrite(&extraFormatInfoSz, 2, 1, f);

		if (m_soundBuffer->m_bufferInfo.m_format == bqSoundFormat::float32)
		{
			fwrite("fact", 4, 1, f);
			int32_t factchunkSz = 4;
			fwrite(&factchunkSz, 4, 1, f);
			fwrite(&m_soundBuffer->m_bufferInfo.m_numOfSamples, 4, 1, f);

		}

		fwrite("data", 4, 1, f);
		fwrite(&m_soundBuffer->m_bufferData.m_dataSize, 4, 1, f);
		fwrite(m_soundBuffer->m_bufferData.m_data, m_soundBuffer->m_bufferData.m_dataSize, 1, f);
		fclose(f);
		return true;
	}

	return false;
}

bool bqSound::LoadFromFile(const bqStringA& fn)
{
	return LoadFromFile(fn.c_str());
}

bool bqSound::LoadFromFile(const char* fn)
{
	Clear();

	bqString path(fn);
	bool r = false;
	if (path.extension(".wav"))
	{
		r = _loadWav(fn);
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
		switch (type)
		{
		case bqSoundFormat::uint8:
			m_soundBuffer->Make8bits();
			break;
		case bqSoundFormat::int16:
			m_soundBuffer->Make16bits();
			break;
		/*case bqSoundFormat::int24:
			m_soundBuffer->Make24bits();
			break;
		case bqSoundFormat::int32:
			m_soundBuffer->Make32bits();
			break;*/
		case bqSoundFormat::float32:
			m_soundBuffer->Make32bitsFloat();
			break;
		/*case bqSoundFormat::float64:
			m_soundBuffer->Make64bitsFloat();
			break;*/
		case bqSoundFormat::unsupported:
			//break;
		default:
			bqLog::PrintError("Need implementation! %s %i\n", BQ_FUNCTION, BQ_LINE);
			break;
		}
	}
}

// Так-же надо обновлять этот метод -> bool bqSoundFile::_openWav(const char* fn)
bool bqSound::_loadWav(uint8_t* buffer, uint32_t bufferSz)
{
	bqFileBuffer file(buffer, bufferSz);

	bqSoundFormat format = bqSoundFormat::unsupported;

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

				uint16_t TYPE = 0;
				file.Read(&TYPE, 2);

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
				file.Read(&channels, 2);

				uint32_t sampleRate = 0;
				file.Read(&sampleRate, 4);

				uint32_t nAvgBytesPerSec = 0;
				file.Read(&nAvgBytesPerSec, 4);

				uint16_t blockSize = 0;
				file.Read(&blockSize, 2);

				uint16_t bitsPerSample = 0;
				file.Read(&bitsPerSample, 2);

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
					if (bitsPerSample == 64)
						format = bqSoundFormat::float64;
				}

				if (FMTChunkSize > 16)
				{
					uint16_t extraFormatInfoSz = 0;
					file.Read(&extraFormatInfoSz, 2);

					if (extraFormatInfoSz == 22)
					{
						uint16_t wValidBitsPerSample = 0;
						uint32_t dwChannelMask = 0;
						uint8_t SubFormat[16];
						file.Read(&wValidBitsPerSample, 2);
						file.Read(&dwChannelMask, 4);
						file.Read(SubFormat, 16);
					}
				}

				if (TYPE != 1)
				{
					char fact[5] = { 0,0,0,0,0 };
					file.Read(fact, 4);
					if (strcmp(fact, "fact") == 0)
					{
						uint32_t factcksize = 0;
						file.Read(&factcksize, 4);
						if(factcksize < 4)
							return false;

						// на каждый канал.
						// наверное Chunk size и содержит размер с учётом количества каналов
						// поэтому получаю это количество через factcksize / 4
						for (uint32_t i = 0, sz = factcksize / 4; i < sz; ++i)
						{
							uint32_t factdwSampleLength = 0;
							file.Read(&factdwSampleLength, 4);
						}

					}
					else
					{
						return false;
					}
				}

				char data[5] = { 0,0,0,0,0 };
				file.Read(data, 4);

				// возможно есть PEAK
				if (strcmp(data, "PEAK") == 0)
				{
					uint32_t peakchunkDataSize = 0;
					uint32_t peakversion = 0;
					uint32_t peaktimeStamp = 0;

					struct PositionPeak
					{
						float   value;    /* signed value of peak */
						uint32_t position; /* the sample frame for the peak */
					};

					file.Read(&peakchunkDataSize, 4);
					file.Read(&peakversion, 4);
					file.Read(&peaktimeStamp, 4);

					if (peakchunkDataSize > 8)// надо ли это делать?
					{
						PositionPeak peak;
						for (uint32_t i = 0; i < channels; ++i)
						{
							file.Read(&peak.value, 4);
							file.Read(&peak.position, 4);
						}
					}

					memset(data, 0, 5);
					file.Read(data, 4);
				}

				if (strcmp(data, "data") == 0)
				{
					uint32_t dataSize = 0;
					file.Read(&dataSize, 4);
					if (dataSize)
					{
						if (format != bqSoundFormat::unsupported)
						{


							Create(0.1f, channels, sampleRate, bitsPerSample);
							if (m_soundBuffer->m_bufferData.m_dataSize < dataSize)
								_reallocate(dataSize);

							m_soundBuffer->m_bufferInfo.m_format = format;

							file.Read(m_soundBuffer->m_bufferData.m_data, m_soundBuffer->m_bufferData.m_dataSize);

							//Convert();

							CalculateTime();
							return true;
						}
						else
						{
							bqLog::PrintWarning("Unsupported sound file format\n");
						}
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
			// скорее всего нет...
			
			// старое
			//m_soundBuffer->m_bufferInfo.m_time = 1.f / m_soundBuffer->m_bufferInfo.m_sampleRate;
			//m_soundBuffer->m_bufferInfo.m_time *= m_soundBuffer->m_bufferInfo.m_numOfSamples;
			
			// новое
			double dataSizePerChannel = m_soundBuffer->m_bufferData.m_dataSize;
			if(m_soundBuffer->m_bufferInfo.m_channels > 1)
			{
				dataSizePerChannel = ceil(dataSizePerChannel / m_soundBuffer->m_bufferInfo.m_channels);
			}

			m_soundBuffer->m_bufferInfo.m_time = dataSizePerChannel 
				/ (m_soundBuffer->m_bufferInfo.m_sampleRate * m_soundBuffer->m_bufferInfo.m_bytesPerSample);
			//!!!
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

void bqSoundBuffer::Resample(uint32_t newSampleRate)
{
	BQ_ASSERT_ST(m_bufferData.m_data);
	BQ_ASSERT_ST(m_bufferData.m_data);
	BQ_ASSERT_STC(newSampleRate != 0, "New sample rate must be between 11000 and 192000");

	// только для формата float32
	if(m_bufferInfo.m_format != bqSoundFormat::float32)
	{
		bqLog::PrintError("%s : sound format is not float32\n", BQ_FUNCTION);
		return;
	}

	if(newSampleRate && (newSampleRate != m_bufferInfo.m_sampleRate)
		&& m_bufferData.m_dataSize)
	{
		if((newSampleRate >= 1000) && (newSampleRate <= 192000))
		{
			// Надо создать новый массив где будет находится звук с новым sample rate

			// Надо вычислить размер нового массива
			uint32_t newDataSize = 0;

			// Вычисляем время в секундах.
			// Потом, используя время, вычисляем размер newDataSize
			// Например если 44100 сэмплрейт 1 канал, 16 бит
			// То на 1 секунду надо 44100*2=88200 байт
			// Например, звук на полторы секунды, 88200*1.5=132300 байт m_dataSize
			// Получим время имея только значение 132300
			// 132300 / байтВСекунду = ?
			// байтВСекунду = 44100*(16/8) = 88200
			// 132300 / 88200 = 1,5

			
			// Время это размер данных делёное на байтВСекунду
//			!!! не учитывается множество каналов.
//			Надо или учитывать каналы, или работать с моно.
//			Возможно надо просто получить размер данных на 1 канал
//			   и использовать это значение. Так-же надо сделать изменение
//			   в методе CalculateTime()
//готово 
			double dataSizePerChannel = m_bufferData.m_dataSize;
			if(m_bufferInfo.m_channels > 1)
			{
				dataSizePerChannel = ceil(dataSizePerChannel / m_bufferInfo.m_channels);
			}

			bqFloat64 time = dataSizePerChannel / (m_bufferInfo.m_sampleRate * m_bufferInfo.m_bytesPerSample);
			//printf("RESAMPLE TIME: %f\n", time);

			if (time > 0.0)
			{
				// теперь вычисляю размер для нового массива
				newDataSize = ceil((bqFloat64)(newSampleRate * m_bufferInfo.m_bytesPerSample * m_bufferInfo.m_channels) * time);
			}

			printf("RESAMPLE newDataSize: %u\n", newDataSize);

			if (newDataSize)
			{
				uint8_t* newData = (uint8_t*)bqMemory::malloc(newDataSize);

				if (m_bufferInfo.m_blockSize && m_bufferData.m_dataSize)
				{
					uint32_t numOfBlocksOld = m_bufferData.m_dataSize / m_bufferInfo.m_blockSize;
					uint32_t numOfBlocksNew = newDataSize / m_bufferInfo.m_blockSize;

					// прохожусь по блокам из source.
					// сую их в новый массив.
					// Надо обрабатывать частями по 1й секунде
					// потому что samplerate это есть секунда, будет проще без всяких вычислений.
					// есть 2 пути. уменьшение и увеличение 
					
					if (newSampleRate < m_bufferInfo.m_sampleRate) // уменьшение
					{
						// Надо проходиться по блокам из нового массива.
						// Блок = сэмпл * количество каналов.



						// для нахождения индекса в старом массиве
						// размер массива 1 делим на размер массива 2
						// (большого массива    на    меньший массив)
						double _m = (double)numOfBlocksOld / (double)numOfBlocksNew;

						// Надо сначала конвертировать один канал, потом другой и т.д.
						for (uint32_t ic = 0; ic < m_bufferInfo.m_channels; ++ic)
						{
							bqFloat32* ptrNew = (bqFloat32*)newData;
							bqFloat32* ptrOld = (bqFloat32*)m_bufferData.m_data;

							// проход по всем блокам - то есть по всем данным
							for (uint32_t ib = 0; ib < numOfBlocksNew; /*++ib*/)
							{
								
								// индекс должен начинаться с нуля так как
								// идея алгоритма такова что работаем с 
								// массивами от начала до конца. Потом
								// надо будет изменить указатели ptrNew и ptrOld
								uint32_t indexNewNoChannels = 0; // без учёта каналов, для 
								                                 //получения индекса старого массива
								uint32_t indexNew = 0; // для прохода по новому массиву

								uint32_t _1_second = newSampleRate;
								if ((numOfBlocksNew - ib) < _1_second)
									_1_second = numOfBlocksNew - ib;

								for (uint32_t i = 0; i < _1_second; ++i)
								{
									// получаю индекс в старом массиве
									double _v = (double)indexNewNoChannels * _m;

									// конвертирую в индекс с учётом каналов
									// индекс * количество канало + индекс текущего канала
									uint32_t indexOld = (uint32_t)floor(_v) * m_bufferInfo.m_channels;
									indexOld += ic;

									ptrNew[indexNew + ic] = ptrOld[indexOld];

									indexNew += m_bufferInfo.m_channels;

									++indexNewNoChannels;
								}

								ib += _1_second;
								ptrNew += newSampleRate * m_bufferInfo.m_channels;
								ptrOld += m_bufferInfo.m_sampleRate * m_bufferInfo.m_channels;
							}
						}
					}
					else // увеличение
					{
					}
				}


				m_bufferInfo.m_sampleRate = newSampleRate;

				bqMemory::free(m_bufferData.m_data);

				m_bufferData.m_data = newData;
				m_bufferData.m_dataSize = newDataSize;
			}
		}
	}
}
