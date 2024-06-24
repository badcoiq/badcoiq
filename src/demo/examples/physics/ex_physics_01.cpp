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

#include "badcoiq/physics/bqPhysicsSystem.h"


ExamplePhysics01::ExamplePhysics01(DemoApp* app)
	:
	DemoExample(app)
{
}

ExamplePhysics01::~ExamplePhysics01()
{
}


bool ExamplePhysics01::Init()
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

	// Создаю коробку и сферу
	bqAabb aabb; // можно создать используя aabb
	aabb.m_min.Set(-2.f, -1.f, -1.f, 0.f);
	aabb.m_max.Set(2.f, -0.8f, 1.f, 0.f);
	bqMat4 transform; // можно повернуть объект, сжать\растянуть
	// в общем всегда будет умножение на матрицу
	bqPolygonMesh pm; // генерируется используя концепцию полигонов
	pm.AddSphere(0.5f, 33, transform);
	pm.GenerateNormals(true);
	auto mesh = pm.SummonMesh();
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

	m_physicsSystem = bqFramework::GetPhysicsSystem();

	m_shape = m_physicsSystem->CreateShapeSphere(0.2f);
	m_rigidBody = m_physicsSystem->CreateRigidBody(m_shape, 1.f);
	
	m_arrayOfBodies.push_back(m_rigidBody);
	m_physicsSystem->AddRigidBodyArray(&m_arrayOfBodies);

	_resetPhysics();

	return true;
}

void ExamplePhysics01::Shutdown()
{
	_resetPhysics();

	m_physicsSystem->RemoveAllGravityObject();
	m_physicsSystem->RemoveAllRigidBodyArrays();

	BQ_SAFEDESTROY(m_rigidBody);
	BQ_SAFEDESTROY(m_shape);

	BQ_SAFEDESTROY(m_meshSphere);
	BQ_SAFEDESTROY(m_camera);
}

void ExamplePhysics01::OnDraw()
{
	if (m_simulate)
	{
		m_physicsSystem->Update(*m_app->m_dt);
	}

	m_camera->Update(0.f);
	m_camera->m_viewProjectionMatrix = m_camera->GetMatrixProjection() * m_camera->GetMatrixView();

	if (bqInput::IsKeyHit(bqInput::KEY_ESCAPE))
	{
		m_app->StopExample();
		return;
	}

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

	if (bqInput::IsKeyHold(bqInput::KEY_Z))
		m_simulate = true;
	if (bqInput::IsKeyHold(bqInput::KEY_X))
	{
		_resetPhysics();
	}


	m_gs->BeginGUI();
	m_gs->DrawGUIText(U"Z - начать симуляцию, X - сброс", 37, bqVec2f(), m_app->GetTextDrawCallback());
	if(m_simulate)
		m_gs->DrawGUIText(U"***Simulation***", 17, bqVec2f(0.f, 15.f), m_app->GetTextDrawCallback());

	m_gs->EndGUI();

	m_gs->BeginDraw();
	m_gs->ClearAll();
	
	m_gs->SetShader(bqShaderType::Standart, 0);
	m_gs->SetMesh(m_meshSphere);
	m_worldSphere = m_rigidBody->m_motionState.m_matrix;
	bqFramework::SetMatrix(bqMatrixType::World, &m_worldSphere);
	m_wvp = m_camera->GetMatrixProjection() * m_camera->GetMatrixView() * m_worldSphere;
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

void ExamplePhysics01::_resetPhysics()
{
	m_simulate = false;
	m_rigidBody->m_motionState.Reset();

	m_rigidBody->m_motionState.m_linearVelocity.x = 1.f;
}