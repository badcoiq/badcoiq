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

void _debugDraw_addCommand_drawAabb(const bqAabb& aabb, bqPhysicsDebugDraw* m_debugDraw)
{
	bqPhysicsDebugDraw::DrawCommands drawCommand;
	drawCommand.m_color = bq::ColorWhite;
	drawCommand.m_reason = m_debugDraw->m_reason;

	auto& p1 = aabb.m_min;
	auto& p2 = aabb.m_max;

	bqColor color;
	color.m_data[0] = 1.f;
	color.m_data[1] = 1.f;
	color.m_data[2] = 1.f;
	color.m_data[3] = 1.f;

	bqVec4 v1 = p1;
	bqVec4 v2 = p2;

	bqVec4 v3(p1.x, p1.y, p2.z, 1.f);
	bqVec4 v4(p2.x, p1.y, p1.z, 1.f);
	bqVec4 v5(p1.x, p2.y, p1.z, 1.f);
	bqVec4 v6(p1.x, p2.y, p2.z, 1.f);
	bqVec4 v7(p2.x, p1.y, p2.z, 1.f);
	bqVec4 v8(p2.x, p2.y, p1.z, 1.f);

	drawCommand.m_v1 = v1;
	drawCommand.m_v2 = v4;
	m_debugDraw->m_array.push_back(drawCommand);

	drawCommand.m_v1 = v5;
	drawCommand.m_v2 = v8;
	m_debugDraw->m_array.push_back(drawCommand);

	drawCommand.m_v1 = v1;
	drawCommand.m_v2 = v5;
	m_debugDraw->m_array.push_back(drawCommand);

	drawCommand.m_v1 = v4;
	drawCommand.m_v2 = v8;
	m_debugDraw->m_array.push_back(drawCommand);

	drawCommand.m_v1 = v3;
	drawCommand.m_v2 = v7;
	m_debugDraw->m_array.push_back(drawCommand);

	drawCommand.m_v1 = v6;
	drawCommand.m_v2 = v2;
	m_debugDraw->m_array.push_back(drawCommand);

	drawCommand.m_v1 = v3;
	drawCommand.m_v2 = v6;
	m_debugDraw->m_array.push_back(drawCommand);

	drawCommand.m_v1 = v7;
	drawCommand.m_v2 = v2;
	m_debugDraw->m_array.push_back(drawCommand);

	drawCommand.m_v1 = v2;
	drawCommand.m_v2 = v8;
	m_debugDraw->m_array.push_back(drawCommand);

	drawCommand.m_v1 = v4;
	drawCommand.m_v2 = v7;
	m_debugDraw->m_array.push_back(drawCommand);

	drawCommand.m_v1 = v5;
	drawCommand.m_v2 = v6;
	m_debugDraw->m_array.push_back(drawCommand);

	drawCommand.m_v1 = v1;
	drawCommand.m_v2 = v3;
	m_debugDraw->m_array.push_back(drawCommand);
}

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

// 1. Двигаем и вращаем объекты.
// 2. Определяем проникновения.
// 3. Выталкиваем.
void bqPhysics::Update(float dt)
{
	m_bodiesAfterOptimization.clear();
	for (size_t i1 = 0; i1 < m_array.m_size; ++i1)
	{
		auto arr = m_array.m_data[i1];
		for (size_t i2 = 0; i2 < arr->m_size; ++i2)
		{
			m_bodiesAfterOptimization.push_back(arr->m_data[i2]);
		}
	}

	// 1
	for (size_t i = 0; i < m_bodiesAfterOptimization.m_size; ++i)
	{
		auto body = m_bodiesAfterOptimization.m_data[i];
		body->m_contacts.clear();

		if (body->m_mass > 0.f)
		{
			body->m_transformation.m_position.x += body->m_motionState.m_linearVelocity.x * dt;
			body->m_transformation.m_position.y += body->m_motionState.m_linearVelocity.y * dt;
			body->m_transformation.m_position.z += body->m_motionState.m_linearVelocity.z * dt;
			body->m_transformation.m_matrix.m_data[3].x = body->m_transformation.m_position.x;
			body->m_transformation.m_matrix.m_data[3].y = body->m_transformation.m_position.y;
			body->m_transformation.m_matrix.m_data[3].z = body->m_transformation.m_position.z;
		}
		body->UpdateBoundingVolume();
	}

	// 2
	// Допустим, берём тело, и проверяем с другими телами.
	// Если с кем-то есть контакт, то сохраняем этот контакт.
	// Контактов может быть несколько, но не бесконечно.
	// После получения всех контактов (и всей дополнительной информации)
	// обрабатываем эти контакты. По сути это будет шагом номер 3.
	for (size_t i = 0; i < m_bodiesAfterOptimization.m_size; ++i)
	{
		auto body = m_bodiesAfterOptimization.m_data[i];
	}

	if (m_debugDraw)
	{
		m_debugDraw->m_array.clear();

		for (size_t i = 0; i < m_bodiesAfterOptimization.m_size; ++i)
		{
			auto body = m_bodiesAfterOptimization.m_data[i];

			if (m_debugDraw->m_reason & bqPhysicsDebugDraw::Reason_DrawAabb)
				_debugDraw_addCommand_drawAabb(body->m_aabb, m_debugDraw);
		}
	}
}

bqPhysicsShapeSphere* bqPhysics::CreateShapeSphere(float radius)
{
	bqPhysicsShapeSphere* newShape = new bqPhysicsShapeSphere();
	newShape->SetRadius(radius);
	return newShape;
}

bqRigidBody* bqPhysics::CreateRigidBody(bqPhysicsShape* sh, float mass, bqRigidBodyMotionState* ms)
{
	bqRigidBody* body = new bqRigidBody(sh, mass, 3);
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

void bqPhysics::SetDebugDraw(bqPhysicsDebugDraw* dd, void* data)
{
	m_debugDraw = dd;
	m_debugDrawData = data;
}

void bqPhysics::DebugDraw()
{
	if (m_debugDraw)
	{
		for (uint32_t i = 0; i < m_debugDraw->m_array.m_size; ++i)
		{
			auto cmd = m_debugDraw->m_array.m_data[i];

			m_debugDraw->DrawLine(m_debugDrawData, 
				cmd.m_reason,
				cmd.m_v1,
				cmd.m_v2,
				cmd.m_color);
		}
	}
}

#endif
