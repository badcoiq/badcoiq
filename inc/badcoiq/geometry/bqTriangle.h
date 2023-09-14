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
#ifndef __BQ_TRI_H__
#define __BQ_TRI_H__

class bqTriangle
{
public:
	bqTriangle() {}
	bqTriangle(const bqVec4& _v1, const bqVec4& _v2, const bqVec4& _v3)
		:
		v1(_v1),
		v2(_v2),
		v3(_v3)
	{
	}

	bqTriangle(const bqVec3f& _v1, const bqVec3f& _v2, const bqVec3f& _v3)
		:
		v1(_v1),
		v2(_v2),
		v3(_v3)
	{
	}

	bqVec4 v1;
	bqVec4 v2;
	bqVec4 v3;
	bqVec4 e1;
	bqVec4 e2;

	void Update()
	{
		e1 = bqVec4(v2.x - v1.x,
			v2.y - v1.y,
			v2.z - v1.z,
			0.f);
		e2 = bqVec4(v3.x - v1.x,
			v3.y - v1.y,
			v3.z - v1.z,
			0.f);
		//	e1.cross(e2, faceNormal);
	}

	void Center(bqVec4& out)
	{
		out = (v1 + v2 + v3) * 0.3333333;
	}

	bool RayTest_MT(const bqRay& ray, bool withBackFace, bqReal& T, bqReal& U, bqReal& V, bqReal& W)
	{
		bqVec4  pvec;
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

		bqVec4 tvec(
			ray.m_origin.x - v1.x,
			ray.m_origin.y - v1.y,
			ray.m_origin.z - v1.z,
			0.f);

		bqReal inv_det = 1.f / det;
		U = tvec.Dot(pvec) * inv_det;

		if (U < 0.f || U > 1.f)
			return false;

		bqVec4  qvec;
		bqMath::Cross(tvec, e1, qvec);

		V = ray.m_direction.Dot(qvec) * inv_det;

		if (V < 0.f || U + V > 1.f)
			return false;

		T = e2.Dot(qvec) * inv_det;

		if (T < bqEpsilon) return false;

		W = 1.f - U - V;
		return true;
	}

	bool RayTest_Watertight(const bqRay& ray, bool withBackFace, bqReal& T, bqReal& U, bqReal& V, bqReal& W)
	{
		v1.w = 1.f;
		v2.w = 1.f;
		v3.w = 1.f;
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
};

#endif

