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


ExampleBasicsMshGnrtr::ExampleBasicsMshGnrtr(DemoApp* app)
	:
	DemoExample(app)
{
}

ExampleBasicsMshGnrtr::~ExampleBasicsMshGnrtr()
{
}


bool ExampleBasicsMshGnrtr::Init()
{
	m_camera = new bqCamera();
	m_camera->m_position = bqVec3(5.f, 5.f, 5.f);
	m_camera->m_aspect = (float)m_app->GetWindow()->GetCurrentSize()->x / (float)m_app->GetWindow()->GetCurrentSize()->y;
	m_camera->SetType(bqCamera::Type::PerspectiveLookAt);
	m_camera->Update(0.f);
	m_camera->m_viewProjectionMatrix = m_camera->GetMatrixProjection() * m_camera->GetMatrixView();

	// для 3D линии
	bqFramework::SetMatrix(bqMatrixType::ViewProjection, &m_camera->m_viewProjectionMatrix);

	// Создаю коробку и сферу
	bqAabb aabb; // можно создать используя aabb
	aabb.m_min.Set(-2.f, -1.f, -1.f, 0.f);
	aabb.m_max.Set(2.f, -0.8f, 1.f, 0.f);
	bqMat4 transform; // можно повернуть объект, сжать\растянуть
	// в общем всегда будет умножение на матрицу
	bqPolygonMesh pm; // генерируется используя концепцию полигонов
	pm.AddBox(aabb, transform); // Добавить коробку. можно вызывать множество Add методов,
	pm.GenerateNormals(false);
	pm.GenerateUVPlanar(1.f);
	// но в этом примере будет 1 на каждый bqGPUMesh
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

	pm.Clear();
	transform.m_data[1].y = 0.5; // сплющю сферу 
	pm.AddSphere(1.f, 33, transform);
	pm.GenerateNormals(true);
	pm.GenerateUVPlanar(100.f);
	mesh = pm.SummonMesh();
	if (mesh)
	{
		m_meshSphere = m_gs->SummonMesh(mesh);
		delete mesh;
	}
	else
	{
		bqLog::PrintError("Can't create mesh: %s %i\n", BQ_FUNCTION, BQ_LINE);
	}

	if (!m_meshSphere || !m_meshBox)
	{
		bqLog::PrintError("Can't create GPU mesh: %s %i\n", BQ_FUNCTION, BQ_LINE);
		return false;
	}

	

	return true;
}

void ExampleBasicsMshGnrtr::Shutdown()
{
	BQ_SAFEDESTROY(m_meshSphere);
	BQ_SAFEDESTROY(m_meshBox);
	BQ_SAFEDESTROY(m_camera);
}

void ExampleBasicsMshGnrtr::OnDraw()
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


	m_gs->BeginGUI();
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

	m_gs->SetMesh(m_meshSphere);
	bqFramework::SetMatrix(bqMatrixType::World, &m_worldSphere);
	m_wvp = m_camera->GetMatrixProjection() * m_camera->GetMatrixView() * m_worldSphere;
	m_gs->Draw();

	

	m_app->DrawGrid(14, (float)m_camera->m_position.y);

	m_gs->EndDraw();
	m_gs->SwapBuffers();
}
