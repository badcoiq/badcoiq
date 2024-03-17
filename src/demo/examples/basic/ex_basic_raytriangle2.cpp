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



ExampleBasicsRayTri2::ExampleBasicsRayTri2(DemoApp* app)
	:
	DemoExample(app)
{
}

ExampleBasicsRayTri2::~ExampleBasicsRayTri2()
{
}

void ExampleBasicsRayTri2::_onCamera()
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

bool ExampleBasicsRayTri2::Init()
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

	m_model = new MyModel(m_gs);
	m_model->Load(bqFramework::GetPath("../data/models/4_objs.obj").c_str());
	
	m_gs->DisableBackFaceCulling();

	return true;
}

void ExampleBasicsRayTri2::Shutdown()
{
	BQ_SAFEDESTROY(m_model);
	BQ_SAFEDESTROY(m_camera);
}

void ExampleBasicsRayTri2::OnDraw()
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
	bqMat4 W, WVP;
	bqFramework::SetMatrix(bqMatrixType::World, &W);
	WVP = m_camera->GetMatrixProjection() * m_camera->GetMatrixView() * W;
	bqFramework::SetMatrix(bqMatrixType::WorldViewProjection, &WVP);
	bqMaterial material;
	material.m_shaderType = bqShaderType::Standart;
	material.m_sunPosition.Set(2.f, 2.f, 1.f);
	material.m_colorDiffuse = bq::ColorBlue;
	material.m_cullBackFace = false;
	m_gs->SetMaterial(&material);
	for (size_t i = 0; i < m_model->m_meshBuffers.m_size; ++i)
	{
		m_gs->SetMesh(m_model->m_meshBuffers.m_data[i]->m_mesh);
		m_gs->Draw();
	}

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
	r.Update();

	bqVec4 ip;
	bqTriangle triangle;
	bqAabb aabb;
	if (_getRayHit(aabb, triangle, r, ip))
	{
		m_gs->SetShader(bqShaderType::Line3D, 0);
		m_gs->DrawLine3D(triangle.v1, triangle.v2, bq::ColorYellow);
		m_gs->DrawLine3D(triangle.v2, triangle.v3, bq::ColorYellow);
		m_gs->DrawLine3D(triangle.v3, triangle.v1, bq::ColorYellow);

		m_gs->DrawLine3D(ip + bqVec4(-1.f, 0.f, 0.f, 0.f), ip + bqVec4(1.f, 0.f, 0.f, 0.f), bq::ColorRed);
		m_gs->DrawLine3D(ip + bqVec4(0.f, -1.f, 0.f, 0.f), ip + bqVec4(0.f, 1.f, 0.f, 0.f), bq::ColorRed);
		m_gs->DrawLine3D(ip + bqVec4(0.f, 0.f, -1.f, 0.f), ip + bqVec4(0.f, 0.f, 1.f, 0.f), bq::ColorRed);

		m_app->DrawAABB(aabb, bq::ColorWhite, bqVec4());
	}

	m_app->DrawGrid(14, (float)m_camera->m_position.y);

	m_gs->EndDraw();
	m_gs->SwapBuffers();
}

bool ExampleBasicsRayTri2::_getRayHit(bqAabb& aabb, bqTriangle& outTri, bqRay& ray, bqVec4& ip)
{
	bool ret = false;
	bqReal len = 9999.0;

	for (size_t i = 0; i < m_model->m_meshBuffers.m_size; ++i)
	{
		auto inf = m_model->m_meshBuffers.m_data[i]->m_CPUMmesh->GetInfo();
		if (inf.m_aabb.RayTest(ray))
		{
			auto VB = m_model->m_meshBuffers.m_data[i]->m_CPUMmesh->GetVBuffer();
			auto IB = m_model->m_meshBuffers.m_data[i]->m_CPUMmesh->GetIBuffer();
			
			bqVertexTriangle* vt = (bqVertexTriangle*)VB;
			bqVertexTriangleSkinned* vts = (bqVertexTriangleSkinned*)VB;
			uint32_t* i32 = (uint32_t*)IB;
			uint16_t* i16 = (uint16_t*)IB;

			for (uint32_t ii = 0; ii < inf.m_iCount; )
			{
				bqTriangle tri;

				uint32_t ind1 = 0;
				uint32_t ind2 = 0;
				uint32_t ind3 = 0;

				if (inf.m_indexType == bqMeshIndexType::u16)
				{
					ind1 = i16[ii];
					ind2 = i16[ii+1];
					ind3 = i16[ii+2];
				}
				else
				{
					ind1 = i32[ii];
					ind2 = i32[ii + 1];
					ind3 = i32[ii + 2];
				}

				if (inf.m_skinned)
				{
					tri.v1 = vts[ind1].BaseData.Position;
					tri.v2 = vts[ind2].BaseData.Position;
					tri.v3 = vts[ind3].BaseData.Position;
				}
				else
				{
					tri.v1 = vt[ind1].Position;
					tri.v2 = vt[ind2].Position;
					tri.v3 = vt[ind3].Position;
				}

				ii += 3;

				tri.Update();

				bqReal T = 0.f;
				bqReal U = 0.f;
				bqReal V = 0.f;
				bqReal W = 0.f;
				ray.Update();
				if (tri.RayTest_Watertight(ray, true, T, U, V, W))
				{
					if (T < len)
					{
						ip = ray.m_origin + (T * ray.m_direction);
						len = T;
						outTri = tri;
						ret = true;
						aabb = inf.m_aabb;
					}
				}
			}
		}
	}

	return ret;
}
