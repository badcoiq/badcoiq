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
#ifndef __BQ_MATH_H__
/// \cond
#define __BQ_MATH_H__
/// \endcond

#include <cmath>
#include <limits>

#define bqInfinity std::numeric_limits<bqReal>::infinity()
#define bqEpsilon std::numeric_limits<bqReal>::epsilon()

#ifdef PI
#undef PI
#endif

const float32_t PIf = static_cast<float32_t>(3.14159265358979323846);
const float32_t PIfHalf = static_cast<float32_t>(3.14159265358979323846 * 0.5);
const float32_t PIfPI = 6.2831853f;
const float64_t PI = 3.14159265358979323846;
const float64_t PIHalf = 3.14159265358979323846 * 0.5;
const float64_t PIPI = 6.283185307179586476925286766559;

// Сюда будут добавляться #include вектр, матрица, кватернион
#include "badcoiq/math/bqVector.h"
#include "badcoiq/math/bqQuaternion.h"
#include "badcoiq/math/bqMatrix.h"

const bqVec4 bqVec4FltMax = bqVec4(DBL_MAX);
const bqVec4 bqVec4FltMaxNeg = bqVec4(-DBL_MAX);
const bqVec3 bqVec3FltMax = bqVec3(DBL_MAX);
const bqVec3 bqVec3FltMaxNeg = bqVec3(-DBL_MAX);
const bqVec4 bqZeroVector4 = bqVec4(0.0, 0.0, 0.0, 0.0);
const bqVec3 bqZeroVector3 = bqVec3(0.0, 0.0, 0.0);
const bqVec3 bqUpVector3 = bqVec3(0.0, 1.0, 0.0);
const bqMat4 bqEmptyMatrix = bqMat4();

#ifdef DegToRad
#undef DegToRad
#endif

#ifdef RadToDeg
#undef RadToDeg
#endif

/// Набор статических методов
class bqMath
{
public:

	/// Градусы в радианы
	static float32_t DegToRad(float32_t degrees);
	static float64_t DegToRad(float64_t degrees);
	
	/// Радианы в градусы
	static float32_t RadToDeg(float32_t radians);
	static float64_t RadToDeg(float64_t radians);

	static float32_t Abs(float32_t);
	static float64_t Abs(float64_t);
	static int32_t Abs(int32_t);

	// Expect value form -1 to +1
	static float32_t Acos(float32_t);
	static float64_t Acos(float64_t);

	// Expect value form -1 to +1
	static float32_t Asin(float32_t);
	static float64_t Asin(float64_t);

	// Expect value form -2 to +2
	static float32_t Atan(float32_t);
	static float64_t Atan(float64_t);

	// Expect value form -1 to +1
	static float32_t Atan2(float32_t y, float32_t x);
	static float64_t Atan2(float64_t y, float64_t x);

	static float32_t Clamp(float32_t, float32_t mn, float32_t mx);
	static float64_t Clamp(float64_t, float64_t mn, float64_t mx);

	// Expect value from -PI to +PI
	// It will do this
	// if (v > 3.141f) v = 3.141f;
	// if (v < -3.141f) v = -3.141f;
	static float32_t Cos(float32_t);
	static float64_t Cos(float64_t);
	static float32_t Sin(float32_t);
	static float64_t Sin(float64_t);
	static float32_t Tan(float32_t);
	static float64_t Tan(float64_t);

	static void Cross(const bqVec3& v1, const bqVec3& v2, bqVec3& r);
	static void Cross(const bqVec3f& v1, const bqVec3f& v2, bqVec3f& r);
	static void Cross(const bqVec4& v1, const bqVec4& v2, bqVec4& r);
	static void Cross(const bqVec4f& v1, const bqVec4f& v2, bqVec4f& r);
	static void Cross(const bqVec3& v1, const bqVec4& v2, bqVec3& r);
	static void Cross(const bqVec3f& v1, const bqVec4f& v2, bqVec3f& r);

	static bqReal Distance(const bqVec3& v1, const bqVec3& v2);
	static bqReal Distance(const bqVec4& v1, const bqVec4& v2);
	static bqReal Distance(const bqVec3& v1, const bqVec4& v2);
	static float32_t Distance(const bqVec3f& v1, const bqVec3f& v2);
	static float32_t Distance(const bqVec4f& v1, const bqVec4f& v2);
	static float32_t Distance(const bqVec3f& v1, const bqVec4f& v2);

	static bqReal Dot(const bqVec3& v1, const bqVec3& v2);
	static bqReal Dot(const bqVec4& v1, const bqVec4& v2);
	static bqReal Dot(const bqVec3& v1, const bqVec4& v2);
	static float32_t Dot(const bqVec3f& v1, const bqVec3f& v2);
	static float32_t Dot(const bqVec4f& v1, const bqVec4f& v2);
	static float32_t Dot(const bqVec3f& v1, const bqVec4f& v2);
	static float32_t Dot(const bqQuaternion& v1, const bqQuaternion& v2);

	static bqReal Length(const bqVec3& v);
	static bqReal Length(const bqVec4& v);
	static float32_t Length(const bqVec3f& v);
	static float32_t Length(const bqVec4f& v);
	static float32_t Length(const bqQuaternion& v);

	static void Reflect(bqVec3& v, const bqVec3& N);
	static void Reflect(bqVec4& v, const bqVec4& N);
	static void Reflect(bqVec3f& v, const bqVec3f& N);
	static void Reflect(bqVec4f& v, const bqVec4f& N);

