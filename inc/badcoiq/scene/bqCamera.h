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

#pragma once
#ifndef __BQ_SCENECAMERA_H__
/// \cond
#define __BQ_SCENECAMERA_H__
/// \endcond

#ifdef BQ_WITH_SCENE

#include "badcoiq/math/bqMath.h"

/// Для frustum culling.
/// Если создаётся своя камера и нужен 
/// frustum culling то можно сделать объект
/// этого класса и вызвать CalculateFrustum
class bqCameraFrustum
{
public:
	bqCameraFrustum();
	~bqCameraFrustum();

	enum FrustumSide
	{
		RIGHT = 0,		// The RIGHT side of the frustum
		LEFT = 1,		// The LEFT	 side of the frustum
		BOTTOM = 2,		// The BOTTOM side of the frustum
		TOP = 3,		// The TOP side of the frustum
		BACK = 4,		// The BACK	side of the frustum
		FRONT = 5			// The FRONT side of the frustum
	};

	bqVec4 m_planes[6u];

	bool PointInFrustum(const bqVec4& point);
	bool SphereInFrustum(bqReal radius, const bqVec3& position);
	bool AABBInFrustum(const bqAabb& aabb);
	void NormalizePlane(bqVec4& plane);
	void CalculateFrustum(bqMat4& P, bqMat4& V);
};


class bqCamera
{
	void(bqCamera::* m_update)(float) = 0;
	
	void _calculateView();
	void _updatePerspective(float dt);
	void _updatePerspectiveLookAt(float dt);
	void _updateOrtho(float dt);
	void _updateOrthoLookAt(float dt);
	void _updateEditor(float dt);

	void _moveCamera(bqVec3& vel);

public:
	bqCamera();
	~bqCamera();
	BQ_PLACEMENT_ALLOCATOR(bqCamera);

	void CalculateViewMatrices();
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
		Editor
	};

	// for Editor camera
	enum class Direction : uint32_t
	{
		North,
		NorthEast,
		East,
		SouthEast,
		South,
		SouthWest,
		West,
		NorthWest
	};
	enum class CameraEditorType
	{
		Perspective,
		Bottom, Left, Right, Back, Front, Top
	};

	bqVec3f m_rotationPlatform;
	bqVec4 m_positionPlatform = bqVec4(0.f,0.f,0.f,10.f); // w = height, zoom, mouse wheel value
	//bqVec4 m_positionCamera; // in world
	Direction m_direction = Direction::North;
	CameraEditorType m_editorCameraType = CameraEditorType::Perspective;
	bool m_forceOrtho = false;
	void EditorPanMove(bqPointf* mouseDelta, float timeDelta);
	void EditorRotate(bqPointf* mouseDelta, float timeDelta);
	void EditorZoom(int wheelDelta);
	void EditorChangeFOV(bqPointf* mouseDelta, float timeDelta);
	void EditorRotateZ(bqPointf* mouseDelta, float timeDelta);
	void EditorReset();

	void SetType(Type t);

	void MoveLeft(float dt);
	void MoveRight(float dt);
	void MoveUp(float dt);
	void MoveDown(float dt);
	void MoveBackward(float dt);
	void MoveForward(float dt);	
	void Rotate(const bqPointf& mouseDelta, float dt);
	void Rotate(float x, float y, float z);

	bqMat4 m_viewMatrix;
	bqMat4 m_projectionMatrix;
	bqMat4 m_viewInvertMatrix; // вычисляется пока только для editor camera. это просто m_view.Invert();
	bqMat4 m_viewProjectionInvertMatrix; // тоже пока только для editor camera для
	bqMat4 m_viewProjectionMatrix;
	bqMat4 m_rotationMatrix;

	const bqMat4& GetMatrixView() { return m_viewMatrix; }
	const bqMat4& GetMatrixProjection() { return m_projectionMatrix; }
	const bqMat4& GetMatrixViewInvert() { return m_viewInvertMatrix; }
	const bqMat4& GetMatrixViewProjectionInvert() { return m_viewProjectionInvertMatrix; }
	const bqMat4& GetMatrixViewProjection() { return m_viewProjectionMatrix; }

	bqVec3 m_position;
	bqVec3 m_lookAtTargett;
	bqVec3 m_upVector = bqVec3(0.0, 1.0, 0.0);

	float m_moveSpeed = 10.f;

	float m_aspect = 800.f / 600.f;
	float m_fov = (float)PI * 0.25f;
	float m_near = 0.1f;
	float m_far = 1000.f;

	float m_orthoWidth = 800.f;
	float m_orthoHeight = 600.f;

	bqQuaternion m_rotation;
	bqCameraFrustum m_frustum;
	void UpdateFrustum();
};

#endif
#endif

