﻿/*
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

#include "../../DemoApp.h"
#include "badcoiq/containers/bqBlockQueue.h"
#include "badcoiq/sound/bqSoundSystem.h"

//#include "../src/kissfft/kiss_fft.h"

#ifdef BQ_WITH_SOUND


class ExampleSoundEffectDelay : public bqSoundEffect
{
	bqBlockQueue<bqFloat32>* m_delayBuffers[2];
	bqFloat32* m_blockOut = 0;
public:
	ExampleSoundEffectDelay()
	{
		auto soundSystem = bqFramework::GetSoundSystem();
		auto soundDeviceInfo = soundSystem->GetDeviceInfo();

		// Размер в семплах на 1 канал
		auto blockSize = soundDeviceInfo.m_bufferSize 
			/ (soundDeviceInfo.m_bitsPerSample / 8)
			/ soundDeviceInfo.m_channels;
		m_delayBuffers[0] = new bqBlockQueue<bqFloat32>(blockSize, 20);
		m_delayBuffers[1] = new bqBlockQueue<bqFloat32>(blockSize, 20);

		m_blockOut = new bqFloat32[blockSize];
	}

	virtual ~ExampleSoundEffectDelay()
	{
		if (m_delayBuffers[0]) delete m_delayBuffers[0];
		if (m_delayBuffers[1]) delete m_delayBuffers[1];
		if (m_blockOut) delete m_blockOut;
	}

	virtual void Process(bqSoundMixer* mixer) override
	{
		auto numOfChannels = mixer->GetNumOfChannels();
		for (uint32_t i = 0; i < numOfChannels; ++i)
		{
			if (i == 2)
				break;

			auto channel = mixer->GetChannel(i);
			m_delayBuffers[i]->Put((bqFloat32*)channel->m_data, m_blockOut);

			bqFloat32* dst32 = (bqFloat32*)channel->m_data;
			for (size_t o = 0; o < m_delayBuffers[i]->BlockSize(); ++o)
			{
				dst32[o] += m_blockOut[o];
				if (dst32[o] > 1.f) dst32[o] = 1.f;
				if (dst32[o] < -1.f) dst32[o] = -1.f;
			}
		}
	}
};

ExampleSoundDelay::ExampleSoundDelay(DemoApp* app)
	:
	DemoExample(app)
{
}

ExampleSoundDelay::~ExampleSoundDelay()
{
}


bool ExampleSoundDelay::Init()
{
	auto soundSystem = bqFramework::GetSoundSystem();
	auto soundDeviceInfo = soundSystem->GetDeviceInfo();

	m_sound = new bqSound;

	// Загрузка звука должна происходить по другому.
	// Пока вручную создаю буфер и указываю m_hasItsOwnSound
	// bqSound держит только указатель, он не должен освобождать память (bqSoundBuffer)
	// m_hasItsOwnSound даёт знать что bqSoundBuffer принадлежит bqSound
	// Выход из ситуации это написать класс обёртку для bqSound
	m_sound->m_soundBuffer = new bqSoundBuffer;
	m_sound->m_hasItsOwnSound = true;
	if (!m_sound->m_soundBuffer->LoadFromFile("../data/sounds/Jimbo's_Lullaby_float32_stereo.wav"))
	{
		bqLog::PrintError(L"Failed to load sound (╯°□°)╯︵ ┻━┻ \n");
		return false;
	}
	m_sound->m_soundBuffer->Make32bitsFloat();
	m_sound->m_soundBuffer->Resample(soundDeviceInfo.m_sampleRate);

	m_mixer = soundSystem->SummonMixer(2);
	m_mixer->AddSound(m_sound);
	soundSystem->AddMixerToProcessing(m_mixer);
	
	m_sound->SetLoop(-1);
	m_sound->PlaybackStart();

	m_effect = new ExampleSoundEffectDelay;
	m_mixer->AddEffect(m_effect);

	return true;
}

void ExampleSoundDelay::Shutdown()
{
	bqFramework::GetSoundSystem()->RemoveAllMixersFromProcessing();

	BQ_SAFEDESTROY(m_mixer);
	BQ_SAFEDESTROY(m_sound);
	BQ_SAFEDESTROY(m_effect);
}

void ExampleSoundDelay::OnDraw()
{
	if (bqInput::IsKeyHit(bqInput::KEY_ESCAPE))
	{
		m_app->StopExample();
		return;
	}

	m_gs->BeginGUI();
	m_gs->EndGUI();

	m_gs->BeginDraw();
	m_gs->ClearAll();

	m_gs->EndDraw();
	m_gs->SwapBuffers();
}
#endif