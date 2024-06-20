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
#include "badcoiq/sound/bqSoundSystem.h"

#ifdef BQ_WITH_SOUND

ExampleSoundStream::ExampleSoundStream(DemoApp* app)
	:
	DemoExample(app)
{
}

ExampleSoundStream::~ExampleSoundStream()
{
}


bool ExampleSoundStream::Init()
{
	auto soundSystem = bqFramework::GetSoundSystem();
	auto soundDeviceInfo = soundSystem->GetDeviceInfo();

	m_stream = soundSystem->SummonStream("../data/music/ogg_stereo_44100_128.ogg");
	if (!m_stream)
	{
		bqLog::PrintError("Can't open file for streaming\n");
		return false;
	}
	
	
	m_mixer = soundSystem->SummonMixer(2);
	m_mixer->AddStream(m_stream);
	soundSystem->AddMixerToProcessing(m_mixer);
	
	m_stream->m_loop = true;

	return true;
}

void ExampleSoundStream::Shutdown()
{
	bqFramework::GetSoundSystem()->RemoveAllMixersFromProcessing();

	BQ_SAFEDESTROY(m_mixer);
	BQ_SAFEDESTROY(m_stream);
}

void ExampleSoundStream::OnDraw()
{
	if (bqInput::IsKeyHit(bqInput::KEY_ESCAPE))
	{
		m_app->StopExample();
		return;
	}

	if (bqInput::IsKeyHit(bqInput::KEY_Q))
	{
		m_stream->PlaybackStart();
	}
	if (bqInput::IsKeyHit(bqInput::KEY_W))
	{
		m_stream->PlaybackReset();
	}
	if (bqInput::IsKeyHit(bqInput::KEY_E))
	{
		m_stream->PlaybackStop();
	}

	m_gs->BeginGUI();
	m_gs->EndGUI();

	m_gs->BeginDraw();
	m_gs->ClearAll();

	m_gs->EndDraw();
	m_gs->SwapBuffers();
}
#endif
