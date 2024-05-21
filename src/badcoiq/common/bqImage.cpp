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
#include "badcoiq/common/bqImageLoader.h"
#include "badcoiq/common/bqColor.h"
#include "badcoiq/string/bqString.h"

#include <string.h>

struct r5g6b5
{
	union {
		uint16_t u16;
		uint8_t  u8[2];
	};
};
struct r8g8b8
{
	uint8_t r, g, b;
};
struct r8g8b8a8
{
	uint8_t r, g, b, a;
};

bqImage::bqImage() {}
bqImage::~bqImage()
{
	if (m_data)
		bqMemory::free(m_data);
}

// fast creating
void bqImage::Create(uint32_t x, uint32_t y)
{
	BQ_ASSERT_ST(x);
	BQ_ASSERT_ST(y);
	BQ_ASSERT_ST(!m_data);
	m_info = bqImageInfo();
	m_info.m_width = x;
	m_info.m_height = y;
	m_info.m_pitch = m_info.m_width * 4;
	m_dataSize = m_info.m_pitch * m_info.m_height;
	m_data = (uint8_t*)bqMemory::malloc(m_dataSize);
}

// must be created
void bqImage::FlipVertical()
{
	BQ_ASSERT_ST(m_data);
	uint8_t* line = nullptr;
	line = (uint8_t*)bqMemory::malloc(m_info.m_pitch);
	uint8_t* p_Up = &m_data[0u];
	uint8_t* p_Down = &m_data[m_info.m_pitch * m_info.m_height - m_info.m_pitch];
	for (uint32_t i = 0u; i < m_info.m_height / 2u; ++i)
	{
		memcpy(line, p_Up, m_info.m_pitch);
		memcpy(p_Up, p_Down, m_info.m_pitch);
		memcpy(p_Down, line, m_info.m_pitch);
		p_Up += m_info.m_pitch;
		p_Down -= m_info.m_pitch;
	}
	bqMemory::free(line);
}

// must be created
void bqImage::FlipPixel()
{
	for (uint32_t i = 0u; i < m_dataSize; )
	{
		uint8_t r = m_data[i];
		uint8_t g = m_data[i + 1u];
		uint8_t b = m_data[i + 2u];
		uint8_t a = m_data[i + 3u];
		m_data[i] = b;
		m_data[i + 1u] = g;
		m_data[i + 2u] = r;
		m_data[i + 3u] = a;
		i += 4;
	}
}

void bqImage_Fill_GetParam2UINT(const char* params, uint32_t& p1, uint32_t& p2)
{
	char* next_token1 = NULL;
	bqStringA P = params;

	char* token = strtok_s((char*)P.data(), " ", &next_token1);
	if (token)
	{
		auto i = atoi(token);
		if (i < 0)
			i = 1;
		p1 = i;
	}
	token = strtok_s(nullptr, " ", &next_token1);
	if (token)
	{
		auto i = atoi(token);
		if (i < 0)
			i = 1;
		p2 = i;
	}
}

