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
#include "badcoiq/geometry/bqSkeleton.h"


ExampleBasicsSkeletalAnimation::ExampleBasicsSkeletalAnimation(DemoApp* app)
	:
	DemoExample(app)
{
}

ExampleBasicsSkeletalAnimation::~ExampleBasicsSkeletalAnimation()
{
}

void ExampleBasicsSkeletalAnimation::_onCamera()
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

bool ExampleBasicsSkeletalAnimation::Init()
{
	m_camera = new bqCamera();
	m_camera->m_position = bqVec3(0.f, 10.f, 8.f);
	m_camera->m_aspect = (float)m_app->GetWindow()->GetCurrentSize()->x / (float)m_app->GetWindow()->GetCurrentSize()->y;
	m_camera->Rotate(0.f, -45.f, 0.f);
	m_camera->SetType(bqCamera::Type::Perspective);
	m_camera->Update(0.f);
	m_camera->m_viewProjectionMatrix = m_camera->GetMatrixProjection() * m_camera->GetMatrixView();

	bqPolygonMesh pm;
	bqMat4 mat;
	pm.AddSphere(1.f, 32, mat);
	pm.GenerateNormals(true);
	pm.GenerateUVPlanar(211.f);
	
	auto m = pm.SummonMesh();
	m->ToSkinned();

	m_skeleton = new bqSkeleton;
	mat.Identity();
	bqVec4 P;
	P.Set(0.f, -1.f, 0.f, 1.f);
	m_skeleton->AddJoint(bqQuaternion(), P, bqVec4(1.0), "0", -1);
	P.Set(0.f, 2.f, 0.f, 1.f);
	m_skeleton->AddJoint(bqQuaternion(), P, bqVec4(1.0), "1", 0);
	P.Set(-1.f, 0.f, 0.f, 1.f);
	m_skeleton->AddJoint(bqQuaternion(), P, bqVec4(1.0), "2", 1);
	m_skeleton->CalculateBind();

	m->ApplySkeleton(m_skeleton);

	m_mesh = m_gs->SummonMesh(m);
	delete m;

	m_sceneObject = new bqSceneObject;

	// для 3D линии
	bqFramework::SetMatrix(bqMatrixType::ViewProjection, &m_camera->m_viewProjectionMatrix);

	m_gs->DisableBackFaceCulling();
	return true;
}

void ExampleBasicsSkeletalAnimation::Shutdown()
{
	BQ_SAFEDESTROY(m_skeleton);
	BQ_SAFEDESTROY(m_camera);
	BQ_SAFEDESTROY(m_mesh);
	BQ_SAFEDESTROY(m_sceneObject);
}

