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
#ifndef __BQ_MATRIX_H__
#define __BQ_MATRIX_H__

template<typename T>
class bqMatrix4_t
{
public:
	bqMatrix4_t() { Identity(); }

	template<typename T2>
	bqMatrix4_t(const bqMatrix4_t<T2>& m)
	{
		m_data[0] = m.m_data[0];
		m_data[1] = m.m_data[1];
		m_data[2] = m.m_data[2];
		m_data[3] = m.m_data[3];
	}

	bqMatrix4_t(const bqQuaternion& q)
	{
		Identity();
		SetRotation(q);
	}

	bqMatrix4_t(const bqVec4_t<T>& x, const bqVec4_t<T>& y, const bqVec4_t<T>& z, const bqVec4_t<T>& w) {
		m_data[0u] = x;
		m_data[1u] = y;
		m_data[2u] = z;
		m_data[3u] = w;
	}

	void Identity() {
		auto* p = this->Data();
		p[0] = static_cast<T>(1.f);
		p[1] = static_cast<T>(0.f);
		p[2] = static_cast<T>(0.f);
		p[3] = static_cast<T>(0.f);

		p[4] = static_cast<T>(0.f);
		p[5] = static_cast<T>(1.f);
		p[6] = static_cast<T>(0.f);
		p[7] = static_cast<T>(0.f);

		p[8] = static_cast<T>(0.f);
		p[9] = static_cast<T>(0.f);
		p[10] = static_cast<T>(1.f);
		p[11] = static_cast<T>(0.f);

		p[12] = static_cast<T>(0.f);
		p[13] = static_cast<T>(0.f);
		p[14] = static_cast<T>(0.f);
		p[15] = static_cast<T>(1.f);
	}

	void Set(
		T xx, T xy, T xz,
		T yx, T yy, T yz,
		T zx, T zy, T zz)
	{
		m_data[0].Set(xx, xy, xz, 0.f);
		m_data[1].Set(yx, yy, yz, 0.f);
		m_data[2].Set(zx, zy, zz, 0.f);
	}

	void SetRotation(const bqQuaternion& q)
	{
		T d = q.Length();
		T s = 2.0f / d;
		T xs = q.x * s, ys = q.y * s, zs = q.z * s;
		T wx = q.w * xs, wy = q.w * ys, wz = q.w * zs;
		T xx = q.x * xs, xy = q.x * ys, xz = q.x * zs;
		T yy = q.y * ys, yz = q.y * zs, zz = q.z * zs;
		Set(
			1.0f - (yy + zz), xy - wz, xz + wy,
			xy + wz, 1.0f - (xx + zz), yz - wx,
			xz - wy, yz + wx, 1.0f - (xx + yy));
	}

	bqVec4_t<T>& operator[](uint32_t i) { assert(i <= 3); return m_data[i]; }
	const bqVec4_t<T>& operator[](uint32_t i) const { assert(i <= 3); return m_data[i]; }

	bqMatrix4_t<T> operator*(const bqMatrix4_t<T>& m) const
	{
		return bqMatrix4_t<T>(
			m_data[0] * m.m_data[0].x + m_data[1] * m.m_data[0].y + m_data[2] * m.m_data[0].z + m_data[3] * m.m_data[0].w,
			m_data[0] * m.m_data[1].x + m_data[1] * m.m_data[1].y + m_data[2] * m.m_data[1].z + m_data[3] * m.m_data[1].w,
			m_data[0] * m.m_data[2].x + m_data[1] * m.m_data[2].y + m_data[2] * m.m_data[2].z + m_data[3] * m.m_data[2].w,
			m_data[0] * m.m_data[3].x + m_data[1] * m.m_data[3].y + m_data[2] * m.m_data[3].z + m_data[3] * m.m_data[3].w
		);
	}

