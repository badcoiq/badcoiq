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
#ifndef __BQ_QUATERNION_H__
#define __BQ_QUATERNION_H__

class bqQuaternion
{
public:
	bqQuaternion() {
		Identity();
	}

	bqQuaternion(const bqQuaternion& o) :
		x(o.x),
		y(o.y),
		z(o.z),
		w(o.w)
	{}

	bqQuaternion(float ax, float ay, float az)
	{
		this->Set(ax, ay, az);
	}

	bqQuaternion(float x, float y, float z, float w) :
		x(x),
		y(y),
		z(z),
		w(w)
	{}

	void Set(const bqVec4& v) { Set((float)v.x, (float)v.y, (float)v.z); }
	void Set(float _x, float _y, float _z)
	{
		_x *= 0.5f;
		_y *= 0.5f;
		_z *= 0.5f;
		float c1 = cos(_x);
		float c2 = cos(_y);
		float c3 = cos(_z);
		float s1 = sin(_x);
		float s2 = sin(_y);
		float s3 = sin(_z);
		w = (c1 * c2 * c3) + (s1 * s2 * s3);
		x = (s1 * c2 * c3) - (c1 * s2 * s3);
		y = (c1 * s2 * c3) + (s1 * c2 * s3);
		z = (c1 * c2 * s3) - (s1 * s2 * c3);
	}

	void Identity()
	{
		x = y = z = 0.f;
		w = 1.f;
	}

	float Length()const
	{
		return (x * x) + (y * y) + (z * z) + (w * w);
	}

	bqQuaternion& operator=(const bqQuaternion& o)
	{
		x = o.x;
		y = o.y;
		z = o.z;
		w = o.w;
		return *this;
	}

	bqQuaternion operator*(const bqQuaternion& q)const
	{
		return bqQuaternion(
			w * q.x + x * q.w + y * q.z - z * q.y,
			w * q.y + y * q.w + z * q.x - x * q.z,
			w * q.z + z * q.w + x * q.y - y * q.x,
			w * q.w - x * q.x - y * q.y - z * q.z);
	}

	bqVec3 RotateVector(const bqVec3& v)const
	{
		bqVec3 qv;
		qv.x = -x;
		qv.y = -y;
		qv.z = -z;
		float _w = w;

		auto _d1 = qv.Dot(v);
		auto _d2 = qv.Dot();
		bqVec3 cr;
		qv.Cross(v, cr);

		bqVec3 rv;
		rv.x = 2.f * _d1 * qv.x + (_w * _w - _d2) * v.x + 2.f * _w * cr.x;
		rv.y = 2.f * _d1 * qv.y + (_w * _w - _d2) * v.y + 2.f * _w * cr.y;
		rv.z = 2.f * _d1 * qv.z + (_w * _w - _d2) * v.z + 2.f * _w * cr.z;
		return rv;
	}

	void FromVector(const bqVec4& v1, const bqVec4& v2)
	{
		bqVec4 a;
		v1.Cross(v2, a);

		x = (float)a.x;
		y = (float)a.y;
		z = (float)a.z;

		// LOL
		w = 1.f + (float)(v1.Dot(v2));

		this->Normalize();
	}

	void FromVector(const bqVec3& v1, const bqVec3& v2)
	{
		bqVec3 a;
		v1.Cross(v2, a);

		x = (float)a.x;
		y = (float)a.y;
		z = (float)a.z;

		// LOL
		w = 1.f + (float)(v1.Dot(v2));

		this->Normalize();
	}

	/*void FromMatrix(const bqMat4& m)
	{
		bqReal trace = m.m_data[0].x + m.m_data[1].y + m.m_data[2].z;
		if (trace > 0)
		{
			bqReal s = 0.5 / sqrt(trace + 1.0);
			w = 0.25 / s;
			x = (m.m_data[2].y - m.m_data[1].z) * s;
			y = (m.m_data[0].z - m.m_data[2].x) * s;
			z = (m.m_data[1].x - m.m_data[0].y) * s;
		}
		else 
		{
			if (m.m_data[0].x > m.m_data[1].y && m.m_data[0].x > m.m_data[2].z)
			{
				bqReal s = 2.0 * sqrt(1.0 + m.m_data[0].x - m.m_data[1].y - m.m_data[2].z);
				w = (m.m_data[2].y - m.m_data[1].z) / s;
				x = 0.25 * s;
				y = (m.m_data[0].y + m.m_data[1].x) / s;
				z = (m.m_data[0].z + m.m_data[2].x) / s;
			}
			else if (m.m_data[1].y > m.m_data[2][2])
			{
				bqReal s = 2.0 * sqrt(1.0 + m.m_data[1].y - m.m_data[0].x - m.m_data[2].z);
				w = (m.m_data[0].z - m.m_data[2].x) / s;
				x = (m.m_data[0].y + m.m_data[1].x) / s;
				y = 0.25 * s;
				z = (m.m_data[1].z + m.m_data[2].y) / s;
			}
			else 
			{
				bqReal s = 2.0 * sqrt(1.0 + m.m_data[2].z - m.m_data[0].x - m.m_data[1].y);
				w = (m.m_data[1].x - m.m_data[0].y) / s;
				x = (m.m_data[0].z + m.m_data[2].x) / s;
				y = (m.m_data[1].z + m.m_data[2].y) / s;
				z = 0.25 * s;
			}
		}
	}*/

	bqQuaternion operator*(float s) const
	{
		return bqQuaternion(s * x, s * y, s * z, s * w);
	}

	void operator*=(const bqQuaternion& q)
	{
		x = w * q.x + x * q.w + y * q.z - z * q.y;
		y = w * q.y + y * q.w + z * q.x - x * q.z;
		z = w * q.z + z * q.w + x * q.y - y * q.x;
		w = w * q.w - x * q.x - y * q.y - z * q.z;
	}

	void operator*=(bqFloat32 v)
	{
		x *= v;
		y *= v;
		z *= v;
		w = w * v - x * v - y * v - z * v;
	}

	bool operator!=(const bqQuaternion& q)const
	{
		if (x != q.x) return true;
		if (y != q.y) return true;
		if (z != q.z) return true;
		if (w != q.w) return true;
		return false;
	}

	bool operator==(const bqQuaternion& q)const
	{
		if (x != q.x) return false;
		if (y != q.y) return false;
		if (z != q.z) return false;
		if (w != q.w) return false;
		return true;
	}

	bqQuaternion operator+(const bqQuaternion& o) const
	{
		return bqQuaternion(
			x + o.x,
			y + o.y,
			z + o.z,
			w + o.w);
	}

	bqQuaternion operator-(const bqQuaternion& o) const
	{
		return bqQuaternion(
			x - o.x,
			y - o.y,
			z - o.z,
			w - o.w);
	}

	bqQuaternion operator-()
	{
		x = -x;
		y = -y;
		z = -z;
		return bqQuaternion(x, y, z, w);
	}

	float operator[](uint32_t index) const
	{
		BQ_ASSERT_ST((index < 4));
		return (&x)[index];
	}

	float& operator[](uint32_t index)
	{
		BQ_ASSERT_ST((index < 4));
		return (&x)[index];
	}

	void Normalize()
	{
		float len = this->Length();
		if (len) {
			float len2 = 1.f / len;
			x *= len2;
			y *= len2;
			z *= len2;
			w *= len2;
		}
	}

	float x, y, z, w;
	float* Data() { return &x; }
};

#endif