// must be created
void bqImage::Fill(
	bqImageFillType ft,
	const bqColor& color1,
	const bqColor& color2,
	const char* params,
	bqRect* rect)
{
	uint8_t* data = m_data;
	bqStringA stra;
	if (params)
		stra.append(params);

	switch (ft)
	{
	case bqImageFillType::Solid:
	{
		for (uint32_t h = 0; h < m_info.m_height; ++h)
		{
			for (uint32_t w = 0; w < m_info.m_width; ++w)
			{
				*data = color1.GetAsByteRed();   ++data;
				*data = color1.GetAsByteGreen(); ++data;
				*data = color1.GetAsByteBlue();  ++data;
				*data = color1.GetAsByteAlpha(); ++data;
			}
		}
	}break;
	case bqImageFillType::HorizontalLines:
	{

		uint32_t thickness = 1;
		uint32_t space = 1;
		if (params)
			bqImage_Fill_GetParam2UINT(params, thickness, space);

		int curCol = 0;
		uint32_t lnCount = 1;
		uint32_t spaceCount = 1;

		bqColor color = color1;

		for (uint32_t h = 0; h < m_info.m_height; ++h)
		{
			for (uint32_t w = 0; w < m_info.m_width; ++w)
			{
				*data = color.GetAsByteRed();   ++data;
				*data = color.GetAsByteGreen(); ++data;
				*data = color.GetAsByteBlue();  ++data;
				*data = color.GetAsByteAlpha(); ++data;
			}

			if (!curCol)
			{
				if (lnCount == thickness)
				{
					++curCol;
					lnCount = 1;
				}
				else ++lnCount;
			}
			else
			{
				if (spaceCount == space)
				{
					++curCol;
					spaceCount = 1;
				}
				else ++spaceCount;
			}

			if (curCol == 2)
				curCol = 0;

			if (!curCol) // color1
				color = color1;
			else // color2
				color = color2;
		}
	}break;
	case bqImageFillType::VerticalLines:
	{
		uint32_t thickness = 1;
		uint32_t space = 1;
		if (params)
			bqImage_Fill_GetParam2UINT(params, thickness, space);

		int curCol = 0;
		uint32_t lnCount = 1;
		uint32_t spaceCount = 1;

		bqColor color = color1;

		for (uint32_t h = 0; h < m_info.m_height; ++h)
		{
			for (uint32_t w = 0; w < m_info.m_width; ++w)
			{
				if (!curCol)
				{
					if (lnCount == thickness)
					{
						++curCol;
						lnCount = 1;
					}
					else ++lnCount;
				}
				else
				{
					if (spaceCount == space)
					{
						++curCol;
						spaceCount = 1;
					}
					else ++spaceCount;
				}

				if (curCol == 2)
					curCol = 0;

				if (!curCol) // color1
					color = color1;
				else // color2
					color = color2;

				*data = color.GetAsByteRed();   ++data;
				*data = color.GetAsByteGreen(); ++data;
				*data = color.GetAsByteBlue();  ++data;
				*data = color.GetAsByteAlpha(); ++data;
			}

			lnCount = 1;
			spaceCount = 1;
			color = color1;
			curCol = 0;
		}
	}break;
	case bqImageFillType::DiagonalLines:
	{
		Fill(bqImageFillType::Solid, color2, color2, 0, rect);

		uint32_t thickness = 1;
		uint32_t space = 1;
		if (params)
			bqImage_Fill_GetParam2UINT(params, thickness, space);

		if (thickness > 1000)
			thickness = 1;
		if (space > 1000)
			space = 1;

		/*uint32_t patternSize = thickness + space;

		uint8_t* pattern = (uint8_t*)bqMemory::calloc(patternSize);
		if (pattern)
		{
			uint8_t* buf = (uint8_t*)bqMemory::calloc(m_info.m_height * m_info.m_width);
			if (buf)
			{
				uint8_t* ptr = buf;
				uint32_t curThickness = 1;
				uint32_t curSpace = 1;
				for (uint32_t w = 0; w < m_info.m_width; ++w)
				{

				}

				bqMemory::free(buf);
			}

			bqMemory::free(pattern);
		}*/



	}break;
	case bqImageFillType::Grid:
		break;
	case bqImageFillType::DiagonalGrid:
		break;
	case bqImageFillType::Percent:
		break;
	case bqImageFillType::Dashed:
		break;
	case bqImageFillType::ZigZag:
		break;
	case bqImageFillType::Wave:
		break;
	case bqImageFillType::Brick:
		break;
	case bqImageFillType::DottedGrid:
		break;
	default:
		break;
	}
}

