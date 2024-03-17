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

#include "badcoiq/scene/bqSceneObject.h"


ExampleBasicsRotations::ExampleBasicsRotations(DemoApp* app)
	:
	DemoExample(app)
{
}

ExampleBasicsRotations::~ExampleBasicsRotations()
{
}

void ExampleBasicsRotations::_onCamera()
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

bool ExampleBasicsRotations::Init()
{
	m_camera = new bqCamera();
	m_camera->m_position = bqVec3(3.f, 10.f, 8.f);
	m_camera->m_aspect = (float)m_app->GetWindow()->GetCurrentSize()->x / (float)m_app->GetWindow()->GetCurrentSize()->y;
	m_camera->Rotate(0.f, -45.f, 0.f);
	m_camera->SetType(bqCamera::Type::Perspective);
	m_camera->Update(0.f);
	m_camera->m_viewProjectionMatrix = m_camera->GetMatrixProjection() * m_camera->GetMatrixView();

	bqPolygonMesh pm;
	bqAabb aabb;
	aabb.m_min.Set(-0.2f, -0.25f, -2.f, 0.f);
	aabb.m_max.Set(0.2f, 0.25f, 2.f, 0.f);
	bqMat4 mat;
	pm.AddBox(aabb, mat);
	aabb.m_min.Set(-2.f, 0.1f, -0.2f, 0.f);
	aabb.m_max.Set(2.f, 0.2f, 0.2f, 0.f);
	pm.AddBox(aabb, mat);
	aabb.m_min.Set(-0.85f, 0.1f, -0.2f, 0.f);
	aabb.m_max.Set(0.85f, 0.2f, 0.2f, 0.f);
	mat.m_data[3].Set(0.f, 0.f, -1.8f, 1.f);
	pm.AddBox(aabb, mat);
	pm.GenerateNormals(false);
	pm.GenerateUVPlanar(211.f);
	auto m = pm.SummonMesh();
	m_airplane = m_gs->SummonMesh(m);
	delete m;

	m_sceneObject1 = new bqSceneObject;
	m_sceneObject2 = new bqSceneObject;
	m_sceneObject2->m_axisAlignedRotation = true;
	m_sceneObject3 = new bqSceneObject;

	// для 3D линии
	bqFramework::SetMatrix(bqMatrixType::ViewProjection, &m_camera->m_viewProjectionMatrix);

	m_gs->DisableBackFaceCulling();
	return true;
}

void ExampleBasicsRotations::Shutdown()
{
	BQ_SAFEDESTROY(m_camera);
	BQ_SAFEDESTROY(m_airplane);
	BQ_SAFEDESTROY(m_sceneObject1);
	BQ_SAFEDESTROY(m_sceneObject2);
	BQ_SAFEDESTROY(m_sceneObject3);
}