void ExampleBasicsSkeletalAnimation::OnDraw()
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

	//m_sceneObject->GetPosition().x = -5.f;
	m_sceneObject->RecalculateWorldMatrix();
	auto & W = m_sceneObject->GetMatrixWorld();
	bqFramework::SetMatrix(bqMatrixType::World, &W);

	bqMat4 WVP;
	WVP = m_camera->m_projectionMatrix * m_camera->m_viewMatrix * W;
	bqFramework::SetMatrix(bqMatrixType::WorldViewProjection, &WVP);

	m_gs->SetMesh(m_mesh);

	bqMaterial material;
	material.m_sunPosition.Set(2.f, 2.f, 1.f);
	material.m_shaderType = bqShaderType::StandartSkinned;
	material.m_maps[0].m_texture = m_app->m_texture4x4;
	m_gs->SetMaterial(&material);
	m_gs->SetShader(material.m_shaderType, 0);

	bqFramework::GetMatrixSkinned()[0] = m_skeleton->GetJoints().m_data[0].m_data.m_matrixFinal;
	bqFramework::GetMatrixSkinned()[1] = m_skeleton->GetJoints().m_data[1].m_data.m_matrixFinal;
	bqFramework::GetMatrixSkinned()[2] = m_skeleton->GetJoints().m_data[2].m_data.m_matrixFinal;

	m_gs->Draw();

	// rotate and draw some joints
	static float a = 0.f; a += 1.1f * (*m_app->m_dt); if (a > PIPI) a = 0.f;
	if (bqInput::IsKeyHit(bqInput::KEY_F1))
		a = 0.f;
	m_skeleton->GetJoints().m_data[2].m_data.m_transformation.m_base.m_rotation.Set(a, 0, 0);
	m_skeleton->GetJoints().m_data[2].m_data.m_transformation.CalculateMatrix();
	m_skeleton->GetJoints().m_data[1].m_data.m_transformation.m_base.m_rotation.Set(0, a, 0);
	m_skeleton->GetJoints().m_data[1].m_data.m_transformation.CalculateMatrix();
	m_skeleton->Update();

	m_app->DrawGrid(14, (float)m_camera->m_position.y);

	m_gs->SetShader(bqShaderType::Line3D, 0);
	m_gs->DisableDepth();
	auto joints = &m_skeleton->GetJoints().m_data[0];
	if (m_skeleton->GetJoints().m_size)
	{
		size_t jsz = m_skeleton->GetJoints().m_size;
		for (size_t i = 0; i < jsz; ++i)
		{
			bqVec4 p;
			bqVec4 p2;
			float sz = 0.3f;
			p = bqVec4(sz, 0., 0., 0.);


			bqMat4 mI2 = joints[i].m_data.m_matrixBindInverse;
			mI2.Invert();

			bqMat4 mI = joints[i].m_data.m_matrixFinal * mI2;

			bqMath::Mul(mI, p, p2);
			m_gs->DrawLine3D(mI.m_data[3], mI.m_data[3] + p2, bq::ColorRed);

			p = bqVec4(0., sz, 0., 0.);
			bqMath::Mul(mI, p, p2);
			m_gs->DrawLine3D(mI.m_data[3], mI.m_data[3] + p2, bq::ColorLime);

			p = bqVec4(0., 0., sz, 0.);
			bqMath::Mul(mI, p, p2);
			m_gs->DrawLine3D(mI.m_data[3], mI.m_data[3] + p2, bq::ColorBlue);
		}
	}
	m_gs->EnableDepth();

	m_gs->EndDraw();
	m_gs->SwapBuffers();
}

// ================================

ExampleBasicsSkeletalAnimation2::ExampleBasicsSkeletalAnimation2(DemoApp* app)
	:
	DemoExample(app)
{
}

ExampleBasicsSkeletalAnimation2::~ExampleBasicsSkeletalAnimation2()
{
}

void ExampleBasicsSkeletalAnimation2::_onCamera()
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

bool ExampleBasicsSkeletalAnimation2::Init()
{
	m_camera = new bqCamera();
	m_camera->m_position = bqVec3(0.f, 10.f, 8.f);
	m_camera->m_aspect = (float)m_app->GetWindow()->GetCurrentSize()->x / (float)m_app->GetWindow()->GetCurrentSize()->y;
	m_camera->Rotate(0.f, -45.f, 0.f);
	m_camera->SetType(bqCamera::Type::Perspective);
	m_camera->Update(0.f);
	m_camera->m_viewProjectionMatrix = m_camera->GetMatrixProjection() * m_camera->GetMatrixView();

	bqPolygonMesh pm;
	bqMat4 mat;
	pm.AddSphere(1.f, 32, mat);
	pm.GenerateNormals(true);
	pm.GenerateUVPlanar(211.f);

	auto m = pm.SummonMesh();
	m->ToSkinned();

	m_skeleton = new bqSkeleton;
	mat.Identity();
	bqVec4 P;
	P.Set(0.f, -1.f, 0.f, 1.f);
	m_skeleton->AddJoint(bqQuaternion(), P, bqVec4(1.0), "0", -1);
	P.Set(0.f, 2.f, 0.f, 1.f);
	m_skeleton->AddJoint(bqQuaternion(), P, bqVec4(1.0), "1", 0);
	P.Set(-1.f, 0.f, 0.f, 1.f);
	m_skeleton->AddJoint(bqQuaternion(), P, bqVec4(1.0), "2", 1);
	m_skeleton->CalculateBind();

	m->ApplySkeleton(m_skeleton);

	m_mesh = m_gs->SummonMesh(m);
	delete m;

	m_sceneObject = new bqSceneObject;

	// для 3D линии
	bqFramework::SetMatrix(bqMatrixType::ViewProjection, &m_camera->m_viewProjectionMatrix);

	m_gs->DisableBackFaceCulling();
	return true;
}

