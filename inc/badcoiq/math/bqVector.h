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
#ifndef __BQ_VECTOR_H__
#define __BQ_VECTOR_H__

template<typename T>
class bqVec2_t
{
public:
	bqVec2_t() {}
	bqVec2_t(T X, T Y) :x(X), y(Y) {}

	template <typename T2>
	bqVec2_t(T2 v) : x(static_cast<T>(v)), y(static_cast<T>(v)) {}

	template <typename T2>
	bqVec2_t(const bqVec2_t<T2>& v) : x(static_cast<T>(v.x)), y(static_cast<T>(v.y)) {}

	void Set(T X, T Y) { x = X; y = Y; }
	void Set(T v) { x = v; y = v; }

	template <typename T2>
	void Set(const bqVec2_t<T2>& v) { x = static_cast<T>(v.x); y = static_cast<T>(v.y); }

	T operator[](uint32_t index) const { BQ_ASSERT_ST(index < 2); return (&x)[index]; }
	T& operator[](uint32_t index) { BQ_ASSERT_ST(index < 2); return (&x)[index]; }

	template<typename T2>
	bqVec2_t<T2> operator*(T2 v)const { bqVec2_t<T2> r; r.x = x * v; r.y = y * v; return r; }

	template<typename T2>
	bqVec2_t<T2> operator+(const bqVec2_t<T2>& v)const { bqVec2_t<T2> r; r.x = x + v.x; r.y = y + v.y; return r; }

	template<typename T2>
	bqVec2_t<T2> operator-(const bqVec2_t<T2>& v)const { bqVec2_t<T2> r; r.x = x - v.x; r.y = y - v.y; return r; }

	template<typename T2>
	bqVec2_t<T2> operator*(const bqVec2_t<T2>& v)const { bqVec2_t<T2> r; r.x = x * v.x; r.y = y * v.y; return r; }

	template<typename T2>
	bqVec2_t<T2> operator/(const bqVec2_t<T2>& v)const { bqVec2_t<T2> r; r.x = x / v.x; r.y = y / v.y; return r; }

	bqVec2_t<T> operator-()const { bqVec2_t<T> r; r.x = -x; r.y = -y; return r; }

	template<typename T2>
	bqVec2_t<T>& operator=(const bqVec2_t<T2>& v) { x = static_cast<T>(v.x); y = static_cast<T>(v.y); }

	bool operator==(const bqVec2_t<T>& v)const {
		if (x != v.x)return false;
		if (y != v.y)return false;
		return true;
	}

	bool operator!=(const bqVec2_t<T>& v)const {
		if (x != v.x)return true;
		if (y != v.y)return true;
		return false;
	}

	template<typename T2>
	void operator+=(const bqVec2_t<T2>& v) { x += static_cast<T>(v.x); y += static_cast<T>(v.y); }

	template<typename T2>
	void operator-=(const bqVec2_t<T2>& v) { x -= static_cast<T>(v.x); y -= static_cast<T>(v.y); }

	template<typename T2>
	void operator*=(const bqVec2_t<T2>& v) { x *= static_cast<T>(v.x); y *= static_cast<T>(v.y); }

	template<typename T2>
	void operator/=(const bqVec2_t<T2>& v) { x /= static_cast<T>(v.x); y /= static_cast<T>(v.y); }

	template<typename T2>
	void operator+=(T2 v) { x += static_cast<T>(v); y += static_cast<T>(v); }

	template<typename T2>
	void operator-=(T2 v) { x -= static_cast<T>(v); y -= static_cast<T>(v); }

	template<typename T2>
	void operator/=(T2 v) { x /= static_cast<T>(v); y /= static_cast<T>(v); }

	template<typename T2>
	void operator*=(T2 v) { x *= static_cast<T>(v); y *= static_cast<T>(v); }

	void Normalize() {
		T len = std::sqrt(Dot());
		if (len > 0)
			len = 1.0 / len;
		x *= static_cast<T>(len);
		y *= static_cast<T>(len);
	}

	T Dot()const { return (x * x) + (y * y); }
	T Length() const { return std::sqrt((x * x) + (y * y)); }

	T x = static_cast<T>(0);
	T y = static_cast<T>(0);
	T* Data() { return &x; }
};

template<typename T>
class bqVec3_t
{
public:
	bqVec3_t() {}

	bqVec3_t(T X, T Y, T Z) :
		x(X),
		y(Y),
		z(Z)
	{}

	template <typename T2>
	bqVec3_t(T2 v) :
		x(static_cast<T>(v)),
		y(static_cast<T>(v)),
		z(static_cast<T>(v))
	{}

