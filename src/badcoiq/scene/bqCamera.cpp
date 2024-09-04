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

#include "badcoiq.h"

#ifdef BQ_WITH_SCENE

#include "badcoiq/scene/bqCamera.h"

bqCamera::bqCamera()
{
	m_update = &bqCamera::_updatePerspective;
}
bqCamera::~bqCamera() {}

void bqCamera::SetType(Type t)
{
	switch (t)
	{
	case bqCamera::Type::Perspective:
	default:
		m_update = &bqCamera::_updatePerspective;
		break;
	case bqCamera::Type::PerspectiveLookAt:
		m_update = &bqCamera::_updatePerspectiveLookAt;
		break;
	case bqCamera::Type::Orthographic:
		m_update = &bqCamera::_updateOrtho;
		break;
	case bqCamera::Type::OrthographicLookAt:
		m_update = &bqCamera::_updateOrthoLookAt;
		break;
	case bqCamera::Type::Editor:
		m_update = &bqCamera::_updateEditor;
		break;
	}
}

void bqCamera::_calculateView()
{
	bqMat4 P;
	P.m_data[3].x = -m_position.x;
	P.m_data[3].y = -m_position.y;
	P.m_data[3].z = -m_position.z;
	P.m_data[3].w = 1.f;

	bqMat4 R;
	R.SetRotation(m_rotation);

	bqVec4 V;
	bqMath::Mul(m_rotationMatrix, -m_position, V);

	//V = -m_position;

	m_viewMatrix = m_rotationMatrix;// *P;
	m_viewMatrix[3] = V;
	m_viewMatrix[3].w = 1.f;
}

void bqCamera::_updatePerspective(float)
{
	_calculateView();
	bqMath::PerspectiveRH(m_projectionMatrix, m_fov, m_aspect, m_near, m_far);
}

void bqCamera::_updatePerspectiveLookAt(float)
{
	bqMath::LookAtRH(m_viewMatrix, m_position, m_lookAtTargett, m_upVector);
	bqMath::PerspectiveRH(m_projectionMatrix, m_fov, m_aspect, m_near, m_far);
}

void bqCamera::_updateOrtho(float)
{
	_calculateView();
	bqMath::OrthoRH(m_projectionMatrix, m_orthoWidth, m_orthoHeight, m_near, m_far);
}

void bqCamera::_updateOrthoLookAt(float)
{
	bqMath::LookAtRH(m_viewMatrix, m_position, m_lookAtTargett, m_upVector);
	bqMath::OrthoRH(m_projectionMatrix, m_orthoWidth, m_orthoHeight, m_near, m_far);
}

void bqCamera::_moveCamera(bqVec4& vel)
{
	auto RotInv = m_rotationMatrix;
	RotInv.Invert();
	bqVec4 v;
	bqMath::Mul(RotInv, vel, v);
	m_position += v; // m_localPosition is just vec4 for position
}

void bqCamera::MoveLeft(float dt)
{
	bqVec4 v(-m_moveSpeed * dt, 0.f, 0.f, 1.f);
	_moveCamera(v);
}

void bqCamera::MoveRight(float dt)
{
	bqVec4 v(m_moveSpeed * dt, 0.f, 0.f, 1.f);
	_moveCamera(v);
}

void bqCamera::MoveUp(float dt)
{
	bqVec4 v(0.f, m_moveSpeed * dt, 0.f, 1.f);
	_moveCamera(v);
}

void bqCamera::MoveDown(float dt)
{
	bqVec4 v(0.f, -m_moveSpeed * dt, 0.f, 1.f);
	_moveCamera(v);
}

void bqCamera::MoveBackward(float dt)
{
	bqVec4 v(0.f, 0.f, m_moveSpeed * dt, 1.f);
	_moveCamera(v);
}

void bqCamera::MoveForward(float dt)
{
	bqVec4 v(0.f, 0.f, -m_moveSpeed * dt, 1.f);
	_moveCamera(v);
}

void bqCamera::Rotate(const bqPointf& mouseDelta, float dt)
{
	float speed = 4.4f;
	bqMat4 RX;
	bqMat4 RY;
	bool update = false;
	if (mouseDelta.x != 0.f)
	{
		update = true;
		RY.SetRotation(bqQuaternion(0.f, bqMath::DegToRad(-mouseDelta.x) * dt * speed, 0.f));
	}
	if (mouseDelta.y != 0.f)
	{
		update = true;
		RX.SetRotation(bqQuaternion(bqMath::DegToRad(-mouseDelta.y) * dt * speed, 0.f, 0.f));
	}

	if (update)
	{
		m_rotationMatrix = RX * m_rotationMatrix * RY;
	}
}

