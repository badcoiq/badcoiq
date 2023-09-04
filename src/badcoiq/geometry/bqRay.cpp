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

#include "badcoiq/geometry/bqRay.h"

inline int bqRay_max_dim(const bqVec4& v)
{
	return (v.x > v.y) ? ((v.x > v.z)
		? 0 : 2) : ((v.y > v.z) ? 1 : 2);
}

void bqRay::CreateFrom2DCoords(
	const bqVec2f& coord,
	const bqVec2f& rc_sz,
	const bqMat4& VPinv,
	const bqVec2f& depthRange)
{
	// cursor ccordinates from -1 до +1
	float pt_x = (coord.x / rc_sz.x) * 2.f - 1.f;
	float pt_y = -(coord.y / rc_sz.y) * 2.f + 1.f;

	//                                           0.f - for d3d
	bqMath::Mul(VPinv, bqVec4(pt_x, pt_y, depthRange.x, 1.f), m_origin);
	bqMath::Mul(VPinv, bqVec4(pt_x, pt_y, depthRange.y, 1.f), m_end);

	m_origin.w = 1.0f / m_origin.w;
	m_origin.x *= m_origin.w;
	m_origin.y *= m_origin.w;
	m_origin.z *= m_origin.w;

	m_end.w = 1.0f / m_end.w;
	m_end.x *= m_end.w;
	m_end.y *= m_end.w;
	m_end.z *= m_end.w;

	Update();
}

void bqRay::Update()
{
	m_direction.x = m_end.x - m_origin.x;
	m_direction.y = m_end.y - m_origin.y;
	m_direction.z = m_end.z - m_origin.z;
	m_direction.w = 0.f;
	m_direction.Normalize();

	m_invDir.x = 1.f / m_direction.x;
	m_invDir.y = 1.f / m_direction.y;
	m_invDir.z = 1.f / m_direction.z;
	m_invDir.w = 1.f / m_direction.w;

	m_kz = bqRay_max_dim
	(
		bqVec4
		(
			std::abs(m_direction.x),
			std::abs(m_direction.y),
			std::abs(m_direction.z),
			1.f
		)
	);

	m_kx = m_kz + 1;
	if (m_kx == 3)
		m_kx = 0;

	m_ky = m_kx + 1;
	if (m_ky == 3)
		m_ky = 0;

	auto dir_data = m_direction.Data();
	if (dir_data[m_kz])
		std::swap(m_kx, m_ky);

	m_Sx = dir_data[m_kx] / dir_data[m_kz];
	m_Sy = dir_data[m_ky] / dir_data[m_kz];
	m_Sz = 1.f / dir_data[m_kz];
}

bqReal bqRay::DistanceToLine(const bqVec4& lineP0, const bqVec4& lineP1)
{
	bqVec4 u = m_end - m_origin;
	bqVec4 v = lineP1 - lineP0;
	bqVec4 w = m_origin - lineP0;
	u.w = 0.f;
	v.w = 0.f;
	w.w = 0.f;
	bqReal a = u.Dot();
	bqReal b = u.Dot(v);
	bqReal c = v.Dot();
	bqReal d = u.Dot(w);
	bqReal e = v.Dot(w);
	bqReal D = a * c - b * b;
	bqReal sc, tc;

	if (D < bqEpsilon)
	{
		sc = 0.f;
		tc = (b > c ? d / b : e / c);
	}
	else
	{
		sc = (b * e - c * d) / D;
		tc = (a * e - b * d) / D;
	}

	//bqVec4 dP = w + (sc * u) - (tc * v);
	bqVec4 dP;
	dP.x = w.x = (sc * u.x) - (tc * v.x);
	dP.y = w.y = (sc * u.y) - (tc * v.y);
	dP.z = w.z = (sc * u.z) - (tc * v.z);
	dP.w = 0.f;
	return std::sqrt(dP.Dot());
}

void bqRay::GetIntersectionPoint(bqReal t, bqVec4& ip)
{
	ip.x = m_origin.x + t * m_direction.x;
	ip.y = m_origin.y + t * m_direction.y;
	ip.z = m_origin.z + t * m_direction.z;
}

bool bqRay::PlaneIntersection(const bqVec4& planePoint, const bqVec4& planeNormal, bqReal& T)
{
	bqReal det = (planeNormal.x * m_direction.x) + (planeNormal.y * m_direction.y) + (planeNormal.z * m_direction.z);

	if (std::abs(det) < bqEpsilon) return false;

	bqVec4 v;
	v.x = planePoint.x - m_origin.x;
	v.y = planePoint.y - m_origin.y;
	v.z = planePoint.z - m_origin.z;

	T = (planeNormal.x * v.x) + (planeNormal.y * v.y) + (planeNormal.z * v.z);

	T /= det;
	return true;
}