void bqImage::ConvertTo(bqImageFormat newFormat)
{
	if (m_info.m_format == newFormat)
		return;

	switch (m_info.m_format)
	{
	case bqImageFormat::r8g8b8: {
		switch (newFormat)
		{
		case bqImageFormat::r8g8b8a8: {
			uint32_t new_pitch = m_info.m_width * 4;
			uint32_t new_dataSize = m_info.m_height * new_pitch;
			uint8_t* new_data = (uint8_t*)bqMemory::malloc(new_dataSize);

			r8g8b8* rgb_data = (r8g8b8*)m_data;
			r8g8b8a8* rgba_data = (r8g8b8a8*)new_data;
			uint32_t num_of_pixels = m_info.m_width * m_info.m_height;
			for (uint32_t i = 0; i < num_of_pixels; ++i)
			{
				rgba_data->r = rgb_data->r;
				rgba_data->g = rgb_data->g;
				rgba_data->b = rgb_data->b;
				rgba_data->a = 255;

				rgba_data++;
				rgb_data++;
			}

			bqMemory::free(m_data);

			m_data = new_data;
			m_dataSize = new_dataSize;
			m_info.m_pitch = new_pitch;
			m_info.m_format = bqImageFormat::r8g8b8a8;
			m_info.m_bits = 32;
		}break;
		default:
			bqLog::PrintWarning("bqImage::ConvertTo: dst format not implemented\n");
			break;
		}
	}break;
	case bqImageFormat::r5g6b5: {
		switch (newFormat)
		{
		case bqImageFormat::r8g8b8a8: {
			uint32_t new_pitch = m_info.m_width * 4;
			uint32_t new_dataSize = m_info.m_height * new_pitch;
			uint8_t* new_data = (uint8_t*)bqMemory::malloc(new_dataSize);

			r5g6b5* data16 = (r5g6b5*)m_data;
			r8g8b8a8* rgba_data = (r8g8b8a8*)new_data;
			uint32_t num_of_pixels = m_info.m_width * m_info.m_height;
			for (uint32_t i = 0; i < num_of_pixels; ++i)
			{
				rgba_data->r = ((data16->u16 & 0xF800) >> 11) * 8;
				rgba_data->g = uint8_t((data16->u16 & 0x7E0) >> 6) * 8;
				rgba_data->b = uint8_t((data16->u16 & 0x1F)) * 8;
				rgba_data->a = 255;

				rgba_data++;
				data16++;
			}
			bqMemory::free(m_data);
			m_data = new_data;
			m_dataSize = new_dataSize;
			m_info.m_pitch = new_pitch;
			m_info.m_format = bqImageFormat::r8g8b8a8;
			m_info.m_bits = 32;
		}break;
		default:
			bqLog::PrintWarning("bqImage::ConvertTo: dst format not implemented\n");
			break;
		}
	}break;
	case bqImageFormat::a1r5g5b5: {
		switch (newFormat)
		{
		case bqImageFormat::r8g8b8a8: {
			uint32_t new_pitch = m_info.m_width * 4;
			uint32_t new_dataSize = m_info.m_height * new_pitch;
			uint8_t* new_data = (uint8_t*)bqMemory::malloc(new_dataSize);

			r5g6b5* data16 = (r5g6b5*)m_data;
			r8g8b8a8* rgba_data = (r8g8b8a8*)new_data;
			uint32_t num_of_pixels = m_info.m_width * m_info.m_height;
			for (uint32_t i = 0; i < num_of_pixels; ++i)
			{
				rgba_data->r = ((data16->u16 & 0xFC00) >> 10) * 8;
				rgba_data->g = uint8_t((data16->u16 & 0x3E0) >> 5) * 8;
				rgba_data->b = ((data16->u16 & 0x1F)) * 8;
				rgba_data->a = (data16->u16 & 0x8000) ? 255 : 0;

				rgba_data++;
				data16++;
			}
			bqMemory::free(m_data);
			m_data = new_data;
			m_dataSize = new_dataSize;
			m_info.m_pitch = new_pitch;
			m_info.m_format = bqImageFormat::r8g8b8a8;
			m_info.m_bits = 32;
		}break;
		default:
			bqLog::PrintWarning("bqImage::ConvertTo: dst format not implemented\n");
			break;
		}
	}break;
	case bqImageFormat::x1r5g5b5: {
		switch (newFormat)
		{
		case bqImageFormat::r8g8b8a8: {
			uint32_t new_pitch = m_info.m_width * 4;
			uint32_t new_dataSize = m_info.m_height * new_pitch;
			uint8_t* new_data = (uint8_t*)bqMemory::malloc(new_dataSize);

			r5g6b5* data16 = (r5g6b5*)m_data;
			r8g8b8a8* rgba_data = (r8g8b8a8*)new_data;
			uint32_t num_of_pixels = m_info.m_width * m_info.m_height;
			for (uint32_t i = 0; i < num_of_pixels; ++i)
			{
				rgba_data->r = ((data16->u16 & 0xFC00) >> 10) * 8;
				rgba_data->g = uint8_t((data16->u16 & 0x3E0) >> 5) * 8;
				rgba_data->b = ((data16->u16 & 0x1F)) * 8;
				rgba_data->a = 255;

				rgba_data++;
				data16++;
			}
			bqMemory::free(m_data);
			m_data = new_data;
			m_dataSize = new_dataSize;
			m_info.m_pitch = new_pitch;
			m_info.m_format = bqImageFormat::r8g8b8a8;
			m_info.m_bits = 32;
		}break;
		default:
			bqLog::PrintWarning("bqImage::ConvertTo: dst format not implemented\n");
			break;
		}
	}break;
	default:
		bqLog::PrintWarning("bqImage::ConvertTo: src format not implemented\n");
		break;
	}

	m_info.m_format = newFormat;
}

void bqImage::Fill(bqColor* palette, uint8_t* data, uint32_t w, uint32_t h, uint32_t whereX, uint32_t whereY)
{
	BQ_ASSERT_ST(palette);
	BQ_ASSERT_ST(data);
	BQ_ASSERT_ST(w);
	BQ_ASSERT_ST(h);
	BQ_ASSERT_ST(m_data);
	BQ_ASSERT_ST(m_info.m_format == bqImageFormat::r8g8b8a8);

	if(whereX >= m_info.m_width)
	{
		whereX = 0;
	}

	if(whereY >= m_info.m_height)
	{
		whereY = 0;
	}

	uint8_t* dptr = m_data + ((whereX * 4) + (m_info.m_pitch * whereY));

	// остаток
	// rest
	uint32_t rx = m_info.m_width - whereX;
	uint32_t ry = m_info.m_height - whereY;
	uint32_t rxi = 0; //index
	uint32_t ryi = 0; //index


	for(uint32_t ih = 0; ih < h; ++ih)
	{
		uint8_t* line = dptr;
		uint8_t* srcLine = data;

		for(uint32_t iw = 0; iw < w; ++iw)
		{
			// data это 8битные индексы
			uint8_t palIndex = *data;

			dptr[0] = palette[palIndex].GetAsByteRed();
			dptr[1] = palette[palIndex].GetAsByteGreen();
			dptr[2] = palette[palIndex].GetAsByteBlue();
			dptr[3] = palette[palIndex].GetAsByteAlpha();

			//dptr[0] = 0;  // R
			//dptr[1] = 0;    // G
			//dptr[2] = 127;    // B
			//dptr[3] = 255;  // A

			dptr += 4;

			++data;

			++rxi;
			if (rxi == rx)
			{
				break;
			}
		}
		rxi = 0;

		// data это 8битные индексы
		data = srcLine + w;

		dptr = line + m_info.m_pitch;

		++ryi;
		if (ryi == ry)
		{
			ryi = 0;
			break;
		}
	}

}

