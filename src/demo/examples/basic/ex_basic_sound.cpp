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

#ifdef BQ_WITH_SOUND

//#include "badcoiq/scene/bqSprite.h"
#include "badcoiq/sound/bqSoundSystem.h"

ExampleBasicsSound::ExampleBasicsSound(DemoApp* app)
	:
	DemoExample(app)
{
}

ExampleBasicsSound::~ExampleBasicsSound()
{
}

void ExampleBasicsSound::_onCamera()
{
	m_camera->Update(0.f);
	m_camera->m_viewProjectionMatrix = m_camera->GetMatrixProjection() * m_camera->GetMatrixView();

	if (bqInput::IsKeyHold(bqInput::KEY_SPACE))
	{
		m_camera->Rotate(bqInput::GetData()->m_mouseMoveDelta, *m_app->m_dt);

		// move cursor to center of the screen
		bqPoint windowCenter;
		m_app->GetWindow()->GetCenter(windowCenter);
		bqInput::SetMousePosition(m_app->GetWindow(), windowCenter.x, windowCenter.y);
	}

	if (bqInput::IsKeyHold(bqInput::KEY_A))
		m_camera->MoveLeft(*m_app->m_dt);
	if (bqInput::IsKeyHold(bqInput::KEY_D))
		m_camera->MoveRight(*m_app->m_dt);
	if (bqInput::IsKeyHold(bqInput::KEY_W))
		m_camera->MoveForward(*m_app->m_dt);
	if (bqInput::IsKeyHold(bqInput::KEY_S))
		m_camera->MoveBackward(*m_app->m_dt);
	if (bqInput::IsKeyHold(bqInput::KEY_Q))
		m_camera->MoveDown(*m_app->m_dt);
	if (bqInput::IsKeyHold(bqInput::KEY_E))
		m_camera->MoveUp(*m_app->m_dt);
	if (bqInput::IsKeyHold(bqInput::KEY_R))
		m_camera->Rotate(0.f, 0.f, 10.f * *m_app->m_dt);
	if (bqInput::IsKeyHold(bqInput::KEY_F))
		m_camera->Rotate(0.f, 0.f, -10.f * *m_app->m_dt);
}

bool ExampleBasicsSound::Init()
{
	m_camera = new bqCamera();
	m_camera->m_position = bqVec3(3.f, 3.f, 3.f);
	m_camera->m_aspect = (float)m_app->GetWindow()->GetCurrentSize()->x / (float)m_app->GetWindow()->GetCurrentSize()->y;
	m_camera->Rotate(36.f, -45.f, 0.f);
	m_camera->SetType(bqCamera::Type::Perspective);
	m_camera->Update(0.f);
	m_camera->m_viewProjectionMatrix = m_camera->GetMatrixProjection() * m_camera->GetMatrixView();

	bqFramework::SetMatrix(bqMatrixType::ViewProjection, &m_camera->m_viewProjectionMatrix);

	m_sound1 = new bqSound();
	m_sound1->m_soundBuffer = new bqSoundBuffer;
	m_sound1->m_soundBuffer->LoadFromFile(bqFramework::GetAppPathA() + "../data/sounds/song1_float32_48000_mono.wav");
	m_sound1->m_hasItsOwnSound = true;
	
	auto ss = bqFramework::GetSoundSystem();

	bqSoundMixer* mixer = ss->SummonMixer(1);

	if (mixer)
	{
		mixer->AddSound(m_sound1);

		mixer->Process();

		delete mixer;
	}

	bqSoundSystemDeviceInfo soundDeviceInfo = ss->GetDeviceInfo();
	bqLog::Print("Device sample rate (%u)\n", soundDeviceInfo.m_sampleRate);
	bqLog::Print("Sound sample rate (%u)\n", m_sound1->m_soundBuffer->m_bufferInfo.m_sampleRate);
	
	if (soundDeviceInfo.m_sampleRate != m_sound1->m_soundBuffer->m_bufferInfo.m_sampleRate)
	{
		bqLog::PrintWarning("Wrong sample rate. Resample.\n");

		if (m_sound1->m_soundBuffer->m_bufferInfo.m_format != bqSoundFormat::float32)
			m_sound1->m_soundBuffer->Make32bitsFloat();

		m_sound1->m_soundBuffer->Resample(soundDeviceInfo.m_sampleRate);
	}


	m_gs->DisableBackFaceCulling();
	return true;
}

void ExampleBasicsSound::Shutdown()
{
	BQ_SAFEDESTROY(m_soundObjectStream);
	BQ_SAFEDESTROY(m_sound1);
	BQ_SAFEDESTROY(m_camera);
}

void ExampleBasicsSound::OnDraw()
{
	if (bqInput::IsKeyHit(bqInput::KEY_ESCAPE))
	{
		m_app->StopExample();
		return;
	}

	_onCamera();
	


	m_gs->BeginGUI();
	m_gs->EndGUI();

	m_gs->BeginDraw();
	m_gs->ClearAll();

	// Обязательно надо установить матрицы
	bqMat4 viewI = m_camera->GetMatrixView();
	viewI.Invert();
	bqFramework::SetMatrix(bqMatrixType::ViewInvert, &viewI);
	bqFramework::SetMatrix(bqMatrixType::View, &m_camera->m_viewMatrix);
	bqFramework::SetMatrix(bqMatrixType::Projection, &m_camera->m_projectionMatrix);

	m_gs->DrawText3D(bqVec4(), U"Hello World!!!", 15, bqFramework::GetDefaultFont(bqGUIDefaultFont::Text),
		bq::ColorRed, 0.01f, 51);
	m_gs->DrawText3D(bqVec4(5.f, 0.f, 0.f, 0.f), U"Чуть дальше", 12, bqFramework::GetDefaultFont(bqGUIDefaultFont::Text),
		bq::ColorLime, 0.01f, 1);
	m_gs->DrawText3D(bqVec4(1.f, 0.f, 1.f, 0.f), U"Малого размера", 15, bqFramework::GetDefaultFont(bqGUIDefaultFont::Text),
		bq::ColorAliceBlue, 0.001f, 1);
	m_gs->DrawText3D(bqVec4(1.f, 0.f, 2.f, 0.f), U"БОЛЬШЕ", 7, bqFramework::GetDefaultFont(bqGUIDefaultFont::Text),
		bq::ColorBlueViolet, 0.1f, 1);
	
	m_app->DrawGrid(14, (float)m_camera->m_position.y);

	m_gs->EndDraw();
	m_gs->SwapBuffers();
}

#endif
