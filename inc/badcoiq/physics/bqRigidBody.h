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
#ifndef __BQRIGIDBODY_H__
#define __BQRIGIDBODY_H__

struct bqRigidBodyTransform
{
	// есть сомнения что это должно быть здесь
	// как устанавливать позицию? наверное через метод.
	// но однозначно позиция и матрица должны быть доступны для чтения
	// 
	bqVec4 m_position;
	bqMat4 m_matrix;

	void Reset()
	{
		m_position.Set(0.f);
		m_matrix.Identity();
	}

	void UpdateMatrix()
	{
		m_matrix.m_data[3].x = m_position.x;
		m_matrix.m_data[3].y = m_position.y;
		m_matrix.m_data[3].z = m_position.z;
	}
};

struct bqRigidBodyMotionState
{
	// линейная скорость. вектр указывающий куда переместится тело.
	// может перемещаться по дуге, если действует какая-то ещё сила
	bqVec3f m_linearVelocity;

	// угловая скорость. хз пока. нужно ли использовать кватернион?
	bqVec3f m_angularVelocity;

	// вес и масса не одно и тоже
	// вес - изменяющееся значение.
	// например чем больше скорость тем больше вес
	float m_weight = 0.f;

	void Reset()
	{
		m_linearVelocity.Set(0.f);
		m_angularVelocity.Set(0.f);
		m_weight = 0.f;
	}
};

struct bqPhysicsMaterial
{
	float m_friction = 0.5f;
	float m_bounce = 0.5f;
};

// При проходе по телам, текущее тело получает силы от других тел
// body получает силы от других тел
struct bqRigidBodyContact
{
	bqRigidBody* m_otherBody = 0;
	bqVec3f m_normal;
	bqVec3f m_point;
	float m_penetration = 0.f;
	bqRigidBodyMotionState m_motionState;
};

class bqRigidBody : public bqUserData
{
	friend class bqPhysics;

	uint32_t m_maxContacts = 3;
	bqArray<bqRigidBodyContact> m_contacts;
public:
	bqRigidBody(bqPhysicsShape*, float mass, uint32_t maxContacts);
	virtual ~bqRigidBody();
	BQ_PLACEMENT_ALLOCATOR(bqRigidBody);

	const bqArray<bqRigidBodyContact>& GetContacts() { return m_contacts; }

	bqRigidBodyMotionState m_motionState;
	bqRigidBodyTransform m_transformation;

	// если масса равна нулю то тело не будет двигаться
	float m_mass = 0.f;

	bqPhysicsShape* m_shape = 0;

	bqPhysicsMaterial m_material;

	enum class BoundingVolumeType
	{
		Sphere,
		AABB
	}m_boundingVolumeType = BoundingVolumeType::Sphere;
	// здесь, BoundingVolume должен быть умноженным на матрицу
	// то есть должно быть повёрнуто и отмасштабировано
	// а так же позиция относительно центра мира
	bqAabb m_aabb;
	float m_radius = 0.f;
	void UpdateBoundingVolume();
};

#endif


