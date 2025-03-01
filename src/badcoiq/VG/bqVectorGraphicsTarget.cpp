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

#include "../framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;

bqVectorGraphicsTarget::bqVectorGraphicsTarget(uint32_t w, uint32_t h)
{
	if (!w) w = 1;
	if (!h) h = 1;
	m_targetWidth = w;
	m_img = new bqImage;
	m_img->Create(w, h);
	m_numPixels = w * h;
	m_masks = new uint8_t[m_numPixels];
	m_startsSz = 1000;
	m_starts = new uint32_t[m_startsSz];
	memset(m_masks, 0, m_numPixels * sizeof(uint8_t));
}

bqVectorGraphicsTarget::~bqVectorGraphicsTarget()
{
	BQ_SAFEDESTROY_A(m_starts);
	BQ_SAFEDESTROY_A(m_masks);
	BQ_SAFEDESTROY(m_img);
}

void bqVectorGraphicsTarget::_onClear()
{
	for (uint32_t i = 0; i < m_numPixels; ++i)
	{
		m_masks[i] = 0;
	}

	for (uint32_t i = 0; i < m_startsSz; ++i)
	{
		m_starts[i] = 0;
	}
}

void bqVectorGraphicsTarget::Clear(const bqColor& c)
{
	_onClear();
	m_img->Fill(bqImageFillType::Solid, c, c);
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
	//	printf("EDGE %u\n", i);
		auto & edge = edges[i];

		int x0 = floorf(edge.x);
		int y0 = floorf(edge.y);
		int x1 = floorf(edge.z);
		int y1 = floorf(edge.w);

		int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
		int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
		int err = dx + dy, e2; /* error value e_xy */

		while (1) 
		{
			//printf("(%d, %d)\n", x0, y0); /* plot the point */
			auto index = x0 + (y0 * m_img->m_info.m_width);

			//if(x0 < m_img->m_info.m_width)
			if (index < m_numPixels)
			{

				if (edge.w < edge.y)
				{
					m_masks[index] |= mask_right;
					m_starts[startIndex] = index;
					++startIndex;
					if (startIndex == m_startsSz)
					{
						break; // exit from loops
					}
				}
				else if (edge.w > edge.y)
				{
					m_masks[index] |= mask_left;
					m_starts[startIndex] = index;
					++startIndex;
					if (startIndex == m_startsSz)
					{
						break; // exit from loops
					}
				}
				else
				{
					m_masks[index] |= mask_hor;
					++startIndex;
					if (startIndex == m_startsSz)
					{
						break; // exit from loops
					}
				}
			}

			if (x0 == x1 && y0 == y1) break;
			e2 = 2 * err;
			if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
			if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
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
				data[index].rgba[0] = m_color8[0];
				data[index].rgba[1] = m_color8[1];
				data[index].rgba[2] = m_color8[2];
				data[index].rgba[3] = m_color8[3];

				uint32_t liSz = 0;
				if (index == 0)
				{
					liSz = m_targetWidth;
				}
				else
				{
					auto in = index % m_targetWidth;
					liSz = m_targetWidth - in;
				}

				for (uint32_t li = 0; li < liSz; ++li)
				{
					uint32_t ii = index + li;
					rgba* data = (rgba*)m_img->m_data;
					data[ii].rgba[0] = m_color8[0];
					data[ii].rgba[1] = m_color8[1];
					data[ii].rgba[2] = m_color8[2];
					data[ii].rgba[3] = m_color8[3];
					if (m_masks[ii] & mask_left)
						break;
				}
			}
			else
			{
				if (m_masks[index] & mask_right )
				{
					uint32_t liSz = 0;
					if (index == 0)
					{
						liSz = m_targetWidth;
					}
					else
					{
						auto in = index % m_targetWidth;
						liSz = m_targetWidth - in;
					}

					for (uint32_t li = 0; li < liSz; ++li)
					{
						uint32_t ii = index + li;

						rgba* data = (rgba*)m_img->m_data;
						data[ii].rgba[0] = m_color8[0];
						data[ii].rgba[1] = m_color8[1];
						data[ii].rgba[2] = m_color8[2];
						data[ii].rgba[3] = m_color8[3];
					
						if (m_masks[ii] & mask_left)
						{
							break;
						}
					}

				}
			}
		}


	}
}

void bqVectorGraphicsTarget::SetColor(const bqColor& c)
{
	m_color = c;
	m_color8[0] = c.GetAsByteRed();
	m_color8[1] = c.GetAsByteGreen();
	m_color8[2] = c.GetAsByteBlue();
	m_color8[3] = c.GetAsByteAlpha();
}

void bqVectorGraphicsTarget::DrawLine(
	uint32_t x1, 
	uint32_t y1, 
	uint32_t x2, 
	uint32_t y2, 
	uint32_t thinkness)
{
	float32_t thinkness_f = thinkness;

	if (!thinkness_f)
	{
		thinkness_f = 0.5f;
	}

	if (thinkness_f >= 1.f)
		thinkness_f *= 0.5f;

	_onClear();
	bqVec4f* buffer = g_framework->m_vgshape_line->GetBuffer();

	bqVec2f p1;
	bqVec2f p2;

	bqMath::PerpendicularVector1(bqVec2f(x1, y1), bqVec2f(x2, y2), p2);
	bqMath::PerpendicularVector2(bqVec2f(x1, y1), bqVec2f(x2, y2), p1);
	p1.Normalize();
	p2.Normalize();

	float32_t p1x = p1.x * thinkness_f;
	float32_t p1y = p1.y * thinkness_f;
	float32_t p2x = p2.x * thinkness_f;
	float32_t p2y = p2.y * thinkness_f;

	buffer[0].x = (float32_t)x1 - p1x;
	buffer[0].y = (float32_t)y1 - p1y;
	buffer[0].z = (float32_t)x2 - p1x;
	buffer[0].w = (float32_t)y2 - p1y;

	buffer[1].x = (float32_t)x2 - p1x;
	buffer[1].y = (float32_t)y2 - p1y;
	buffer[1].z = (float32_t)x2 - p2x;
	buffer[1].w = (float32_t)y2 - p2y;

	buffer[2].x = (float32_t)x2 - p2x;
	buffer[2].y = (float32_t)y2 - p2y;
	buffer[2].z = (float32_t)x1 - p2x;
	buffer[2].w = (float32_t)y1 - p2y;

	buffer[3].x = (float32_t)x1 - p2x;
	buffer[3].y = (float32_t)y1 - p2y;
	buffer[3].z = (float32_t)x1 - p1x;
	buffer[3].w = (float32_t)y1 - p1y;

	Draw(g_framework->m_vgshape_line);
}
