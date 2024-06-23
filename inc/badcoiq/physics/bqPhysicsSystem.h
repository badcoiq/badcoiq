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
	bqVec4 m_position;
};

class bqPhysics
{
	bqArray<bqArray<bqRigidBody*>*> m_array;
	bqArray<bqGravityObject*> m_gravityObjects;
public:
	bqPhysics();
	~bqPhysics();
	BQ_PLACEMENT_ALLOCATOR(bqPhysics);

	void AddRigidBodyArray(bqArray<bqRigidBody*>*);
	void RemoveAllRigidBodyArrays();

	bqGravityObject* CreateGravityObject(bqReal radius, const bqVec4& position);
	void AddGravityObject(bqGravityObject*);
	void RemoveAllGravityObject();

	void Update(float dt);

	bqPhysicsShapeSphere* CreateShapeSphere(float radius);
	bqRigidBody* CreateRigidBody(bqPhysicsShape*, float mass, const bqMotionState&);
};

#endif
