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

bqVectorGraphicsTarget::bqVectorGraphicsTarget()
{
	// Scale the coordinates by SUBPIXEL_COUNT in vertical direction
	bqVec2f scale(1.f, float(SUBPIXEL_COUNT));
	//MATRIX2D_MAKESCALING(mRemappingMatrix, scale);
	mRemappingMatrix.MakeScaling(scale);

	// The sampling point for the sub-pixel is at the top right corner. This
	// adjustment moves it to the pixel center.
	bqVec2f translation((0.5f / SUBPIXEL_COUNT), -0.5f);
	mRemappingMatrix.Translate(translation);
}

bqVectorGraphicsTarget::~bqVectorGraphicsTarget()
{
	delete[] mMaskBuffer;
	delete[] mWindingBuffer;
	delete[] mEdgeTable;
	delete[] mEdgeStorage;
}

void bqVectorGraphicsTarget::RenderEvenOdd(
	const bqVectorGraphicsShape* shape,
	const bqColor& aColor,
	const bqMat2& aTransformation)
{
	mVerticalExtents.reset();
	mCurrentEdge = 0;

	bqMat2 transform = aTransformation;
	transform.Multiply(mRemappingMatrix);

	bq::ClipRectangle clipRect;
	clipRect.setClip(0, 0, m_img->m_info.m_width,
		m_img->m_info.m_height, SUBPIXEL_COUNT);
	clipRect.intersect(mClipRect);

	bool success = true;

	int subPolyCount = 1;//`` aPolygon->getSubPolygonCount();
	int n = 0;
	while (n < subPolyCount && success)
	{
	   //const SubPolygon& poly = aPolygon->getSubPolygon(n);

		//int count = poly.getVertexCount();
		int count = shape->GetBufSz();

		// The maximum amount of edges is 3 x the vertices.
		int freeCount = getFreeEdgeCount() - count * 3;
		if (freeCount < 0 &&
			!resizeEdgeStorage(-freeCount))
		{
			success = false;
		}
		else
		{
			bq::PolygonScanEdge* edges = &mEdgeStorage[mCurrentEdge];
			//int edgeCount = poly.getScanEdges(edges, transform, clipRect);
			//int edgeCount = poly.getScanEdges(edges, transform, clipRect);

			//int p;
			//for (p = 0; p < edgeCount; p++)
			//{
			//	int firstLine = edges[p].mFirstLine >> SUBPIXEL_SHIFT;
			//	int lastLine = edges[p].mLastLine >> SUBPIXEL_SHIFT;

			//	edges[p].mNextEdge = mEdgeTable[firstLine];
			//	mEdgeTable[firstLine] = &edges[p];

			//	mVerticalExtents.mark(firstLine, lastLine);
			//}
			//mCurrentEdge += edgeCount;
		}

		n++;
	}

	//if (success)
	//	fillEvenOdd(aTarget, aColor);
	//else
	//{
	//	unsigned int y;
	//	for (y = 0; y < mHeight; y++)
	//		mEdgeTable[y] = NULL;
	//}
}

bool bqVectorGraphicsTarget::resizeEdgeStorage(int aIncrement)
{
	unsigned int newCount = mEdgeCount + aIncrement;

	bq::PolygonScanEdge* newStorage = new bq::PolygonScanEdge[newCount];
	if (newStorage == NULL)
		return false;

	// Some pointer arithmetic to keep the linked lists in sync.
	// Make this 64-bit just for future compatibilty
	uint64_t diff = (uint64_t)newStorage - (uint64_t)mEdgeStorage;

	unsigned int n;
	for (n = 0; n < mHeight; n++)
	{
		if (mEdgeTable[n] != NULL)
			mEdgeTable[n] = (bq::PolygonScanEdge*)((uint64_t)mEdgeTable[n] + diff);
	}

	// Copy edge storage...
	memcpy(newStorage, mEdgeStorage, sizeof(bq::PolygonScanEdge) * mEdgeCount);

	// ...and fix the pointers.
	for (n = 0; n < mEdgeCount; n++)
	{
		bq::PolygonScanEdge* edge = &newStorage[n];
		if (edge->mNextEdge != NULL)
			edge->mNextEdge = (bq::PolygonScanEdge*)((uint64_t)edge->mNextEdge + diff);	}

	delete[] mEdgeStorage;
	mEdgeStorage = newStorage;
	mEdgeCount = newCount;

	return true;
}