void bqCamera::Rotate(float x, float y, float z)
{
	bqMat4 RX;
	bqMat4 RY;
	bqMat4 RZ;
	RY.SetRotation(bqQuaternion(0.f, bqMath::DegToRad(x), 0.f));
	RX.SetRotation(bqQuaternion(bqMath::DegToRad(y), 0.f, 0.f));
	RZ.SetRotation(bqQuaternion(0.f, 0.f, bqMath::DegToRad(z)));

	m_rotationMatrix = RX * m_rotationMatrix * RY * RZ;
}

void bqCamera::_updateEditor(float)
{
	if ((m_editorCameraType != CameraEditorType::Perspective) || m_forceOrtho)
	{
		float zoom = (float)m_positionPlatform.w;
		bqMath::OrthoRH(
			m_projectionMatrix,
			zoom * m_aspect,
			zoom,
			-m_far,
			m_far);

	}
	else
	{
		bqMath::PerspectiveRH(m_projectionMatrix, m_fov, m_aspect, m_near, m_far);
	}

	//m_projectionMatrix.m_data[0].x = -m_projectionMatrix.m_data[0].x;

	bqMat4 MX(bqQuaternion(m_rotationPlatform.x, 0.f, 0.f));
	bqMat4 MY(bqQuaternion(0.f, m_rotationPlatform.y, 0.f));
	//Mat4 MZ(Quat(0.f, 0.f, m_rotationPlatform.z));

	m_position = bqVec4(0.f, m_positionPlatform.w, 0.f, 1.f);

	bqMath::Mul((MY * MX), bqVec4(m_position), m_position);

	m_position += bqVec4(m_positionPlatform.x, m_positionPlatform.y, m_positionPlatform.z, 0.f);

	bqMat4 T;
	T.m_data[3].x = -m_position.x;
	T.m_data[3].y = -m_position.y;
	T.m_data[3].z = -m_position.z;
	T.m_data[3].w = 1.f;

	bqMat4 P(bqQuaternion(m_rotationPlatform.x + bqMath::DegToRad(-90.f), 0.f, 0.f));
	bqMat4 Y(bqQuaternion(0.f, m_rotationPlatform.y + bqMath::DegToRad(0.f), 0.f));
	bqMat4 R(bqQuaternion(0.f, 0.f, m_rotationPlatform.z));

	m_viewMatrix = (R * (P * Y)) * T;
	CalculateViewMatrices();
	
	m_frustum.CalculateFrustum(m_projectionMatrix, m_viewMatrix);

	m_direction = Direction::NorthEast;

	if (m_rotationPlatform.y >= 2.7488936 && m_rotationPlatform.y <= 3.5342917)
		m_direction = Direction::North;
	else if (m_rotationPlatform.y >= 5.8904862 || m_rotationPlatform.y <= 0.3926991)
		m_direction = Direction::South;
	else if (m_rotationPlatform.y >= 1.1780972 && m_rotationPlatform.y <= 1.9634954)
		m_direction = Direction::West;
	else if (m_rotationPlatform.y >= 4.3196899 && m_rotationPlatform.y <= 5.1050881)
		m_direction = Direction::East;
	else if (m_rotationPlatform.y >= 1.9634954 && m_rotationPlatform.y <= 2.7488936)
		m_direction = Direction::NorthWest;
	else if (m_rotationPlatform.y >= 0.3926991 && m_rotationPlatform.y <= 1.1780972)
		m_direction = Direction::SouthWest;
	else if (m_rotationPlatform.y >= 5.1050881 && m_rotationPlatform.y <= 5.8904862)
		m_direction = Direction::SouthEast;
	
	// не помню почему, надо было заново вычислить матрицы
	// возможно надо убрать
	/*m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
	m_viewInvertMatrix = m_viewMatrix;
	m_viewInvertMatrix.Invert();
	m_viewProjectionInvertMatrix = m_viewInvertMatrix * pi;*/

	//m_frustum.CalculateFrustum(m_projectionMatrix, m_viewMatrix);
}