	bqMatrix4_t<T> MulBasis(const bqMatrix4_t<T>& m) const
	{
		return bqMatrix4_t<T>(
			m_data[0] * m.m_data[0].x + m_data[1] * m.m_data[0].y + m_data[2] * m.m_data[0].z + m_data[3] * 1.f,
			m_data[0] * m.m_data[1].x + m_data[1] * m.m_data[1].y + m_data[2] * m.m_data[1].z + m_data[3] * 1.f,
			m_data[0] * m.m_data[2].x + m_data[1] * m.m_data[2].y + m_data[2] * m.m_data[2].z + m_data[3] * 1.f,
			bqVec4_t<T>(0.f, 0.f, 0.f, 1.f)
			//m_data[0] * m.m_data[3].x + m_data[1] * m.m_data[3].y + m_data[2] * m.m_data[3].z + m_data[3] * 1.f
		);
	}

	bqVec4_t<T> operator*(const bqVec4_t<T>& v) const {
		return bqVec4_t<T>
			(
				v.x * m_data[0].x + v.y * m_data[1].x + v.z * m_data[2].x + v.w * m_data[2].x,
				v.x * m_data[0].y + v.y * m_data[1].y + v.z * m_data[2].y + v.w * m_data[2].y,
				v.x * m_data[0].z + v.y * m_data[1].z + v.z * m_data[2].z + v.w * m_data[2].z,
				v.x * m_data[0].w + v.y * m_data[1].w + v.z * m_data[2].w + v.w * m_data[2].w
			);
	}

	bqMatrix4_t<T>& operator*=(const bqMatrix4_t<T>& m) {
		(*this) = (*this) * m;
		return *this;
	}

	void Transpose()
	{
		bqMatrix4_t<T> tmp;
		tmp[0].x = this->m_data[0].x; //0
		tmp[0].y = this->m_data[1].x; //1
		tmp[0].z = this->m_data[2].x; //2
		tmp[0].w = this->m_data[3].x; //3

		tmp[1].x = this->m_data[0].y; //4
		tmp[1].y = this->m_data[1].y; //5
		tmp[1].z = this->m_data[2].y; //6
		tmp[1].w = this->m_data[3].y; //7

		tmp[2].x = this->m_data[0].z; //8
		tmp[2].y = this->m_data[1].z; //9
		tmp[2].z = this->m_data[2].z; //10
		tmp[2].w = this->m_data[3].z; //11

		tmp[3].x = this->m_data[0].w; //12
		tmp[3].y = this->m_data[1].w; //13
		tmp[3].z = this->m_data[2].w; //14
		tmp[3].w = this->m_data[3].w; //15
		this->m_data[0] = tmp[0];
		this->m_data[1] = tmp[1];
		this->m_data[2] = tmp[2];
		this->m_data[3] = tmp[3];
	}

	//https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/matrix-inverse
	bool Invert()
	{
		bqMatrix4_t<T> mat;
		//auto ptr = this->Data();
		for (unsigned column = 0; column < 4; ++column)
		{
			// Swap row in case our pivot point is not working
			auto column_data = m_data[column].Data();
			if (column_data[column] == 0)
			{
				unsigned big = column;
				for (unsigned row = 0; row < 4; ++row)
				{
					auto row_data = m_data[row].Data();
					auto big_data = m_data[big].Data();
					if (fabs(row_data[column]) > fabs(big_data[column]))
						big = row;
				}
				// Print this is a singular matrix, return identity ?
				if (big == column)
					fprintf(stderr, "Singular matrix\n");
				// Swap rows                               
				else for (unsigned j = 0; j < 4; ++j)
				{
					auto big_data = m_data[big].Data();
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
					auto row_data = m_data[row].Data();
					T coeff = row_data[column] / column_data[column];
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
				auto row_data = m_data[row].Data();
				other_row_data[column] /= row_data[row];
			}
		}

		*this = mat;
		return true;
	}

	bqMatrix4_t<T> GetBasis() {
		bqMatrix4_t<T> other;
		other.m_data[0].x = m_data[0].x;
		other.m_data[0].y = m_data[0].y;
		other.m_data[0].z = m_data[0].z;
		other.m_data[1].x = m_data[1].x;
		other.m_data[1].y = m_data[1].y;
		other.m_data[1].z = m_data[1].z;
		other.m_data[2].x = m_data[2].x;
		other.m_data[2].y = m_data[2].y;
		other.m_data[2].z = m_data[2].z;
		return other;
	}

	bqVec4_t<T> m_data[4];
	T* Data() { return &m_data[0].x; }
};

template<typename T>
class bqMatrix2_t
{
public:
	bqMatrix2_t() { Identity(); }

