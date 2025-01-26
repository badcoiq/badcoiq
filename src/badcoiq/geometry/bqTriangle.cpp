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
	normal.Normalize();
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
	bqReal _e1 = AB.Dot();
	bqReal _e2 = BC.Dot();
	bqReal e3 = CA.Dot();
	bqVec3 Q1 = A * bqVec3(_e1) - bqVec3(d1) * AB;
	bqVec3 Q2 = B * bqVec3(_e2) - bqVec3(d2) * BC;
	bqVec3 Q3 = C * bqVec3(e3) - bqVec3(d3) * CA;
	bqVec3 QC = C * _e1 - Q1;
	bqVec3 QA = A * _e2 - Q2;
	bqVec3 QB = B * e3 - Q3;

	if ((Q1.Dot() > (rr * _e1 * _e1)) && (Q1.Dot(QC) >= 0.0)) return false;
	if ((Q2.Dot() > (rr * _e2 * _e2)) && (Q2.Dot(QA) >= 0.0)) return false;
	if ((Q3.Dot() > (rr * e3 * e3)) && (Q3.Dot(QB) >= 0.0)) return false;

	T = std::sqrt(d * d / e) - radius;

	// нужно ли инвертировать нормаль?
	ip = origin + (normal * T);

	return true;
}

bool bqTriangle::RayIntersect_MT(const bqRay& ray, bool withBackFace, bqReal& T, bqReal& U, bqReal& V, bqReal& W, bool segment)
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
	
	if (segment)
	{
		if (ray.m_segmentLen < T)
			return false;
	}

	return true;
}

bool bqTriangle::RayIntersect_Watertight(const bqRay& ray, bool withBackFace, bqReal& T, bqReal& U, bqReal& V, bqReal& W, bool segment)
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
	if (segment)
	{
		if (ray.m_segmentLen < T)
			return false;
	}
	return true;
}

/* this edge to edge test is based on Franlin Antonio's gem:
   "Faster Line Segment Intersection", in Graphics Gems III,
   pp. 199-202 */
#define EDGE_EDGE_TEST(V0,U0,U1)                      \
  Bx=U0[i0]-U1[i0];                                   \
  By=U0[i1]-U1[i1];                                   \
  Cx=V0[i0]-U0[i0];                                   \
  Cy=V0[i1]-U0[i1];                                   \
  f=Ay*Bx-Ax*By;                                      \
  d=By*Cx-Bx*Cy;                                      \
  if((f>0 && d>=0 && d<=f) || (f<0 && d<=0 && d>=f))  \
  {                                                   \
    e=Ax*Cy-Ay*Cx;                                    \
    if(f>0)                                           \
    {                                                 \
      if(e>=0 && e<=f) return 1;                      \
    }                                                 \
    else                                              \
    {                                                 \
      if(e<=0 && e>=f) return 1;                      \
    }                                                 \
  }