	template <typename T2>
	bqVec3_t(const bqVec3_t<T2>& v) :
		x(static_cast<T>(v.x)),
		y(static_cast<T>(v.y)),
		z(static_cast<T>(v.z))
	{}

	void Set(T X, T Y, T Z)
	{
		x = X;
		y = Y;
		z = Z;
	}

	void Set(T v) { x = v; y = v; z = v; }

	template <typename T2>
	void Set(const bqVec3_t<T2>& v)
	{
		x = static_cast<T>(v.x);
		y = static_cast<T>(v.y);
		z = static_cast<T>(v.z);
	}

	T operator[](uint32_t index) const
	{
		BQ_ASSERT_ST((index >= 0) && (index < 3));
		return (&x)[index];
	}

	T& operator[](uint32_t index)
	{
		BQ_ASSERT_ST((index >= 0) && (index < 3));
		return (&x)[index];
	}
	template<typename T2>
	bqVec3_t<T2> operator*(T2 v)const
	{
		bqVec3_t<T2> r;
		r.x = x * v;
		r.y = y * v;
		r.z = z * v;
		return r;
	}

	template<typename T2>
	bqVec3_t<T2> operator+(const bqVec3_t<T2>& v)const
	{
		bqVec3_t<T2> r;
		r.x = x + v.x;
		r.y = y + v.y;
		r.z = z + v.z;
		return r;
	}

	template<typename T2>
	bqVec3_t<T2> operator-(const bqVec3_t<T2>& v)const
	{
		bqVec3_t<T2> r;
		r.x = x - v.x;
		r.y = y - v.y;
		r.z = z - v.z;
		return r;
	}

	template<typename T2>
	bqVec3_t<T2> operator*(const bqVec3_t<T2>& v)const
	{
		bqVec3_t<T2> r;
		r.x = x * v.x;
		r.y = y * v.y;
		r.z = z * v.z;
		return r;
	}

	template<typename T2>
	bqVec3_t<T2> operator/(const bqVec3_t<T2>& v)const
	{
		bqVec3_t<T2> r;
		r.x = x / v.x;
		r.y = y / v.y;
		r.z = z / v.z;
		return r;
	}

	bqVec3_t<T> operator-()const
	{
		bqVec3_t<T> r;
		r.x = -x;
		r.y = -y;
		r.z = -z;
		return r;
	}

	template<typename T2>
	bqVec3_t<T>& operator=(const bqVec3_t<T2>& v)
	{
		x = static_cast<T>(v.x);
		y = static_cast<T>(v.y);
		z = static_cast<T>(v.z);
		return *this;
	}

	bool operator==(const bqVec3_t<T>& v)const
	{
		if (x != v.x)
			return false;
		if (y != v.y)
			return false;
		if (z != v.z)
			return false;
		return true;
	}

	bool operator!=(const bqVec3_t<T>& v)const
	{
		if (x != v.x)
			return true;
		if (y != v.y)
			return true;
		if (z != v.z)
			return true;
		return false;
	}

	template<typename T2>
	void operator+=(const bqVec3_t<T2>& v)
	{
		x += static_cast<T>(v.x);
		y += static_cast<T>(v.y);
		z += static_cast<T>(v.z);
	}
	template<typename T2>
	void operator-=(const bqVec3_t<T2>& v)
	{
		x -= static_cast<T>(v.x);
		y -= static_cast<T>(v.y);
		z -= static_cast<T>(v.z);
	}
	template<typename T2>
	void operator*=(const bqVec3_t<T2>& v)
	{
		x *= static_cast<T>(v.x);
		y *= static_cast<T>(v.y);
		z *= static_cast<T>(v.z);
	}
	template<typename T2>
	void operator/=(const bqVec3_t<T2>& v)
	{
		x /= static_cast<T>(v.x);
		y /= static_cast<T>(v.y);
		z /= static_cast<T>(v.z);
	}

	template<typename T2>
	void operator+=(T2 v)
	{
		x += static_cast<T>(v);
		y += static_cast<T>(v);
		z += static_cast<T>(v);
	}
	template<typename T2>
	void operator-=(T2 v)
	{
		x -= static_cast<T>(v);
		y -= static_cast<T>(v);
		z -= static_cast<T>(v);
	}
	template<typename T2>
	void operator/=(T2 v)
	{
		x /= static_cast<T>(v);
		y /= static_cast<T>(v);
		z /= static_cast<T>(v);
	}
	template<typename T2>
	void operator*=(T2 v)
	{
		x *= static_cast<T>(v);
		y *= static_cast<T>(v);
		z *= static_cast<T>(v);
	}