void ExampleBasicsRotations::OnDraw()
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

	// Обязательно надо установить матрицы
	bqMat4 viewI = m_camera->GetMatrixView();
	viewI.Invert();
	bqFramework::SetMatrix(bqMatrixType::ViewInvert, &viewI);
	bqFramework::SetMatrix(bqMatrixType::View, &m_camera->m_viewMatrix);
	bqFramework::SetMatrix(bqMatrixType::Projection, &m_camera->m_projectionMatrix);

	/*m_gs->DrawText3D(bqVec4(), U"Hello World!!!", 15, bqFramework::GetDefaultFont(bqGUIDefaultFont::Text),
		bq::ColorRed, 0.01f, 51);
	m_gs->DrawText3D(bqVec4(5.f, 0.f, 0.f, 0.f), U"Чуть дальше", 12, bqFramework::GetDefaultFont(bqGUIDefaultFont::Text),
		bq::ColorLime, 0.01f, 1);
	m_gs->DrawText3D(bqVec4(1.f, 0.f, 1.f, 0.f), U"Малого размера", 15, bqFramework::GetDefaultFont(bqGUIDefaultFont::Text),
		bq::ColorAliceBlue, 0.001f, 1);
	m_gs->DrawText3D(bqVec4(1.f, 0.f, 2.f, 0.f), U"БОЛЬШЕ", 7, bqFramework::GetDefaultFont(bqGUIDefaultFont::Text),
		bq::ColorBlueViolet, 0.1f, 1);*/

	float rotationSpeed = 50.f;
	if (bqInput::IsKeyHold(bqInput::KEY_1))
	{
		m_sceneObject1->RotateX(bqMath::DegToRad(rotationSpeed) * (*m_app->m_dt));
		m_sceneObject2->RotateX(bqMath::DegToRad(rotationSpeed) * (*m_app->m_dt));
	}
	if (bqInput::IsKeyHold(bqInput::KEY_2))
	{
		m_sceneObject1->RotateX(bqMath::DegToRad(-rotationSpeed) * (*m_app->m_dt));
		m_sceneObject2->RotateX(bqMath::DegToRad(-rotationSpeed) * (*m_app->m_dt));
	}
	if (bqInput::IsKeyHold(bqInput::KEY_3))
	{
		m_sceneObject1->RotateY(bqMath::DegToRad(rotationSpeed) * (*m_app->m_dt));
		m_sceneObject2->RotateY(bqMath::DegToRad(rotationSpeed) * (*m_app->m_dt));
	}
	if (bqInput::IsKeyHold(bqInput::KEY_4))
	{
		m_sceneObject1->RotateY(bqMath::DegToRad(-rotationSpeed) * (*m_app->m_dt));
		m_sceneObject2->RotateY(bqMath::DegToRad(-rotationSpeed) * (*m_app->m_dt));
	}
	if (bqInput::IsKeyHold(bqInput::KEY_5))
	{
		m_sceneObject1->RotateZ(bqMath::DegToRad(rotationSpeed) * (*m_app->m_dt));
		m_sceneObject2->RotateZ(bqMath::DegToRad(rotationSpeed) * (*m_app->m_dt));
	}
	if (bqInput::IsKeyHold(bqInput::KEY_6))
	{
		m_sceneObject1->RotateZ(bqMath::DegToRad(-rotationSpeed) * (*m_app->m_dt));
		m_sceneObject2->RotateZ(bqMath::DegToRad(-rotationSpeed) * (*m_app->m_dt));
	}

	m_sceneObject1->GetPosition().x = -5.f;
	m_sceneObject1->RecalculateWorldMatrix();
	auto & W = m_sceneObject1->GetMatrixWorld();
	bqFramework::SetMatrix(bqMatrixType::World, &W);

	bqMat4 WVP;
	WVP = m_camera->m_projectionMatrix * m_camera->m_viewMatrix * W;
	bqFramework::SetMatrix(bqMatrixType::WorldViewProjection, &WVP);

	m_gs->SetMesh(m_airplane);

	bqMaterial material;
	material.m_sunPosition.Set(2.f, 2.f, 1.f);
	material.m_shaderType = bqShaderType::Standart;
	material.m_maps[0].m_texture = m_app->m_texture4x4;
	m_gs->SetMaterial(&material);
	m_gs->SetShader(material.m_shaderType, 0);
	m_gs->Draw();

	m_sceneObject2->GetPosition().x = 5.f;
	m_sceneObject2->RecalculateWorldMatrix();
	W = m_sceneObject2->GetMatrixWorld();
	bqFramework::SetMatrix(bqMatrixType::World, &W);
	WVP = m_camera->m_projectionMatrix * m_camera->m_viewMatrix * W;
	bqFramework::SetMatrix(bqMatrixType::WorldViewProjection, &WVP);
	m_gs->Draw();
	
	
	m_sceneObject3->GetPosition().z = -5.f;
	bqQuaternion qq;
	bqVec4 vv = m_camera->m_position - m_sceneObject3->GetPosition(); // direction
	vv.w = 0.f;
	vv.Normalize();
	qq.FromVector(vv, bqVec4(0.f, 0.f, 1.f, 0.f));
	m_sceneObject3->m_qOrientation = qq;
	m_sceneObject3->RecalculateWorldMatrix();
	W = m_sceneObject3->GetMatrixWorld();
	bqFramework::SetMatrix(bqMatrixType::World, &W);
	WVP = m_camera->m_projectionMatrix * m_camera->m_viewMatrix * W;
	bqFramework::SetMatrix(bqMatrixType::WorldViewProjection, &WVP);
	m_gs->Draw();

	m_app->DrawGrid(14, (float)m_camera->m_position.y);

	// рисую линию которая покажет направление куда смотрит модель
	m_gs->DisableDepth();
	bqVec4 point(0., 0., 2., 1.);
	bqVec4 p;
	p = m_sceneObject1->m_qOrientation.RotateVector(point);
	m_gs->DrawLine3D(m_sceneObject1->GetPosition(), 
		m_sceneObject1->GetPosition() + p, bq::ColorRed);

	p = m_sceneObject2->m_qOrientation.RotateVector(point);
	m_gs->DrawLine3D(m_sceneObject2->GetPosition(),
		m_sceneObject2->GetPosition() + p, bq::ColorRed);

	p = m_sceneObject3->m_qOrientation.RotateVector(point);
	m_gs->DrawLine3D(m_sceneObject3->GetPosition(),
		m_sceneObject3->GetPosition() + p, bq::ColorRed);

	m_gs->EndDraw();
	m_gs->SwapBuffers();
}