#define EDGE_AGAINST_TRI_EDGES(V0,V1,U0,U1,U2) \
{                                              \
  float Ax,Ay,Bx,By,Cx,Cy,e,d,f;               \
  Ax=V1[i0]-V0[i0];                            \
  Ay=V1[i1]-V0[i1];                            \
  /* test edge U0,U1 against V0,V1 */          \
  EDGE_EDGE_TEST(V0,U0,U1);                    \
  /* test edge U1,U2 against V0,V1 */          \
  EDGE_EDGE_TEST(V0,U1,U2);                    \
  /* test edge U2,U1 against V0,V1 */          \
  EDGE_EDGE_TEST(V0,U2,U0);                    \
}
#define POINT_IN_TRI(V0,U0,U1,U2)           \
{                                           \
  float a,b,c,d0,d1,d2;                     \
  /* is T1 completly inside T2? */          \
  /* check if V0 is inside tri(U0,U1,U2) */ \
  a=U1[i1]-U0[i1];                          \
  b=-(U1[i0]-U0[i0]);                       \
  c=-a*U0[i0]-b*U0[i1];                     \
  d0=a*V0[i0]+b*V0[i1]+c;                   \
                                            \
  a=U2[i1]-U1[i1];                          \
  b=-(U2[i0]-U1[i0]);                       \
  c=-a*U1[i0]-b*U1[i1];                     \
  d1=a*V0[i0]+b*V0[i1]+c;                   \
                                            \
  a=U0[i1]-U2[i1];                          \
  b=-(U0[i0]-U2[i0]);                       \
  c=-a*U2[i0]-b*U2[i1];                     \
  d2=a*V0[i0]+b*V0[i1]+c;                   \
  if(d0*d1>0.0)                             \
  {                                         \
    if(d0*d2>0.0) return 1;                 \
  }                                         \
}
int coplanar_tri_tri(const bqVec3& N, 
	const bqVec3& V0, 
	const bqVec3& V1, 
	const bqVec3& V2,
	const bqVec3& U0,
	const bqVec3& U1,
	const bqVec3& U2)
{
	float A[3];
	short i0, i1;
	/* first project onto an axis-aligned plane, that maximizes the area */
	/* of the triangles, compute indices: i0,i1. */
	A[0] = fabs(N[0]);
	A[1] = fabs(N[1]);
	A[2] = fabs(N[2]);
	if (A[0] > A[1])
	{
		if (A[0] > A[2])
		{
			i0 = 1;      /* A[0] is greatest */
			i1 = 2;
		}
		else
		{
			i0 = 0;      /* A[2] is greatest */
			i1 = 1;
		}
	}
	else   /* A[0]<=A[1] */
	{
		if (A[2] > A[1])
		{
			i0 = 0;      /* A[2] is greatest */
			i1 = 1;
		}
		else
		{
			i0 = 0;      /* A[1] is greatest */
			i1 = 2;
		}
	}

	/* test all edges of triangle 1 against the edges of triangle 2 */
	EDGE_AGAINST_TRI_EDGES(V0, V1, U0, U1, U2);
	EDGE_AGAINST_TRI_EDGES(V1, V2, U0, U1, U2);
	EDGE_AGAINST_TRI_EDGES(V2, V0, U0, U1, U2);

	/* finally, test if tri1 is totally contained in tri2 or vice versa */
	POINT_IN_TRI(V0, U0, U1, U2);
	POINT_IN_TRI(U0, V0, V1, V2);

	return 0;
}
#define NEWCOMPUTE_INTERVALS(VV0,VV1,VV2,D0,D1,D2,D0D1,D0D2,A,B,C,X0,X1) \
{ \
        if(D0D1>0.0f) \
        { \
                /* here we know that D0D2<=0.0 */ \
            /* that is D0, D1 are on the same side, D2 on the other or on the plane */ \
                A=VV2; B=(VV0-VV2)*D2; C=(VV1-VV2)*D2; X0=D2-D0; X1=D2-D1; \
        } \
        else if(D0D2>0.0f)\
        { \
                /* here we know that d0d1<=0.0 */ \
            A=VV1; B=(VV0-VV1)*D1; C=(VV2-VV1)*D1; X0=D1-D0; X1=D1-D2; \
        } \
        else if(D1*D2>0.0f || D0!=0.0f) \
        { \
                /* here we know that d0d1<=0.0 or that D0!=0.0 */ \
                A=VV0; B=(VV1-VV0)*D0; C=(VV2-VV0)*D0; X0=D0-D1; X1=D0-D2; \
        } \
        else if(D1!=0.0f) \
        { \
                A=VV1; B=(VV0-VV1)*D1; C=(VV2-VV1)*D1; X0=D1-D0; X1=D1-D2; \
        } \
        else if(D2!=0.0f) \
        { \
                A=VV2; B=(VV0-VV2)*D2; C=(VV1-VV2)*D2; X0=D2-D0; X1=D2-D1; \
        } \
        else \
        { \
                /* triangles are coplanar */ \
                return coplanar_tri_tri(N1,V0,V1,V2,v1,v2,v3); \
        } \
}
/* sort so that a<=b */
#define SORT(a,b)       \
             if(a>b)    \
             {          \
               float _c; \
               _c=a;     \
               a=b;     \
               b=_c;     \
             }
