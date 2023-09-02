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

#pragma once
#ifndef __BQ_SCENECAMERA_H__
#define __BQ_SCENECAMERA_H__

#include "badcoiq/math/bqMath.h"

// Простой класс который будет вычислять View и Projection матрицы
class bqCamera
{
	void(bqCamera::* m_update)(float) = 0;
	
	void _calculateView()
	{
		bqMat4 P;
		P.m_data[3].x = -m_position.x;
		P.m_data[3].y = -m_position.y;
		P.m_data[3].z = -m_position.z;
		P.m_data[3].w = 1.f;

		bqMat4 R;
		R.SetRotation(m_rotation);

		m_view = R * P;
	}

	void _updatePerspective(float dt)
	{
		_calculateView();
		bqMath::PerspectiveRH(m_projection, m_fov, m_aspect, m_near, m_far);
	}

	void _updatePerspectiveLookAt(float dt)
	{
		bqMath::LookAtRH(m_view, m_position, m_lookAtTargett, m_upVector);
		bqMath::PerspectiveRH(m_projection, m_fov, m_aspect, m_near, m_far);
	}

	void _updateOrtho(float dt)
	{
		_calculateView();
		bqMath::OrthoRH(m_projection, m_orthoWidth, m_orthoHeight, m_near, m_far);
	}

	void _updateOrthoLookAt(float dt)
	{
		bqMath::LookAtRH(m_view, m_position, m_lookAtTargett, m_upVector);
		bqMath::OrthoRH(m_projection, m_orthoWidth, m_orthoHeight, m_near, m_far);
	}

public:
	bqCamera() 
	{
		m_update = &bqCamera::_updatePerspective;
	}
	~bqCamera() {}
	BQ_PLACEMENT_ALLOCATOR(bqCamera);

	void Update(float dt)
	{
		(this->*m_update)(dt);
	}

	enum class Type
	{
		Perspective,
		PerspectiveLookAt,
		Orthographic,
		OrthographicLookAt,
	};

	void SetType(Type t)
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
		}
	}

	bqMat4 m_view;
	bqMat4 m_projection;
	bqMat4 m_viewProjectionMatrix;

	bqVec4 m_position;
	bqVec4 m_lookAtTargett;
	bqVec4 m_upVector = bqVec4(0.0, 1.0, 0.0, 0.0);

	float m_aspect = 800.f / 600.f;
	float m_fov = (float)PI * 0.25f;
	float m_near = 0.1f;
	float m_far = 1000.f;

	float m_orthoWidth = 800.f;
	float m_orthoHeight = 600.f;

	bqQuaternion m_rotation;
};

#endif