	// wikipedia:
	// Imprecise method, which does not guarantee v = v1 when t = 1, due to floating-point arithmetic error.
	// This method is monotonic. This form may be used when the hardware has a native fused Multiply-add instruction.
	// 
	// типа, Lerp1 не гарантирует что x станет равным y (...Lerp2 тоже)
	//  ... имеет 1 умножение. Lerp2 имеет 2 умножения.
	static float32_t Lerp1(float32_t x, float32_t y, float32_t t);
	static float64_t Lerp1(float64_t x, float64_t y, float64_t t);
	static void Lerp1(const bqVec3& x, const bqVec3& y, bqReal t, bqVec3& r);
	static void Lerp1(const bqVec3& x, const bqVec4& y, bqReal t, bqVec3& r);
	static void Lerp1(const bqVec4& x, const bqVec4& y, bqReal t, bqVec4& r);
	static void Lerp1(const bqVec3f& x, const bqVec3f& y, float32_t t, bqVec3f& r);
	static void Lerp1(const bqVec3f& x, const bqVec4f& y, float32_t t, bqVec3f& r);
	static void Lerp1(const bqVec4f& x, const bqVec4f& y, float32_t t, bqVec4f& r);
	static void Lerp1(const bqQuaternion& x, const bqQuaternion& y, float32_t t, bqQuaternion& r);

	// wikipedia:
	// Precise method, which guarantees v = v1 when t = 1. This method is monotonic only when v0 * v1 < 0.
	// Lerping between same values might not produce the same value
	static float32_t Lerp2(float32_t x, float32_t y, float32_t t);
	static float64_t Lerp2(float64_t x, float64_t y, float64_t t);
	static void Lerp2(const bqVec3& x, const bqVec3& y, bqReal t, bqVec3& r);
	static void Lerp2(const bqVec3& x, const bqVec4& y, bqReal t, bqVec3& r);
	static void Lerp2(const bqVec4& x, const bqVec4& y, bqReal t, bqVec4& r);
	static void Lerp2(const bqVec3f& x, const bqVec3f& y, float32_t t, bqVec3f& r);
	static void Lerp2(const bqVec3f& x, const bqVec4f& y, float32_t t, bqVec3f& r);
	static void Lerp2(const bqVec4f& x, const bqVec4f& y, float32_t t, bqVec4f& r);
	static void Lerp2(const bqQuaternion& x, const bqQuaternion& y, float32_t t, bqQuaternion& r);

	/// Находится ли точка в прямоугольной области
	static bool PointInRect(const bqPoint&, const bqRect&);
	static bool PointInRect(const bqPoint&, const bqVec4f&);
	static bool PointInRect(const bqPointf&, const bqRect&);
	static bool PointInRect(const bqPointf&, const bqVec4f&);
	static bool PointInRect(const bqVec2f&, const bqVec4f&);

	static void Normalize(bqVec3& v);
	static void Normalize(bqVec3f& v);
	static void Normalize(bqVec4& v);
	static void Normalize(bqVec4f& v);
	static void Normalize(bqQuaternion& v);

	static void SetRotation(bqQuaternion&, float32_t x, float32_t y, float32_t z);
	static void SetRotation(bqQuaternion&, const bqVec3& axis, const bqReal& _angle);
	static void SetRotation(bqQuaternion&, const bqVec3f& axis, const float32_t& _angle);
	static void SetRotation(bqQuaternion&, const bqVec4& axis, const bqReal& _angle);
	static void SetRotation(bqQuaternion&, const bqVec4f& axis, const float32_t& _angle);
	static void SetRotation(bqMat4&, const bqQuaternion&);
	// from irrlicht or assimp?
	static void Slerp(bqQuaternion& q1, bqQuaternion& q2, float32_t time, float32_t threshold, bqQuaternion& r);

	static void Mul(const bqMat4&, const bqMat4&, bqMat4&);
	static void Mul(const bqMat4&, const bqVec3&, bqVec3&);
	static void Mul(const bqMat4&, const bqVec3&, bqVec4&);
	static void Mul(const bqMat4&, const bqVec4&, bqVec4&);
	static void Mul(const bqMat4&, const bqVec4f&, bqVec4f&);
	static void Mul(const bqMat4&, const bqVec3f&, bqVec3f&);
	static void Mul(const bqQuaternion&, const bqQuaternion&, bqQuaternion&);

	static void Transpose(bqMat4&);

	static void Invert(bqMat4&);

	/// View матрица
	static void LookAtLH(bqMat4&, const bqVec3& eye, const bqVec3& center, const bqVec3& up);
	static void LookAtRH(bqMat4&, const bqVec3& eye, const bqVec3& center, const bqVec3& up);

	/// Projection матрица
	static void PerspectiveLH(bqMat4&, bqReal FOV, bqReal aspect, bqReal Near, bqReal Far);
	static void PerspectiveRH(bqMat4&, bqReal FOV, bqReal aspect, bqReal Near, bqReal Far);
	static void OrthoLH(bqMat4&, float32_t width, float32_t height, float32_t near, float32_t far);
	static void OrthoRH(bqMat4&, float32_t width, float32_t height, float32_t near, float32_t far);
	static void OrthoOfCenterLH(bqMat4&, float32_t left, float32_t right, float32_t top, float32_t bottom, float32_t near, float32_t far);
	static void OrthoOfCenterRH(bqMat4&, float32_t left, float32_t right, float32_t top, float32_t bottom, float32_t near, float32_t far);

	/// Получить текстурную координату
	/// Если картинка имеет ширину например 100 пикселей, то,
	/// value равное 100 должно вернуть 1.f
	/// textureSz не может равняться 0
	static float32_t CoordToUV(float32_t value, float32_t textureSz);

	static bqMat4 ScaleMatrix(float32_t v);
};


#endif

