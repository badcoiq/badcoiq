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

#include "../../DemoApp.h"

#include "badcoiq/scene/bqSceneObject.h"
#include "badcoiq/geometry/bqSkeleton.h"
#include "badcoiq/geometry/bqMDL.h"


ExampleBasicsMDL::ExampleBasicsMDL(DemoApp* app)
	:
	DemoExample(app)
{
}

ExampleBasicsMDL::~ExampleBasicsMDL()
{
}

void ExampleBasicsMDL::_onCamera()
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

bool ExampleBasicsMDL::Init()
{
	m_camera = new bqCamera();
	m_camera->m_position = bqVec3(0.f, 10.f, 8.f);
	m_camera->m_aspect = (float)m_app->GetWindow()->GetCurrentSize()->x / (float)m_app->GetWindow()->GetCurrentSize()->y;
	m_camera->Rotate(0.f, -45.f, 0.f);
	m_camera->SetType(bqCamera::Type::Perspective);
	m_camera->Update(0.f);
	m_camera->m_viewProjectionMatrix = m_camera->GetMatrixProjection() * m_camera->GetMatrixView();

	
	// для 3D линии
	bqFramework::SetMatrix(bqMatrixType::ViewProjection, &m_camera->m_viewProjectionMatrix);

	m_gs->DisableBackFaceCulling();

	m_mdl = new bqMDL;
	if (!m_mdl->Load(bqFramework::GetPath("../data/models/Jill.mdl").c_str(), m_gs, true))
	{
		BQ_SAFEDESTROY(m_mdl);
		return false;
	}

	m_texture = bqFramework::SummonTexture(m_gs, bqFramework::GetPath("../data/textures/st1.jpg").c_str());

	return true;
}

void ExampleBasicsMDL::Shutdown()
{
	BQ_SAFEDESTROY(m_mdl);
	BQ_SAFEDESTROY(m_camera);
	BQ_SAFEDESTROY(m_texture);
}

void ExampleBasicsMDL::OnDraw()
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

	bqMat4 viewI = m_camera->GetMatrixView();
	viewI.Invert();
	bqFramework::SetMatrix(bqMatrixType::ViewInvert, &viewI);
	bqFramework::SetMatrix(bqMatrixType::View, &m_camera->m_viewMatrix);
	bqFramework::SetMatrix(bqMatrixType::Projection, &m_camera->m_projectionMatrix);

	bqMat4 W, WVP;
	bqFramework::SetMatrix(bqMatrixType::World, &W);
	WVP = m_camera->GetMatrixProjection() * m_camera->GetMatrixView() * W;
	bqFramework::SetMatrix(bqMatrixType::WorldViewProjection, &WVP);

	bqMaterial material;
	m_gs->SetMaterial(&material);
	m_gs->EnableBackFaceCulling();

	material.m_sunPosition.Set(1.f, 20.f, 1.f);
	material.m_maps[0].m_texture = m_texture;
	

	auto skeleton = m_mdl->GetSkeleton();
	if (skeleton)
	{
		auto joints = &skeleton->GetJoints().m_data[0];
		if (joints) {
			for (size_t o = 0; o < skeleton->GetJoints().m_size; ++o) {
				bqFramework::GetMatrixSkinned()[o] = joints[o].m_data.m_matrixFinal;
			}
		}
	}

	for (size_t i = 0; i < m_mdl->GetMeshNum(); ++i)
	{
		auto gpumesh = m_mdl->GetGPUMesh(i);

		material.m_shaderType = m_mdl->GetShaderType(i);
		m_gs->SetShader(material.m_shaderType, 0);

		/*if (m_model->m_meshBuffers.m_data[i]->m_texture)
			material.m_maps[0].m_texture = m_model->m_meshBuffers.m_data[i]->m_texture;
		else*/
			//material.m_maps[0].m_texture = m_app->m_whiteTexture;

		m_gs->SetMesh(gpumesh);
		m_gs->Draw();
	}
	

	float a = 0.f;
	if (bqInput::IsKeyHold(bqInput::KEY_Z))
		a = 0.1f;
	if (bqInput::IsKeyHold(bqInput::KEY_X))
		a = -0.1f;

	if (skeleton)
	{
		auto bone = skeleton->GetJoint("head neck upper");
		if (bone)
		{
			bqQuaternion q;
			q.Set(0, a, 0);

			bone->m_data.m_transformation.m_base.m_rotation *= q;
			bone->m_data.m_transformation.CalculateMatrix();
			skeleton->Update();
		}
	}

	m_app->DrawGrid(14, (float)m_camera->m_position.y);
	

	m_gs->SetShader(bqShaderType::Line3D, 0);
	m_gs->DisableDepth();

	if (skeleton)
	{
		auto joints = &skeleton->GetJoints().m_data[0];
		if (skeleton->GetJoints().m_size)
		{
			size_t jsz = skeleton->GetJoints().m_size;
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

				if (joints[i].m_base.m_parentIndex != -1)
				{
					auto parent = joints[joints[i].m_base.m_parentIndex];

					bqMat4 _mI2 = parent.m_data.m_matrixBindInverse;
					_mI2.Invert();
					bqMat4 _mI = parent.m_data.m_matrixFinal * _mI2;
					m_gs->DrawLine3D(mI.m_data[3], _mI.m_data[3], bq::ColorWhite);
				}
			}
		}
	}

	m_gs->EnableDepth();

	m_gs->EndDraw();
	m_gs->SwapBuffers();
}
