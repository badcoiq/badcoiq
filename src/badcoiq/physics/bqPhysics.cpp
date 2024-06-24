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

#ifdef BQ_WITH_PHYSICS

#include "badcoiq/physics/bqPhysicsSystem.h"

#include "../framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;

bqPhysics::bqPhysics()
{
}

bqPhysics::~bqPhysics()
{
}

void bqPhysics::AddRigidBodyArray(bqArray<bqRigidBody*>* arr)
{
	BQ_ASSERT_ST(arr);
	m_array.push_back(arr);
}

void bqPhysics::RemoveAllRigidBodyArrays()
{
	m_array.clear();
}

void bqPhysics::Update(float dt)
{
	for (size_t i1 = 0; i1 < m_array.m_size; ++i1)
	{
		auto arr = m_array.m_data[i1];
		for (size_t i2 = 0; i2 < arr->m_size; ++i2)
		{
			auto body = arr->m_data[i2];
			if (body->m_mass > 0.f)
			{
				body->m_motionState.m_position.x += body->m_motionState.m_linearVelocity.x * dt;
				body->m_motionState.m_position.y += body->m_motionState.m_linearVelocity.y * dt;
				body->m_motionState.m_position.z += body->m_motionState.m_linearVelocity.z * dt;
				body->m_motionState.m_matrix.m_data[3].x = body->m_motionState.m_position.x;
				body->m_motionState.m_matrix.m_data[3].y = body->m_motionState.m_position.y;
				body->m_motionState.m_matrix.m_data[3].z = body->m_motionState.m_position.z;
			}
		}
	}

}

bqPhysicsShapeSphere* bqPhysics::CreateShapeSphere(float radius)
{
	bqPhysicsShapeSphere* newShape = new bqPhysicsShapeSphere();
	newShape->m_raduis = radius;
	return newShape;
}

bqRigidBody* bqPhysics::CreateRigidBody(bqPhysicsShape* sh, float mass, bqMotionState* ms)
{
	bqRigidBody* body = new bqRigidBody(sh, mass);
	if (ms)
	{
		body->m_motionState = *ms;
	}
	return body;
}

bqGravityObject* bqPhysics::CreateGravityObject(bqReal radius, const bqVec4& position)
{
	BQ_ASSERT_ST(radius>0.0);
	if (radius <= 0.0)
		radius = 1.0;
	bqGravityObject* go = new bqGravityObject();
	go->m_position = position;
	go->m_radius = radius;
	return go;
}

void bqPhysics::AddGravityObject(bqGravityObject* go)
{
	BQ_ASSERT_ST(go);

	if(go)
		m_gravityObjects.push_back(go);
}

void bqPhysics::RemoveAllGravityObject()
{
	m_gravityObjects.clear();
}

#endif
