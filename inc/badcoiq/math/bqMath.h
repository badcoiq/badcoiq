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
#ifndef __BQ_MATH_H__
#define __BQ_MATH_H__

#include <cmath>
#include <limits>

#define bqInfinity std::numeric_limits<bqReal>::infinity()
#define bqEpsilon std::numeric_limits<bqReal>::epsilon()

const float PIf = static_cast<float>(3.14159265358979323846);
const float PIfHalf = static_cast<float>(3.14159265358979323846 * 0.5);
const float PIfPI = 6.2831853f;
const double PI = 3.14159265358979323846;
const double PIHalf = 3.14159265358979323846 * 0.5;
const double PIPI = 6.283185307179586476925286766559;

// Сюда будут добавляться #include вектр, матрица, кватернион

class bqMath
{
public:
	static float DegToRad(float degrees);
	static double DegToRad(double degrees);
	static float RadToDeg(float radians);
	static double RadToDeg(double radians);

	static float Abs(float);
	static double Abs(double);
	static int32_t Abs(int32_t);

	// Expect value form -1 to +1
	static float Acos(float);
	static double Acos(double);

	// Expect value form -1 to +1
	static float Asin(float);
	static double Asin(double);

	// Expect value form -2 to +2
	static float Atan(float);
	static double Atan(double);

	// Expect value form -1 to +1
	static float Atan2(float y, float x);
	static double Atan2(double y, double x);

	static float Clamp(float, float mn, float mx);
	static double Clamp(double, double mn, double mx);

	// Expect value from -PI to +PI
	// It will do this
	// if (v > 3.141f) v = 3.141f;
	// if (v < -3.141f) v = -3.141f;
	static float Cos(float);
	static double Cos(double);
	static float Sin(float);
	static double Sin(double);
	static float Tan(float);
	static double Tan(double);

	// wikipedia:
	// Imprecise method, which does not guarantee v = v1 when t = 1, due to floating-point arithmetic error.
	// This method is monotonic. This form may be used when the hardware has a native fused multiply-add instruction.
	// 
	// типа, Lerp1 не гарантирует что x станет равным y (...Lerp2 тоже)
	//  ... имеет 1 умножение. Lerp2 имеет 2 умножения.
	static float Lerp1(float x, float y, float t);
	static double Lerp1(double x, double y, double t);

	// wikipedia:
	// Precise method, which guarantees v = v1 when t = 1. This method is monotonic only when v0 * v1 < 0.
	// Lerping between same values might not produce the same value
	static float Lerp2(float x, float y, float t);
	static double Lerp2(double x, double y, double t);

	// Находится ли точка в прямоугольной области
	static bool PointInRect(const bqPoint&, const bqRect&);
};


#endif

