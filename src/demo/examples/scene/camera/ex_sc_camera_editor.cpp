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

#include "../../../DemoApp.h"


ExampleSceneCameraEdtr::ExampleSceneCameraEdtr(DemoApp* app)
	:
	DemoExample(app)
{
}

ExampleSceneCameraEdtr::~ExampleSceneCameraEdtr()
{
}


bool ExampleSceneCameraEdtr::Init()
{
	m_camera = new bqCamera();
	m_camera->SetType(bqCamera::Type::Editor);
	m_camera->EditorReset();
	m_camera->Update(0.f);
	m_camera->m_aspect = (float)m_app->GetWindow()->GetCurrentSize()->x / (float)m_app->GetWindow()->GetCurrentSize()->y;
	
	m_camera->m_viewProjectionMatrix = m_camera->GetMatrixProjection() * m_camera->GetMatrixView();

	// для 3D линии
	bqFramework::SetMatrix(bqMatrixType::ViewProjection, &m_camera->m_viewProjectionMatrix);

	return true;
}

void ExampleSceneCameraEdtr::Shutdown()
{
	BQ_SAFEDESTROY(m_camera);
}

void ExampleSceneCameraEdtr::OnDraw()
{
	if (bqInput::IsKeyHit(bqInput::KEY_ESCAPE))
	{
		m_app->StopExample();
		return;
	}
	m_camera->Update(0.f);
	m_camera->m_viewProjectionMatrix = m_camera->GetMatrixProjection() * m_camera->GetMatrixView();

	if (bqInput::GetData()->m_mouseWheelDelta != 0.f)
		m_camera->EditorZoom((int)bqInput::GetData()->m_mouseWheelDelta);

	if (bqInput::IsAlt())
	{
		if (bqInput::IsRMBHold())
			m_camera->EditorRotate(&bqInput::GetData()->m_mouseMoveDelta, *m_app->m_dt);
	}
	else
	{
		if (bqInput::IsRMBHold())
			m_camera->EditorPanMove(&bqInput::GetData()->m_mouseMoveDelta, *m_app->m_dt);
	}

	m_gs->BeginGUI();
	m_gs->EndGUI();

	m_gs->BeginDraw();
	m_gs->ClearAll();

	m_app->DrawGrid(14, (float)m_camera->m_position.y);

	m_gs->EndDraw();
	m_gs->SwapBuffers();
}
