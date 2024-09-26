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
#ifdef BQ_WITH_MESH
#include "badcoiq/geometry/bqRay.h"
#include "badcoiq/geometry/bqTriangle.h"

bqTriangle::bqTriangle() {}
bqTriangle::bqTriangle(const bqVec3& _v1, const bqVec3& _v2, const bqVec3& _v3)
	:
	v1(_v1.x, _v1.y, _v1.z),
	v2(_v2.x, _v2.y, _v2.z),
	v3(_v3.x, _v3.y, _v3.z)
{
}

bqTriangle::bqTriangle(const bqVec3f& _v1, const bqVec3f& _v2, const bqVec3f& _v3)
	:
	v1(_v1),
	v2(_v2),
	v3(_v3)
{
}

void bqTriangle::Update()
{
	e1 = bqVec3(v2.x - v1.x,
		v2.y - v1.y,
		v2.z - v1.z);
	e2 = bqVec3(v3.x - v1.x,
		v3.y - v1.y,
		v3.z - v1.z);
	e1.Cross(e2, normal);
}

void bqTriangle::Center(bqVec3& out)
{
	out = (v1 + v2 + v3) * 0.3333333;
}

bool bqTriangle::SphereIntersect(bqReal radius, const bqVec3& origin, bqReal& T, bqVec3& ip)
{
	bqVec3 A = v1 - origin;
	bqVec3 B = v2 - origin;
	bqVec3 C = v3 - origin;

	bqReal rr = radius * radius;

	bqVec3 V;
	bqMath::Cross(B - A, C - A, V);

	bqReal d = A.Dot(V);
	bqReal e = V.Dot();

	if ((d * d) > (rr * e))
		return false;

	bqReal aa = A.Dot();
	bqReal ab = A.Dot(B);
	bqReal ac = A.Dot(C);
	bqReal bb = B.Dot();
	bqReal bc = B.Dot(C);
	bqReal cc = C.Dot();

	if ((aa > rr) && (ab > aa) && (ac > aa))
		return false;
	if ((bb > rr) && (ab > bb) && (bc > bb))
		return false;
	if ((cc > rr) && (ac > cc) && (bc > cc))
		return false;

	bqVec3 AB = B - A;
	bqVec3 BC = C - B;
	bqVec3 CA = A - C;
	bqReal d1 = ab - aa;
	bqReal d2 = bc - bb;
	bqReal d3 = ac - cc;
	bqReal e1 = AB.Dot();
	bqReal e2 = BC.Dot();
	bqReal e3 = CA.Dot();
	bqVec3 Q1 = A * bqVec3(e1) - bqVec3(d1) * AB;
	bqVec3 Q2 = B * bqVec3(e2) - bqVec3(d2) * BC;
	bqVec3 Q3 = C * bqVec3(e3) - bqVec3(d3) * CA;
	bqVec3 QC = C * e1 - Q1;
	bqVec3 QA = A * e2 - Q2;
	bqVec3 QB = B * e3 - Q3;

	if ((Q1.Dot() > (rr * e1 * e1)) && (Q1.Dot(QC) >= 0.0)) return false;
	if ((Q2.Dot() > (rr * e2 * e2)) && (Q2.Dot(QA) >= 0.0)) return false;
	if ((Q3.Dot() > (rr * e3 * e3)) && (Q3.Dot(QB) >= 0.0)) return false;

	T = std::sqrt(d * d / e) - radius;

	// нужно ли инвертировать нормаль?
	ip = origin + (normal * T);

	return true;
}

bool bqTriangle::RayIntersect_MT(const bqRay& ray, bool withBackFace, bqReal& T, bqReal& U, bqReal& V, bqReal& W)
{
	bqVec3  pvec;
	bqMath::Cross(ray.m_direction, e2, pvec);
	bqReal det = e1.Dot(pvec);

	if (withBackFace)
	{
		if (std::fabs(det) < bqEpsilon)
			return false;
	}
	else
	{
		if (det < bqEpsilon && det > -bqEpsilon)
			return false;
	}

	bqVec3 tvec(
		ray.m_origin.x - v1.x,
		ray.m_origin.y - v1.y,
		ray.m_origin.z - v1.z);

	bqReal inv_det = 1.f / det;
	U = tvec.Dot(pvec) * inv_det;

	if (U < 0.f || U > 1.f)
		return false;

	bqVec3  qvec;
	bqMath::Cross(tvec, e1, qvec);

	V = ray.m_direction.Dot(qvec) * inv_det;

	if (V < 0.f || U + V > 1.f)
		return false;

	T = e2.Dot(qvec) * inv_det;

	if (T < bqEpsilon) return false;

	W = 1.f - U - V;
	return true;
}

bool bqTriangle::RayIntersect_Watertight(const bqRay& ray, bool withBackFace, bqReal& T, bqReal& U, bqReal& V, bqReal& W)
{
	const auto A = v2 - ray.m_origin;
	const auto B = v3 - ray.m_origin;
	const auto C = v1 - ray.m_origin;

	const bqReal Ax = A[ray.m_kx] - (ray.m_Sx * A[ray.m_kz]);
	const bqReal Ay = A[ray.m_ky] - (ray.m_Sy * A[ray.m_kz]);
	const bqReal Bx = B[ray.m_kx] - (ray.m_Sx * B[ray.m_kz]);
	const bqReal By = B[ray.m_ky] - (ray.m_Sy * B[ray.m_kz]);
	const bqReal Cx = C[ray.m_kx] - (ray.m_Sx * C[ray.m_kz]);
	const bqReal Cy = C[ray.m_ky] - (ray.m_Sy * C[ray.m_kz]);

	U = (Cx * By) - (Cy * Bx);
	V = (Ax * Cy) - (Ay * Cx);
	W = (Bx * Ay) - (By * Ax);

	if (U == 0.f || V == 0.f || W == 0.f)
	{
		double CxBy = (double)Cx * (double)By;
		double CyBx = (double)Cy * (double)Bx;
		U = (float)(CxBy - CyBx);

		double AxCy = (double)Ax * (double)Cy;
		double AyCx = (double)Ay * (double)Cx;
		V = (float)(AxCy - AyCx);

		double BxAy = (double)Bx * (double)Ay;
		double ByAx = (double)By * (double)Ax;
		W = (float)(BxAy - ByAx);
	}

	if (withBackFace)
	{
		if ((U < 0.f || V < 0.f || W < 0.f) &&
			(U > 0.f || V > 0.f || W > 0.f))
			return false;
	}
	else
	{
		if (U < 0.f || V < 0.f || W < 0.f)
			return false;
	}

	bqReal det = U + V + W;

	if (det == 0.f)
		return false;

	const bqReal Az = ray.m_Sz * A[ray.m_kz];
	const bqReal Bz = ray.m_Sz * B[ray.m_kz];
	const bqReal Cz = ray.m_Sz * C[ray.m_kz];
	const bqReal Ts = (U * Az) + (V * Bz) + (W * Cz);

	if (!withBackFace) // CULL
	{
		if (Ts < 0.f || Ts > bqInfinity * det)
			return false;
	}
	else
	{
		if (det < 0.f && (Ts >= 0.f || Ts < bqInfinity * det))
			return false;
		else if (det > 0.f && (Ts <= 0.f || Ts > bqInfinity * det))
			return false;
	}

	const bqReal invDet = 1.f / det;
	U = U * invDet;
	V = V * invDet;
	W = W * invDet;
	T = Ts * invDet;
	if (T < bqEpsilon)
		return false;
	return true;
}

#endif