void bqImage::Resize(uint32_t newWidth, uint32_t newHeight)
{
	BQ_ASSERT_ST(m_info.m_format == bqImageFormat::r8g8b8a8);

	if (m_info.m_format != bqImageFormat::r8g8b8a8)
	{
		bqLog::PrintWarning("Unable to resize image: format is not r8g8b8a8\n");
		return;
	}

	if (newHeight <= 0) newHeight = 1;
	if (newWidth <= 0) newWidth = 1;

	// Size does not change
	if (newHeight == m_info.m_height && newWidth == m_info.m_width) return;

	uint32_t newPitch = newWidth * 4;
	uint32_t newDataSize = newHeight * newPitch;
	auto newData = static_cast<uint8_t*>(bqMemory::malloc(newDataSize));
	auto oldDataRGBA = reinterpret_cast<r8g8b8a8*>(m_data);
	auto dataRGBA = reinterpret_cast<r8g8b8a8*>(newData);

	// Optimization precalc width indexes
	auto precalcWidthIdxs = static_cast<uint32_t*>(bqMemory::malloc(sizeof(uint32_t) * newWidth));
	auto precalcWidthFrac = static_cast<float*>(bqMemory::malloc(sizeof(float) * newWidth));
	for (uint32_t w = 0; w < newWidth; ++w)
	{
		auto oldWidth = (static_cast<float>(w)) / newWidth * (m_info.m_width - 1);
		precalcWidthIdxs[w] = static_cast<uint32_t>(oldWidth);
		precalcWidthFrac[w] = oldWidth - precalcWidthIdxs[w];
	}

	auto BiLerp = [](uint8_t c00, uint8_t c10, uint8_t c01, uint8_t c11, float fracX, float fracY) -> uint8_t
		{
			return static_cast<uint8_t>(bqMath::Lerp1(
				bqMath::Lerp1(c00, c10, fracX),
				bqMath::Lerp1(c01, c11, fracX),
				fracY));
		};

	// Create new rgba data
	for (uint32_t h = 0; h < newHeight; ++h)
	{
		auto oldHeight = (static_cast<float>(h)) / newHeight * (m_info.m_height - 1);
		auto oldHeightIndex = static_cast<uint32_t>(oldHeight);
		float heightFrac = oldHeight - oldHeightIndex;
		uint32_t offsetByHeight = oldHeightIndex * m_info.m_width;
		for (uint32_t w = 0; w < newWidth; ++w)
		{
			auto color00 = oldDataRGBA[precalcWidthIdxs[w] + offsetByHeight];
			auto color10 = oldDataRGBA[precalcWidthIdxs[w] + 1 + offsetByHeight];
			auto color01 = oldDataRGBA[precalcWidthIdxs[w] + offsetByHeight + m_info.m_width];
			auto color11 = oldDataRGBA[precalcWidthIdxs[w] + 1 + offsetByHeight + m_info.m_width];

			dataRGBA->r = BiLerp(color00.r, color10.r, color01.r, color11.r, precalcWidthFrac[w], heightFrac);
			dataRGBA->g = BiLerp(color00.g, color10.g, color01.g, color11.g, precalcWidthFrac[w], heightFrac);
			dataRGBA->b = BiLerp(color00.b, color10.b, color01.b, color11.b, precalcWidthFrac[w], heightFrac);
			dataRGBA->a = BiLerp(color00.a, color10.a, color01.a, color11.a, precalcWidthFrac[w], heightFrac);

			dataRGBA++;
		}
	}

	bqMemory::free(precalcWidthIdxs);
	bqMemory::free(precalcWidthFrac);

	// Set new data
	bqMemory::free(m_data);
	m_data = newData;
	m_dataSize = newDataSize;
	m_info.m_height = newHeight;
	m_info.m_width = newWidth;
	m_info.m_pitch = newPitch;
}