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



ExampleBasicsSprite::ExampleBasicsSprite(DemoApp* app)
	:
	DemoExample(app)
{
}

ExampleBasicsSprite::~ExampleBasicsSprite()
{
}

void ExampleBasicsSprite::_onCamera()
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

bool ExampleBasicsSprite::Init()
{
	m_camera = new bqCamera();
	m_camera->m_position = bqVec3(3.f, 3.f, 3.f);
	m_camera->m_aspect = (float)m_app->GetWindow()->GetCurrentSize()->x / (float)m_app->GetWindow()->GetCurrentSize()->y;
	m_camera->Rotate(36.f, -45.f, 0.f);
	m_camera->SetType(bqCamera::Type::Perspective);
	m_camera->Update(0.f);
	m_camera->m_viewProjectionMatrix = m_camera->GetMatrixProjection() * m_camera->GetMatrixView();

	// для 3D линии
	bqFramework::SetMatrix(bqMatrixType::ViewProjection, &m_camera->m_viewProjectionMatrix);

	m_gs->DisableBackFaceCulling();

	m_sprite = new bqSprite;
	m_sprite->SetSize(10.f / 22.f, 10.f / 22.f);

	bqImage* img = bqFramework::CreateImage(bqFramework::GetPath("../data/sprites/Full Coins.png").c_str());
	if (img)
	{
		bqTextureInfo ti;
		auto texture = m_gs->CreateTexture(img, ti);
		delete img;
		if (texture)
		{
			m_loadedTextures.push_back(texture);
			m_sprite->SetTexture(texture);
		}
		else
		{
			m_sprite->SetTexture(m_app->m_texture4x4);
		}
	}


	m_spriteWithState = new bqSprite(m_sprite->GetTexture());
	m_spriteWithState->SetSize(10.f / 22.f, 10.f / 22.f);
	m_spriteWithState->GetPosition().Set(1.f, 0.f, 0.f);
	m_spriteWithState->RecalculateWorldMatrix();
	auto spriteState = m_spriteWithState->CreateNewState();
	spriteState->AddFrame(bqVec4f(0.f, 0.f, 14.f, 15.f));
	m_spriteWithState->SetActiveState(spriteState);

	m_spriteWithStateAndAnimation = new bqSprite(m_sprite->GetTexture());
	m_spriteWithStateAndAnimation->SetSize(10.f / 22.f, 10.f / 22.f);
	m_spriteWithStateAndAnimation->GetPosition().Set(2.f, 0.f, 0.f);
	m_spriteWithStateAndAnimation->RecalculateWorldMatrix();
	spriteState = m_spriteWithStateAndAnimation->CreateNewState();
	spriteState->AddFrame(bqVec4f(0.f, 0.f, 14.f, 15.f));
	spriteState->AddFrame(bqVec4f(16.f, 0.f, 30.f, 15.f));
	spriteState->AddFrame(bqVec4f(32.f, 0.f, 46.f, 15.f));
	spriteState->AddFrame(bqVec4f(48.f, 0.f, 62.f, 15.f));
	spriteState->AddFrame(bqVec4f(64.f, 0.f, 78.f, 15.f));
	spriteState->AddFrame(bqVec4f(80.f, 0.f, 94.f, 15.f));
	spriteState->AddFrame(bqVec4f(96.f, 0.f, 110.f, 15.f));
	spriteState->AddFrame(bqVec4f(112.f, 0.f, 126.f, 15.f));
	spriteState->m_isAnimation = true;
	spriteState->SetFPS(10.f);
	m_spriteWithStateAndAnimation->SetActiveState(spriteState);

	m_spriteWithStateAndAnimationAndColor = new bqSprite(m_sprite->GetTexture());
	m_spriteWithStateAndAnimationAndColor->SetSize(10.f / 22.f, 10.f / 22.f);
	m_spriteWithStateAndAnimationAndColor->GetPosition().Set(3.f, 0.f, 0.f);
	m_spriteWithStateAndAnimationAndColor->RecalculateWorldMatrix();
	spriteState = m_spriteWithStateAndAnimationAndColor->CreateNewState();
	spriteState->AddFrame(bqVec4f(0.f, 0.f, 14.f, 15.f));
	spriteState->AddFrame(bqVec4f(16.f, 0.f, 30.f, 15.f));
	spriteState->AddFrame(bqVec4f(32.f, 0.f, 46.f, 15.f));
	spriteState->AddFrame(bqVec4f(48.f, 0.f, 62.f, 15.f));
	spriteState->AddFrame(bqVec4f(64.f, 0.f, 78.f, 15.f));
	spriteState->AddFrame(bqVec4f(80.f, 0.f, 94.f, 15.f));
	spriteState->AddFrame(bqVec4f(96.f, 0.f, 110.f, 15.f));
	spriteState->AddFrame(bqVec4f(112.f, 0.f, 126.f, 15.f));
	spriteState->m_isAnimation = true;
	spriteState->SetFPS(20.f);
	m_spriteWithStateAndAnimationAndColor->SetActiveState(spriteState);
	m_spriteWithStateAndAnimationAndColor->GetColor() = bq::ColorYellow;

	m_spriteFire = new bqSprite;
	img = bqFramework::CreateImage(bqFramework::GetPath("../data/sprites/22 TorchDrippingYellow.png").c_str());
	if (img)
	{
		bqTextureInfo ti;
		auto texture = m_gs->CreateTexture(img, ti);
		delete img;
		if (texture)
		{
			m_loadedTextures.push_back(texture);
			m_spriteFire->SetTexture(texture);
		}
		else
		{
			m_spriteFire->SetTexture(m_app->m_texture4x4);
		}
	}
	m_spriteFire->SetSize(30.f / 22.f, 30.f / 22.f);
	m_spriteFire->GetPosition().Set(0.f, 0.f, 1.f);
	m_spriteFire->RecalculateWorldMatrix();
	spriteState = m_spriteFire->CreateNewState();
	spriteState->CreateAnimation(32, bqVec2f(192.f, 192.f), bqVec2f(0.f));
	spriteState->SetFPS(30.f);
	m_spriteFire->SetActiveState(spriteState);
	m_spriteFire->m_alphaDiscard = 0.f;

	return true;
}

