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

bqVectorGraphicsTarget::bqVectorGraphicsTarget(uint32_t w, uint32_t h)
{
	if (!w) w = 1;
	if (!h) h = 1;

	m_img = new bqImage;
	m_img->Create(w, h);
	m_numPixels = w * h;
	m_masks = new uint8_t[m_numPixels];
	memset(m_masks, 0, m_numPixels * sizeof(uint8_t));
}

bqVectorGraphicsTarget::~bqVectorGraphicsTarget()
{
	BQ_SAFEDESTROY_A(m_masks);
	BQ_SAFEDESTROY(m_img);
}

//bool bqVectorGraphicsTarget::Init(bqImage* i)
//{
//	BQ_ASSERT_ST(i);
//	BQ_ASSERT_ST(i->m_info.m_width);
//	BQ_ASSERT_ST(i->m_info.m_height);
//	m_img = i;
//	m_width = i->m_info.m_width;
//	m_height = i->m_info.m_height;
//	m_numPixels = m_width * m_height;
//	m_masks = new uint8_t[m_numPixels];
//	memset(m_masks, 0, m_numPixels * sizeof(uint8_t));
//
//	return true;
//}

void bqVectorGraphicsTarget::Clear(const bqColor& c)
{
	m_img->Fill(bqImageFillType::Solid, c, c);
}

void bqVectorGraphicsTarget::Draw(bqVectorGraphicsShape* sh)
{
}
