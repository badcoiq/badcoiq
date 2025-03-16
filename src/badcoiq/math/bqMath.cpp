/*
BSD 2-Clause License

Copyright (c) 2025, badcoiq
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
#include "badcoiq/math/bqMath.h"

#include "math_private.h"


//struct A
//{
//	1 - 10
//	...
//	20000 - 10
//};
//
//A a[100];


// Сгенерированные результаты функций:

static float g_acosf[] = {
#include "acosf.inl"
};
static double g_acos[] = {
#include "acos.inl"
};
static float g_asinf[] = {
#include "asinf.inl"
};
static double g_asin[] = {
#include "asin.inl"
};
static float g_atanf[] = {
#include "atanf.inl"
};
static double g_atan[] = {
#include "atan.inl"
};
static double g_atan2[] = {
#include "atan2.inl"
};
static float g_atan2f[] = {
#include "atan2f.inl"
};
static double g_cos[] = {
#include "cos.inl"
};
static float g_cosf[] = {
#include "cosf.inl"
};
static double g_sin[] = {
#include "sin.inl"
};
static float g_sinf[] = {
#include "sinf.inl"
};
static double g_tan[] = {
#include "tan.inl"
};
static float g_tanf[] = {
#include "tanf.inl"
};

float bqMath::DegToRad(float degrees)
{
	return degrees * (PIf / 180.f);
}

double bqMath::DegToRad(double degrees)
{
	return degrees * (PI / 180.0);
}

float bqMath::RadToDeg(float radians)
{
	return radians * (180.f / PIf);
}

double bqMath::RadToDeg(double radians)
{
	return radians * (180.0 / PI);
}

float bqMath::Abs(float x)
{
	uint32_t ix;
	GET_FLOAT_WORD(ix, x);
	SET_FLOAT_WORD(x, ix & 0x7fffffff);
	return x;
}

double bqMath::Abs(double x)
{
	uint32_t high;
	GET_HIGH_WORD(high, x);
	SET_HIGH_WORD(x, high & 0x7fffffff);
	return x;
}

int32_t bqMath::Abs(int32_t v)
{
	return(v < 0 ? -v : v);
}

float bqMath::Acos(float v)
{
	if (v >= -1.f && v <= 1.f)
	{
		uint32_t ind = (uint32_t)roundf((v + 1.f) * 1000.f);
		BQ_ASSERT_ST(ind < 2001);
		return g_acosf[ind];
	}
	return 0;
}

double bqMath::Acos(double v)
{
	if (v >= -1.0 && v <= 1.0)
	{
		uint32_t ind = (uint32_t)round((v + 1.0) * 1000.0);
		BQ_ASSERT_ST(ind < 2001);
		return g_acos[ind];
	}
	return 0;
}

float bqMath::Asin(float v)
{
	if (v >= -1.f && v <= 1.f)
	{
		uint32_t ind = (uint32_t)roundf((v + 1.f) * 1000.f);
		BQ_ASSERT_ST(ind < 2001);
		return g_asinf[ind];
	}
	return 0;
}

double bqMath::Asin(double v)
{
	if (v >= -1.0 && v <= 1.0)
	{
		uint32_t ind = (uint32_t)round((v + 1.0) * 1000.0);
		BQ_ASSERT_ST(ind < 2001);
		return g_asin[ind];
	}
	return 0;
}

float bqMath::Atan(float v)
{
	if (v >= -2.f && v <= 2.f)
	{
		uint32_t ind = (uint32_t)roundf((v + 2.f) * 1000.f);
		BQ_ASSERT_ST(ind < 4001);
		return g_atanf[ind];
	}
	return 0;
}

double bqMath::Atan(double v)
{
	if (v >= -2.f && v <= 2.f)
	{
		uint32_t ind = (uint32_t)round((v + 2.0) * 1000.0);
		BQ_ASSERT_ST(ind < 4001);
		return g_atan[ind];
	}
	return 0;
}

float bqMath::Atan2(float y, float x)
{
	if (y >= -1.0f && y <= 1.0f)
	{
		if (x >= -1.0f && x <= 1.0f)
		{
			uint32_t ix = (uint32_t)roundf((x + 1.0f) * 100.0f);
			uint32_t iy = (uint32_t)roundf((y + 1.0f) * 100.0f);
			uint32_t ind = (uint32_t)(((float)iy * 201.f) + (float)ix);
			BQ_ASSERT_ST(ind < 40401);

			if (ind > 40400)
			{
				bqLog::PrintWarning("ind > 40400\n");
				return 0.f;
			}

			return g_atan2f[ind];
		}
	}
	return 0.f;
}

double bqMath::Atan2(double y, double x)
{
	if (y >= -1.0 && y <= 1.0)
	{
		if (x >= -1.0 && x <= 1.0)
		{
			uint32_t ix = (uint32_t)round((x + 1.0) * 100.0);
			uint32_t iy = (uint32_t)round((y + 1.0) * 100.0);
			uint32_t ind = (uint32_t)(((float)iy * 201.f) + (float)ix);
			BQ_ASSERT_ST(ind < 40401);

			if (ind > 40400)
			{
				bqLog::PrintWarning("ind > 40400\n");
				return 0;
			}

			return g_atan2[ind];
		}
	}
	return 0.f;
}

float bqMath::Clamp(float v, float mn, float mx)
{
	if (v < mn)
		return mn;
	else if (v > mx)
		return mx;
	return v;
}

double bqMath::Clamp(double v, double mn, double mx)
{
	if (v < mn)
		return mn;
	else if (v > mx)
		return mx;
	return v;
}

float bqMath::Cos(float v)
{
	if (v < -3.141f) v = -3.141f;
	if (v < 0.f) v = bqMath::Abs(v);
	if (v > 3.141f) v = 3.141f;

	uint32_t ind = (uint32_t)roundf(v * 1000.0f);

	if (ind < 3143)
		return g_cosf[ind];
	return 0;
}

double bqMath::Cos(double v)
{
	if (v < -3.141) v = -3.141;
	if (v < 0.) v = bqMath::Abs(v);
	if (v > 3.141) v = 3.141;

	uint32_t ind = (uint32_t)round(v * 1000.0);

	if (ind < 3143)
		return g_cos[ind];
	return 0;
}

float bqMath::Sin(float v)
{
	bool neg = false;
	if (v < -3.141f) v = -3.141f;
	if (v < 0.f)
	{
		neg = true;
		v = bqMath::Abs(v);
	}
	if (v > 3.141f) v = 3.141f;

	uint32_t ind = (uint32_t)roundf(v * 1000.0f);

	if (ind < 3143)
		return neg ? -g_sinf[ind] : g_sinf[ind];
	return 0;
}

double bqMath::Sin(double v)
{
	bool neg = false;
	if (v < -3.141) v = -3.141;
	if (v < 0.)
	{
		neg = true;
		v = bqMath::Abs(v);
	}
	if (v > 3.141) v = 3.141;

	uint32_t ind = (uint32_t)round(v * 1000.0);

	if (ind < 3143)
		return neg ? -g_sin[ind] : g_sin[ind];
	return 0;
}

float bqMath::Tan(float v)
{
	bool neg = false;
	if (v < -3.141f) v = -3.141f;
	if (v < 0.f)
	{
		neg = true;
		v = bqMath::Abs(v);
	}
	if (v > 3.141f) v = 3.141f;

	uint32_t ind = (uint32_t)roundf(v * 1000.0f);

	if (ind < 3143)
		return neg ? -g_tanf[ind] : g_tanf[ind];
	return 0;
}

double bqMath::Tan(double v)
{
	bool neg = false;
	if (v < -3.141) v = -3.141;
	if (v < 0.)
	{
		neg = true;
		v = bqMath::Abs(v);
	}
	if (v > 3.141) v = 3.141;

	uint32_t ind = (uint32_t)round(v * 1000.0);

	if (ind < 3143)
		return neg ? -g_tan[ind] : g_tan[ind];
	return 0;
}

float bqMath::Lerp1(float x, float y, float t)
{
	return x + t * (y - x);
}

double bqMath::Lerp1(double x, double y, double t)
{
	return x + t * (y - x);
}

float bqMath::Lerp2(float x, float y, float t)
{
	return (1.f - t) * x + t * y;
}

double bqMath::Lerp2(double x, double y, double t)
{
	return (1.f - t) * x + t * y;
}

bool bqMath::PointInRect(const bqPoint& p, const bqRect& r)
{
	if ((p.x >= r.left) && (p.x <= r.right) && (p.y >= r.top) && (p.y <= r.bottom))
		return true;
	return false;
}

bool bqMath::PointInRect(const bqPointf& p, const bqRect& r)
{
	if ((p.x >= r.left) && (p.x <= r.right) && (p.y >= r.top) && (p.y <= r.bottom))
		return true;
	return false;
}

bool bqMath::PointInRect(const bqPointf& p, const bqVec4f& r)
{
	if ((p.x >= r.x) && (p.x <= r.z) && (p.y >= r.y) && (p.y <= r.w))
		return true;
	return false;
}

void bqMath::PerpendicularVector1(const bqVec2f& v1, const bqVec2f& v2, bqVec2f& r)
{
	r.x = (v2.y - v1.y);
	r.y = -(v2.x - v1.x);
}

void bqMath::PerpendicularVector2(const bqVec2f& v1, const bqVec2f& v2, bqVec2f& r)
{
	r.x = -(v2.y - v1.y);
	r.y = (v2.x - v1.x);
}

void bqMath::Cross(const bqVec3& v1, const bqVec3& v2, bqVec3& r)
{
	r.x = (v1.y * v2.z) - (v1.z * v2.y);
	r.y = (v1.z * v2.x) - (v1.x * v2.z);
	r.z = (v1.x * v2.y) - (v1.y * v2.x);
}

void bqMath::Cross(const bqVec3f& v1, const bqVec3f& v2, bqVec3f& r)
{
	r.x = (v1.y * v2.z) - (v1.z * v2.y);
	r.y = (v1.z * v2.x) - (v1.x * v2.z);
	r.z = (v1.x * v2.y) - (v1.y * v2.x);
}

void bqMath::Cross(const bqVec4& v1, const bqVec4& v2, bqVec4& r)
{
	r.x = (v1.y * v2.z) - (v1.z * v2.y);
	r.y = (v1.z * v2.x) - (v1.x * v2.z);
	r.z = (v1.x * v2.y) - (v1.y * v2.x);
}

void bqMath::Cross(const bqVec4f& v1, const bqVec4f& v2, bqVec4f& r)
{
	r.x = (v1.y * v2.z) - (v1.z * v2.y);
	r.y = (v1.z * v2.x) - (v1.x * v2.z);
	r.z = (v1.x * v2.y) - (v1.y * v2.x);
}

void bqMath::Cross(const bqVec3& v1, const bqVec4& v2, bqVec3& r)
{
	r.x = (v1.y * v2.z) - (v1.z * v2.y);
	r.y = (v1.z * v2.x) - (v1.x * v2.z);
	r.z = (v1.x * v2.y) - (v1.y * v2.x);
}

void bqMath::Cross(const bqVec3f& v1, const bqVec4f& v2, bqVec3f& r)
{
	r.x = (v1.y * v2.z) - (v1.z * v2.y);
	r.y = (v1.z * v2.x) - (v1.x * v2.z);
	r.z = (v1.x * v2.y) - (v1.y * v2.x);
}

float32_t bqMath::Distance(const bqVec2f& v1, const bqVec2f& v2)
{
	float xx = v2.x - v1.x;
	float yy = v2.y - v1.y;

	return sqrtf((xx * xx) + (yy * yy));
}

bqReal bqMath::Distance(const bqVec3& v1, const bqVec3& v2)
{
	bqReal xx = v2.x - v1.x;
	bqReal yy = v2.y - v1.y;
	bqReal zz = v2.z - v1.z;

	return sqrt((xx * xx) + (yy * yy) + (zz * zz));
}

bqReal bqMath::Distance(const bqVec4& v1, const bqVec4& v2)
{
	bqReal xx = v2.x - v1.x;
	bqReal yy = v2.y - v1.y;
	bqReal zz = v2.z - v1.z;

	return sqrt((xx * xx) + (yy * yy) + (zz * zz));
}

bqReal bqMath::Distance(const bqVec3& v1, const bqVec4& v2)
{
	bqReal xx = v2.x - v1.x;
	bqReal yy = v2.y - v1.y;
	bqReal zz = v2.z - v1.z;

	return sqrt((xx * xx) + (yy * yy) + (zz * zz));
}

float bqMath::Distance(const bqVec3f& v1, const bqVec3f& v2)
{
	float xx = v2.x - v1.x;
	float yy = v2.y - v1.y;
	float zz = v2.z - v1.z;

	return sqrtf((xx * xx) + (yy * yy) + (zz * zz));
}

float bqMath::Distance(const bqVec4f& v1, const bqVec4f& v2)
{
	float xx = v2.x - v1.x;
	float yy = v2.y - v1.y;
	float zz = v2.z - v1.z;

	return sqrtf((xx * xx) + (yy * yy) + (zz * zz));
}

float bqMath::Distance(const bqVec3f& v1, const bqVec4f& v2)
{
	float xx = v2.x - v1.x;
	float yy = v2.y - v1.y;
	float zz = v2.z - v1.z;

	return sqrtf((xx * xx) + (yy * yy) + (zz * zz));
}

bqReal bqMath::Dot(const bqVec3& v1, const bqVec3& v2)
{
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

bqReal bqMath::Dot(const bqVec4& v1, const bqVec4& v2)
{
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z) + (v1.w * v2.w);
}

bqReal bqMath::Dot(const bqVec3& v1, const bqVec4& v2)
{
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

float bqMath::Dot(const bqVec3f& v1, const bqVec3f& v2)
{
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

float bqMath::Dot(const bqVec4f& v1, const bqVec4f& v2)
{
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z) + (v1.w * v2.w);
}

float bqMath::Dot(const bqVec3f& v1, const bqVec4f& v2)
{
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

float bqMath::Dot(const bqQuaternion& v1, const bqQuaternion& v2)
{
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z) + (v1.w * v2.w);
}

bqReal bqMath::Length(const bqVec3& v)
{
	return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

bqReal bqMath::Length(const bqVec4& v)
{
	return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w));
}

float bqMath::Length(const bqVec3f& v)
{
	return sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

float bqMath::Length(const bqVec4f& v)
{
	return sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w));
}

float bqMath::Length(const bqQuaternion& v)
{
	return sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w));
}


void bqMath::Lerp1(const bqVec3& x, const bqVec3& y, double t, bqVec3& r)
{
	r.x = x.x + t * (y.x - x.x);
	r.y = x.y + t * (y.y - x.y);
	r.z = x.z + t * (y.z - x.z);
}

void bqMath::Lerp1(const bqVec3& x, const bqVec4& y, double t, bqVec3& r)
{
	r.x = x.x + t * (y.x - x.x);
	r.y = x.y + t * (y.y - x.y);
	r.z = x.z + t * (y.z - x.z);
}

void bqMath::Lerp1(const bqVec4& x, const bqVec4& y, double t, bqVec4& r)
{
	r.x = x.x + t * (y.x - x.x);
	r.y = x.y + t * (y.y - x.y);
	r.z = x.z + t * (y.z - x.z);
	r.w = x.w + t * (y.w - x.w);
}

void bqMath::Lerp1(const bqVec3f& x, const bqVec3f& y, float t, bqVec3f& r)
{
	r.x = x.x + t * (y.x - x.x);
	r.y = x.y + t * (y.y - x.y);
	r.z = x.z + t * (y.z - x.z);
}

void bqMath::Lerp1(const bqVec3f& x, const bqVec4f& y, float t, bqVec3f& r)
{
	r.x = x.x + t * (y.x - x.x);
	r.y = x.y + t * (y.y - x.y);
	r.z = x.z + t * (y.z - x.z);
}

void bqMath::Lerp1(const bqVec4f& x, const bqVec4f& y, float t, bqVec4f& r)
{
	r.x = x.x + t * (y.x - x.x);
	r.y = x.y + t * (y.y - x.y);
	r.z = x.z + t * (y.z - x.z);
	r.w = x.w + t * (y.w - x.w);
}

void bqMath::Lerp1(const bqQuaternion& x, const bqQuaternion& y, float t, bqQuaternion& r)
{
	r.x = x.x + t * (y.x - x.x);
	r.y = x.y + t * (y.y - x.y);
	r.z = x.z + t * (y.z - x.z);
	r.w = x.w + t * (y.w - x.w);
}

void bqMath::Lerp2(const bqVec3& x, const bqVec3& y, double t, bqVec3& r)
{
	r.x = (1.0 - t) * x.x + t * y.x;
	r.y = (1.0 - t) * x.y + t * y.y;
	r.z = (1.0 - t) * x.z + t * y.z;
}

void bqMath::Lerp2(const bqVec3& x, const bqVec4& y, double t, bqVec3& r)
{
	r.x = (1.0 - t) * x.x + t * y.x;
	r.y = (1.0 - t) * x.y + t * y.y;
	r.z = (1.0 - t) * x.z + t * y.z;
}

void bqMath::Lerp2(const bqVec4& x, const bqVec4& y, double t, bqVec4& r)
{
	r.x = (1.0 - t) * x.x + t * y.x;
	r.y = (1.0 - t) * x.y + t * y.y;
	r.z = (1.0 - t) * x.z + t * y.z;
	r.w = (1.0 - t) * x.w + t * y.w;
}

void bqMath::Lerp2(const bqVec3f& x, const bqVec3f& y, float t, bqVec3f& r)
{
	r.x = (1.0f - t) * x.x + t * y.x;
	r.y = (1.0f - t) * x.y + t * y.y;
	r.z = (1.0f - t) * x.z + t * y.z;
}

void bqMath::Lerp2(const bqVec3f& x, const bqVec4f& y, float t, bqVec3f& r)
{
	r.x = (1.0f - t) * x.x + t * y.x;
	r.y = (1.0f - t) * x.y + t * y.y;
	r.z = (1.0f - t) * x.z + t * y.z;
}

void bqMath::Lerp2(const bqVec4f& x, const bqVec4f& y, float t, bqVec4f& r)
{
	r.x = (1.0f - t) * x.x + t * y.x;
	r.y = (1.0f - t) * x.y + t * y.y;
	r.z = (1.0f - t) * x.z + t * y.z;
	r.w = (1.0f - t) * x.w + t * y.w;
}

void bqMath::Lerp2(const bqQuaternion& x, const bqQuaternion& y, float t, bqQuaternion& r)
{
	r.x = (1.0f - t) * x.x + t * y.x;
	r.y = (1.0f - t) * x.y + t * y.y;
	r.z = (1.0f - t) * x.z + t * y.z;
	r.w = (1.0f - t) * x.w + t * y.w;
}

void bqMath::Normalize(bqVec3& v)
{
	double len = sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
	if (len > 0)
		len = 1.0f / len;
	v.x *= len;
	v.y *= len;
	v.z *= len;
}

void bqMath::Normalize(bqVec3f& v)
{
	float len = sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
	if (len > 0)
		len = 1.0f / len;
	v.x *= len;
	v.y *= len;
	v.z *= len;
}

void bqMath::Normalize(bqVec4& v)
{
	double len = sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w));
	if (len > 0)
		len = 1.0f / len;
	v.x *= len;
	v.y *= len;
	v.z *= len;
	v.w *= len;
}

void bqMath::Normalize(bqVec4f& v)
{
	float len = sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w));
	if (len > 0)
		len = 1.0f / len;
	v.x *= len;
	v.y *= len;
	v.z *= len;
	v.w *= len;
}

void bqMath::Normalize(bqQuaternion& v)
{
	float len = sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w));
	if (len > 0)
		len = 1.0f / len;
	v.x *= len;
	v.y *= len;
	v.z *= len;
	v.w *= len;
}

bool bqMath::PointInRect(const bqPoint& p, const bqVec4f& r)
{
	if ((p.x >= r.x) && (p.x <= r.z) && (p.y >= r.y) && (p.y <= r.w))
		return true;
	return false;
}

bool bqMath::PointInRect(const bqVec2f& p, const bqVec4f& r)
{
	if ((p.x >= r.x) && (p.x <= r.z) && (p.y >= r.y) && (p.y <= r.w))
		return true;
	return false;
}

void bqMath::SetRotation(bqQuaternion& q, float x, float y, float z)
{
	x *= 0.5f;
	y *= 0.5f;
	z *= 0.5f;
	float c1 = ::cosf(x);
	float c2 = ::cosf(y);
	float c3 = ::cosf(z);
	float s1 = ::sinf(x);
	float s2 = ::sinf(y);
	float s3 = ::sinf(z);
	q.w = (c1 * c2 * c3) + (s1 * s2 * s3);
	q.x = (s1 * c2 * c3) - (c1 * s2 * s3);
	q.y = (c1 * s2 * c3) + (s1 * c2 * s3);
	q.z = (c1 * c2 * s3) - (s1 * s2 * c3);
}

void bqMath::SetRotation(bqQuaternion& q, const bqVec3& axis, const bqReal& _angle)
{
	bqReal d = (bqReal)Length(axis);
	bqReal s = std::sin(_angle * 0.5f) / d;
	q.x = (float)(axis.x * s);
	q.y = (float)(axis.y * s);
	q.z = (float)(axis.z * s);
	q.w = (float)std::cos(_angle * 0.5);
}

void bqMath::SetRotation(bqQuaternion& q, const bqVec3f& axis, const float& _angle)
{
	float d = Length(axis);
	float s = std::sin(_angle * 0.5f) / d;
	q.x = axis.x * s;
	q.y = axis.y * s;
	q.z = axis.z * s;
	q.w = std::cos(_angle * 0.5f);
}

void bqMath::SetRotation(bqQuaternion& q, const bqVec4& axis, const bqReal& _angle)
{
	bqReal d = (bqReal)Length(axis);
	bqReal s = std::sin(_angle * 0.5f) / d;
	q.x = (float)(axis.x * s);
	q.y = (float)(axis.y * s);
	q.z = (float)(axis.z * s);
	q.w = (float)std::cos(_angle * 0.5f);
}

void bqMath::SetRotation(bqQuaternion& q, const bqVec4f& axis, const float& _angle)
{
	float d = Length(axis);
	float s = std::sin(_angle * 0.5f) / d;
	q.x = axis.x * s;
	q.y = axis.y * s;
	q.z = axis.z * s;
	q.w = std::cos(_angle * 0.5f);
}

void bqMath::SetRotation(bqMat4& m, const bqQuaternion& q)
{
	bqReal d = Length(q);
	bqReal s = 2.0f / d;
	bqReal xs = q.x * s, ys = q.y * s, zs = q.z * s;
	bqReal wx = q.w * xs, wy = q.w * ys, wz = q.w * zs;
	bqReal xx = q.x * xs, xy = q.x * ys, xz = q.x * zs;
	bqReal yy = q.y * ys, yz = q.y * zs, zz = q.z * zs;
	m.Set(
		1.0f - (yy + zz), xy - wz, xz + wy,
		xy + wz, 1.0f - (xx + zz), yz - wx,
		xz - wy, yz + wx, 1.0f - (xx + yy));
}

// irrlicht или assimp?
void bqMath::Slerp(bqQuaternion& q1, bqQuaternion& q2, float time, float threshold, bqQuaternion& r)
{
	float angle = Dot(q1, q2);
	// make sure we use the short rotation
	if (angle < 0.0f)
	{
		q1 = q1 * -1.0f;
		angle *= -1.0f;
	}

	if (angle <= (1 - threshold)) // spherical interpolation
	{
		const float theta = ::acosf(angle);
		const float invsintheta = 1.f / (::sinf(theta));
		const float scale = ::sinf(theta * (1.0f - time)) * invsintheta;
		const float invscale = ::sinf(theta * time) * invsintheta;
		r = (q1 * scale) + (q2 * invscale);
	}
	else // linear interploation
		Lerp1(q1, q2, time, r);
}

void bqMath::Mul(const bqMat4& m1, const bqMat4& m2, bqMat4& r)
{
	r.m_data[0].x = m1.m_data[0].x * m2.m_data[0].x + m1.m_data[1].x * m2.m_data[0].y + m1.m_data[2].x * m2.m_data[0].z + m1.m_data[3].x * m2.m_data[0].w;
	r.m_data[0].y = m1.m_data[0].y * m2.m_data[0].x + m1.m_data[1].y * m2.m_data[0].y + m1.m_data[2].y * m2.m_data[0].z + m1.m_data[3].y * m2.m_data[0].w;
	r.m_data[0].z = m1.m_data[0].z * m2.m_data[0].x + m1.m_data[1].z * m2.m_data[0].y + m1.m_data[2].z * m2.m_data[0].z + m1.m_data[3].z * m2.m_data[0].w;
	r.m_data[0].w = m1.m_data[0].w * m2.m_data[0].x + m1.m_data[1].w * m2.m_data[0].y + m1.m_data[2].w * m2.m_data[0].z + m1.m_data[3].w * m2.m_data[0].w;

	r.m_data[1].x = m1.m_data[0].x * m2.m_data[1].x + m1.m_data[1].x * m2.m_data[1].y + m1.m_data[2].x * m2.m_data[1].z + m1.m_data[3].x * m2.m_data[1].w;
	r.m_data[1].y = m1.m_data[0].y * m2.m_data[1].x + m1.m_data[1].y * m2.m_data[1].y + m1.m_data[2].y * m2.m_data[1].z + m1.m_data[3].y * m2.m_data[1].w;
	r.m_data[1].z = m1.m_data[0].z * m2.m_data[1].x + m1.m_data[1].z * m2.m_data[1].y + m1.m_data[2].z * m2.m_data[1].z + m1.m_data[3].z * m2.m_data[1].w;
	r.m_data[1].w = m1.m_data[0].w * m2.m_data[1].x + m1.m_data[1].w * m2.m_data[1].y + m1.m_data[2].w * m2.m_data[1].z + m1.m_data[3].w * m2.m_data[1].w;

	r.m_data[2].x = m1.m_data[0].x * m2.m_data[2].x + m1.m_data[1].x * m2.m_data[2].y + m1.m_data[2].x * m2.m_data[2].z + m1.m_data[3].x * m2.m_data[2].w;
	r.m_data[2].y = m1.m_data[0].y * m2.m_data[2].x + m1.m_data[1].y * m2.m_data[2].y + m1.m_data[2].y * m2.m_data[2].z + m1.m_data[3].y * m2.m_data[2].w;
	r.m_data[2].z = m1.m_data[0].z * m2.m_data[2].x + m1.m_data[1].z * m2.m_data[2].y + m1.m_data[2].z * m2.m_data[2].z + m1.m_data[3].z * m2.m_data[2].w;
	r.m_data[2].w = m1.m_data[0].w * m2.m_data[2].x + m1.m_data[1].w * m2.m_data[2].y + m1.m_data[2].w * m2.m_data[2].z + m1.m_data[3].w * m2.m_data[2].w;

	r.m_data[3].x = m1.m_data[0].x * m2.m_data[3].x + m1.m_data[1].x * m2.m_data[3].y + m1.m_data[2].x * m2.m_data[3].z + m1.m_data[3].x * m2.m_data[3].w;
	r.m_data[3].y = m1.m_data[0].y * m2.m_data[3].x + m1.m_data[1].y * m2.m_data[3].y + m1.m_data[2].y * m2.m_data[3].z + m1.m_data[3].y * m2.m_data[3].w;
	r.m_data[3].z = m1.m_data[0].z * m2.m_data[3].x + m1.m_data[1].z * m2.m_data[3].y + m1.m_data[2].z * m2.m_data[3].z + m1.m_data[3].z * m2.m_data[3].w;
	r.m_data[3].w = m1.m_data[0].w * m2.m_data[3].x + m1.m_data[1].w * m2.m_data[3].y + m1.m_data[2].w * m2.m_data[3].z + m1.m_data[3].w * m2.m_data[3].w;
}

void bqMath::Mul(const bqMat4& m, const bqVec3& v, bqVec3& r)
{
	r.x = v.x * m.m_data[0].x + v.y * m.m_data[1].x + v.z * m.m_data[2].x;
	r.y = v.x * m.m_data[0].y + v.y * m.m_data[1].y + v.z * m.m_data[2].y;
	r.z = v.x * m.m_data[0].z + v.y * m.m_data[1].z + v.z * m.m_data[2].z;
}

void bqMath::Mul(const bqMat4& m, const bqVec3& v, bqVec4& r)
{
	r.x = v.x * m.m_data[0].x + v.y * m.m_data[1].x + v.z * m.m_data[2].x + m.m_data[3].x;
	r.y = v.x * m.m_data[0].y + v.y * m.m_data[1].y + v.z * m.m_data[2].y + m.m_data[3].y;
	r.z = v.x * m.m_data[0].z + v.y * m.m_data[1].z + v.z * m.m_data[2].z + m.m_data[3].z;
	r.w = v.x * m.m_data[0].w + v.y * m.m_data[1].w + v.z * m.m_data[2].w + m.m_data[3].w;
}

void bqMath::Mul(const bqMat4& m, const bqVec3f& v, bqVec3f& r)
{
	r.x = v.x * (float)m.m_data[0].x + v.y * (float)m.m_data[1].x + v.z * (float)m.m_data[2].x;
	r.y = v.x * (float)m.m_data[0].y + v.y * (float)m.m_data[1].y + v.z * (float)m.m_data[2].y;
	r.z = v.x * (float)m.m_data[0].z + v.y * (float)m.m_data[1].z + v.z * (float)m.m_data[2].z;
}

void bqMath::Mul(const bqMat4& m, const bqVec4& v, bqVec4& r)
{
	r.x = v.x * m.m_data[0].x + v.y * m.m_data[1].x + v.z * m.m_data[2].x + v.w * m.m_data[3].x;
	r.y = v.x * m.m_data[0].y + v.y * m.m_data[1].y + v.z * m.m_data[2].y + v.w * m.m_data[3].y;
	r.z = v.x * m.m_data[0].z + v.y * m.m_data[1].z + v.z * m.m_data[2].z + v.w * m.m_data[3].z;
	r.w = v.x * m.m_data[0].w + v.y * m.m_data[1].w + v.z * m.m_data[2].w + v.w * m.m_data[3].w;
}

void bqMath::Mul(const bqQuaternion& q1, const bqQuaternion& q2, bqQuaternion& r)
{
	r.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
	r.x = q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z;
	r.x = q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x;
	r.x = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
}

void bqMath::Transpose(bqMat4& m)
{
	bqMat4 tmp;
	tmp[0u].x = m.m_data[0u].x;
	tmp[0u].y = m.m_data[1u].x;
	tmp[0u].z = m.m_data[2u].x;
	tmp[0u].w = m.m_data[3u].x;

	tmp[1u].x = m.m_data[0u].y;
	tmp[1u].y = m.m_data[1u].y;
	tmp[1u].z = m.m_data[2u].y;
	tmp[1u].w = m.m_data[3u].y;

	tmp[2u].x = m.m_data[0u].z;
	tmp[2u].y = m.m_data[1u].z;
	tmp[2u].z = m.m_data[2u].z;
	tmp[2u].w = m.m_data[3u].z;

	m.m_data[0u] = tmp[0u];
	m.m_data[1u] = tmp[1u];
	m.m_data[2u] = tmp[2u];
	m.m_data[3u] = tmp[3u];
}

//https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/matrix-inverse
void bqMath::Invert(bqMat4& m)
{
	bqMat4 mat;
	//auto ptr = m.Data();
	for (unsigned column = 0; column < 4; ++column)
	{
		// Swap row in case our pivot point is not working
		auto column_data = m.m_data[column].Data();
		if (column_data[column] == 0)
		{
			unsigned big = column;
			for (unsigned row = 0; row < 4; ++row)
			{
				auto row_data = m.m_data[row].Data();
				auto big_data = m.m_data[big].Data();
				if (fabs(row_data[column]) > fabs(big_data[column]))
					big = row;
			}
			// Print this is a singular matrix, return identity ?
			if (big == column)
				fprintf(stderr, "Singular matrix\n");
			// Swap rows                               
			else for (unsigned j = 0; j < 4; ++j)
			{
				auto big_data = m.m_data[big].Data();
				std::swap(column_data[j], big_data[j]);

				auto other_column_data = mat.m_data[column].Data();
				auto other_big_data = mat.m_data[big].Data();
				std::swap(other_column_data[j], other_big_data[j]);
			}
		}

		// Set each row in the column to 0  
		for (unsigned row = 0; row < 4; ++row)
		{
			if (row != column)
			{
				auto row_data = m.m_data[row].Data();
				bqReal coeff = row_data[column] / column_data[column];
				if (coeff != 0)
				{
					for (unsigned j = 0; j < 4; ++j)
					{
						row_data[j] -= coeff * column_data[j];

						auto other_row_data = mat.m_data[row].Data();
						auto other_column_data = mat.m_data[column].Data();
						other_row_data[j] -= coeff * other_column_data[j];
					}
					// Set the element to 0 for safety
					row_data[column] = 0;
				}
			}
		}
	}

	// Set each element of the diagonal to 1
	for (unsigned row = 0; row < 4; ++row)
	{
		for (unsigned column = 0; column < 4; ++column)
		{
			auto other_row_data = mat.m_data[row].Data();
			auto row_data = m.m_data[row].Data();
			other_row_data[column] /= row_data[row];
		}
	}
	m = mat;
}

void bqMath::LookAtLH(bqMat4& m, const bqVec3& eye, const bqVec3& center, const bqVec3& up)
{
	bqVec3 f(center - eye);
	Normalize(f);

	bqVec3 s;
	Cross(f, up, s);
	Normalize(s);

	bqVec3 u;
	Cross(s, f, u);

	m.m_data[0].x = s.x;
	m.m_data[1].x = s.y;
	m.m_data[2].x = s.z;
	m.m_data[0].y = u.x;
	m.m_data[1].y = u.y;
	m.m_data[2].y = u.z;
	m.m_data[0].z = f.x;
	m.m_data[1].z = f.y;
	m.m_data[2].z = f.z;
	m.m_data[3].x = -Dot(s, eye);
	m.m_data[3].y = -Dot(u, eye);
	m.m_data[3].z = -Dot(f, eye);
}

void bqMath::LookAtRH(bqMat4& m, const bqVec3& eye, const bqVec3& center, const bqVec3& up)
{
	bqVec3 f(center - eye);
	Normalize(f);

	bqVec3 s;
	Cross(f, up, s);
	Normalize(s);

	bqVec3 u;
	Cross(s, f, u);

	m.m_data[0].x = s.x;
	m.m_data[1].x = s.y;
	m.m_data[2].x = s.z;
	m.m_data[0].y = u.x;
	m.m_data[1].y = u.y;
	m.m_data[2].y = u.z;
	m.m_data[0].z = -f.x;
	m.m_data[1].z = -f.y;
	m.m_data[2].z = -f.z;
	m.m_data[3].x = -Dot(s, eye);
	m.m_data[3].y = -Dot(u, eye);
	m.m_data[3].z = Dot(f, eye);
}

void bqMath::PerspectiveLH(bqMat4& m, bqReal FOV, bqReal aspect, bqReal Near, bqReal Far)
{
	BQ_ASSERT_ST(FOV != 0.f);
	BQ_ASSERT_ST(aspect != 0.f);

	bqReal S = ::sin(0.5 * FOV);
	bqReal C = ::cos(0.5 * FOV);
	bqReal H = C / S;
	bqReal W = H / aspect;
	m.m_data[0] = bqVec4(W, 0., 0., 0.);
	m.m_data[1] = bqVec4(0., H, 0., 0.);
	m.m_data[2] = bqVec4(0., 0., Far / (Far - Near), 1.);
	m.m_data[3] = bqVec4(0., 0., -m.m_data[2].z * Near, 0.);
}

void bqMath::PerspectiveRH(bqMat4& m, bqReal FOV, bqReal aspect, bqReal Near, bqReal Far)
{
	BQ_ASSERT_ST(FOV != 0.f);
	BQ_ASSERT_ST(aspect != 0.f);

	bqReal S = ::sin(0.5 * FOV);
	bqReal C = ::cos(0.5 * FOV);
	bqReal H = C / S;
	bqReal W = H / aspect;
	m.m_data[0] = bqVec4(W, 0., 0., 0.);
	m.m_data[1] = bqVec4(0., H, 0., 0.);
	m.m_data[2] = bqVec4(0., 0., Far / (Near - Far), -1.);
	m.m_data[3] = bqVec4(0., 0., m.m_data[2].z * Near, 0.);
}

void bqMath::OrthoLH(bqMat4& out, float width, float height, float near, float far)
{
	BQ_ASSERT_ST(width != 0.f);
	BQ_ASSERT_ST(height != 0.f);
	BQ_ASSERT_ST(near != 0.f);
	BQ_ASSERT_ST(far != 0.f);

	float fRange = 1.0f / (far - near);
	out.m_data[0u].Set(2.f / width, 0.f, 0.f, 0.f);
	out.m_data[1u].Set(0.f, 2.f / height, 0.f, 0.f);
	out.m_data[2u].Set(0.f, 0.f, fRange, 0.f);
	out.m_data[3u].Set(0.f, 0.f, -fRange * near, 1.f);
}

void bqMath::OrthoRH(bqMat4& out, float width, float height, float near, float far)
{
	BQ_ASSERT_ST(width != 0.f);
	BQ_ASSERT_ST(height != 0.f);
	BQ_ASSERT_ST(near != 0.f);
	BQ_ASSERT_ST(far != 0.f);

	out.m_data[0u].Set(2.f / width, 0.f, 0.f, 0.f);
	out.m_data[1u].Set(0.f, 2.f / height, 0.f, 0.f);
	out.m_data[2u].Set(0.f, 0.f, 1.f / (near - far), 0.f);
	out.m_data[3u].Set(0.f, 0.f, out[2u].z * near, 1.f);
}

// based on XNA math library
void bqMath::OrthoOfCenterLH(bqMat4& out, float left, float right, float top, float bottom, float near, float far)
{
	float    ReciprocalWidth;
	float    ReciprocalHeight;

	ReciprocalWidth = 1.0f / (right - left);
	ReciprocalHeight = 1.0f / (top - bottom);

	out.m_data[0].Set(ReciprocalWidth + ReciprocalWidth, 0.0f, 0.0f, 0.0f);
	out.m_data[1].Set(0.0f, ReciprocalHeight + ReciprocalHeight, 0.0f, 0.0f);
	out.m_data[2].Set(0.0f, 0.0f, 1.0f / (far - near), 0.0f);
	out.m_data[3].Set(
		-(left + right) * ReciprocalWidth,
		-(top + bottom) * ReciprocalHeight,
		-out.m_data[2].z * near,
		1.0f);
}

// based on XNA math library
void bqMath::OrthoOfCenterRH(bqMat4& out, float left, float right, float top, float bottom, float near, float far)
{
	float    ReciprocalWidth;
	float    ReciprocalHeight;

	ReciprocalWidth = 1.0f / (right - left);
	ReciprocalHeight = 1.0f / (top - bottom);

	out.m_data[0].Set(ReciprocalWidth + ReciprocalWidth, 0.0f, 0.0f, 0.0f);
	out.m_data[1].Set(0.0f, ReciprocalHeight + ReciprocalHeight, 0.0f, 0.0f);
	out.m_data[2].Set(0.0f, 0.0f, 1.0f / (near - far), 0.0f);
	out.m_data[3].Set(
		-(left + right) * ReciprocalWidth,
		-(top + bottom) * ReciprocalHeight,
		out.m_data[2].z * near,
		1.0f);
}

float bqMath::CoordToUV(float value, float textureSz)
{
	BQ_ASSERT_ST(textureSz != 0.f);
	return value * (1.f / textureSz);
}

bqMat4 bqMath::ScaleMatrix(float v)
{
	bqMat4 sM;
	sM.m_data[0].x = v;
	sM.m_data[1].y = v;
	sM.m_data[2].z = v;
	return sM;
}

void bqMath::Reflect(bqVec3& v, const bqVec3& N)
{
	bqReal DN2 = N.Dot(v) * 2.0;
	v.x = v.x - DN2 * N.x;
	v.y = v.y - DN2 * N.y;
	v.z = v.z - DN2 * N.z;
}

void bqMath::Reflect(bqVec4& v, const bqVec4& N)
{
	bqReal DN2 = N.Dot(v) * 2.0;
	v.x = v.x - DN2 * N.x;
	v.y = v.y - DN2 * N.y;
	v.z = v.z - DN2 * N.z;
}

void bqMath::Reflect(bqVec3f& v, const bqVec3f& N)
{
	float32_t DN2 = N.Dot(v) * 2.f;
	v.x = v.x - DN2 * N.x;
	v.y = v.y - DN2 * N.y;
	v.z = v.z - DN2 * N.z;
}

void bqMath::Reflect(bqVec4f& v, const bqVec4f& N)
{
	float32_t DN2 = N.Dot(v) * 2.f;
	v.x = v.x - DN2 * N.x;
	v.y = v.y - DN2 * N.y;
	v.z = v.z - DN2 * N.z;
}

