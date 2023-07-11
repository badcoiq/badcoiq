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
#ifndef __BQ_AABB_H__
#define __BQ_AABB_H__

#include "badcoiq/math/bqMath.h"

// Axis-Aligned Bounding Box
class bqAabb
{
public:
	bqAabb()
		:
		m_min(bqVec4fFltMax),
		m_max(bqVec4fFltMaxNeg)
	{}

	bqAabb(const bqVec4& min, const bqVec4& max) :
		m_min(min),
		m_max(max)
	{}

	/*
	Transforming Axis-Aligned Bounding Boxes
	by Jim Arvo
	from "Graphics Gems", Academic Press, 1990
*/
	void Transform(bqAabb* original, bqMat4* matrix, bqVec4* position)
	{
		bqReal  a, b;
		bqReal  Amin[3], Amax[3];
		bqReal  Bmin[3], Bmax[3];
		int32_t    i, j;

		/*Copy box A into a min array and a max array for easy reference.*/
		Amin[0] = original->m_min.x;  Amax[0] = original->m_max.x;
		Amin[1] = original->m_min.y;  Amax[1] = original->m_max.y;
		Amin[2] = original->m_min.z;  Amax[2] = original->m_max.z;

		/* Take care of translation by beginning at T. */
		Bmin[0] = Bmax[0] = position->x;
		Bmin[1] = Bmax[1] = position->y;
		Bmin[2] = Bmax[2] = position->z;

		/* Now find the extreme points by considering the product of the */
		/* min and max with each component of M.  */
		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 3; j++)
			{
				// mimi: I swapped j and i
				a = (matrix->m_data[j][i] * Amin[j]);
				b = (matrix->m_data[j][i] * Amax[j]);
				if (a < b)
				{
					Bmin[i] += a;
					Bmax[i] += b;
				}
				else
				{
					Bmin[i] += b;
					Bmax[i] += a;
				}
			}
		}
		/* Copy the result into the new box. */
		m_min.x = Bmin[0];  m_max.x = Bmax[0];
		m_min.y = Bmin[1];  m_max.y = Bmax[1];
		m_min.z = Bmin[2];  m_max.z = Bmax[2];
	}

	void Add(const bqVec4& point)
	{
		if (point.x < m_min.x) m_min.x = point.x;
		if (point.y < m_min.y) m_min.y = point.y;
		if (point.z < m_min.z) m_min.z = point.z;

		if (point.x > m_max.x) m_max.x = point.x;
		if (point.y > m_max.y) m_max.y = point.y;
		if (point.z > m_max.z) m_max.z = point.z;
	}

	void Add(const bqVec3& point)
	{
		if (point.x < m_min.x) m_min.x = point.x;
		if (point.y < m_min.y) m_min.y = point.y;
		if (point.z < m_min.z) m_min.z = point.z;

		if (point.x > m_max.x) m_max.x = point.x;
		if (point.y > m_max.y) m_max.y = point.y;
		if (point.z > m_max.z) m_max.z = point.z;
	}

	void Add(const bqVec4f& point)
	{
		if (point.x < m_min.x) m_min.x = point.x;
		if (point.y < m_min.y) m_min.y = point.y;
		if (point.z < m_min.z) m_min.z = point.z;

		if (point.x > m_max.x) m_max.x = point.x;
		if (point.y > m_max.y) m_max.y = point.y;
		if (point.z > m_max.z) m_max.z = point.z;
	}

	void Add(const bqVec3f& point)
	{
		if (point.x < m_min.x) m_min.x = point.x;
		if (point.y < m_min.y) m_min.y = point.y;
		if (point.z < m_min.z) m_min.z = point.z;

		if (point.x > m_max.x) m_max.x = point.x;
		if (point.y > m_max.y) m_max.y = point.y;
		if (point.z > m_max.z) m_max.z = point.z;
	}

	void Add(const bqAabb& box)
	{
		if (box.m_min.x < m_min.x) m_min.x = box.m_min.x;
		if (box.m_min.y < m_min.y) m_min.y = box.m_min.y;
		if (box.m_min.z < m_min.z) m_min.z = box.m_min.z;

		if (box.m_max.x > m_max.x) m_max.x = box.m_max.x;
		if (box.m_max.y > m_max.y) m_max.y = box.m_max.y;
		if (box.m_max.z > m_max.z) m_max.z = box.m_max.z;
	}

	bool RayTest(const bqRay& r)
	{
		bqReal t1 = (m_min.x - r.m_origin.x) * r.m_invDir.x;
		bqReal t2 = (m_max.x - r.m_origin.x) * r.m_invDir.x;
		bqReal t3 = (m_min.y - r.m_origin.y) * r.m_invDir.y;
		bqReal t4 = (m_max.y - r.m_origin.y) * r.m_invDir.y;
		bqReal t5 = (m_min.z - r.m_origin.z) * r.m_invDir.z;
		bqReal t6 = (m_max.z - r.m_origin.z) * r.m_invDir.z;

		bqReal tmin = bqMax(bqMax(bqMin(t1, t2), bqMin(t3, t4)), bqMin(t5, t6));
		bqReal tmax = bqMin(bqMin(bqMax(t1, t2), bqMax(t3, t4)), bqMax(t5, t6));

		if (tmax < 0 || tmin > tmax) return false;

		return true;
	}

	void Center(bqVec4& v) const
	{
		v = bqVec4(m_min + m_max);
		v *= 0.5f;
	}

	bqReal Radius()
	{
		return bqMath::Distance(m_min, m_max) * 0.5;
	}

	void Extent(bqVec4& v)
	{
		v = bqVec4(m_max - m_min);
	}

	bool IsEmpty() const
	{
		return ((m_min == bqVec4fFltMax) && (m_max == bqVec4fFltMaxNeg))
			|| (m_min == m_max);
	}

	void Reset()
	{
		m_min = bqVec4fFltMax;
		m_max = bqVec4fFltMaxNeg;
	}

	bqVec4 m_min;
	bqVec4 m_max;
};


#endif

