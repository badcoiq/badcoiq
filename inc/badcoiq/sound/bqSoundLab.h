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

#pragma once
#ifndef __BQ_SOUNDLAB_H__
#define __BQ_SOUNDLAB_H__

#ifdef BQ_WITH_SOUND

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

#endif
#endif
