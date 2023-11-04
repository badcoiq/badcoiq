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

#include "badcoiq.h"

#include "badcoiq/geometry/bqSkeleton.h"

void bqJointTransformation::CalculateMatrix()
{
	bqMat4 sM;
	sM.m_data[0].x = m_scale.x;
	sM.m_data[1].y = m_scale.y;
	sM.m_data[2].z = m_scale.z;

	m_matrix.Identity();
	m_matrix.SetRotation(m_rotation);

	m_matrix = m_matrix * sM;

	m_matrix.m_data[3].x = m_position.x;
	m_matrix.m_data[3].y = m_position.y;
	m_matrix.m_data[3].z = m_position.z;
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

	//joint.m_matrixBind = m;
	//joint.m_matrixBind.m_data[3].x = position.x;
	//joint.m_matrixBind.m_data[3].y = position.y;
	//joint.m_matrixBind.m_data[3].z = position.z;

	//joint.m_transformation.m_matrix = m;
	joint.m_data.m_transformation.m_position = position;
	joint.m_data.m_transformation.m_rotation = rotation;
	joint.m_data.m_transformation.m_scale = scale;
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
