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
	const float m = 0.0000305185094759971922971282;// 1:32767
	float r = ((float)v * m);
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


#endif
