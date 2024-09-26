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
#ifndef __BQPHYSICSH_
#define __BQPHYSICSH_

#include "badcoiq/containers/bqArray.h"
#include "badcoiq/common/bqColor.h"

#include "bqPhysicsShape.h"
#include "bqPhysicsMesh.h"
#include "bqRigidBody.h"

// Вектр гравитации не вшит в bqRigidBody
// Вместо этого есть объект который будет 
// притягивать bqRigidBody.
// Пока ничего особенного не вижу, радиус 
// и позиция - всё что нужно.
class bqGravityObject
{
public:
	bqGravityObject() {}
	~bqGravityObject() {}
	BQ_PLACEMENT_ALLOCATOR(bqGravityObject);

	bqReal m_radius = 100.0;
	bqVec3 m_position;
};

class bqPhysicsDebugDraw
{
public:
	bqPhysicsDebugDraw() {}
	virtual ~bqPhysicsDebugDraw() {}
	BQ_PLACEMENT_ALLOCATOR(bqPhysicsDebugDraw);

	// Флаги, что надо рисовать
	enum : uint32_t
	{
		Reason_DrawAabb = 0x1,
		Reason_DrawAll = 0xffffffff
	};
	uint32_t m_reason = 0;
	void AddReason(uint32_t r)
	{
		m_reason |= r;
	}
	void RemoveReason(uint32_t r)
	{
		m_reason &= ~r;
	}
	void ClearReasons()
	{
		m_reason = 0;
	}

	virtual void DrawLine(void* data, uint32_t, const bqVec3& v1, const bqVec3& v2, const bqColor&) = 0;

	struct DrawCommands
	{
		uint32_t m_reason = 0;
		bqVec3 m_v1;
		bqVec3 m_v2;
		bqColor m_color;

	};
	bqArray<DrawCommands> m_array;
};

class bqPhysics
{
	bqArray<bqArray<bqRigidBody*>*> m_array;
	bqArray<bqGravityObject*> m_gravityObjects;
	bqArray<bqRigidBody*> m_bodiesAfterOptimization;
	bqPhysicsDebugDraw* m_debugDraw = 0;
	void* m_debugDrawData = 0;
public:
	bqPhysics();
	~bqPhysics();
	BQ_PLACEMENT_ALLOCATOR(bqPhysics);

	void AddRigidBodyArray(bqArray<bqRigidBody*>*);
	void RemoveAllRigidBodyArrays();

	bqGravityObject* CreateGravityObject(bqReal radius, const bqVec3& position);
	void AddGravityObject(bqGravityObject*);
	void RemoveAllGravityObject();

	void Update(float dt);

	bqPhysicsShapeSphere* CreateShapeSphere(float radius);
	bqRigidBody* CreateRigidBody(bqPhysicsShape*, float mass, bqRigidBodyMotionState* = 0);

	void SetDebugDraw(bqPhysicsDebugDraw*, void* data);
	void DebugDraw();
};

#endif