	template<typename T2>
	void Cross(const bqVec3_t<T2>& a, bqVec3_t<T>& out)const {
		out.x = (y * static_cast<T>(a.z)) - (z * static_cast<T>(a.y));
		out.y = (z * static_cast<T>(a.x)) - (x * static_cast<T>(a.z));
		out.z = (x * static_cast<T>(a.y)) - (y * static_cast<T>(a.x));
	}

	void Normalize()
	{
		T len = std::sqrt(Dot());
		if (len > 0)
			len = static_cast<T>(1.0) / len;
		x *= static_cast<T>(len);
		y *= static_cast<T>(len);
		z *= static_cast<T>(len);
	}

	T Dot()const { return (x * x) + (y * y) + (z * z); }
	T Dot(const bqVec3_t<T>& v)const { return (x * v.x) + (y * v.y) + (z * v.z); }
	T Length() const { return std::sqrt((x * x) + (y * y) + (z * z)); }

	T x = static_cast<T>(0);
	T y = static_cast<T>(0);
	T z = static_cast<T>(0);
	T* Data() { return &x; }
};

template<typename T>
class bqVec4_t
{
public:
	bqVec4_t() {}

	bqVec4_t(T X, T Y, T Z, T W) :
		x(X),
		y(Y),
		z(Z),
		w(W)
	{}

	template <typename T2>
	bqVec4_t(T2 v) :
		x(static_cast<T>(v)),
		y(static_cast<T>(v)),
		z(static_cast<T>(v)),
		w(static_cast<T>(v))
	{}

	template <typename T2>
	bqVec4_t(const bqVec4_t<T2>& v) :
		x(static_cast<T>(v.x)),
		y(static_cast<T>(v.y)),
		z(static_cast<T>(v.z)),
		w(static_cast<T>(v.w))
	{}

	template <typename T2>
	bqVec4_t(const bqVec3_t<T2>& v) :
		x(static_cast<T>(v.x)),
		y(static_cast<T>(v.y)),
		z(static_cast<T>(v.z))
	{}

	void Set(T X, T Y, T Z, T W)
	{
		x = X;
		y = Y;
		z = Z;
		w = W;
	}

	void Set(T v) { x = v; y = v; z = v; w = v; }

	template <typename T2>
	void Set(const bqVec4_t<T2>& v)
	{
		x = static_cast<T>(v.x);
		y = static_cast<T>(v.y);
		z = static_cast<T>(v.z);
		w = static_cast<T>(v.w);
	}

	T operator[](uint32_t index) const
	{
		BQ_ASSERT_ST((index >= 0) && (index < 4));
		return (&x)[index];
	}

	T& operator[](uint32_t index)
	{
		BQ_ASSERT_ST((index >= 0) && (index < 4));
		return (&x)[index];
	}

	template<typename T2>
	bqVec4_t<T2> operator*(T2 v)const
	{
		bqVec4_t<T2> r;
		r.x = x * v;
		r.y = y * v;
		r.z = z * v;
		r.w = w * v;
		return r;
	}

	template<typename T2>
	bqVec4_t<T2> operator+(const bqVec4_t<T2>& v)const
	{
		bqVec4_t<T2> r;
		r.x = x + v.x;
		r.y = y + v.y;
		r.z = z + v.z;
		r.w = w + v.w;
		return r;
	}

	template<typename T2>
	bqVec4_t<T2> operator-(const bqVec4_t<T2>& v)const
	{
		bqVec4_t<T2> r;
		r.x = x - v.x;
		r.y = y - v.y;
		r.z = z - v.z;
		r.w = w - v.w;
		return r;
	}

	template<typename T2>
	bqVec4_t<T2> operator*(const bqVec4_t<T2>& v)const
	{
		bqVec4_t<T2> r;
		r.x = x * v.x;
		r.y = y * v.y;
		r.z = z * v.z;
		r.w = w * v.w;
		return r;
	}

	template<typename T2>
	bqVec4_t<T2> operator/(const bqVec4_t<T2>& v)const
	{
		bqVec4_t<T2> r;
		r.x = x / v.x;
		r.y = y / v.y;
		r.z = z / v.z;
		r.w = w / v.w;
		return r;
	}

	bqVec4_t<T> operator-()const
	{
		bqVec4_t<T> r;
		r.x = -x;
		r.y = -y;
		r.z = -z;
		r.w = -w;
		return r;
	}

