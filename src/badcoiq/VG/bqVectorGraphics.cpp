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


#include "vg_include.h"

#include "badcoiq/common/bqColor.h"


bool bqVectorGraphicsTarget::Init(bqImage* img)
{
	BQ_ASSERT_ST(img);
	BQ_ASSERT_ST(!m_img);

	if (!m_img)
	{
		int aWidth = img->m_info.m_width;
		int aHeight = img->m_info.m_height;
		int aEdgeCount = 300;

		unsigned int bufferWidth = aWidth + 3;

		mMaskBuffer = new bq::SUBPIXEL_DATA[bufferWidth];
		if (mMaskBuffer == NULL)
			return false;
		memset(mMaskBuffer, 0, bufferWidth * sizeof(bq::SUBPIXEL_DATA));

		mWindingBuffer = new bq::NonZeroMask[bufferWidth];
		if (mWindingBuffer == NULL)
			return false;
		memset(mWindingBuffer, 0, bufferWidth * sizeof(bq::NonZeroMask));

		mEdgeTable = new bq::PolygonScanEdge * [aHeight];
		if (mEdgeTable == NULL)
			return false;
		memset(mEdgeTable, 0, aHeight * sizeof(bq::PolygonScanEdge*));

		mEdgeStorage = new bq::PolygonScanEdge[aEdgeCount];
		if (mEdgeStorage == NULL)
			return false;
		mEdgeCount = aEdgeCount;

		mWidth = aWidth;
		mBufferWidth = bufferWidth;
		mHeight = aHeight;

		mClipRect.setClip(0, 0, mWidth, mHeight, SUBPIXEL_COUNT);

		m_img = img;
		return true;
	}

	return false;
}

// ==============================================================

bqVectorGraphics::bqVectorGraphics()
{
}

bqVectorGraphics::~bqVectorGraphics()
{
}

void bqVectorGraphics::SetTransformation(const bqMat2& m)
{
	m_transformation = m;
}

void bqVectorGraphics::SetColor(const bqColor& c)
{
	m_color = c;
}

void bqVectorGraphics::SetShape(bqVectorGraphicsShape* s)
{
	BQ_ASSERT_ST(s);
	m_shape = s;
}

bqVectorGraphicsTarget* bqVectorGraphics::CreateTarget(bqImage* img)
{
	BQ_ASSERT_ST(img);
	if (img)
	{
		if (img->m_info.m_width > 0)
		{
			if (img->m_info.m_height > 0)
			{
				bqVectorGraphicsTarget* t = new bqVectorGraphicsTarget;
				if (!t->Init(img))
				{
					delete t;
					t = 0;
				}
				return t;
			}
		}
	}
	return 0;
}

void bqVectorGraphics::SetTarget(bqVectorGraphicsTarget* t)
{
	BQ_ASSERT_ST(t);
	m_target = t;
}

void bqVectorGraphics::Draw()
{
	if (m_target && m_shape)
	{
		m_target->RenderEvenOdd(m_shape, m_color, m_transformation);
	}
}

void bqVectorGraphics::DrawLine(
	const bqVec2i& p1,
	const bqVec2i& p2,
	const bqColor& col,
	float thickness)
{
	static bqVectorGraphicsShape line;
	static bool lineReady = false;
//	if (!lineReady)
//	{
//		float aVertexCount = 3;
//		float* vertices = new float[2 * aVertexCount];
//		if (!vertices)
//			return;
//		int rnd = 1;
//#define RAND_MULTIPLIER 1103515245
//#define RAND_ADDER 12345
//#define UPDATE_RAND(a) RAND_MULTIPLIER * a + RAND_ADDER
//		rnd = UPDATE_RAND(rnd);
//		int xi = rnd >> 16;
//		rnd = UPDATE_RAND(rnd);
//		int yi = rnd >> 16;
//		float aSize = 1.f;
//		float aMinStep = 1.f;
//		float aMaxStep = 2.f;
//		float x = (float)xi * aSize / float(0x7fff);
//		float y = (float)yi * aSize / float(0x7fff);
//		float minSq = aMinStep * aMinStep;
//		float maxSq = aMaxStep * aMaxStep;
//		float sizeSq = aSize * aSize;
//		int current = 0;
//		int n;
//		bqVec2i aCenter;
//		for (n = 0; n < aVertexCount; n++)
//		{
//			float xs, xt, ys, yt, lsq;
//			do
//			{
//				do
//				{
//					rnd = UPDATE_RAND(rnd);
//					xi = rnd >> 16;
//					rnd = UPDATE_RAND(rnd);
//					yi = rnd >> 16;
//
//					xs = (float)xi * aMaxStep / float(0x7fff);
//					ys = (float)yi * aMaxStep / float(0x7fff);
//
//					lsq = xs * xs + ys * ys;
//				} while (lsq > maxSq || lsq < minSq);
//
//				xt = x + xs;
//				yt = y + ys;
//
//				lsq = xt * xt + yt * yt;
//			} while (lsq > sizeSq);
//
//			x = xt;
//			y = yt;
//
//			vertices[current++] = aCenter.x + x;
//			vertices[current++] = aCenter.y + y;
//		}
//
//		float* vertexData[1] = { vertices };
//		int vertexCounts[1] = { aVertexCount };
//
//		bq::PolygonData pdata(vertexData, vertexCounts, 1);
//		bq::PolygonData* polygons[1] = { &pdata };
//
//		unsigned long colors[1] = { col.GetAsByteRed()};
//		RENDERER_FILLMODE fillmodes[1] = { 1 };
//		VectorGraphic* vg = VectorGraphic::create(polygons, colors, fillmodes, 1);
//
//		delete[] vertices;
//
//		//return vg;
//		lineReady = true;
//	}

	SetColor(col);
	SetShape(&line);
	Draw();
}


bqVectorGraphicsShape::bqVectorGraphicsShape()
{
	m_vBufSz = 3;
	m_vBuffer = new VertexData[m_vBufSz];
	
	m_vBuffer[0].mPosition.Set(0.f, 0.f);
	m_vBuffer[1].mPosition.Set(0.f, 10.f);
	m_vBuffer[2].mPosition.Set(10.f, 10.f);


}

bqVectorGraphicsShape::~bqVectorGraphicsShape()
{
	if (m_vBuffer)
		delete m_vBuffer;
}

//VertexData* bqVectorGraphicsShape::GetBuffer()
//{
//	return m_vBuffer;
//}
//
//uint32_t bqVectorGraphicsShape::GetBufSz() const
//{
//	return m_vBufSz;
//}
//

