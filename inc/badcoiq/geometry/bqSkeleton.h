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
#ifndef __BQ_SKELETON_H__
#define __BQ_SKELETON_H__

#ifdef BQ_WITH_MESH

#include "badcoiq/containers/bqArray.h"
#include "badcoiq/math/bqMath.h"

// Трансформация джоинтов происходит изменяя эти значения.
struct bqJointTransformationBase
{
	bqVec4 m_position;
	bqVec4 m_scale = bqVec4(1.f, 1.f, 1.f, 0.f);
	bqQuaternion m_rotation;
};

// Надо будет вызвать CalculateMatrix() после изменения.
struct bqJointTransformation
{
	bqJointTransformationBase m_base;
	
	bqJointTransformationBase m_add;
	bool m_useAdd = false;

	bqMat4 m_matrix;

	void CalculateMatrix();
};

// В некоторых случаях нужна только базовая информация о джоинте.
struct bqJointBase
{
	// джоинты должны распологаться в массиве, в правильном порядке, от корня к листьям.
	// не надо использовать список или дерево.
	// если у джоинта нет родителя, то индекс -1
	int32_t m_parentIndex = -1;

	// Максимальная длина имени которое я видел 55 символов
	// Надеюсь 100 хватит
	char m_name[101];
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
	BQ_PLACEMENT_ALLOCATOR(bqSkeleton);

	bqJoint* AddJoint(const bqQuaternion& rotation, const bqVec4& position, const bqVec4& scale,
		const char* name, int32_t parentIndex);

	bqArray<bqJoint>& GetJoints() { return m_joints; }
	uint32_t GetJointNum() { return (uint32_t)m_joints.m_size; }
	bqJoint* GetJoint(uint32_t i) { return &m_joints.m_data[i]; }
	bqJoint* GetJoint(const char* name);

	void CalculateBind();
	void Update();
	bqSkeleton* Duplicate();

	// Некоторые 3D редакоторы поворачивают модель на 90 градусов.
	// Проще умножить на матрицу чем исправлять при загрузке.
	// m_preRotation.SetRotation(slQuaternion(PI * 0.5f, 0.f, 0.f));
	bqMat4 m_preRotation;
};

// По сути - поза
// Массив хранит данные описывающие позицию джоинтов.
struct bqSkeletonAnimationFrame
{
	bqArray<bqJointTransformationBase> m_transformations;
};

// Содержит массив имён джоинтов которые будут анимироваться.
//      Почему имён а не указателей? 
//      Потому что объект этого класса он один для всех.
//      Скелеты у каждого объекта свой, так как все могут иметь свою уникальную позу.
//      А для хранения анимации достаточно одного объекта.
//      Для воспроизведения анимации нужно будет добавить ещё один класс
//       который будет на основе имён делать поиск указателей на конкретные джоинты.
//       
// И массив с фреймами.
// Каждый фрейм должен содержать массив трансформаций. 
//     Трансформации на каждый джоинт. Размер этих массивов должен быть
//     равен размеру массива с именами джоинтов.
// 
// Всё должно быть удобным для использования в программе. Удобно использовать
//     в программе, но не удобно использовать в загрузчике модели. Поэтому
//     нужно будет добавить методы для удобного добавления джоинтов и фреймов.
class bqSkeletonAnimation
{
	bqArray<bqJointBase> m_joints;
	bqArray<bqSkeletonAnimationFrame*> m_frames; // "кадры"
public:
	// При создании объекта надо указать количество джоинтов и количество фреймов.
	// Должна произойти инициализация чтобы потом не возникли ошибки.
	// Инициализация такая: заполнить массив с джоинтами, и фреймы со всеми массивами.
	bqSkeletonAnimation(uint32_t jtNum, uint32_t frNum, const char* name);
	~bqSkeletonAnimation();
	BQ_PLACEMENT_ALLOCATOR(bqSkeletonAnimation);

	bqJointBase* GetJoint(size_t i) { return &m_joints.m_data[i]; }
	void SetJoint(bqJointBase* j, size_t i) { m_joints.m_data[i] = *j; }

	size_t GetJointsNumber() { return m_joints.m_size; }

	bqSkeletonAnimationFrame* GetFrame(size_t i) { return m_frames.m_data[i]; }
	size_t GetFramesNumber() { return m_frames.m_size; }

	void SetTransformation(uint32_t joint, uint32_t frame, const bqJointTransformationBase&);

	char m_name[100];
	float m_fps = 30.f;
};

struct bqSkeletonAnimationObjectJointData
{
	bqJoint* m_joint = 0;
	uint32_t m_flags = 0;

	enum
	{
		flag_skipTransform = 0x1
	};
};

// Для проигрывания анимации надо получить указатели на джоинты
class bqSkeletonAnimationObject
{
	// можно дать только определённые джоинты
	bqArray<bqSkeletonAnimationObjectJointData> m_joints;
	bqStringA m_name;
	bqSkeletonAnimation* m_animation = 0;
	//bqSkeleton* m_skeleton = 0; // пока лишнее
	float m_frameCurr = 0.f;
	float m_frameBegin = 0.f;
	float m_frameEnd = 0.f;
	float m_fps = 30.f;
public:
	bqSkeletonAnimationObject();

	// Init, SetRegion
	bqSkeletonAnimationObject(bqSkeletonAnimation*, bqSkeleton*, const char* name, float b, float e);
	// Init, SetRegion SetFPS
	bqSkeletonAnimationObject(bqSkeletonAnimation*, bqSkeleton*, const char* name, float b, float e, float fps);
	
	~bqSkeletonAnimationObject();
	BQ_PLACEMENT_ALLOCATOR(bqSkeletonAnimationObject);

	void Init(bqSkeletonAnimation*, bqSkeleton*, const char* name);
	const bqStringA& GetName() { return m_name; }
	void Animate(float dt);
	void AnimateInterpolate(float dt);
	void SetFPS(float);
	float GetFPS() { return m_fps; }
	void SetRegion(float begin, float end);

	void AddJoint(bqJoint*);

	bqSkeletonAnimationObjectJointData& GetJointData(uint32_t i) { return m_joints[i]; }
	uint32_t GetJointDataNum() { return m_joints.size(); }
	bqSkeletonAnimationObjectJointData* GetJointData(const char*);
};

class bqSkeletalAnimationObjectMany
{
public:
	bqSkeletalAnimationObjectMany();
	~bqSkeletalAnimationObjectMany();
};


#endif
#endif