	template<typename T2>
	bqVec4_t<T>& operator=(const bqVec4_t<T2>& v)
	{
		x = static_cast<T>(v.x);
		y = static_cast<T>(v.y);
		z = static_cast<T>(v.z);
		w = static_cast<T>(v.w);
		return *this;
	}

	bool operator==(const bqVec4_t<T>& v)const
	{
		if (x != v.x)
			return false;
		if (y != v.y)
			return false;
		if (z != v.z)
			return false;
		if (w != v.w)
			return false;
		return true;
	}

	bool operator!=(const bqVec4_t<T>& v)const
	{
		if (x != v.x)
			return true;
		if (y != v.y)
			return true;
		if (z != v.z)
			return true;
		if (w != v.w)
			return true;
		return false;
	}

	template<typename T2>
	void operator+=(const bqVec4_t<T2>& v)
	{
		x += static_cast<T>(v.x);
		y += static_cast<T>(v.y);
		z += static_cast<T>(v.z);
		w += static_cast<T>(v.w);
	}
	template<typename T2>
	void operator-=(const bqVec4_t<T2>& v)
	{
		x -= static_cast<T>(v.x);
		y -= static_cast<T>(v.y);
		z -= static_cast<T>(v.z);
		w -= static_cast<T>(v.w);
	}
	template<typename T2>
	void operator*=(const bqVec4_t<T2>& v)
	{
		x *= static_cast<T>(v.x);
		y *= static_cast<T>(v.y);
		z *= static_cast<T>(v.z);
		w *= static_cast<T>(v.w);
	}
	template<typename T2>
	void operator/=(const bqVec4_t<T2>& v)
	{
		x /= static_cast<T>(v.x);
		y /= static_cast<T>(v.y);
		z /= static_cast<T>(v.z);
		w /= static_cast<T>(v.w);
	}

	template<typename T2>
	void operator+=(T2 v)
	{
		x += static_cast<T>(v);
		y += static_cast<T>(v);
		z += static_cast<T>(v);
		w += static_cast<T>(v);
	}
	template<typename T2>
	void operator-=(T2 v)
	{
		x -= static_cast<T>(v);
		y -= static_cast<T>(v);
		z -= static_cast<T>(v);
		w -= static_cast<T>(v);
	}
	template<typename T2>
	void operator/=(T2 v)
	{
		x /= static_cast<T>(v);
		y /= static_cast<T>(v);
		z /= static_cast<T>(v);
		w /= static_cast<T>(v);
	}
	template<typename T2>
	void operator*=(T2 v)
	{
		x *= static_cast<T>(v);
		y *= static_cast<T>(v);
		z *= static_cast<T>(v);
		w *= static_cast<T>(v);
	}

	template<typename T2>
	void Cross(const bqVec4_t<T2>& a, bqVec4_t<T>& out)const {
		out.x = (y * static_cast<T>(a.z)) - (z * static_cast<T>(a.y));
		out.y = (z * static_cast<T>(a.x)) - (x * static_cast<T>(a.z));
		out.z = (x * static_cast<T>(a.y)) - (y * static_cast<T>(a.x));
	}

	void Normalize()
	{
		T len = std::sqrt(Dot());
		if (len > 0)
			len = static_cast<T>(1.0) / len;
		x *= static_cast<T>(len);
		y *= static_cast<T>(len);
		z *= static_cast<T>(len);
		w *= static_cast<T>(len);
	}

	T Dot()const { return (x * x) + (y * y) + (z * z) + (w * w); }
	T Dot(const bqVec4_t<T>& v)const { return (x * v.x) + (y * v.y) + (z * v.z) + (w * v.w); }
	T Length() const { return std::sqrt((x * x) + (y * y) + (z * z) + (w * w)); }

	T x = static_cast<T>(0);
	T y = static_cast<T>(0);
	T z = static_cast<T>(0);
	T w = static_cast<T>(0);
	T* Data() { return &x; }

	bqVec3_t<T> GetVec3() { return bqVec3_t<T>(x, y, z); }

	bqRect GetRect() { return bqRect((bqRect::_type)x, (bqRect::_type)y, (bqRect::_type)z, (bqRect::_type)w); }
	bqRectf GetRectf() { return bqRectf((bqRect::_type)x, (bqRect::_type)y, (bqRect::_type)z, (bqRect::_type)w); }
};
BQ_FORCEINLINE bqVec4f operator*(const bqReal& s, const bqVec4f& v) { return v * s; }
BQ_FORCEINLINE bqVec4 operator*(const bqReal& s, const bqVec4& v) { return v * s; }


#endif

