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
#include "badcoiq/common/bqImage.h"
#include "badcoiq/math/bqMath.h"
#include "badcoiq/VG/bqVectorGraphics.h"

#include "badcoiq/VG/bqVectorGraphicsTarget.h"

#include "badcoiq/common/bqColor.h"



// ==============================================================

bqVectorGraphics::bqVectorGraphics()
{
}

bqVectorGraphics::~bqVectorGraphics()
{
}

//void bqVectorGraphics::SetTransformation(const bqMat2& m)
//{
//	m_transformation = m;
//}
//
//void bqVectorGraphics::SetColor(const bqColor& c)
//{
//	m_color = c;
//}
//
//void bqVectorGraphics::SetShape(bqVectorGraphicsShape* s)
//{
//	BQ_ASSERT_ST(s);
//	m_shape = s;
//}

bqVectorGraphicsTarget* bqVectorGraphics::CreateTarget(uint32_t w, uint32_t h)
{
	bqVectorGraphicsTarget* t = new bqVectorGraphicsTarget(w,h);
	return t;
}

//void bqVectorGraphics::SetTarget(bqVectorGraphicsTarget* t)
//{
//	BQ_ASSERT_ST(t);
//	m_target = t;
//}
//
//void bqVectorGraphics::Draw()
//{
//	if (m_target && m_shape)
//	{
//		auto edges = m_shape->GetBuffer();
//		auto edgeNum = m_shape->GetBufSz();
//		for (uint32_t i = 0; i < edgeNum; ++i)
//		{
//			auto & edge = edges[i];
//			
//			//m_target->
//		}
//	}
//}
//
//void bqVectorGraphics::DrawLine(
//	const bqVec2i& p1,
//	const bqVec2i& p2,
//	const bqColor& col,
//	float thickness)
//{
//	static bqVectorGraphicsShape line;
//	static bool lineReady = false;
//
//	SetColor(col);
//	SetShape(&line);
//	Draw();
//}


bqVectorGraphicsShape::bqVectorGraphicsShape()
{
//	m_eSz = 4;
//	m_edges = new bqVec4f[m_eSz];
//	
//	bqVec2f P1(-1.f, 0.f);
//	bqVec2f P2(0.f, -1);
//	bqVec2f P3(1.f, 0.f);
//
//	float angle = 90.f;
//	float sn = sin(bqMath::DegToRad(angle));
//	float cs = cos(bqMath::DegToRad(angle));
//	float32_t x0 = P1.x * cs - P1.y * sn;
//	float32_t y0 = P1.x * sn + P1.y * cs;
//	float32_t x1 = P2.x * cs - P2.y * sn;
//	float32_t y1 = P2.x * sn + P2.y * cs;
//	float32_t x2 = P3.x * cs - P3.y * sn;
//	float32_t y2 = P3.x * sn + P3.y * cs;
//
//	P1.x = x0;
//	P1.y = y0;
//	P2.x = x1;
//	P2.y = y1;
//	P3.x = x2;
//	P3.y = y2;
//
////	printf("%f %f\n", P1.x, P1.y);
////	printf("%f %f\n", P2.x, P2.y);
////	printf("%f %f\n", P3.x, P3.y);
//
//	bqVec2f scale(40.f);
//
//	P1 *= scale;
//	P2 *= scale;
//	P3 *= scale;
//	
//	bqVec2f position(50.f, 50.f);
//
//	P1 += position;
//	P2 += position;
//	P3 += position;
//
//	m_eSz = 3;
//	m_edges[0].Set(P1.x, P1.y, P2.x, P2.y);
//	m_edges[1].Set(P2.x, P2.y, P3.x, P3.y);
//	m_edges[2].Set(P3.x, P3.y, P1.x, P1.y);
}

bqVectorGraphicsShape::~bqVectorGraphicsShape()
{
	Clear();
}

void bqVectorGraphicsShape::Clear()
{
	if (m_edges)
	{
		delete[] m_edges;
		m_edges = 0;
	}
	m_eSz = 0;
}

bqVec4f* bqVectorGraphicsShape::Create(uint32_t n)
{
	Clear();
	BQ_ASSERT_ST(n);
	if (!n)
		return 0;

	m_eSz = n;
	m_edges = new bqVec4f[m_eSz];
	return m_edges;
}

void bqVectorGraphicsShape::Create(bqVec2f* arr, uint32_t sz)
{
	if(m_eSz != sz)
		Clear();
	if (sz > 2)
	{
		m_eSz = sz;
		m_edges = new bqVec4f[m_eSz];

		bqVec4f* currEdge = m_edges;
		uint32_t ai = 0;
		for (uint32_t i = 0; i < m_eSz; ++i)
		{
			uint32_t ai_next = ai + 1;
			if (ai_next == sz)
				ai_next = 0;

			bqVec2f* p1 = &arr[ai];
			bqVec2f* p2 = &arr[ai_next];

			currEdge->x = p1->x;
			currEdge->y = p1->y;
			currEdge->z = p2->x;
			currEdge->w = p2->y;

			++currEdge;
			++ai;
		}
	}
}
