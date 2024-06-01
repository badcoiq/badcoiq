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
#ifndef _BQ_SNDFXDELAYIMPL_H_
#define _BQ_SNDFXDELAYIMPL_H_

#include "badcoiq/sound/bqSoundSystem.h"
#include "badcoiq/containers/bqBlockQueue.h"

class bqSoundEffectDelayImpl : public bqSoundEffectDelay
{
	struct DelayStep
	{
		DelayStep(int o)
		{
			auto soundSystem = bqFramework::GetSoundSystem();
			auto soundDeviceInfo = soundSystem->GetDeviceInfo();

			auto blockSize = soundDeviceInfo.m_bufferSize
				/ (soundDeviceInfo.m_bitsPerSample / 8)
				/ soundDeviceInfo.m_channels;

			m_delayBuffers[0] = new bqBlockQueue<bqFloat32>(blockSize, o);
			m_delayBuffers[1] = new bqBlockQueue<bqFloat32>(blockSize, o);
		}

		~DelayStep()
		{
			if (m_delayBuffers[0]) delete m_delayBuffers[0];
			if (m_delayBuffers[1]) delete m_delayBuffers[1];
		}

		// Стерео
		bqBlockQueue<bqFloat32>* m_delayBuffers[2];

	};

	bqArray<DelayStep*> m_delaySteps;

	uint32_t m_delayStepsNum = 5;
	uint32_t m_delayTime = 20;

	bqFloat32* m_blockOut = 0;

	void _deleteSteps();
	void _createSteps();

public:
	bqSoundEffectDelayImpl(uint32_t, uint32_t);
	virtual ~bqSoundEffectDelayImpl();

	virtual void Process(bqSoundMixer*) override;
	virtual void SetDelay(uint32_t steps, uint32_t time) override;
};

#endif
