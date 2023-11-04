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

#include "badcoiq/scene/bqSceneObject.h"


ExampleBasicsOclCul::ExampleBasicsOclCul(DemoApp* app)
	:
	DemoExample(app)
{
	for (int i = 0; i < 1000; ++i)
	{
		m_objects[i] = 0;
	}
}

ExampleBasicsOclCul::~ExampleBasicsOclCul()
{
}

void ExampleBasicsOclCul::_onCamera()
{
	m_camera->Update(0.f);
	m_camera->UpdateFrustum();
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



bool ExampleBasicsOclCul::Init()
{
	m_camera = new bqCamera();
	m_camera->m_position = bqVec3(3.f, 10.f, 8.f);
	m_camera->m_aspect = (float)m_app->GetWindow()->GetCurrentSize()->x / (float)m_app->GetWindow()->GetCurrentSize()->y;
	m_camera->Rotate(0.f, -45.f, 0.f);
	m_camera->SetType(bqCamera::Type::Perspective);
	m_camera->Update(0.f);
	m_camera->m_viewProjectionMatrix = m_camera->GetMatrixProjection() * m_camera->GetMatrixView();

	bqPolygonMesh pm;
	bqMat4 mat;
	pm.AddCube(1.f, mat);
	pm.GenerateNormals(false);
	pm.GenerateUVPlanar(211.f);
	auto m = pm.SummonMesh();
	m_cube = m_gs->SummonMesh(m);
	delete m;

	pm.Clear();
	pm.AddSphere(0.5f, 30, mat);
	pm.GenerateNormals(true);
	pm.GenerateUVPlanar(211.f);
	m = pm.SummonMesh();
	m_sphere = m_gs->SummonMesh(m);
	delete m;

	pm.Clear();
	bqAabb aabb;
	aabb.m_min.Set(-5.f, -5.f, 0.1f, 0.f);
	aabb.m_max.Set(5.f, 5.f, 0.2f, 0.f);
	pm.AddBox(aabb, mat);
	pm.GenerateNormals(false);
	pm.GenerateUVPlanar(211.f);
	m = pm.SummonMesh();
	m_wall = m_gs->SummonMesh(m);
	delete m;

	m_wallObject = bqCreate<SceneObject_ExampleOcclusionCulling>();
	m_wallObject->m_occlusionObject = m_gs->SummonOcclusionObject();
	m_wallObject->m_occlusionObject->m_visible = false;
	m_wallObject->GetPosition().Set(0.f, 0.f, 10.f, 0.f);
	m_wallObject->RecalculateWorldMatrix();
	m_wallObject->GetAabb() = m_wall->m_meshInfo.m_aabb;
	m_wallObject->UpdateBV();

	bqReal x = -5.0, z = -5.0, y = -5.0;
	for (int i = 0; i < 1000; ++i)
	{
		m_objects[i] = bqCreate<SceneObject_ExampleOcclusionCulling>();

		auto& P = m_objects[i]->GetPosition();
		P.x = x;
		P.y = y;
		P.z = z;

		m_objects[i]->m_occlusionObject = m_gs->SummonOcclusionObject();
		m_objects[i]->m_occlusionObject->m_visible = false;

		m_objects[i]->RecalculateWorldMatrix();
		m_objects[i]->GetAabb() = m_sphere->m_meshInfo.m_aabb;
		m_objects[i]->UpdateBV();

		x += 1.1f;
		if (x > 6.f)
		{
			x = -5.f;
			z += 1.1f;

			if (z > 6.f)
			{
				y += 1.1f;
				z = -5.0;
			}
		}
	}

	// для 3D линии
	bqFramework::SetMatrix(bqMatrixType::ViewProjection, &m_camera->m_viewProjectionMatrix);

	m_gs->DisableBackFaceCulling();
	return true;
}

void ExampleBasicsOclCul::Shutdown()
{
	m_objectsVisible.clear();

	for (int i = 0; i < 1000; ++i)
	{
		if (m_objects[i])
		{
			bqDestroy(m_objects[i]);
			m_objects[i] = 0;
		}
	}

	BQ_SAFEDESTROY(m_camera);
	BQ_SAFEDESTROY(m_wallObject);
	BQ_SAFEDESTROY(m_sphere);
	BQ_SAFEDESTROY(m_cube);
	BQ_SAFEDESTROY(m_wall);
}

void ExampleBasicsOclCul::OnDraw()
{
	if (bqInput::IsKeyHit(bqInput::KEY_ESCAPE))
	{
		m_app->StopExample();
		return;
	}

	_onCamera();

	m_objectsInFrustum.clear();
	for (int i = 0; i < 1000; ++i)
	{
		if (m_camera->m_frustum.SphereInFrustum(m_objects[i]->GetBVRadius(), m_objects[i]->GetPosition()))
		{
			m_objectsInFrustum.push_back(m_objects[i]);
			m_objects[i]->WVP = m_camera->m_projectionMatrix * m_camera->m_viewMatrix * m_objects[i]->GetMatrixWorld();
			m_objects[i]->m_distanceToCamera = bqMath::Distance(m_objects[i]->GetPosition(), m_camera->m_position);
			m_objects[i]->m_occluderMesh = m_cube;
			m_objects[i]->m_drawMesh = m_sphere;
		}
	}
	if (m_camera->m_frustum.SphereInFrustum(m_wallObject->GetBVRadius(), m_wallObject->GetPosition()))
	{
		m_objectsInFrustum.push_back(m_wallObject);
		m_wallObject->WVP = m_camera->m_projectionMatrix * m_camera->m_viewMatrix * m_wallObject->GetMatrixWorld();
		m_wallObject->m_distanceToCamera = bqMath::Distance(m_wallObject->GetPosition(), m_camera->m_position);
		m_wallObject->m_occluderMesh = m_wall;
		m_wallObject->m_drawMesh = m_wall;
	}

	struct _pred
	{
		bool operator() (SceneObject_ExampleOcclusionCulling* a, SceneObject_ExampleOcclusionCulling* b) const
		{
			return a->m_distanceToCamera > b->m_distanceToCamera;
		}
	};
	m_objectsInFrustum.sort_insertion(_pred());

	m_gs->BeginGUI();
	m_textNumVisObjs.clear();
	m_textNumVisObjs = U"Objects in frustum: ";
	m_textNumVisObjs += m_objectsInFrustum.m_size;
	m_gs->DrawGUIText(m_textNumVisObjs.c_str(), m_textNumVisObjs.size(), bqVec2f(), m_app->GetTextDrawCallback());
	m_textNumVisObjs.clear();
	m_textNumVisObjs = U"After occlusion culling: ";
	m_textNumVisObjs += m_objectsVisible.m_size;
	m_gs->DrawGUIText(m_textNumVisObjs.c_str(), m_textNumVisObjs.size(), bqVec2f(0.f, 20.f), m_app->GetTextDrawCallback());
	m_gs->EndGUI();

	m_gs->BeginDraw();
	m_gs->ClearAll();

	m_gs->OcclusionBegin();
	for (size_t i = 0; i < m_objectsInFrustum.m_size; ++i)
	{
		auto& W = m_objectsInFrustum.m_data[i]->GetMatrixWorld();
		bqFramework::SetMatrix(bqMatrixType::World, &W);
		bqFramework::SetMatrix(bqMatrixType::WorldViewProjection, &m_objectsInFrustum.m_data[i]->WVP);
		m_gs->SetMesh(m_objectsInFrustum.m_data[i]->m_occluderMesh);
		m_gs->OcclusionDraw(m_objectsInFrustum.m_data[i]->m_occlusionObject);
	}
	for (size_t i = 0; i < m_objectsInFrustum.m_size; ++i)
	{
		auto& W = m_objectsInFrustum.m_data[i]->GetMatrixWorld();
		bqFramework::SetMatrix(bqMatrixType::World, &W);
		bqFramework::SetMatrix(bqMatrixType::WorldViewProjection, &m_objectsInFrustum.m_data[i]->WVP);
		m_gs->OcclusionResult(m_objectsInFrustum.m_data[i]->m_occlusionObject);
	}
	m_gs->OcclusionEnd();
	m_objectsVisible.clear();
	for (size_t i = 0; i < m_objectsInFrustum.m_size; ++i)
	{
		if(m_objectsInFrustum.m_data[i]->m_occlusionObject->m_visible)
			m_objectsVisible.push_back(m_objectsInFrustum.m_data[i]);
	}
	m_gs->ClearAll();

	// Обязательно надо установить матрицы
	bqMat4 viewI = m_camera->GetMatrixView();
	viewI.Invert();
	bqFramework::SetMatrix(bqMatrixType::ViewInvert, &viewI);
	bqFramework::SetMatrix(bqMatrixType::View, &m_camera->m_viewMatrix);
	bqFramework::SetMatrix(bqMatrixType::Projection, &m_camera->m_projectionMatrix);

	bqMaterial material;
	material.m_sunPosition.Set(2.f, 2.f, 1.f);
	material.m_shaderType = bqShaderType::Standart;
	material.m_maps[0].m_texture = m_app->m_texture4x4;
	m_gs->SetMaterial(&material);
	m_gs->SetShader(material.m_shaderType, 0);
	for (size_t i = 0; i < m_objectsVisible.m_size; ++i)
	{
		auto& W = m_objectsVisible.m_data[i]->GetMatrixWorld();
		bqFramework::SetMatrix(bqMatrixType::World, &W);
		bqFramework::SetMatrix(bqMatrixType::WorldViewProjection, &m_objectsVisible.m_data[i]->WVP);
		m_gs->SetMesh(m_objectsVisible.m_data[i]->m_drawMesh);
		m_gs->Draw();
	}

	m_app->DrawGrid(14, (float)m_camera->m_position.y);

	m_gs->EndDraw();
	m_gs->SwapBuffers();
}
