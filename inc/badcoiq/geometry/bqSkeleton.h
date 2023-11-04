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
#ifndef __BQ_SKELETON_H__
#define __BQ_SKELETON_H__

#include "badcoiq/containers/bqArray.h"
#include "badcoiq/math/bqVector.h"
#include "badcoiq/math/bqMatrix.h"
#include "badcoiq/math/bqQuaternion.h"

// Трансформация джоинтов происходит изменяя эти значения.
// Надо будет вызвать CalculateMatrix() после изменения.
struct bqJointTransformation
{
	bqVec4 m_position;
	bqQuaternion m_rotation;
	bqVec4 m_scale = bqVec4(1.f, 1.f, 1.f, 0.f);
	bqMat4 m_matrix;

	void CalculateMatrix();
};

// В некоторых случаях нужна базовая информация о джоинте.
struct bqJointBase
{
	// джоинты должны распологаться в массиве, в правильном порядке, от корня к листьям.
	// не надо использовать список или дерево.
	// если у джоинта нет родителя, то индекс -1
	int32_t m_parentIndex = -1;

	char m_name[50];
};

struct bqJointData
{
	bqJointTransformation m_transformation;

	bqMat4 m_matrixBindInverse; // та матрица которая переместит вершины на нужную позицию
	bqMat4 m_matrixFinal;     // эта матрица пойдёт в GPU
};

// Джоинт состоит из базовой информации и данных с матрицами и т.д.
// Я решил разбить джоинт на структуры. Трансформации отдельно, базовая информация отдельно.
// m_data имеет m_transformation которых можно изменять, и матрицы которые используются внутри движка
struct bqJoint
{
	bqJointBase m_base;
	bqJointData m_data;
};

// Иерархия костей\джоинтов.
// Определяет так называемую bind позу.
// Так-же используется для анимации. Так как для анимации надо хранить
//     трансформацию джоинтов, для хранения можно использовать копию
//     скелетона. Для создания копии нужно вызвать метод Duplicate()
class bqSkeleton
{
	bqArray<bqJoint> m_joints;
public:
	bqSkeleton();
	~bqSkeleton();

	bqJoint* AddJoint(const bqQuaternion& rotation, const bqVec4& position, const bqVec4& scale,
		const char* name, int32_t parentIndex);

	bqArray<bqJoint>& GetJoints() { return m_joints; }

	bqJoint* GetJoint(const char* name);

	void CalculateBind();
	void Update();
	bqSkeleton* Duplicate();

	// Некоторые 3D редакоторы поворачивают модель на 90 градусов.
	// Проще умножить на матрицу чем исправлять при загрузке.
	// m_preRotation.SetRotation(slQuaternion(PI * 0.5f, 0.f, 0.f));
	bqMat4 m_preRotation;
};

#endif

