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

#include "badcoiq/geometry/bqTriangle.h"


ExampleBasicsRayTri::ExampleBasicsRayTri(DemoApp* app)
	:
	DemoExample(app)
{
}

ExampleBasicsRayTri::~ExampleBasicsRayTri()
{
}


bool ExampleBasicsRayTri::Init()
{
	m_camera = new bqCamera();
	m_camera->m_position = bqVec3(5.f, 5.f, 5.f);
	m_camera->m_aspect = (float)m_app->GetWindow()->GetCurrentSize()->x / (float)m_app->GetWindow()->GetCurrentSize()->y;
	m_camera->Rotate(36.f, -45.f, 0.f);
	m_camera->SetType(bqCamera::Type::Perspective);
	m_camera->Update(0.f);
	m_camera->m_viewProjectionMatrix = m_camera->GetMatrixProjection() * m_camera->GetMatrixView();

	// для 3D линии
	bqFramework::SetMatrix(bqMatrixType::ViewProjection, &m_camera->m_viewProjectionMatrix);

	bqMat4 transform;
	bqMesh* mesh = 0;
	m_polygonMesh = new bqPolygonMesh;
	m_polygonMesh->AddSphere(1.f, 33, transform);
	m_polygonMesh->GenerateNormals(true);
	m_polygonMesh->GenerateUVPlanar(1.f);
	mesh = m_polygonMesh->SummonMesh();
	if (mesh)
	{
		m_meshSphere = m_gs->SummonMesh(mesh);
		delete mesh;
	}
	else
	{
		bqLog::PrintError("Can't create mesh: %s %i\n", BQ_FUNCTION, BQ_LINE);
	}

	if (!m_meshSphere)
	{
		bqLog::PrintError("Can't create GPU mesh: %s %i\n", BQ_FUNCTION, BQ_LINE);
		return false;
	}

	
	return true;
}

void ExampleBasicsRayTri::Shutdown()
{
	BQ_SAFEDESTROY(m_polygonMesh);
	BQ_SAFEDESTROY(m_meshSphere);
	BQ_SAFEDESTROY(m_camera);
}

void ExampleBasicsRayTri::_onCamera()
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

void ExampleBasicsRayTri::OnDraw()
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
	
	m_gs->SetShader(bqShaderType::Standart, 0);

	bqMaterial material;
	material.m_shaderType = bqShaderType::Standart;
	material.m_sunPosition.Set(2.f, 2.f, 1.f);
	material.m_maps[0].m_texture = m_app->m_texture4x4;
	m_gs->SetMaterial(&material);

	m_gs->SetMesh(m_meshSphere);

	bqFramework::SetMatrix(bqMatrixType::World, &m_worldSphere);

	m_wvp = m_camera->GetMatrixProjection() * m_camera->GetMatrixView() * m_worldSphere;
	bqFramework::SetMatrix(bqMatrixType::WorldViewProjection, &m_wvp);

	m_gs->Draw();

	// Луч от курсора
	bqMat4 VPi = m_camera->m_viewProjectionMatrix;
	VPi.Invert();

	bqRay r;
	r.CreateFrom2DCoords(
		bqVec2f(bqInput::GetData()->m_mousePosition.x,
			bqInput::GetData()->m_mousePosition.y),
		bqVec2f((float)m_app->GetWindow()->GetCurrentSize()->x,
			(float)m_app->GetWindow()->GetCurrentSize()->y),
		VPi,
		m_app->GetGS()->GetDepthRange());

	bqVec3 ip;
	bqTriangle triangle;
	if (_getTriangle(&triangle , &r, ip))
	{
		m_gs->SetShader(bqShaderType::Line3D, 0);
		m_gs->DrawLine3D(triangle.v1, triangle.v2, bq::ColorYellow);
		m_gs->DrawLine3D(triangle.v2, triangle.v3, bq::ColorYellow);
		m_gs->DrawLine3D(triangle.v3, triangle.v1, bq::ColorYellow);

		m_gs->DrawLine3D(ip + bqVec3(-1.f, 0.f, 0.f), ip + bqVec3(1.f, 0.f, 0.f), bq::ColorRed);
		m_gs->DrawLine3D(ip + bqVec3(0.f, -1.f, 0.f), ip + bqVec3(0.f, 1.f, 0.f), bq::ColorRed);
		m_gs->DrawLine3D(ip + bqVec3(0.f, 0.f, -1.f), ip + bqVec3(0.f, 0.f, 1.f), bq::ColorRed);
	}

	m_app->DrawGrid(14, (float)m_camera->m_position.y);

	m_gs->EndDraw();
	m_gs->SwapBuffers();
}

bool ExampleBasicsRayTri::_getTriangle(bqTriangle* out, bqRay* ray, bqVec3& ip)
{
	bool ret = false;
	bqReal len = 9999.0;
	if (m_polygonMesh->m_polygons.m_head)
	{
		for (auto p : m_polygonMesh->m_polygons)
		{
			auto V1 = p->m_vertices.m_head;
			auto last = V1->m_left;
			while (true)
			{
				auto V2 = V1->m_right;
				auto V3 = V2->m_right;

				bqTriangle tri;
				tri.v1 = V1->m_data->m_data.BaseData.Position;
				tri.v2 = V2->m_data->m_data.BaseData.Position;
				tri.v3 = V3->m_data->m_data.BaseData.Position;
				tri.Update();

				bqReal T = 0.f;
				bqReal U = 0.f;
				bqReal V = 0.f;
				bqReal W = 0.f;
				ray->Update();
				if (tri.RayIntersect_MT(*ray, true, T, U, V, W))
				{
					if (T < len)
					{
						ip = ray->m_origin + (T * ray->m_direction);
						len = T;
						*out = tri;
						ret = true;
					}
				}

				if (V1 == last)
					break;
				V1 = V1->m_right;
			}
		}
	}
	return ret;
}
