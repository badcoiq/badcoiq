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
#ifndef __BQ_SCENEobj_H__
#define __BQ_SCENEobj_H__

#include "badcoiq/common/bqHierarchy.h"
#include "badcoiq/common/bqUserData.h"
#include "badcoiq/geometry/bqAABB.h"

//BoundingVolume
enum class bqSceneObjectBV
{
	Aabb,
	Sphere
};

class bqSceneObject : public bqUserData, public bqHierarchy
{
	enum
	{
		sceneObjectFlag_visible = 0x1
	};
	uint32_t m_flags = 0;

	bqAabb m_aabb;
	bqAabb m_aabbTransformed;
	bqReal m_BVRadius = 0.f;
	uint32_t m_id = 0;

	bqMat4 m_matrixWorld;
	bqMat4 m_matrixRotation;

	bqVec4 m_position;
	bqVec3 m_scale = bqVec3(1.0);

	// Order for rotation will be X Y Z
	bqQuaternion m_qX;
	bqQuaternion m_qY;
	bqQuaternion m_qZ;
	bqQuaternion m_qOrientation;

public:
	bqSceneObject() {}
	virtual ~bqSceneObject() {}
	BQ_PLACEMENT_ALLOCATOR(bqSceneObject);

	bqVec4& GetPosition() { return m_position; }
	bqVec3& GetScale() { return m_scale; }
	bqAabb& GetAabb() { return m_aabb; }
	bqAabb& GetAabbTransformed() { return m_aabbTransformed; }
	bqMat4& GetMatrixWorld() { return m_matrixWorld; }
	bqMat4& GetMatrixRotation() { return m_matrixRotation; }
	bqQuaternion& GetQuaternion() { return m_qOrientation; }

	virtual void SetVisible(bool v) { v ? m_flags |= sceneObjectFlag_visible : m_flags &= ~sceneObjectFlag_visible; }
	bool IsIvisible() { return m_flags & sceneObjectFlag_visible; }

	uint32_t GetID() { return m_id; }
	void SetID(uint32_t id) { m_id = id; }

	virtual void RecalculateWorldMatrix()
	{
		m_matrixWorld.Identity();
		m_matrixWorld.m_data[0].x = m_scale.x;
		m_matrixWorld.m_data[1].y = m_scale.y;
		m_matrixWorld.m_data[2].z = m_scale.z;

		// I don't know how to do it fast and better
		bqQuaternion Q;
		Q = Q * m_qX;
		Q = Q * m_qY;
		Q = Q * m_qZ;

		if (m_axisAlignedRotation)
			m_qOrientation = m_qOrientation * Q;
		else
			m_qOrientation = Q * m_qOrientation; // like a airplane

		m_qX.Identity();
		m_qY.Identity();
		m_qZ.Identity();
		m_matrixRotation.SetRotation(m_qOrientation);

		m_matrixWorld = m_matrixWorld * m_matrixRotation;

		m_matrixWorld.m_data[3].x = m_position.x;
		m_matrixWorld.m_data[3].y = m_position.y;
		m_matrixWorld.m_data[3].z = m_position.z;

		bqSceneObject* parent = dynamic_cast<bqSceneObject*>(m_parent);
		if (parent)
			m_matrixWorld = m_matrixWorld * parent->m_matrixWorld;
	}
	// I don't know how to call it.
	// if false then object will rotates like airplane
	bool m_axisAlignedRotation = false;

	virtual void RotateY(float rad)
	{
		bqQuaternion q;
		q.Set(0.f, rad, 0.f);
		m_qY = q * m_qY;
	}

	virtual void RotateX(float rad)
	{
		bqQuaternion q;
		q.Set(rad, 0.f, 0.f);
		m_qX = q * m_qX;
	}

	virtual void RotateZ(float rad)
	{
		bqQuaternion q;
		q.Set(0.f, 0.f, rad);
		m_qZ = q * m_qZ;
	}

	bqSceneObjectBV m_BV = bqSceneObjectBV::Sphere;
	virtual void UpdateBV()
	{
		m_aabbTransformed.Transform(&m_aabb, &m_matrixWorld, &m_position);
		m_BVRadius = bqMath::Distance(m_aabb.m_min, m_aabb.m_max) * 0.5;
	}

	bqReal GetBVRadius() { return m_BVRadius; }
};

#endif