bool bqTriangle::TriangleIntersect(
	const bqVec3& V0, 
	const bqVec3& V1, 
	const bqVec3& V2)
{
	//float N2[3], d1, d2;
	//float du0, du1, du2, dv0, dv1, dv2;
	//float D[3];
	float isect1[2], isect2[2];
	float du0du1, du0du2, dv0dv1, dv0dv2;
	short index;
	float vp0, vp1, vp2;
	float up0, up1, up2;
	float bb, cc, max;

	/* compute plane equation of triangle(V0,V1,V2) */
	bqVec3 E1 = V1 - V0;
	bqVec3 E2 = V2 - V0;
	bqVec3 N1 = E1.Cross(E2);
	bqReal d1 = -N1.Dot(V0);
	/* plane equation 1: N1.X+d1=0 */

	/* put U0,U1,U2 into plane equation 1 to compute signed distances to the plane*/
	bqReal du0 = N1.Dot(v1) + d1;
	bqReal du1 = N1.Dot(v2) + d1;
	bqReal du2 = N1.Dot(v3) + d1;

	du0du1 = du0 * du1;
	du0du2 = du0 * du2;

	if (du0du1 > 0.0f && du0du2 > 0.0f) /* same sign on all of them + not equal 0 ? */
		return false;                    /* no intersection occurs */

	/* compute plane of triangle (U0,U1,U2) */
	E1 = v2 - v1;
	E2 = v3 - v1;
	bqVec3 N2 = E1.Cross(E2);
	bqReal d2 = -N2.Dot(v1);
	/* plane equation 2: N2.X+d2=0 */

	/* put V0,V1,V2 into plane equation 2 */
	bqReal dv0 = N2.Dot(V0) + d2;
	bqReal dv1 = N2.Dot(V1) + d2;
	bqReal dv2 = N2.Dot(V2) + d2;

	dv0dv1 = dv0 * dv1;
	dv0dv2 = dv0 * dv2;

	if (dv0dv1 > 0.0f && dv0dv2 > 0.0f) /* same sign on all of them + not equal 0 ? */
		return false;                    /* no intersection occurs */

	/* compute direction of intersection line */
	bqVec3 D = N1.Cross(N2);

	/* compute and index to the largest component of D */
	max = fabs(D[0]);
	index = 0;
	bb = fabs(D[1]);
	cc = fabs(D[2]);
	if (bb > max) max = bb, index = 1;
	if (cc > max) max = cc, index = 2;

	/* this is the simplified projection onto L*/
	vp0 = V0[index];
	vp1 = V1[index];
	vp2 = V2[index];

	up0 = v1[index];
	up1 = v2[index];
	up2 = v3[index];

	/* compute interval for triangle 1 */
	float a, b, c, x0, x1;
	NEWCOMPUTE_INTERVALS(vp0, vp1, vp2, dv0, dv1, dv2, dv0dv1, dv0dv2, a, b, c, x0, x1);

	/* compute interval for triangle 2 */
	float d, e, f, y0, y1;
	NEWCOMPUTE_INTERVALS(up0, up1, up2, du0, du1, du2, du0du1, du0du2, d, e, f, y0, y1);

	float xx, yy, xxyy, tmp;
	xx = x0 * x1;
	yy = y0 * y1;
	xxyy = xx * yy;

	tmp = a * xxyy;
	isect1[0] = tmp + b * x1 * yy;
	isect1[1] = tmp + c * x0 * yy;

	tmp = d * xxyy;
	isect2[0] = tmp + e * xx * y1;
	isect2[1] = tmp + f * xx * y0;

	SORT(isect1[0], isect1[1]);
	SORT(isect2[0], isect2[1]);

	if (isect1[1] < isect2[0] || isect2[1] < isect1[0]) return 0;
	return 1;
}

bool bqTriangle::TriangleIntersect(bqTriangle* t)
{
	return TriangleIntersect(t->v1, t->v2, t->v3);
}

//bool bqTriangle::AabbIntersect(const bqAabb& a)
//{
//}

