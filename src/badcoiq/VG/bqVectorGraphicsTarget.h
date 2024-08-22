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
#ifndef _BQVGTARGET_H_
#define _BQVGTARGET_H_

class SpanExtents
{
public:
    // Marks a span. aStart and aEnd should be sorted.
    inline void mark(int aStart, int aEnd)
    {
        if (aStart < mMinimum)
            mMinimum = aStart;
        if (aEnd > mMaximum)
            mMaximum = aEnd;
    }

    // Marks a span. aStart and aEnd don't have to be sorted.
    inline void markWithSort(int aStart, int aEnd)
    {
        if (aStart <= aEnd)
            mark(aStart, aEnd);
        else
            mark(aEnd, aStart);
    }

    inline void reset()
    {
        mMinimum = 0x7fffffff;
        mMaximum = 0x80000000;
    }

    int mMinimum = 0x7fffffff;
    int mMaximum = 0x80000000;
};

class bqVectorGraphicsTarget
{
	bqImage* m_img = 0;
	bq::SUBPIXEL_DATA* mMaskBuffer = 0;
	bq::NonZeroMask* mWindingBuffer;
	bq::PolygonScanEdge** mEdgeTable;
	bq::PolygonScanEdge* mEdgeStorage;
	int mEdgeCount = 0;
	uint32_t mWidth = 0;
    uint32_t mBufferWidth = 0;
    uint32_t mHeight = 0;
	bq::ClipRectangle mClipRect;
	bqMat2 mRemappingMatrix;
	SpanExtents mVerticalExtents;
    uint32_t mCurrentEdge = 0;
   
    inline int getFreeEdgeCount()
    {
        return mEdgeCount - mCurrentEdge - 1;
    }
    bool resizeEdgeStorage(int aIncrement);
public:
	bqVectorGraphicsTarget();
	~bqVectorGraphicsTarget();


	bool Init(bqImage*);

	void RenderEvenOdd(
		const bqVectorGraphicsShape* shape,
        const bqColor& aColor,
		const bqMat2& aTransformation);


    uint32_t Width()const { return mWidth; }
    uint32_t Height()const { return mHeight; }
};

#endif
