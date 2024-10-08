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
#include "badcoiq/scene/bqSprite.h"
#include "badcoiq/sound/bqSoundSystem.h"

//#include "../src/kissfft/kiss_fft.h"

#ifdef BQ_WITH_SOUND

class ExampleSoundFFTSoundEffect : public bqSoundEffect
{
public:
	ExampleSoundFFTSoundEffect()
	{

	}

	//kiss_fft_cpx* m_fftDataIn = 0;
	//kiss_fft_cpx* m_fftDataOut = 0;

	virtual ~ExampleSoundFFTSoundEffect()
	{
	//	if (m_fftDataIn) delete[]m_fftDataIn;
	//	if (m_fftDataOut) delete[]m_fftDataOut;
	}

	virtual void Process(bqSoundMixer* mixer) override
	{
	//	auto bd = mixer->GetChannel(0);
	//	float* data32 = (float*)bd->m_data;
	//	
	//	bqSoundBufferInfo bufferInfo;
	//	mixer->GetSoundBufferInfo(bufferInfo);
	//	uint32_t samples = bd->m_dataSize / 4;
	////	printf("samples = %u\n", samples);

	////	samples = 512;

	//	kiss_fft_cfg cfg = kiss_fft_alloc(samples, 0, 0, 0);
	//	
	//	if (!m_fftDataIn)
	//	{
	//		m_fftDataIn = new kiss_fft_cpx[samples];
	//		m_fftDataOut = new kiss_fft_cpx[samples];
	//	}

	//	for (int i = 0; i < samples; ++i)
	//	{
	//		m_fftDataIn[i].i = 0;
	//		m_fftDataIn[i].r = data32[i];

	//		m_fftDataOut[i].i = 0;
	//		m_fftDataOut[i].r = 0;
	//	}

	//	kiss_fft(cfg, m_fftDataIn, m_fftDataOut);

	////	if (FFT(m_rPart, m_iPart, samples, 10, FT_DIRECT))
	//	{

	//		FILE* f = 0;
	//		fopen_s(&f, "spectrum.txt", "w");
	//		for (int i = 0; i < samples; i++)
	//		{
	//			fprintf(f, "SAMPLE: %10.6f %10.6f  %10.6f  %10.6f\n",
	//				data32[i],
	//				m_fftDataOut[i].r,
	//				m_fftDataOut[i].i,
	//				m_fftDataOut[i].r * m_fftDataOut[i].r 
	//				+ m_fftDataOut[i].i * m_fftDataOut[i].i);
	//		}
	//		fclose(f);
	//	}


	}
};

ExampleSoundFFT::ExampleSoundFFT(DemoApp* app)
	:
	DemoExample(app)
{
}

ExampleSoundFFT::~ExampleSoundFFT()
{
}


bool ExampleSoundFFT::Init()
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

	m_effect = new ExampleSoundFFTSoundEffect;
	m_mixer->AddEffect(m_effect);

	return true;
}

void ExampleSoundFFT::Shutdown()
{
	bqFramework::GetSoundSystem()->RemoveAllMixersFromProcessing();

	BQ_SAFEDESTROY(m_mixer);
	BQ_SAFEDESTROY(m_sound);
	BQ_SAFEDESTROY(m_effect);
}

void ExampleSoundFFT::OnDraw()
{
	if (bqInput::IsKeyHit(bqInput::KEY_ESCAPE))
	{
		m_app->StopExample();
		return;
	}

	ExampleSoundFFTSoundEffect* ef = dynamic_cast<ExampleSoundFFTSoundEffect*>(m_effect);

	m_gs->BeginGUI();
	//if (ef->m_fftDataOut)
	//{
	//	float Y = 1.f;
	//	float X = 0.f;
	//	auto f = [&](int i)
	//	{
	//		float V = ef->m_fftDataOut[i].r * ef->m_fftDataOut[i].r + 
	//			ef->m_fftDataOut[i].i * ef->m_fftDataOut[i].i;

	//		bqVec4f r;
	//		r.x = X;
	//		r.w = 300;
	//		r.y = r.w - 10 - (Y * V);
	//		r.z = r.x + 2;
	//		m_gs->DrawGUIRectangle(r, bq::ColorWhite, bq::ColorWhite, 0, 0);
	//	//	Y += 10.f;
	//		X += 3.f;
	//	};

	//	for (int i = 0; i < 500; ++i)
	//	{
	//		f(i);
	//	}
	//}

	m_gs->EndGUI();

	m_gs->BeginDraw();
	m_gs->ClearAll();

	m_gs->EndDraw();
	m_gs->SwapBuffers();
}
#endif