void ExampleBasicsSkeletalAnimation2::Shutdown()
{
	BQ_SAFEDESTROY(m_skeleton);
	BQ_SAFEDESTROY(m_camera);
	BQ_SAFEDESTROY(m_mesh);
	BQ_SAFEDESTROY(m_sceneObject);
}

void ExampleBasicsSkeletalAnimation2::OnDraw()
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

	//m_sceneObject->GetPosition().x = -5.f;
	m_sceneObject->RecalculateWorldMatrix();
	auto& W = m_sceneObject->GetMatrixWorld();
	bqFramework::SetMatrix(bqMatrixType::World, &W);

	bqMat4 WVP;
	WVP = m_camera->m_projectionMatrix * m_camera->m_viewMatrix * W;
	bqFramework::SetMatrix(bqMatrixType::WorldViewProjection, &WVP);

	m_gs->SetMesh(m_mesh);

	bqMaterial material;
	material.m_sunPosition.Set(2.f, 2.f, 1.f);
	material.m_shaderType = bqShaderType::StandartSkinned;
	material.m_maps[0].m_texture = m_app->m_texture4x4;
	m_gs->SetMaterial(&material);
	m_gs->SetShader(material.m_shaderType, 0);

	bqFramework::GetMatrixSkinned()[0] = m_skeleton->GetJoints().m_data[0].m_data.m_matrixFinal;
	bqFramework::GetMatrixSkinned()[1] = m_skeleton->GetJoints().m_data[1].m_data.m_matrixFinal;
	bqFramework::GetMatrixSkinned()[2] = m_skeleton->GetJoints().m_data[2].m_data.m_matrixFinal;

	m_gs->Draw();

	// rotate and draw some joints
	static float a = 0.f; a += 1.1f * (*m_app->m_dt); if (a > PIPI) a = 0.f;
	if (bqInput::IsKeyHit(bqInput::KEY_F1))
		a = 0.f;
	m_skeleton->GetJoints().m_data[2].m_data.m_transformation.m_base.m_rotation.Set(a, 0, 0);
	m_skeleton->GetJoints().m_data[2].m_data.m_transformation.CalculateMatrix();
	m_skeleton->GetJoints().m_data[1].m_data.m_transformation.m_base.m_rotation.Set(0, a, 0);
	m_skeleton->GetJoints().m_data[1].m_data.m_transformation.CalculateMatrix();
	m_skeleton->Update();

	m_app->DrawGrid(14, (float)m_camera->m_position.y);

	m_gs->SetShader(bqShaderType::Line3D, 0);
	m_gs->DisableDepth();
	auto joints = &m_skeleton->GetJoints().m_data[0];
	if (m_skeleton->GetJoints().m_size)
	{
		size_t jsz = m_skeleton->GetJoints().m_size;
		for (size_t i = 0; i < jsz; ++i)
		{
			bqVec4 p;
			bqVec4 p2;
			float sz = 0.3f;
			p = bqVec4(sz, 0., 0., 0.);


			bqMat4 mI2 = joints[i].m_data.m_matrixBindInverse;
			mI2.Invert();

			bqMat4 mI = joints[i].m_data.m_matrixFinal * mI2;

			bqMath::Mul(mI, p, p2);
			m_gs->DrawLine3D(mI.m_data[3], mI.m_data[3] + p2, bq::ColorRed);

			p = bqVec4(0., sz, 0., 0.);
			bqMath::Mul(mI, p, p2);
			m_gs->DrawLine3D(mI.m_data[3], mI.m_data[3] + p2, bq::ColorLime);

			p = bqVec4(0., 0., sz, 0.);
			bqMath::Mul(mI, p, p2);
			m_gs->DrawLine3D(mI.m_data[3], mI.m_data[3] + p2, bq::ColorBlue);
		}
	}
	m_gs->EnableDepth();

	m_gs->EndDraw();
	m_gs->SwapBuffers();
}