void ExampleBasicsSprite::Shutdown()
{
	for (size_t i = 0; i < m_loadedTextures.m_size; ++i)
	{
		delete m_loadedTextures.m_data[i];
	}
	m_loadedTextures.clear();

	BQ_SAFEDESTROY(m_sprite);
	BQ_SAFEDESTROY(m_spriteWithState);
	BQ_SAFEDESTROY(m_spriteWithStateAndAnimation);
	BQ_SAFEDESTROY(m_spriteWithStateAndAnimationAndColor);
	BQ_SAFEDESTROY(m_spriteFire);
	BQ_SAFEDESTROY(m_camera);
}

void ExampleBasicsSprite::OnDraw()
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

	m_gs->SetShader(bqShaderType::Sprite, 0);
	bqMat4 viewI = m_camera->GetMatrixView();
	viewI.Invert();
	bqFramework::SetMatrix(bqMatrixType::ViewInvert, &viewI);
	bqFramework::SetMatrix(bqMatrixType::View, &m_camera->m_viewMatrix);
	bqFramework::SetMatrix(bqMatrixType::Projection, &m_camera->m_projectionMatrix);
	bqFramework::SetMatrix(bqMatrixType::World, &m_sprite->GetMatrixWorld());
	m_gs->DrawSprite(m_sprite);
	
	bqFramework::SetMatrix(bqMatrixType::World, &m_spriteWithState->GetMatrixWorld());
	m_gs->DrawSprite(m_spriteWithState);
	
	bqFramework::SetMatrix(bqMatrixType::World, &m_spriteWithStateAndAnimation->GetMatrixWorld());
	m_spriteWithStateAndAnimation->AnimationUpdate(*m_app->m_dt);
	m_gs->DrawSprite(m_spriteWithStateAndAnimation);
	
	bqFramework::SetMatrix(bqMatrixType::World, &m_spriteWithStateAndAnimationAndColor->GetMatrixWorld());
	m_spriteWithStateAndAnimationAndColor->AnimationUpdate(*m_app->m_dt);
	m_gs->DrawSprite(m_spriteWithStateAndAnimationAndColor);
	
	bqFramework::SetMatrix(bqMatrixType::World, &m_spriteFire->GetMatrixWorld());
	m_spriteFire->AnimationUpdate(*m_app->m_dt);
	m_gs->DrawSprite(m_spriteFire);
	
	m_app->DrawGrid(14, (float)m_camera->m_position.y);

	m_gs->EndDraw();
	m_gs->SwapBuffers();
}
