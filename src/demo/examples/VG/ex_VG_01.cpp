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
#include "badcoiq/VG/bqVectorGraphics.h"
#include "badcoiq/VG/bqVectorGraphicsTarget.h"


Example_ex_VG_01_H_::Example_ex_VG_01_H_(DemoApp* app)
	:
	DemoExample(app)
{
}

Example_ex_VG_01_H_::~Example_ex_VG_01_H_()
{
}

void Example_ex_VG_01_H_::_onCamera()
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

bool Example_ex_VG_01_H_::Init()
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

	m_vg = new bqVectorGraphics;
	m_target = m_vg->CreateTarget(500, 500);
	m_target->Clear(bq::ColorWhite);
	bqVectorGraphicsShape line;
	bqVec2f quad[4] = 
	{
		bqVec2f(10.f, 10.f),
		bqVec2f(10.f, 9.f),

		bqVec2f(100.f, 99.f),
		bqVec2f(100.f, 100.f),
	};
	line.Create(quad, 4);
	//m_target->Draw(&line);
	uint32_t th = 13;
	bqColor ca[] = 
	{
		bq::ColorBlack,
		bq::ColorRed,
		bq::ColorLime,
		bq::ColorBlue,
		bq::ColorCornflowerBlue,
		bq::ColorBlanchedAlmond,
		bq::ColorAquamarine,
		bq::ColorWheat,
		bq::ColorRosyBrown,
		bq::ColorTan,
		bq::ColorYellow,
		bq::ColorAzure,
		bq::ColorIndianRed,
		bq::ColorOldLace,
		bq::ColorPaleGoldenRod,
		bq::ColorSaddleBrown,
		bq::ColorDarkBlue,
		bq::ColorFireBrick,
		bq::ColorGainsboro,
		bq::ColorHoneyDew,
		bq::ColorNavajoWhite,
	};
	for (int i = 0, i2 = 0; i < 100; i += 20, ++i2)
	{
		if (i2 > 20)
			i2 = 0;

		m_target->SetColor(ca[i2]);
	//	m_target->DrawLine(20, i,180,80, th);
	}
	m_target->DrawLine(20, 20, 180, 220, 13);
	/*m_target->DrawLine(20, 40, 180, 240, 12);
	m_target->DrawLine(20, 60, 180, 260, 11);
	m_target->DrawLine(20, 80, 180, 280, 10);
	m_target->DrawLine(20, 100, 180, 300, 9);
	m_target->DrawLine(20, 120, 180, 320, 8);
	m_target->DrawLine(20, 140, 180, 340, 7);
	m_target->DrawLine(20, 160, 180, 360, 6);
	m_target->DrawLine(20, 180, 180, 380, 5);
	m_target->DrawLine(20, 200, 180, 400, 4);
	m_target->DrawLine(20, 220, 180, 420, 3);
	m_target->DrawLine(20, 240, 180, 440, 2);
	m_target->DrawLine(20, 260, 180, 460, 1);*/

	m_texture = m_gs->CreateTexture(m_target->GetImage());
	
	m_rect = bqVec4f(0.f, 0.f, 500, 500);

	bqFont f;
	f.CreateFromFile(bqFramework::GetPath("../data/fonts/montserrat/Montserrat-Regular.ttf").c_str());

	return true;
}

void Example_ex_VG_01_H_::Shutdown()
{
	BQ_SAFEDESTROY(m_texture);
	BQ_SAFEDESTROY(m_target);
	BQ_SAFEDESTROY(m_vg);
	BQ_SAFEDESTROY(m_camera);
}

void Example_ex_VG_01_H_::OnDraw()
{
	if (bqInput::IsKeyHit(bqInput::KEY_ESCAPE))
	{
		m_app->StopExample();
		return;
	}

	bool redraw = false;
	static float32_t lineX = 1.f;
	static float32_t lineY = 1.f;
	if (bqInput::IsKeyHit(bqInput::KEY_UP))
	{
		lineY++;
		redraw = true;
	}
	else if (bqInput::IsKeyHit(bqInput::KEY_DOWN))
	{
		lineY--;
		redraw = true;
	}
	if (bqInput::IsKeyHit(bqInput::KEY_LEFT))
	{
		lineX--;
		redraw = true;
	}
	else if (bqInput::IsKeyHit(bqInput::KEY_RIGHT))
	{
		lineX++;
		redraw = true;
	}
	if (redraw)
	{
		m_target->Clear(bq::ColorWhite);
		//m_target->DrawLine(20, 20, 180+ lineX, 220 + lineY, 13);
		//m_target->DrawLine(20, 40, 180 + lineX, 240 + lineY, 12);
		m_target->DrawLine(20, 60, 180 + lineX, 260 + lineY, 11);
		//m_target->DrawLine(20, 80, 180 + lineX, 280 + lineY, 10);
		//m_target->DrawLine(20, 100, 180 + lineX, 300 + lineY, 9);
		m_target->DrawLine(20, 120, 180 + lineX, 320 + lineY, 8);
		//m_target->DrawLine(20, 140, 180 + lineX, 340 + lineY, 7);
		//m_target->DrawLine(20, 160, 180 + lineX, 360 + lineY, 6);
		m_target->DrawLine(20, 180, 180 + lineX, 380 + lineY, 5);
		m_target->DrawLine(20, 200, 180 + lineX, 400 + lineY, 4);
		m_target->DrawLine(20, 220, 180 + lineX, 420 + lineY, 3);
		m_target->DrawLine(20, 240, 180 + lineX, 440 + lineY, 2);
		m_target->DrawLine(20, 260, 180 + lineX, 460 + lineY, 1);
		//m_texture->
		BQ_SAFEDESTROY(m_texture);
		m_texture = m_gs->CreateTexture(m_target->GetImage());
	}

	m_gs->BeginGUI();
	{
		m_gs->DrawGUIRectangle(m_rect, bq::ColorWhite, bq::ColorWhite, m_texture, 0);
	}
	m_gs->EndGUI();

	m_gs->BeginDraw();
	m_gs->ClearAll();
	m_gs->SetClearColor(1.f, 0, 0, 0.f);

//	if (m_texture)

	m_gs->EndDraw();
	m_gs->SwapBuffers();
}

