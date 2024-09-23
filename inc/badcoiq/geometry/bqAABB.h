﻿/*
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

#include "badcoiq/geometry/bqRay.h"
#include "bqmdlinfo.h"

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

	/*bqAabb(const bqMDLAABB& aabb) :
		m_min(aabb.m_aabbMin[0], aabb.m_aabbMin[1], aabb.m_aabbMin[2], 0.f),
		m_max(aabb.m_aabbMax[0], aabb.m_aabbMax[1], aabb.m_aabbMax[2], 0.f)
	{}*/

	void Transform(const bqAabb& original, const bqMat4& matrix, const bqVec3& position);
	void Add(const bqVec4& point);
	void Add(const bqVec3& point);
	void Add(const bqVec4f& point);
	void Add(const bqVec3f& point);
	void Add(float32_t x, float32_t y, float32_t z);
	void Add(float64_t x, float64_t y, float64_t z);
	void Add(const bqAabb& box);
	bool RayTest(const bqRay& r) const;
	
	bool SphereIntersect(const bqVec3& p, float32_t r) const;
	bool SphereIntersect(const bqVec3f& p, float32_t r) const;
	bool SphereIntersect(const bqVec4& p, float32_t r) const;
	bool SphereIntersect(const bqVec4f& p, float32_t r) const;

	void Center(bqVec4& v) const
	{
		v = bqVec4(m_min + m_max);
		v *= 0.5f;
	}

	bqReal Radius()
	{
		m_radius = bqMath::Distance(m_min, m_max) * 0.5;
		return m_radius;
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

	bqReal m_radius = 0.f;
};


#endif