	template<typename T2>
	bqMatrix2_t(const bqMatrix2_t<T2>& m)
	{
		m_data[0] = m.m_data[0];
		m_data[1] = m.m_data[1];
	}


	void Identity() {
		m_data[0].x = 1.f;
		m_data[0].y = 0.f;
		m_data[1].x = 0.f;
		m_data[1].y = 1.f;
		m_data[2].x = 0.f;
		m_data[2].y = 0.f;
	}

	void MakeRotation(T aAngle)
	{
		T sinRot = (T)sin(aAngle);
		T cosRot = (T)cos(aAngle);

		m_data[0].x = cosRot;
		m_data[0].y = sinRot;
		m_data[1].x = -sinRot;
		m_data[1].y = cosRot;
		m_data[2].x = 0.f;
		m_data[2].y = 0.f;
	}

	void MakeTranslation(const bqVec2_t<T>& aTranslation)
	{
		m_data[0].x = 1.f;
		m_data[0].y = 0.f;
		m_data[1].x = 0.f;
		m_data[1].y = 1.f;
		m_data[2].x = aTranslation.x;
		m_data[2].y = aTranslation.y;
	}

	void MakeScaling(const bqVec2_t<T>& aScale)
	{
		m_data[0].x = aScale.x;
		m_data[0].y = 0.f;
		m_data[1].x = 0.f;
		m_data[1].y = aScale.y;
		m_data[2].x = 0.f;
		m_data[2].y = 0.f;
	}

	void Rotate(T rotate)
	{
		bqMatrix2_t<T> tmp;
		tmp.MakeRotation(rotate);
		Multiply(tmp);
	}

	void Translate(const bqVec2_t<T>& aTranslation)
	{
		bqMatrix2_t<T> tmp;
		tmp.MakeTranslation(aTranslation);
		Multiply(tmp);
	}

	void Scale(const bqVec2_t<T>& aScale)
	{
		bqMatrix2_t<T> tmp;
		tmp.MakeScaling(aScale);
		Multiply(tmp);
	}

	void Multiply(const bqMatrix2_t<T>& aMatrix)
	{
		bqMatrix2_t<T> tmp = *this;
		m_data[0].x = tmp.m_data[0].x * aMatrix.m_data[0].x +
			tmp.m_data[0].y * aMatrix.m_data[1].x;
		m_data[0].y = tmp.m_data[0].x * aMatrix.m_data[0].y +
			tmp.m_data[0].y * aMatrix.m_data[1].y;
		m_data[1].x = tmp.m_data[1].x * aMatrix.m_data[0].x +
			tmp.m_data[1].y * aMatrix.m_data[1].x;
		m_data[1].y = tmp.m_data[1].x * aMatrix.m_data[0].y +
			tmp.m_data[1].y * aMatrix.m_data[1].y;
		m_data[2].x = tmp.m_data[2].x * aMatrix.m_data[0].x +
			tmp.m_data[2].y * aMatrix.m_data[1].x +
			aMatrix.m_data[2].x;
		m_data[2].y = tmp.m_data[2].x * aMatrix.m_data[0].y +
			tmp.m_data[2].y * aMatrix.m_data[1].y +
			aMatrix.m_data[2].y;
	}

	bqVec2_t<T> m_data[3];
	T* Data() { return &m_data[0].x; }
};


#endif

