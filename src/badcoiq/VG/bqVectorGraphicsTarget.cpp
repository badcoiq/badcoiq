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

bqVectorGraphicsTarget::bqVectorGraphicsTarget(uint32_t w, uint32_t h)
{
	if (!w) w = 1;
	if (!h) h = 1;
	m_targetWidth = w;
	m_img = new bqImage;
	m_img->Create(w, h);
	m_numPixels = w * h;
	m_masks = new uint8_t[m_numPixels];
	m_starts = new uint32_t[300];
	memset(m_masks, 0, m_numPixels * sizeof(uint8_t));
}

bqVectorGraphicsTarget::~bqVectorGraphicsTarget()
{
	BQ_SAFEDESTROY_A(m_starts);
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
	for (uint32_t i = 0; i < m_numPixels; ++i)
	{
		m_masks[i] = 0;
	}

	for (uint32_t i = 0; i < 300; ++i)
	{
		m_starts[i] = 0;
	}
}

template <typename T>
void bqSwap(T& o1, T& o2)
{
	T t = o1;
	o2 = o1;
	o1 = t;
}

void bqVectorGraphicsTarget::Draw(bqVectorGraphicsShape* sh)
{
	struct rgba
	{
		uint8_t rgba[4];
	};

	uint32_t startIndex = 0;

	auto edges = sh->GetBuffer();
	auto edgeNum = sh->GetBufSz();
	for (uint32_t i = 0; i < edgeNum; ++i)
	{
		printf("EDGE %u\n", i);

		auto & edge = edges[i];

		float32_t xLn = fabs(edge.x - edge.z)+1;
		float32_t yLn = fabs(edge.y - edge.w)+1;
		
		
		float32_t t = 0.f;
		float32_t tt = 0.f;
		int32_t ss = 0;
		if (xLn > yLn)
		{
			ss = ceilf(xLn)+1;
			t = 1.f / (xLn);
		}
		else
		{
			ss = ceilf(yLn)+1;
			t = 1.f / (yLn);
		}

		int32_t iX, iY, iXo, iYo;
		iX = iY = 0;
		iXo = iYo = -1;

		for (int32_t fi = 0; fi < ss; ++fi)
		{
			float32_t x = bqMath::Lerp2(edge.x, edge.z, tt);
			float32_t y = bqMath::Lerp2(edge.y, edge.w, tt);
			iX = floorf(x);
			iY = floorf(y);
			printf("iX:%i iY:%i x:%f y:%f tt:%f t:%f\n", iX, iY, x, y, tt, t);

			if ((iX != iXo) || (iY != iYo))
			{
				auto index = iX + (iY * m_img->m_info.m_width);
				/*rgba* data = (rgba*)m_img->m_data;
				data[index].rgba[0] = 0;
				data[index].rgba[1] = 0;
				data[index].rgba[2] = 0;`
				data[index].rgba[3] = 255;*/

				if (edge.w < edge.y)
				{
					//if ((m_masks[index] & mask_start) == 0)
					{
						m_masks[index] |= mask_right;
						m_starts[startIndex] = index;
						++startIndex;
						if (startIndex == 300)
						{
							fi = ss; // exit from loops
							i = edgeNum;
						}
					}
					//else
					//{
					////	m_masks[index] |= mask_stop;
					//}
				}
				else if (edge.w > edge.y)
				{
					//if ((m_masks[index] & mask_start) == 0)
					//m_masks[index] |= mask_stop;
					m_masks[index] |= mask_left;
				}
				else
				{
					m_masks[index] |= mask_hor;
					//m_masks[index] |= mask_start;
					//m_masks[index] |= mask_stop;
					m_starts[startIndex] = index;
					++startIndex;
					if (startIndex == 300)
					{
						fi = ss; // exit from loops
						i = edgeNum;
					}
				}

			}
			else
			{
	//			printf("Else\n");
			}

			iXo = iX;
			iYo = iY;
			
			tt += t;
			if (tt > 0.9999f)
				tt = 1.f;
		}
	}

	if (startIndex)
	{
	//	printf("startIndex %i\n", startIndex);

		for (uint32_t i = 0; i < startIndex; ++i)
		{
			auto index = m_starts[i];

			if ((m_masks[index] & mask_hor))
			{
				rgba* data = (rgba*)m_img->m_data;
				data[index].rgba[0] = 255;
				data[index].rgba[1] = 0;
				data[index].rgba[2] = 0;
				data[index].rgba[3] = 255;
			}
			else
			{
				if (m_masks[index] & mask_right)
				{
					printf("s[%u]", i);
					uint32_t liSz = 0;
					if (index == 0)
					{
						liSz = m_targetWidth;
					}
					else
					{
						auto in = index % m_targetWidth;
				//		printf("%u\n", in);
						liSz = m_targetWidth - in;
					}

					for (uint32_t li = 0; li < liSz; ++li)
					{
						uint32_t ii = index + li;
						printf(".");


						rgba* data = (rgba*)m_img->m_data;
						data[ii].rgba[0] = 0;
						data[ii].rgba[1] = 0;
						data[ii].rgba[2] = 0;
						data[ii].rgba[3] = 255;
					
						if (m_masks[ii] & mask_left)
						{
							printf("S\n");
							break;
						}
					}

				}
			}

		}
	}
}
