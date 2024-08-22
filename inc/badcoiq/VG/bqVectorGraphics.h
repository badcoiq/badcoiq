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
#ifndef __BQ_VECTORGRAPHICS_H__
#define __BQ_VECTORGRAPHICS_H__

#include "badcoiq/common/bqForward.h"
#include "badcoiq/common/bqColor.h"


namespace bq
{
	struct NonZeroMask;
	using SUBPIXEL_DATA = unsigned char;

	//! A more optimized representation of a polygon edge.
	class PolygonScanEdge
	{
	public:
		int mLastLine;
		int mFirstLine;
		short mWinding;
		int mX;
		int mSlope;
		int mSlopeFix;
		class PolygonScanEdge* mNextEdge;
	};

    //! A class for defining a clip rectangle.
    class ClipRectangle
    {
    public:
        //! Constructor.
        ClipRectangle()
        {
            setClip(0, 0, 0, 0, 1);
        }

        //! Constructor with initialization.
        ClipRectangle(int aX, int aY, int aWidth, int aHeight, int aScale)
        {
            setClip(aX, aY, aWidth, aHeight, aScale);
        }

        //! Sets the clip values.
        void setClip(int aX, int aY, int aWidth, int aHeight, int aScale)
        {
            mMinXi = aX;
            mMinYi = aY * aScale;
            mMaxXi = aX + aWidth;
            mMaxYi = (aY + aHeight) * aScale;

            mXOffset = 0.99f / (float)aScale;

            mMinXf = (float)mMinXi + mXOffset;
            mMaxXf = (float)mMaxXi - mXOffset;

            mMinYf = (float)mMinYi;
            mMaxYf = (float)mMaxYi;
        }

        //! Intersects the clip rectangle with another clip rectangle.
        void intersect(const ClipRectangle& aClipRectangle)
        {
            if (mMinXi < aClipRectangle.mMinXi)
                mMinXi = aClipRectangle.mMinXi;
            if (mMaxXi > aClipRectangle.mMaxXi)
                mMaxXi = aClipRectangle.mMaxXi;
            if (mMinXi > mMaxXi)
                mMinXi = mMaxXi;

            if (mMinYi < aClipRectangle.mMinYi)
                mMinYi = aClipRectangle.mMinYi;
            if (mMaxYi > aClipRectangle.mMaxYi)
                mMaxYi = aClipRectangle.mMaxYi;
            if (mMinYi > mMaxYi)
                mMinYi = mMaxYi;

            mMinXf = (float)mMinXi + mXOffset;
            mMaxXf = (float)mMaxXi - mXOffset;

            mMinYf = (float)mMinYi;
            mMaxYf = (float)mMaxYi;
        }

        //! Returns the minimum X as integer.
        inline int getMinXi() const { return mMinXi; }

        //! Returns the minimum Y as integer.
        inline int getMinYi() const { return mMinYi; }

        //! Returns the maximum X as integer.
        inline int getMaxXi() const { return mMaxXi; }

        //! Returns the maximum Y as integer.
        inline int getMaxYi() const { return mMaxYi; }

        //! Returns the minimum X as rational number (typically float).
        inline float getMinXf() const { return mMinXf; }

        //! Returns the minimum Y as rational number (typically float).
        inline float getMinYf() const { return mMinYf; }

        //! Returns the maximum X as rational number (typically float).
        inline float getMaxXf() const { return mMaxXf; }

        //! Returns the maximum Y as rational number (typically float).
        inline float getMaxYf() const { return mMaxYf; }

    protected:
        int mMinXi = 0;
        int mMinYi = 0;
        int mMaxXi = 0;
        int mMaxYi = 0;
        float mMinXf = 0;
        float mMinYf = 0;
        float mMaxXf = 0;
        float mMaxYf = 0;
        float mXOffset = 0;
    };
}


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

	void SetTransformation(const bqMat2&);
	void SetColor(const bqColor&);
	void SetShape(bqVectorGraphicsShape*);

	bqVectorGraphicsTarget* CreateTarget(bqImage*);
	void SetTarget(bqVectorGraphicsTarget*);

	void Draw();
	void DrawLine(const bqVec2i& p1, const bqVec2i& p2, const bqColor&, float thickness);
};

class VertexData
{
public:
	bqVec2f mPosition;
	int mClipFlags;
	int mLine;
};

class bqVectorGraphicsShape
{
	uint32_t m_vBufSz = 0;
	VertexData* m_vBuffer = 0;
	bq::ClipRectangle m_clipRect;

public:
	bqVectorGraphicsShape();
	~bqVectorGraphicsShape();

	VertexData* GetBuffer()
	{
		return m_vBuffer;
	}

	uint32_t GetBufSz() const
	{
		return m_vBufSz;
	}

	bq::ClipRectangle& ClipRect() { return m_clipRect; }
};


#endif

