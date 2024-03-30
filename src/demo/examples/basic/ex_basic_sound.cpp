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
	m_sound1->LoadFromFile(bqFramework::GetAppPathA() + "../data/sounds/song1_float32_48000_mono.wav");
	
	
	auto ss = bqFramework::GetSoundSystem();

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


	//m_sound1->Generate(bqSoundWaveType::sin, 10.f, 440);
	//m_sound1->Convert(bqSoundFormat::float32_mono_44100);

	m_soundObject = ss->SummonObject(m_sound1);

	// Звуковые объекты созданные напрямую звуковым движком
	// играются напрямую этим движком
	/*m_soundObjectE = m_soundEngine->SummonObject(m_sound1);
	if (!m_soundObjectE)
	{
		bqLog::PrintError("Can't create sound object\n");
		return false;
	}
	
	m_soundObjectEloop = m_soundEngine->SummonObject(m_sound1);
	if (!m_soundObjectEloop)
	{
		bqLog::PrintError("Can't create sound object\n");
		return false;
	}
	m_soundObjectEloop->Loop(2);*/

//	m_soundObjectEloop->Use3D(true);
//	m_soundObjectEloop->SetEmitterPosition(bqVec3());

//	m_soundObjectStream = m_soundEngine->SummonStreamObject(bqFramework::GetAppPathA() + "../data/sounds/16bit2ch_MakeMono.wav");
	//4m_soundObjectStream->Loop(1);
	//m_soundObjectStream->SetVolume(0.19);

	m_gs->DisableBackFaceCulling();
	return true;
}

void ExampleBasicsSound::Shutdown()
{
	BQ_SAFEDESTROY(m_soundObjectStream);
	BQ_SAFEDESTROY(m_soundObjectEloop);
	BQ_SAFEDESTROY(m_soundObject);
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
	
//	m_soundObjectEloop->SetListenerPosition(bqVec3(m_camera->m_position.x, m_camera->m_position.y, m_camera->m_position.z));
//	m_soundObjectEloop->Update3D();

	//m_soundEngine->Test(m_camera->m_position.GetVec3());

	if (bqInput::IsKeyHit(bqInput::KEY_1))
	{
		m_soundObject->Play();
	}
	if (bqInput::IsKeyHit(bqInput::KEY_2))
	{
		m_soundObject->Stop();
	}
	//if (bqInput::IsKeyHit(bqInput::KEY_2))
	//{
	//	m_soundObjectEloop->Play();
	//}
	//if (bqInput::IsKeyHit(bqInput::KEY_3))
	//{
	//	m_soundObjectEloop->Stop();
	//}

	//if (bqInput::IsKeyHit(bqInput::KEY_Z))
	//{
	//	m_soundObjectStream->Play();
	//}
	//if (bqInput::IsKeyHit(bqInput::KEY_X))
	//{
	//	m_soundObjectStream->Stop();
	//}
	//if (bqInput::IsKeyHit(bqInput::KEY_C))
	//{
	//	m_soundObjectStream->Pause();
	//}

	static float volume = 0.5f;
	if (bqInput::IsKeyHold(bqInput::KEY_NUM_ADD))
	{
		volume += 0.01f;
		if (volume > 1.f) volume = 1.f;
//		m_soundObjectE->SetVolume(volume);
	}
	if (bqInput::IsKeyHold(bqInput::KEY_NUM_SUB))
	{
		volume -= 0.01f;
		if (volume < 0.f) volume = 0.f;
//		m_soundObjectE->SetVolume(volume);
	}


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
