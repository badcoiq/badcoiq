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

#include "../../../DemoApp.h"

namespace math
{
	bqVec3f mul(const bqVec3f& vec, const bqMat4& mat)
	{
		return bqVec3f
		(
			mat[0u].x * vec.x + mat[1u].x * vec.y + mat[2u].x * vec.z + mat[3u].x,
			mat[0u].y * vec.x + mat[1u].y * vec.y + mat[2u].y * vec.z + mat[3u].y,
			mat[0u].z * vec.x + mat[1u].z * vec.y + mat[2u].z * vec.z + mat[3u].z
		);
	}
}



//void miViewportCamera::PanMove() {
//	float32_t speed = 1.f * (m_positionPlatform.w * 0.01f);
//
//	auto id = bqInput::GetData();
//
//	bqVec3f vec(speed * -id->m_mouseMoveDelta.x,
//		0.f,
//		speed * -id->m_mouseMoveDelta.y);
//	bqMat4 MX(bqQuaternion(m_rotationPlatform.x, 0.f, 0.f));
//	bqMat4 MY(bqQuaternion(0.f, m_rotationPlatform.y, 0.f));
//	//Mat4 MZ(Quat(0.f, 0.f, m_rotationPlatform.z));
//	vec = math::mul(vec, MY * MX);
//	m_positionPlatform.x += vec.x;
//	m_positionPlatform.y += vec.y;
//	m_positionPlatform.z += vec.z;
//}
//
//void miViewportCamera::Rotate(float32_t x, float32_t y, float32_t dt) {
//	const float32_t speed = 0.69f * dt;
//	m_rotationPlatform.x += y * speed;
//	m_rotationPlatform.y += x * speed;
//
//	if (m_rotationPlatform.y < 0.f) m_rotationPlatform.y = m_rotationPlatform.y + PIPI;
//
//	if (m_rotationPlatform.x > PIPI) m_rotationPlatform.x = 0.f;
//	if (m_rotationPlatform.y > PIPI) m_rotationPlatform.y = 0.f;
//	//if (m_rotationPlatform.x < -math::PIPI) m_rotationPlatform.x = 0.f;
//	//if (m_rotationPlatform.y < -math::PIPI) m_rotationPlatform.y = 0.f;
//	//Update();
//}
//void miViewportCamera::Update() {
//	{
//		float32_t zoom = m_positionPlatform.w;
//		bqMath::OrthoRH(m_projectionMatrixOrtho,
//			zoom * m_aspect,
//			zoom,
//			-m_far,
//			m_far);
//		bqMath::PerspectiveRH(m_projectionMatrixPersp, m_fov, m_aspect, m_near, m_far);
//	}
//
//	m_projectionMatrix = m_projectionMatrixPersp;
//
//	bqMat4 MX(bqQuaternion(m_rotationPlatform.x, 0.f, 0.f));
//	bqMat4 MY(bqQuaternion(0.f, m_rotationPlatform.y, 0.f));
//	//Mat4 MZ(Quat(0.f, 0.f, m_rotationPlatform.z));
//
//	m_positionCamera = bqVec3f(0.f, m_positionPlatform.w, 0.f);
//	bqMath::Mul((MY * MX), bqVec3f(m_positionCamera), m_positionCamera);
//	m_positionCamera += bqVec3f(m_positionPlatform.x, m_positionPlatform.y, m_positionPlatform.z);
//
//	bqMat4 T;
//	T.m_data[3].x = -m_positionCamera.x;
//	T.m_data[3].y = -m_positionCamera.y;
//	T.m_data[3].z = -m_positionCamera.z;
//	T.m_data[3].w = 1.f;
//
//	bqMat4 P(bqQuaternion(-m_rotationPlatform.x + bqMath::DegToRad(-90.f), 0.f, 0.f));
//	bqMat4 Y(bqQuaternion(0.f, -m_rotationPlatform.y + bqMath::DegToRad(0.f), 0.f));
//	bqMat4 R(bqQuaternion(0.f, 0.f, m_rotationPlatform.z));
//
//	m_viewMatrix = (R * (P * Y)) * T;
//	m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
//
//
//	m_viewMatrixInvert = m_viewMatrix; m_viewMatrixInvert.Invert();
//	auto pi = m_projectionMatrix; pi.Invert();
//	m_viewProjectionInvertMatrix = m_viewMatrixInvert * pi;
//	
//	//printf("%f %s\n", m_rotationPlatform.y, miGetDirectionName(m_direction));
//}

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
	//m_camera = new miViewportCamera();
	m_camera = new bqCamera();
	m_camera->SetType(bqCamera::Type::Editor);
	m_camera->EditorReset();
//	m_camera->Update();
	m_camera->Update(0.f);
	m_camera->CalculateViewMatrices();
	m_camera->m_aspect = (float)m_app->GetWindow()->GetCurrentSize()->x / (float)m_app->GetWindow()->GetCurrentSize()->y;
	
	//m_camera->m_viewProjectionMatrix = m_camera->GetMatrixProjection() * m_camera->GetMatrixView();
	m_camera->m_viewProjectionMatrix = m_camera->m_projectionMatrix * m_camera->m_viewMatrix;

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
//	m_camera->Update();
	m_camera->Update(*m_app->m_dt);
	m_camera->m_viewProjectionMatrix = m_camera->m_projectionMatrix * m_camera->m_viewMatrix;

	/*if (bqInput::GetData()->m_mouseWheelDelta != 0.f)
		m_camera->EditorZoom((int)bqInput::GetData()->m_mouseWheelDelta);*/

	if (bqInput::IsAlt())
	{
		if (bqInput::IsRMBHold())
		{
			m_camera->EditorRotate(&bqInput::GetData()->m_mouseMoveDelta, *m_app->m_dt);
			//auto id = bqInput::GetData();
			//m_camera->Rotate(id->m_mouseMoveDelta.x, id->m_mouseMoveDelta.y, *m_app->m_dt);
		}
	}
	else
	{
		if (bqInput::IsRMBHold())
		{
			m_camera->EditorPanMove(&bqInput::GetData()->m_mouseMoveDelta, *m_app->m_dt);
			//m_camera->PanMove();
		}
	}

	m_gs->BeginGUI();
	m_gs->EndGUI();

	m_gs->BeginDraw();
	m_gs->ClearAll();

	m_app->DrawGrid(14, (float)m_camera->m_position.y);

	m_gs->EndDraw();
	m_gs->SwapBuffers();
}
