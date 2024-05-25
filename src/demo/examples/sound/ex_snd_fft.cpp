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

#include "../../DemoApp.h"
#include "badcoiq/scene/bqSprite.h"
#include "badcoiq/sound/bqSoundSystem.h"

#ifdef BQ_WITH_SOUND

class ExampleSoundFFTSoundEffect : public bqSoundEffect
{
public:
	ExampleSoundFFTSoundEffect()
	{

	}

	virtual ~ExampleSoundFFTSoundEffect()
	{
		if (m_iPart) delete[]m_iPart;
		if (m_rPart) delete[]m_rPart;
	}

#define  NUMBER_IS_2_POW_K(x)   ((!((x)&((x)-1)))&&((x)>1))  // x is pow(2, k), k=1,2, ...
#define  FT_DIRECT        -1    // Direct transform.
#define  FT_INVERSE        1    // Inverse transform.

	bool  FFT(float* Rdat, float* Idat, int N, int LogN, int Ft_Flag)
	{
		// parameters error check:
		if ((Rdat == NULL) || (Idat == NULL))                  return false;
		if ((N > 16384) || (N < 1))                            return false;
		if (!NUMBER_IS_2_POW_K(N))                             return false;
		if ((LogN < 2) || (LogN > 14))                         return false;
		if ((Ft_Flag != FT_DIRECT) && (Ft_Flag != FT_INVERSE)) return false;

		register int  i, j, n, k, io, ie, in, nn;
		float         ru, iu, rtp, itp, rtq, itq, rw, iw, sr;

		static const float Rcoef[14] =
		{ -1.0000000000000000F,  0.0000000000000000F,  0.7071067811865475F,
			0.9238795325112867F,  0.9807852804032304F,  0.9951847266721969F,
			0.9987954562051724F,  0.9996988186962042F,  0.9999247018391445F,
			0.9999811752826011F,  0.9999952938095761F,  0.9999988234517018F,
			0.9999997058628822F,  0.9999999264657178F
		};
		static const float Icoef[14] =
		{ 0.0000000000000000F, -1.0000000000000000F, -0.7071067811865474F,
		   -0.3826834323650897F, -0.1950903220161282F, -0.0980171403295606F,
		   -0.0490676743274180F, -0.0245412285229122F, -0.0122715382857199F,
		   -0.0061358846491544F, -0.0030679567629659F, -0.0015339801862847F,
		   -0.0007669903187427F, -0.0003834951875714F
		};

		nn = N >> 1;
		ie = N;
		for (n = 1; n <= LogN; n++)
		{
			rw = Rcoef[LogN - n];
			iw = Icoef[LogN - n];
			if (Ft_Flag == FT_INVERSE) iw = -iw;
			in = ie >> 1;
			ru = 1.0F;
			iu = 0.0F;
			for (j = 0; j < in; j++)
			{
				for (i = j; i < N; i += ie)
				{
					io = i + in;
					rtp = Rdat[i] + Rdat[io];
					itp = Idat[i] + Idat[io];
					rtq = Rdat[i] - Rdat[io];
					itq = Idat[i] - Idat[io];
					Rdat[io] = rtq * ru - itq * iu;
					Idat[io] = itq * ru + rtq * iu;
					Rdat[i] = rtp;
					Idat[i] = itp;
				}

				sr = ru;
				ru = ru * rw - iu * iw;
				iu = iu * rw + sr * iw;
			}

			ie >>= 1;
		}

		for (j = i = 1; i < N; i++)
		{
			if (i < j)
			{
				io = i - 1;
				in = j - 1;
				rtp = Rdat[in];
				itp = Idat[in];
				Rdat[in] = Rdat[io];
				Idat[in] = Idat[io];
				Rdat[io] = rtp;
				Idat[io] = itp;
			}

			k = nn;

			while (k < j)
			{
				j = j - k;
				k >>= 1;
			}

			j = j + k;
		}

		if (Ft_Flag == FT_DIRECT) return true;

		rw = 1.0F / N;

		for (i = 0; i < N; i++)
		{
			Rdat[i] *= rw;
			Idat[i] *= rw;
		}

		return true;
	}

	float* m_iPart = 0;
	float* m_rPart = 0;

	virtual void Process(bqSoundMixer* mixer) override
	{
		auto bd = mixer->GetChannel(0);
		float* data32 = (float*)bd->m_data;
		
		bqSoundBufferInfo bufferInfo;
		mixer->GetSoundBufferInfo(bufferInfo);
		uint32_t samples = 512;

		if (!m_iPart)
		{
			m_iPart = new float[samples];
			m_rPart = new float[samples];
		}
		for (int i = 0; i < samples; ++i)
		{
			m_iPart[i] = 0;
			m_rPart[i] = data32[i];
		}

			if (FFT(m_rPart, m_iPart, samples, 10, FT_DIRECT))
			{
				//for (int i = 0; i < 8; ++i)
				//{

				//}

				FILE* f = 0;
				fopen_s(&f, "spectrum.txt", "w");
				for (int i = 0; i < samples; i++)
				{
					fprintf(f, "SAMPLE: %10.6f %10.6f  %10.6f  %10.6f\n",
						data32[i],
						m_rPart[i],
						m_iPart[i],
						m_rPart[i] * m_rPart[i] + m_iPart[i] * m_iPart[i]);
				}
				fclose(f);
			}


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
	if (ef->m_rPart)
	{
		float Y = 3.f;
		float X = 0.f;
		auto f = [&](int i)
		{
			bqVec4f r;
			r.x = X;
			r.w = 300;
			r.y = r.w - 10 - (Y * ef->m_rPart[i]);
			r.z = r.x + 10;
			m_gs->DrawGUIRectangle(r, bq::ColorWhite, bq::ColorWhite, 0, 0);
		//	Y += 10.f;
			X += 12.f;
		};

		f(0);
		f(1);
		f(2);
		f(3);
		f(4);
		f(5);
		f(6);
	//	f(7);
	}

	m_gs->EndGUI();

	m_gs->BeginDraw();
	m_gs->ClearAll();

	m_gs->EndDraw();
	m_gs->SwapBuffers();
}
#endif
