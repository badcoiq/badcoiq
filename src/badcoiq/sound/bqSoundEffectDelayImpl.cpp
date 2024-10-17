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

#include "bqSoundEffectDelayImpl.h"

#include "../framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;

bqSoundEffectDelayImpl::bqSoundEffectDelayImpl(uint32_t steps, uint32_t time)
	:
	m_delayStepsNum(steps),
	m_delayTime(time)
{
	if (m_delayStepsNum < 1)
		m_delayStepsNum = 1;

	if (m_delayStepsNum > 10)
		m_delayStepsNum = 10;

	if (m_delayTime < 1)
		m_delayTime = 1;

	auto soundSystem = bqFramework::GetSoundSystem();
	auto soundDeviceInfo = soundSystem->GetDeviceInfo();
	// Размер в семплах на 1 канал
	auto blockSize = soundDeviceInfo.m_bufferSize
		/ (soundDeviceInfo.m_bitsPerSample / 8)
		/ soundDeviceInfo.m_channels;
	m_blockOut = new float32_t[blockSize];
	_createSteps();
}

bqSoundEffectDelayImpl::~bqSoundEffectDelayImpl()
{
	if (m_blockOut) delete m_blockOut;
	_deleteSteps();
}

void bqSoundEffectDelayImpl::_createSteps()
{
	auto soundSystem = bqFramework::GetSoundSystem();
	auto soundDeviceInfo = soundSystem->GetDeviceInfo();

	auto blockSize = soundDeviceInfo.m_bufferSize
		/ (soundDeviceInfo.m_bitsPerSample / 8)
		/ soundDeviceInfo.m_channels;

	for (uint32_t i = 0; i < m_delayStepsNum; ++i)
	{
		m_delaySteps.push_back(new DelayStep(m_delayTime));
	}
}

void bqSoundEffectDelayImpl::_deleteSteps()
{
	for (uint32_t i = 0; i < m_delayStepsNum; ++i)
	{
		delete m_delaySteps.m_data[i];
		m_delaySteps.m_data[i] = 0;
	}
	m_delaySteps.clear();
}

void bqSoundEffectDelayImpl::Process(bqSoundMixer* mixer)
{
	auto numOfChannels = mixer->GetNumOfChannels();
	for (uint32_t si = 0; si < m_delaySteps.m_size; ++si)
	{
		for (uint32_t i = 0; i < numOfChannels; ++i)
		{
			if (i == 2)
				break;
			auto channel = mixer->GetChannel(i);

			float32_t* src = (float32_t*)channel->m_data;
			float32_t* dst32 = (float32_t*)channel->m_data;

			if (si)
				src = m_delaySteps.m_data[si - 1]->m_delayBuffers[i]->GetCurrentData();

			m_delaySteps.m_data[si]->m_delayBuffers[i]->
				Put(src, m_blockOut);

			for (size_t o = 0; o < m_delaySteps.m_data[si]->m_delayBuffers[i]->BlockSize(); ++o)
			{
				dst32[o] += m_blockOut[o] * (1.f - (log10f(si + 1)));
				if (dst32[o] > 1.f) dst32[o] = 1.f;
				if (dst32[o] < -1.f) dst32[o] = -1.f;
			}
		}
	}
}

void bqSoundEffectDelayImpl::SetDelay(uint32_t steps, uint32_t time)
{
	_deleteSteps();

	m_delayStepsNum = steps;
	m_delayTime = time;

	if (m_delayStepsNum < 1)
		m_delayStepsNum = 1;

	if (m_delayStepsNum > 10)
		m_delayStepsNum = 10;

	if (m_delayTime < 1)
		m_delayTime = 1;

	_createSteps();
}

#endif
