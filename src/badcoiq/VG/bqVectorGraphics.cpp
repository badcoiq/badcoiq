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

#include "bqVectorGraphicsTarget.h"

#include "badcoiq/common/bqColor.h"



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
		auto edges = m_shape->GetBuffer();
		auto edgeNum = m_shape->GetBufSz();
		for (uint32_t i = 0; i < edgeNum; ++i)
		{
			auto & edge = edges[i];
			
			m_target->
		}
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

	SetColor(col);
	SetShape(&line);
	Draw();
}


bqVectorGraphicsShape::bqVectorGraphicsShape()
{
	m_eSz = 1;
	m_edges = new bqVec4f[1];
	
	m_edges[0].Set(0.f, 0.f, 0.5f, 0.f);
}

bqVectorGraphicsShape::~bqVectorGraphicsShape()
{
	if (m_edges)
		delete m_edges;
}
