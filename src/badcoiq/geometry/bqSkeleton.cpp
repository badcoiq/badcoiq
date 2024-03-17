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

#include "badcoiq/geometry/bqSkeleton.h"

void bqJointTransformation::CalculateMatrix()
{
	bqMat4 sM;
	sM.m_data[0].x = m_base.m_scale.x;
	sM.m_data[1].y = m_base.m_scale.y;
	sM.m_data[2].z = m_base.m_scale.z;

	m_matrix.Identity();
	m_matrix.SetRotation(m_base.m_rotation);

	m_matrix = m_matrix * sM;

	m_matrix.m_data[3].x = m_base.m_position.x;
	m_matrix.m_data[3].y = m_base.m_position.y;
	m_matrix.m_data[3].z = m_base.m_position.z;
}

bqSkeleton::bqSkeleton()
{
}

bqSkeleton::~bqSkeleton()
{
}

bqJoint* bqSkeleton::AddJoint(const bqQuaternion& rotation, const bqVec4& position, const bqVec4& scale,
	const char* name, int32_t parentIndex)
{
	BQ_ASSERT_ST(name);
	BQ_ASSERT_ST(m_joints.size() < 255);

	bqJoint joint;
	memset(&joint.m_base.m_name, 0, sizeof(joint.m_base.m_name));

	joint.m_data.m_transformation.m_base.m_position = position;
	joint.m_data.m_transformation.m_base.m_rotation = rotation;
	joint.m_data.m_transformation.m_base.m_scale = scale;
	joint.m_data.m_transformation.CalculateMatrix();

	joint.m_base.m_parentIndex = parentIndex;

	strcpy_s(joint.m_base.m_name, 50, name);
	joint.m_base.m_name[49] = 0;

	m_joints.push_back(joint);

	bqJoint* ret = &m_joints[m_joints.m_size - 1];
	return ret;
}

// Может быть правильнее назвать CalculateBindInverse?
void bqSkeleton::CalculateBind()
{
	if (m_joints.m_size)
	{
		for (size_t i = 0; i < m_joints.m_size; ++i)
		{
			bqJoint* joint = &m_joints.m_data[i];

			joint->m_data.m_matrixBindInverse = joint->m_data.m_transformation.m_matrix;
			joint->m_data.m_matrixBindInverse.Invert();

			if (joint->m_base.m_parentIndex != -1)
			{
				joint->m_data.m_matrixBindInverse = joint->m_data.m_matrixBindInverse *
					m_joints[joint->m_base.m_parentIndex].m_data.m_matrixBindInverse;
			}
		}
	}
}

bqJoint* bqSkeleton::GetJoint(const char* name)
{
	BQ_ASSERT_ST(name);
	for (size_t i = 0; i < m_joints.m_size; ++i)
	{
		if (strcmp(m_joints.m_data[i].m_base.m_name, name) == 0)
			return &m_joints.m_data[i];
	}
	return 0;
}

void bqSkeleton::Update()
{
	if (m_joints.m_size)
	{
		for (size_t i = 0; i < m_joints.m_size; ++i)
		{
			bqJoint* joint = &m_joints.m_data[i];

			joint->m_data.m_matrixFinal =
				joint->m_data.m_transformation.m_matrix;

			if (joint->m_base.m_parentIndex == -1)
			{
				joint->m_data.m_matrixFinal = m_preRotation * joint->m_data.m_matrixFinal;
			}

			if (joint->m_base.m_parentIndex != -1)
				joint->m_data.m_matrixFinal
				= m_joints[joint->m_base.m_parentIndex].m_data.m_matrixFinal
				* joint->m_data.m_matrixFinal;

		}
		for (size_t i = 0; i < m_joints.m_size; ++i)
		{
			bqJoint* joint = &m_joints.m_data[i];
			joint->m_data.m_matrixFinal = joint->m_data.m_matrixFinal
				* joint->m_data.m_matrixBindInverse;
		}
	}
}

bqSkeleton* bqSkeleton::Duplicate()
{
	bqSkeleton* newSkeleton = new bqSkeleton;

	newSkeleton->m_joints.reserve(m_joints.m_size);
	for (size_t i = 0; i < m_joints.m_size; ++i)
	{
		newSkeleton->m_joints.push_back(m_joints.m_data[i]);
	}
	newSkeleton->m_preRotation = m_preRotation;
	return newSkeleton;
}

