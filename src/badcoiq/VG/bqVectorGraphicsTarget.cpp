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
struct rgba
{
	uint8_t rgba[4];
};

bqVectorGraphicsTarget::bqVectorGraphicsTarget(uint32_t w, uint32_t h)
{
	if (!w) w = 1;
	if (!h) h = 1;
	m_targetWidth = w;
	
	m_img = new bqImage;
	m_img->Create(w, h);
	
	m_numPixels = w * h;
	m_masks = new uint8_t[m_numPixels];
	m_aa = new float32_t[m_numPixels];
	m_startsSz = 1000;
	m_starts = new uint32_t[m_startsSz];
	memset(m_masks, 0, m_numPixels * sizeof(uint8_t));
}

bqVectorGraphicsTarget::~bqVectorGraphicsTarget()
{
	BQ_SAFEDESTROY_A(m_starts);
	BQ_SAFEDESTROY_A(m_masks);
	BQ_SAFEDESTROY(m_img);
	BQ_SAFEDESTROY(m_aa);
}

void bqVectorGraphicsTarget::_onClear()
{
	for (uint32_t i = 0; i < m_numPixels; ++i)
	{
		m_masks[i] = 0;
		m_aa[i] = 0.f;
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

float32_t point_to_line_distance(bqPoint p,
	float32_t x1, float32_t y1, float32_t x2, float32_t y2) 
{
	// Calculate the direction vector of the line (b - a)
	float directionX = x2 - x1;
	float directionY = y2 - y1;

	// Calculate the numerator of the perpendicular distance formula
	float numerator = fabs((directionY * p.x) - (directionX * p.y) + (x2 * y1) - (y2 * x1));

	// Calculate the denominator of the perpendicular distance formula
	float denominator = sqrt((directionX * directionX) + (directionY * directionY));

	// Calculate the perpendicular distance
	float distance = numerator / denominator;

	return distance;
}

void 
intensify_pixel(int x, int y, float32_t r, rgba* data, bqColor* color)
{
	float32_t alpha = (1 - (r * r)) * 255;
	//color->m_data[3] = alpha;
	//draw_pixel(color, x, y);
	data->rgba[0] = color->GetAsByteRed();
	data->rgba[1] = color->GetAsByteGreen();
	data->rgba[2] = color->GetAsByteBlue();
	data->rgba[3] = color->GetAsByteAlpha();
}

void bqVectorGraphicsTarget::Draw(bqVectorGraphicsShape* sh)
{
	

	uint32_t startIndex = 0;

	auto edges = sh->GetBuffer();
	auto edgeNum = sh->GetBufSz();
	for (uint32_t i = 0; i < edgeNum; ++i)
	{
		printf("\nEDGE %u\n", i);
		auto & edge = edges[i];

		bqVec2f edgeDir(edge.x - edge.z, edge.y - edge.w);
		edgeDir.Normalize();

		int x0 = (edge.x);
		int y0 = (edge.y);
		int x1 = (edge.z);
		int y1 = (edge.w);

		int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
		int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
		int err = dx - dy, e2; /* error value e_xy */

		while (1) 
		{
			float32_t pld = point_to_line_distance(
				bqPoint((x0), (y0)),
				(edge.z), (edge.w),
					(edge.x), (edge.y)
			)
				//*0.95f
				;
			if (isnan(pld))pld = 0.f;
			pld = fabs(pld);

			float32_t pldMax = 0.f;
			/*{
				float ddd = point_to_line_distance(bqPoint(1,2.6),
					0,-0.1, 10, 0);
				printf("DDD %f\n", ddd);
			}*/
			//printf("(%d, %d) : [%f %f : %f %f] : %f\n", 
			//	x0, y0,
			//	edge.x,edge.y,edge.z,edge.z,
			//	pld); /* plot the point */


			auto index = x0 + (y0 * m_img->m_info.m_width);

			//if(x0 < m_img->m_info.m_width)
			if (index < m_numPixels)
			{
				if (pld > 1.f)
					pld = 1.f;
				bqVec2f pDir((float32_t)x0 - edge.z,
						(float32_t)y0 - edge.w);
				pDir.Normalize();
				bqVec2f pvec;
				if (edge.w < edge.y)
				{
					bqMath::PerpendicularVector2(edgeDir, pDir, pvec);

					float32_t ddd = pvec.Dot(pDir);
					printf("Dot %f len(%f)\n", ddd, pld);
					if (ddd < 0.f)
					{

						m_masks[index] |= mask_aa;
					//	if (edge.w < edge.y)
							m_aa[index] = 1.f - pld;
					//	else
					//		m_aa[index] = pld;
					}
					else
					{
						m_masks[index] |= mask_aa;
						//	if (edge.w < edge.y)
						m_aa[index] = pld;
						m_aa[index] = 1.f - pld;

						//	printf("d2 %f\n", ddd);
							//m_aa[index] = pld;
					}
				}
				//if (edge.w < edge.y)
				//{
				//	bqVec2f pDir((float32_t)x0 - edge.z,
				//		(float32_t)y0 - edge.w);
				//	pDir.Normalize();
				//	//float32_t ddd = edgeDir.Dot(pDir);
				//	//printf("d1 %f\n", ddd);

				//	bqVec2f pvec;
				//	bqMath::PerpendicularVector2(edgeDir, pDir, pvec);
				//	float32_t ddd = pvec.Dot(pDir);
				////	printf("d1 %f\n", ddd);
				//	if (ddd < 0.f)
				//	{
				//		m_aa[index] = 1.f - pld;
				//	}
				//	else
				//	{
				//		m_aa[index] = pld;
				//	}
				//}
				//else
				//{
				//	bqVec2f pDir((float32_t)x0 - edge.z,
				//		(float32_t)y0 - edge.w);
				//	pDir.Normalize();
				//	//float32_t ddd = edgeDir.Dot(pDir);
				//	//printf("d2 %f\n", ddd);

				//	bqVec2f pvec;
				//	bqMath::PerpendicularVector1(edgeDir, pDir, pvec);
				//	float32_t ddd = pvec.Dot(pDir);
				//	printf("d2 %f len(%f)\n", ddd, pld);

				//	if (ddd < 0.f)
				//	{
				//		m_aa[index] = pld;
				//	}
				//	else
				//	{
				//		m_aa[index] = 1.f - pld;
				//	}
				////	m_aa[index] = pld;
				//}


				if (m_aa[index] < 0.f)m_aa[index] = 0.f;

			//	m_aa[index] *= 0.75f;

				if (edge.w < edge.y)
				{
					m_masks[index] |= mask_right;
					m_starts[startIndex] = index;
					/*if (pld > pldMax)
					{
						m_masks[index] |= mask_aa;
					}*/

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
					/*if (pld > pldMax)
					{
						m_masks[index] |= mask_aa;
					}*/

					++startIndex;
					if (startIndex == m_startsSz)
					{
						break; // exit from loops
					}
				}
				else
				{
					m_masks[index] |= mask_hor;
					/*if (pld > pldMax)
					{
						m_masks[index] |= mask_aa;
					}*/
					++startIndex;
					if (startIndex == m_startsSz)
					{
						break; // exit from loops
					}
				}
			}

			if (x0 == x1 && y0 == y1) break;
			e2 = 2 * err;
			if (e2 > -dy) { err -= dy; x0 += sx; } /* e_xy+e_x > 0 */
			if (e2 < dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
		}
	}
	for (uint32_t i = 0; i < edgeNum; ++i)
	{
		//	printf("EDGE %u\n", i);
		auto& edge = edges[i];
		
		/*int x0 = floorf(edge.x);
		int y0 = floorf(edge.y);
		int x1 = floorf(edge.z);
		int y1 = floorf(edge.w);

		if (x1 < x0 && y1 < y0)
		{
			int tx1 = x0;
			int ty1 = y0;

			x0 = x1;
			y0 = y1;

			x1 = tx1;
			y1 = ty1;
		}

		int x = x0;
		int y = y0;
		int dx = x1 - x0;
		int dy = y1 - y0;
		float32_t d = 2 * dy - dx;
		float32_t D = 0;
		float32_t line_length = sqrt(dx * dx + dy * dy);
		float32_t sn = dx / line_length;
		float32_t cs = dy / line_length;
		bqColor color = m_color;
		while (x <= x1)
		{
			auto index = x + (y * m_img->m_info.m_width);
			rgba* data = (rgba*)m_img->m_data;

			intensify_pixel(x, y - 1, D + cs, &data[index], &color);
			intensify_pixel(x, y, D, &data[index], &color);
			intensify_pixel(x, y + 1, D - cs, &data[index], &color);
			x++;

			if (d <= 0) {
				D += sn;
				d = d + 2 * dy;
			}
			else {
				D = D + sn - cs;
				d = d + 2 * (dy - dx);
				y++;
			}
		}*/
	}

	//startIndex = 0;
	if (startIndex)
	{
	//	printf("startIndex %i\n", startIndex);
		for (uint32_t i = 0; i < startIndex; ++i)
		{
			auto index = m_starts[i];
			if (m_masks[index] & mask_aa)
			{
				if (m_aa[index] < 0.85f)
				{
					m_masks[index] |= mask_aa_done;
					rgba* data = (rgba*)m_img->m_data;
					bqColor oldColor;
					oldColor.SetAsByteRed(data[index].rgba[0]);
					oldColor.SetAsByteGreen(data[index].rgba[1]);
					oldColor.SetAsByteBlue(data[index].rgba[2]);
					oldColor.Blend(m_color, m_aa[index]);

					data[index].rgba[0] = oldColor.GetAsByteRed();
					data[index].rgba[1] = oldColor.GetAsByteGreen();
					data[index].rgba[2] = oldColor.GetAsByteBlue();
				}
			}
		}

		for (uint32_t i = 0; i < startIndex; ++i)
		{
			auto index = m_starts[i];

			if ((m_masks[index] & mask_hor))
			{
				rgba* data = (rgba*)m_img->m_data;
				if ((m_masks[index] & mask_aa_done) == 0)
				{
					data[index].rgba[0] = m_color.GetAsByteRed();
					data[index].rgba[1] = m_color.GetAsByteGreen();
					data[index].rgba[2] = m_color.GetAsByteBlue();
				}

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
					rgba* data2 = (rgba*)m_img->m_data;
					if ((m_masks[ii] & mask_aa_done) == 0)
					{
						data2[ii].rgba[0] = m_color.GetAsByteRed();
						data2[ii].rgba[1] = m_color.GetAsByteGreen();
						data2[ii].rgba[2] = m_color.GetAsByteBlue();
					}

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

						rgba* data2 = (rgba*)m_img->m_data;
						if ((m_masks[ii] & mask_aa_done) == 0)
						{
							data2[ii].rgba[0] = m_color.GetAsByteRed();
							data2[ii].rgba[1] = m_color.GetAsByteGreen();
							data2[ii].rgba[2] = m_color.GetAsByteBlue();
						}

						if (m_masks[ii] & mask_left)
							break;
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
	uint32_t _x1, 
	uint32_t _y1, 
	uint32_t _x2, 
	uint32_t _y2, 
	uint32_t thinkness)
{
	if (thinkness == 0)
		thinkness = 1;

	

	if (thinkness == 1)
	{
		int x0 = (_x1);
		int y0 = (_y1);
		int x1 = (_x2);
		int y1 = (_y2);

		int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
		int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
		int err = dx + dy, e2; /* error value e_xy */

		while (1)
		{
			auto index = x0 + (y0 * m_img->m_info.m_width);
			if (index < m_numPixels)
			{
				rgba* data = (rgba*)m_img->m_data;
				data[index].rgba[0] = m_color8[0];
				data[index].rgba[1] = m_color8[1];
				data[index].rgba[2] = m_color8[2];
				data[index].rgba[3] = m_color8[3];
			}

			if (x0 == x1 && y0 == y1) break;
			e2 = 2 * err;
			if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
			if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
		}

		return;
	}

	_onClear();

	float32_t thinkness_f = thinkness - 1;
	float32_t thinknessHalf = thinkness_f * 0.5f;

	bqVec4f* buffer = g_framework->m_vgshape_line->GetBuffer();
	float32_t x1 = (float32_t)_x1;
	float32_t x2 = (float32_t)_x2;
	float32_t y1 = (float32_t)_y1;
	float32_t y2 = (float32_t)_y2;
	
	bqVec2f p1;
	bqMath::PerpendicularVector2(bqVec2f(x1, y1), bqVec2f(x2, y2), p1);
	p1.Normalize();
	float32_t p1x = (p1.x);
	float32_t p1y = (p1.y);
	buffer[0].x = x1 - thinknessHalf * p1x;
	buffer[0].y = y1 - thinknessHalf * p1y;
	buffer[0].z = x2 - thinknessHalf * p1x;
	buffer[0].w = y2 - thinknessHalf * p1y;

	buffer[2].x = buffer[0].z + thinkness_f * p1x;
	buffer[2].y = buffer[0].w + thinkness_f * p1y;
	buffer[2].z = buffer[0].x + thinkness_f * p1x;
	buffer[2].w = buffer[0].y + thinkness_f * p1y;

	buffer[1].x = buffer[0].z;
	buffer[1].y = buffer[0].w;
	buffer[1].z = buffer[2].x;
	buffer[1].w = buffer[2].y;

	buffer[3].x = buffer[2].z;
	buffer[3].y = buffer[2].w;
	buffer[3].z = buffer[0].x;
	buffer[3].w = buffer[0].y;

	Draw(g_framework->m_vgshape_line);
}
