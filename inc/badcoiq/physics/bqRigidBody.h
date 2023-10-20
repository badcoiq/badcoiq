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
#ifndef __BQRIGIDBODY_H__
#define __BQRIGIDBODY_H__

// тело имеет изменяющиеся параметры
// эти параметры изменяются под действием
// сил, внешних и внутренних.
struct bqMotionState
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
};


class bqRigidBody : public bqUserData
{
public:
bqRigidBody();
virtual ~bqRigidBody();
BQ_PLACEMENT_ALLOCATOR(bqRigidBody);

bqMotionState m_motionState;

// если масса равна нулю то тело не будет двигаться
float m_mass = 0.f;
};

#endif