// инициализация джоинтов и фреймов
bqSkeletonAnimation::bqSkeletonAnimation(uint32_t jtNum, uint32_t frNum, const char* name)
{
	BQ_ASSERT_ST(jtNum);
	BQ_ASSERT_ST(frNum);

	strcpy_s(m_name, sizeof(m_name), name);

	m_joints.reserve(jtNum);
	for (uint32_t i = 0; i < jtNum; ++i)
	{
		bqJointBase jb;
		strcpy_s(jb.m_name, sizeof(jb.m_name), "-");
		m_joints.push_back(jb);
	}

	m_frames.reserve(frNum);
	for (uint32_t i = 0; i < frNum; ++i)
	{
		bqSkeletonAnimationFrame* newFrame = bqCreate<bqSkeletonAnimationFrame>();
		m_frames.push_back(newFrame);
		newFrame->m_transformations.reserve(jtNum);
		for (uint32_t o = 0; o < jtNum; ++o)
		{
			newFrame->m_transformations.push_back(bqJointTransformationBase());
		}
	}
}

bqSkeletonAnimation::~bqSkeletonAnimation()
{
	for (size_t i = 0; i < m_frames.m_size; ++i)
	{
		bqDestroy(m_frames.m_data[i]);
	}
}

void bqSkeletonAnimation::SetTransformation(uint32_t joint, uint32_t frame, const bqJointTransformationBase& jt)
{
	BQ_ASSERT_ST(joint < m_joints.m_size);
	BQ_ASSERT_ST(frame < m_frames.m_size);

	bqSkeletonAnimationFrame* F = m_frames.m_data[frame];
	F->m_transformations.m_data[joint] = jt;
}

bqSkeletonAnimationObject::bqSkeletonAnimationObject()
{
}

bqSkeletonAnimationObject::~bqSkeletonAnimationObject()
{
}

void bqSkeletonAnimationObject::Init(bqSkeletonAnimation* a, bqSkeleton* s)
{
	BQ_ASSERT_ST(a);
	BQ_ASSERT_ST(s);

	m_joints.clear();
//	m_skeleton = s;
	m_animation = a;
	m_frameMax = (float)a->GetFramesNumber();
	m_frameCurr = 0.f;

	for (size_t i = 0, sz = a->GetJointsNumber(); i < sz; ++i)
	{
		auto J = a->GetJoint(i);
		bqJoint* joint = s->GetJoint(J->m_name);
		if (joint)
		{
			m_joints.push_back(joint);
		}
	}
}

void bqSkeletonAnimationObject::Animate(float dt)
{
	uint32_t currFrameIndex = (uint32_t)floor(m_frameCurr);

	bqSkeletonAnimationFrame* frame = m_animation->GetFrame(currFrameIndex);
	for (size_t i = 0; i < frame->m_transformations.m_size; ++i)
	{
		m_joints.m_data[i]->m_data.m_transformation.m_base.m_position = frame->m_transformations.m_data[i].m_position;
		m_joints.m_data[i]->m_data.m_transformation.m_base.m_rotation = frame->m_transformations.m_data[i].m_rotation;
		m_joints.m_data[i]->m_data.m_transformation.m_base.m_scale = frame->m_transformations.m_data[i].m_scale;
		m_joints.m_data[i]->m_data.m_transformation.CalculateMatrix();
	}

	m_frameCurr += (dt * m_animation->m_fps);

	if (m_frameCurr >= m_frameMax)
		m_frameCurr = 0.f;
}

void bqSkeletonAnimationObject::AnimateInterpolate(float dt)
{
	uint32_t currFrameIndex = (uint32_t)floor(m_frameCurr);
	float t = m_frameCurr - (float)currFrameIndex;

	bqSkeletonAnimationFrame* frame = m_animation->GetFrame(currFrameIndex);

	uint32_t prevFrameIndex = currFrameIndex;
	if (prevFrameIndex == 0)
		prevFrameIndex = (uint32_t)floor(m_frameMax) - 1;
	else
		--prevFrameIndex;
	bqSkeletonAnimationFrame* prevFrame = m_animation->GetFrame(prevFrameIndex);

	for (size_t i = 0; i < frame->m_transformations.m_size; ++i)
	{
		bqVec4 P;
		bqMath::Lerp1(prevFrame->m_transformations.m_data[i].m_position,
			frame->m_transformations.m_data[i].m_position, t, P);

		m_joints.m_data[i]->m_data.m_transformation.m_base.m_position = P;

		bqQuaternion Q;
		bqMath::Slerp(prevFrame->m_transformations.m_data[i].m_rotation,
			frame->m_transformations.m_data[i].m_rotation,
			t, 1.f, Q);
		m_joints.m_data[i]->m_data.m_transformation.m_base.m_rotation = Q;

		bqVec4 S;
		bqMath::Lerp1(prevFrame->m_transformations.m_data[i].m_scale,
			frame->m_transformations.m_data[i].m_scale, t, S);
		m_joints.m_data[i]->m_data.m_transformation.m_base.m_scale = S;

		m_joints.m_data[i]->m_data.m_transformation.CalculateMatrix();
	}

	m_frameCurr += (dt * m_animation->m_fps);

	if (m_frameCurr >= m_frameMax)
		m_frameCurr = 0.f;
}

