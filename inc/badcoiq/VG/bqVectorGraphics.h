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

#pragma once
#ifndef __BQ_VECTORGRAPHICS_H__
#define __BQ_VECTORGRAPHICS_H__

#include "badcoiq/common/bqForward.h"
#include "badcoiq/common/bqColor.h"

// куда рисовать
class bqVectorGraphicsTarget;

class bqVectorGraphics
{
	bqVectorGraphicsTarget* m_target = 0;
	bqVectorGraphicsShape* m_shape = 0;
	bqColor m_color;
	bqMat2 m_transformation;
public:
	bqVectorGraphics();
	~bqVectorGraphics();

	//void SetTransformation(const bqMat2&);
	//void SetColor(const bqColor&);
	//void SetShape(bqVectorGraphicsShape*);

	bqVectorGraphicsTarget* CreateTarget(uint32_t w, uint32_t h);
	//void SetTarget(bqVectorGraphicsTarget*);
	//void Draw();
	//void DrawLine(const bqVec2i& p1, const bqVec2i& p2, const bqColor&, float thickness);
};

//class VertexData
//{
//public:
//	bqVec2f mPosition;
////	int mClipFlags;
////	int mLine;
//};

/// \brief Набор рёбер
class bqVectorGraphicsShape
{
	uint32_t m_eSz = 0;
	bqVec4f* m_edges = 0;
public:
	bqVectorGraphicsShape();
	~bqVectorGraphicsShape();

	/// \brief Освободить память
	void Clear();

	/// \brief Выделить память для n количества рёбер
	///
	/// Вернётся указатель m_edges.
	/// Ребро состоит из двух точек
	/// arr[0] = p1 p2
	/// arr[1] = p2 p3
	/// arr[2] = p3 p1
	/// в данном примере получится треугольник
	bqVec4f* Create(uint32_t n);
	
	/// \brief Создать на основе массива точек
	/// 
	/// Например для треугольника достаточно
	/// послать массив из 3х точек.
	/// Размер массива минимум 3 точки.
	void Create(bqVec2f* , uint32_t sz);

	bqVec4f* GetBuffer()
	{
		return m_edges;
	}

	uint32_t GetBufSz() const
	{
		return m_eSz;
	}

};


#endif