void bqCamera::CalculateViewMatrices()
{
	m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;

	m_viewInvertMatrix = m_viewMatrix;
	m_viewInvertMatrix.Invert();

	auto pi = m_projectionMatrix;
	pi.Invert();
	m_viewProjectionInvertMatrix = m_viewInvertMatrix * pi;
}

void bqCamera::EditorPanMove(bqPointf* mouseDelta, float timeDelta)
{
	float speed = 10.f * ((float)m_positionPlatform.w * 0.01f);

	bqVec4 vec(
		speed * mouseDelta->x * timeDelta,
		0.f,
		speed * -mouseDelta->y * timeDelta,
		0.f);
	bqMat4 MX(bqQuaternion(m_rotationPlatform.x, 0.f, 0.f));
	bqMat4 MY(bqQuaternion(0.f, m_rotationPlatform.y, 0.f));
	//Mat4 MZ(Quat(0.f, 0.f, m_rotationPlatform.z));
	bqMath::Mul(MY * MX, bqVec4(vec), vec);
	m_positionPlatform += vec;
}

void bqCamera::EditorRotate(bqPointf* mouseDelta, float timeDelta)
{
	const float speed = 0.69f * timeDelta;
	m_rotationPlatform.x += mouseDelta->y * speed;
	m_rotationPlatform.y += -mouseDelta->x * speed;

	if (m_rotationPlatform.y < 0.f) m_rotationPlatform.y = m_rotationPlatform.y + (float)PIPI;

	if (m_rotationPlatform.x > PIPI) m_rotationPlatform.x = 0.f;
	if (m_rotationPlatform.y > PIPI) m_rotationPlatform.y = 0.f;
	//Update();
}

void bqCamera::EditorZoom(int wheelDelta)
{
	float Mult = 1.f;

	if (wheelDelta > 0)
		m_positionPlatform.w *= 0.9f * (1.f / Mult);
	else
		m_positionPlatform.w *= 1.1f * Mult;

	if (m_positionPlatform.w < 0.01f)
		m_positionPlatform.w = 0.01f;

	//Update();
}

void bqCamera::EditorChangeFOV(bqPointf* mouseDelta, float timeDelta)
{
	m_fov += mouseDelta->x * timeDelta;
	if (m_fov < 0.01f)
		m_fov = 0.01f;
	if (m_fov > PI)
		m_fov = PI;
}

void bqCamera::EditorRotateZ(bqPointf* mouseDelta, float timeDelta)
{
	m_rotationPlatform.z += mouseDelta->x * timeDelta;
}

void bqCamera::EditorReset()
{
	m_near = 0.01f;
	m_far = 2500.f;
	m_fov = 0.683264f;
	m_aspect = 800.f / 600.f;
	m_positionPlatform = bqVec4(0.f, 0.f, 0.f, 15.f);
	switch (m_editorCameraType)
	{
	case CameraEditorType::Perspective:
		m_rotationPlatform = bqVec3f(bqMath::DegToRad(-45.f), 0.f, 0.f);
		break;
	case CameraEditorType::Bottom:
		m_rotationPlatform = bqVec3f(bqMath::DegToRad(-180.f), 0.f, 0.f);
		break;
	case CameraEditorType::Left:
		m_rotationPlatform = bqVec3f(bqMath::DegToRad(-90.f), bqMath::DegToRad(-90.f), 0.f);
		break;
	case CameraEditorType::Right:
		m_rotationPlatform = bqVec3f(bqMath::DegToRad(-90.f), bqMath::DegToRad(90.f), 0.f);
		break;
	case CameraEditorType::Back:
		m_rotationPlatform = bqVec3f(bqMath::DegToRad(-90.f), bqMath::DegToRad(180.f), 0.f);
		break;
	case CameraEditorType::Front:
		m_rotationPlatform = bqVec3f(bqMath::DegToRad(-90.f), bqMath::DegToRad(0.f), 0.f);
		break;
	case CameraEditorType::Top:
		m_rotationPlatform = bqVec3f();
		break;
	}

	bqPointf rp;
	EditorRotate(&rp, 1.f);
}

void bqCamera::UpdateFrustum()
{
	m_frustum.CalculateFrustum(m_projectionMatrix, m_viewMatrix);
}
#endif
