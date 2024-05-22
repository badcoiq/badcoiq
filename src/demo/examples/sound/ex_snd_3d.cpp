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


ExampleSound3D::ExampleSound3D(DemoApp* app)
	:
	DemoExample(app)
{
}

ExampleSound3D::~ExampleSound3D()
{
}


bool ExampleSound3D::Init()
{
	m_camera = new bqCamera();
	m_camera->m_position = bqVec3(3.f, 3.f, 3.f);
	m_camera->m_aspect = (float)m_app->GetWindow()->GetCurrentSize()->x / (float)m_app->GetWindow()->GetCurrentSize()->y;
	m_camera->Rotate(36.f, -45.f, 0.f);
	m_camera->SetType(bqCamera::Type::Perspective);
	m_camera->Update(0.f);
	m_camera->m_viewProjectionMatrix = m_camera->m_projectionMatrix * m_camera->m_viewMatrix;

	// для 3D линии
	bqFramework::SetMatrix(bqMatrixType::ViewProjection, &m_camera->m_viewProjectionMatrix);

	m_spriteSpeaker = new bqSprite;
	m_spriteSpeaker->SetSize(10.f * 0.1f, 10.f * 0.1f);

	bqImage* img = bqFramework::SummonImage(bqFramework::GetPath("../data/images/editor/speaker.png").c_str());
	if (img)
	{
		bqTextureInfo ti;
		auto texture = m_gs->SummonTexture(img, ti);
		delete img;
		if (texture)
		{
			m_loadedTextures.push_back(texture);
			m_spriteSpeaker->SetTexture(texture);
		}
		else
		{
			m_spriteSpeaker->SetTexture(m_app->m_texture4x4);
		}
	}

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
	if (!m_sound->m_soundBuffer->LoadFromFile("../data/sounds/alien_beacon44100_float.wav"))
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
	m_sound->m_use3D = true;

	return true;
}

void ExampleSound3D::Shutdown()
{
	bqFramework::GetSoundSystem()->RemoveAllMixersFromProcessing();

	for (size_t i = 0; i < m_loadedTextures.m_size; ++i)
	{
		delete m_loadedTextures.m_data[i];
	}

	BQ_SAFEDESTROY(m_mixer);
	BQ_SAFEDESTROY(m_sound);
	BQ_SAFEDESTROY(m_camera);
	BQ_SAFEDESTROY(m_spriteSpeaker);
}

void ExampleSound3D::OnDraw()
{
	if (bqInput::IsKeyHit(bqInput::KEY_ESCAPE))
	{
		m_app->StopExample();
		return;
	}

	m_camera->Update(0.f);
	m_camera->m_viewProjectionMatrix = m_camera->GetMatrixProjection() * m_camera->GetMatrixView();

	m_sound->m_soundPosition = m_spriteSpeaker->GetPosition();
	m_sound->m_listenerPosition = m_camera->m_position;
	m_sound->Update3D(m_camera->GetMatrixView());

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

	if (bqInput::IsKeyHold(bqInput::KEY_UP))
	{
		m_spriteSpeaker->GetPosition().x += 10.f * (*m_app->m_dt);
		m_spriteSpeaker->RecalculateWorldMatrix();
	}
	if (bqInput::IsKeyHold(bqInput::KEY_DOWN))
	{
		m_spriteSpeaker->GetPosition().x -= 10.f * (*m_app->m_dt);
		m_spriteSpeaker->RecalculateWorldMatrix();
	}
	if (bqInput::IsKeyHold(bqInput::KEY_LEFT))
	{
		m_spriteSpeaker->GetPosition().z += 10.f * (*m_app->m_dt);
		m_spriteSpeaker->RecalculateWorldMatrix();
	}
	if (bqInput::IsKeyHold(bqInput::KEY_RIGHT))
	{
		m_spriteSpeaker->GetPosition().z -= 10.f * (*m_app->m_dt);
		m_spriteSpeaker->RecalculateWorldMatrix();
	}
	if (bqInput::IsKeyHold(bqInput::KEY_PGUP))
	{
		m_spriteSpeaker->GetPosition().y += 10.f * (*m_app->m_dt);
		m_spriteSpeaker->RecalculateWorldMatrix();
	}
	if (bqInput::IsKeyHold(bqInput::KEY_PGDOWN))
	{
		m_spriteSpeaker->GetPosition().y -= 10.f * (*m_app->m_dt);
		m_spriteSpeaker->RecalculateWorldMatrix();
	}

	m_gs->BeginGUI();
	m_gs->EndGUI();

	m_gs->BeginDraw();
	m_gs->ClearAll();

	m_gs->SetShader(bqShaderType::Sprite, 0);
	bqMat4 viewI = m_camera->GetMatrixView();
	viewI.Invert();
	bqFramework::SetMatrix(bqMatrixType::ViewInvert, &viewI);
	bqFramework::SetMatrix(bqMatrixType::View, &m_camera->m_viewMatrix);
	bqFramework::SetMatrix(bqMatrixType::Projection, &m_camera->m_projectionMatrix);
	bqFramework::SetMatrix(bqMatrixType::World, &m_spriteSpeaker->GetMatrixWorld());
	m_gs->DrawSprite(m_spriteSpeaker);

	m_app->DrawGrid(14, (float)m_camera->m_position.y);

	m_gs->EndDraw();
	m_gs->SwapBuffers();
}
