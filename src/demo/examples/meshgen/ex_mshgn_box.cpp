/*
BSD 2-Clause License

Copyright (c) 2023, badcoiq
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


ExampleMshGnBox::ExampleMshGnBox(DemoApp* app)
	:
	DemoExample(app)
{
}

ExampleMshGnBox::~ExampleMshGnBox()
{
}


bool ExampleMshGnBox::Init()
{
	m_camera = new bqCamera();
	m_camera->m_position = bqVec3(5.f, 5.f, 5.f);
	m_camera->m_aspect = (float)m_app->GetWindow()->GetCurrentSize()->x / (float)m_app->GetWindow()->GetCurrentSize()->y;
	m_camera->SetType(bqCamera::Type::PerspectiveLookAt);
	m_camera->Update(0.f);
	m_camera->m_viewProjectionMatrix = m_camera->GetMatrixProjection() * m_camera->GetMatrixView();

	bqFramework::SetMatrix(bqMatrixType::ViewProjection, &m_camera->m_viewProjectionMatrix);

	bqAabb aabb;
	aabb.m_min.Set(-2.f, -1.f, -1.f, 0.f);
	aabb.m_max.Set(2.f, -0.8f, 1.f, 0.f);
	bqMat4 transform;
	bqPolygonMesh pm;
	pm.AddBox(aabb, transform);
	
	pm.GenerateNormals(false);
	pm.GenerateUVPlanar(1.f);

	bqMesh* mesh = pm.SummonMesh();
	if (mesh)
	{
		m_meshBox = m_gs->SummonMesh(mesh);
		delete mesh;
	}
	else
	{
		bqLog::PrintError("Can't create mesh: %s %i\n", BQ_FUNCTION, BQ_LINE);
	}

	if (!m_meshBox)
	{
		bqLog::PrintError("Can't create GPU mesh: %s %i\n", BQ_FUNCTION, BQ_LINE);
		return false;
	}

	
	m_guiWindow = bqFramework::SummonGUIWindow(m_app->GetWindow(), bqVec2f(), bqVec2f(300.f, 300.f));
	m_guiWindow->m_windowFlags |= bqGUIWindow::windowFlag_withTitleBar;
	m_guiWindow->m_windowFlags |= bqGUIWindow::windowFlag_canMove;
	m_guiWindow->m_windowFlags |= bqGUIWindow::windowFlag_withCloseButton;
	m_guiWindow->SetTitle(U"Parameters");
	bqFramework::RebuildGUI();

	return true;
}

void ExampleMshGnBox::Shutdown()
{
	if (m_guiWindow)
	{
		bqFramework::Destroy(m_guiWindow);
		m_guiWindow = 0;
	}

	BQ_SAFEDESTROY(m_meshBox);
	BQ_SAFEDESTROY(m_camera);
}

void ExampleMshGnBox::OnDraw()
{
	if (bqInput::IsKeyHit(bqInput::KEY_ESCAPE))
	{
		m_app->StopExample();
		return;
	}

	m_camera->Update(0.f);
	m_camera->m_viewProjectionMatrix = m_camera->GetMatrixProjection() * m_camera->GetMatrixView();

	if (bqInput::IsKeyHold(bqInput::KEY_A))
		m_camera->m_position.x += 10.0 * (double)(*m_app->m_dt);
	if (bqInput::IsKeyHold(bqInput::KEY_D))
		m_camera->m_position.x -= 10.0 * (double)(*m_app->m_dt);
	if (bqInput::IsKeyHold(bqInput::KEY_W))
		m_camera->m_position.z += 10.0 * (double)(*m_app->m_dt);
	if (bqInput::IsKeyHold(bqInput::KEY_S))
		m_camera->m_position.z -= 10.0 * (double)(*m_app->m_dt);
	if (bqInput::IsKeyHold(bqInput::KEY_Q))
		m_camera->m_position.y += 10.0 * (double)(*m_app->m_dt);
	if (bqInput::IsKeyHold(bqInput::KEY_E))
		m_camera->m_position.y -= 10.0 * (double)(*m_app->m_dt);

	bqFramework::UpdateGUI();
	m_gs->BeginGUI();
	bqFramework::DrawGUI(m_gs);
	m_gs->EndGUI();

	m_gs->BeginDraw();
	m_gs->ClearAll();
	
	m_gs->SetShader(bqShaderType::Standart, 0);
	m_gs->SetMesh(m_meshBox);
	bqFramework::SetMatrix(bqMatrixType::World, &m_worldBox);
	m_wvp = m_camera->GetMatrixProjection() * m_camera->GetMatrixView() * m_worldBox;
	bqFramework::SetMatrix(bqMatrixType::WorldViewProjection, &m_wvp);
	bqMaterial material;
	material.m_shaderType = bqShaderType::Standart;
	material.m_sunPosition.Set(2.f, 2.f, 1.f);
	material.m_maps[0].m_texture = m_app->m_texture4x4;
	m_gs->SetMaterial(&material);
	m_gs->Draw();

	m_app->DrawGrid(14, (float)m_camera->m_position.y);

	m_gs->EndDraw();
	m_gs->SwapBuffers();
}
