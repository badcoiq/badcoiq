/*
BSD 2-Clause License

Copyright (c) 2023, badcoiq
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
#ifndef __BQ_COLOR_H__
#define __BQ_COLOR_H__

#include "badcoiq/math/bqMath.h"

class bqColor
{
public:
	float	m_data[4];

	bqColor()
	{
		m_data[0] = m_data[1] = m_data[2] = 0.f;
		m_data[3] = 1.f;
	}

	bqColor(float v)
	{
		m_data[0] = m_data[1] = m_data[2] = v;
		m_data[3] = 1.f;
	}

	bqColor(float r, float g, float b, float a = 1.f)
	{
		m_data[0] = r;
		m_data[1] = g;
		m_data[2] = b;
		m_data[3] = a;
	}

	bqColor(int32_t r, int32_t g, int32_t b, int32_t a = 255)
	{
		this->SetAsByteAlpha(a);
		this->SetAsByteRed(r);
		this->SetAsByteGreen(g);
		this->SetAsByteBlue(b);
	}

	bqColor(uint32_t uint_data) // 0xFF112233 ARGB
	{
		SetAsInteger(uint_data);
	}

	bqColor(int32_t int_data) // 0xFF112233 ARGB
	{
		SetAsInteger(0xFF000000 | (uint32_t)int_data);
	}

	float X() { return m_data[0]; }
	float Y() { return m_data[1]; }
	float Z() { return m_data[2]; }
	float W() { return m_data[3]; }

	const float* Data() const { return &m_data[0u]; }

	float GetRed() { return m_data[0u]; }
	float GetGreen() { return m_data[1u]; }
	float GetBlue() { return m_data[2u]; }
	float GetAlpha() { return m_data[3u]; }

	uint8_t GetAsByteRed() { return static_cast<uint8_t>(m_data[0u] * 255.); }
	uint8_t GetAsByteGreen() { return static_cast<uint8_t>(m_data[1u] * 255.); }
	uint8_t GetAsByteBlue() { return static_cast<uint8_t>(m_data[2u] * 255.); }
	uint8_t GetAsByteAlpha() { return static_cast<uint8_t>(m_data[3u] * 255.); }

	bool	operator==(const bqColor& v) const
	{
		if (m_data[0] != v.m_data[0]) return false;
		if (m_data[1] != v.m_data[1]) return false;
		if (m_data[2] != v.m_data[2]) return false;
		if (m_data[3] != v.m_data[3]) return false;
		return true;
	}

	bool	operator!=(const bqColor& v) const
	{
		if (m_data[0] != v.m_data[0]) return true;
		if (m_data[1] != v.m_data[1]) return true;
		if (m_data[2] != v.m_data[2]) return true;
		if (m_data[3] != v.m_data[3]) return true;
		return false;
	}

	void Normalize()
	{
		float len = sqrtf((m_data[0] * m_data[0]) + (m_data[1] * m_data[1]) + (m_data[2] * m_data[2]));
		if (len > 0)
		{
			len = 1.0f / len;
		}
		m_data[0] *= len;
		m_data[1] *= len;
		m_data[2] *= len;
	}

	// 0xff112233
	uint32_t getAsInteger()
	{
		return BQ_MAKEFOURCC(
			this->GetAsByteBlue(),
			this->GetAsByteGreen(),
			this->GetAsByteRed(),
			this->GetAsByteAlpha()
		);
	}

	void SetAlpha(float v) { m_data[3] = v; }
	void SetRed(float v) { m_data[0] = v; }
	void SetGreen(float v) { m_data[1] = v; }
	void SetBlue(float v) { m_data[2] = v; }

	void SetAsByteAlpha(int32_t v) { m_data[3] = static_cast<float>(v) * 0.00392156862745f; }
	void SetAsByteRed(int32_t v) { m_data[0] = static_cast<float>(v) * 0.00392156862745f; }
	void SetAsByteGreen(int32_t v) { m_data[1] = static_cast<float>(v) * 0.00392156862745f; }
	void SetAsByteBlue(int32_t v) { m_data[2] = static_cast<float>(v) * 0.00392156862745f; }

	void SetAsInteger(uint32_t v)
	{
		this->SetAsByteRed(static_cast<uint8_t>(v >> 16));
		this->SetAsByteGreen(static_cast<uint8_t>(v >> 8));
		this->SetAsByteBlue(static_cast<uint8_t>(v));
		this->SetAsByteAlpha(static_cast<uint8_t>(v >> 24));
	}

	void Set(float r, float g, float b, float a = 1.)
	{
		SetAlpha(a);
		SetRed(r);
		SetGreen(g);
		SetBlue(b);
	}

	void Set(float v, float a = 1.)
	{
		SetAlpha(a);
		SetRed(v);
		SetGreen(v);
		SetBlue(v);
	}

	void Set(const bqColor& other)
	{
		*this = other;
	}

	bqVec4f GetVec4() 
	{
		return bqVec4f(m_data[0], m_data[1], m_data[2], m_data[3]);
	}
};

namespace bq
{
	//	HTML colors
	const bqColor ColorAliceBlue = 0xffF0F8FF;
	const bqColor ColorAntiqueWhite = 0xffFAEBD7;
	const bqColor ColorAqua = 0xff00FFFF;
	const bqColor ColorAquamarine = 0xff7FFFD4;
	const bqColor ColorAzure = 0xffF0FFFF;
	const bqColor ColorBeige = 0xffF5F5DC;
	const bqColor ColorBisque = 0xffFFE4C4;
	const bqColor ColorBlack = 0xff000000;
	const bqColor ColorBlanchedAlmond = 0xffFFEBCD;
	const bqColor ColorBlue = 0xff0000FF;
	const bqColor ColorBlueViolet = 0xff8A2BE2;
	const bqColor ColorBrown = 0xffA52A2A;
	const bqColor ColorBurlyWood = 0xffDEB887;
	const bqColor ColorCadetBlue = 0xff5F9EA0;
	const bqColor ColorChartreuse = 0xff7FFF00;
	const bqColor ColorChocolate = 0xffD2691E;
	const bqColor ColorCoral = 0xffFF7F50;
	const bqColor ColorCornflowerBlue = 0xff6495ED;
	const bqColor ColorCornsilk = 0xffFFF8DC;
	const bqColor ColorCrimson = 0xffDC143C;
	const bqColor ColorCyan = 0xff00FFFF;
	const bqColor ColorDarkBlue = 0xff00008B;
	const bqColor ColorDarkCyan = 0xff008B8B;
	const bqColor ColorDarkGoldenRod = 0xffB8860B;
	const bqColor ColorDarkGray = 0xffA9A9A9;
	const bqColor ColorDarkGrey = 0xffA9A9A9;
	const bqColor ColorDarkGreen = 0xff006400;
	const bqColor ColorDarkKhaki = 0xffBDB76B;
	const bqColor ColorDarkMagenta = 0xff8B008B;
	const bqColor ColorDarkOliveGreen = 0xff556B2F;
	const bqColor ColorDarkOrange = 0xffFF8C00;
	const bqColor ColorDarkOrchid = 0xff9932CC;
	const bqColor ColorDarkRed = 0xff8B0000;
	const bqColor ColorDarkSalmon = 0xffE9967A;
	const bqColor ColorDarkSeaGreen = 0xff8FBC8F;
	const bqColor ColorDarkSlateBlue = 0xff483D8B;
	const bqColor ColorDarkSlateGray = 0xff2F4F4F;
	const bqColor ColorDarkSlateGrey = 0xff2F4F4F;
	const bqColor ColorDarkTurquoise = 0xff00CED1;
	const bqColor ColorDarkViolet = 0xff9400D3;
	const bqColor ColorDeepPink = 0xffFF1493;
	const bqColor ColorDeepSkyBlue = 0xff00BFFF;
	const bqColor ColorDimGray = 0xff696969;
	const bqColor ColorDimGrey = 0xff696969;
	const bqColor ColorDodgerBlue = 0xff1E90FF;
	const bqColor ColorFireBrick = 0xffB22222;
	const bqColor ColorFloralWhite = 0xffFFFAF0;
	const bqColor ColorForestGreen = 0xff228B22;
	const bqColor ColorFuchsia = 0xffFF00FF;
	const bqColor ColorGainsboro = 0xffDCDCDC;
	const bqColor ColorGhostWhite = 0xffF8F8FF;
	const bqColor ColorGold = 0xffFFD700;
	const bqColor ColorGoldenRod = 0xffDAA520;
	const bqColor ColorGray = 0xff808080;
	const bqColor ColorGrey = 0xff808080;
	const bqColor ColorGreen = 0xff008000;
	const bqColor ColorGreenYellow = 0xffADFF2F;
	const bqColor ColorHoneyDew = 0xffF0FFF0;
	const bqColor ColorHotPink = 0xffFF69B4;
	const bqColor ColorIndianRed = 0xffCD5C5C;
	const bqColor ColorIndigo = 0xff4B0082;
	const bqColor ColorIvory = 0xffFFFFF0;
	const bqColor ColorKhaki = 0xffF0E68C;
	const bqColor ColorLavender = 0xffE6E6FA;
	const bqColor ColorLavenderBlush = 0xffFFF0F5;
	const bqColor ColorLawnGreen = 0xff7CFC00;
	const bqColor ColorLemonChiffon = 0xffFFFACD;
	const bqColor ColorLightBlue = 0xffADD8E6;
	const bqColor ColorLightCoral = 0xffF08080;
	const bqColor ColorLightCyan = 0xffE0FFFF;
	const bqColor ColorLightGoldenRodYellow = 0xffFAFAD2;
	const bqColor ColorLightGray = 0xffD3D3D3;
	const bqColor ColorLightGrey = 0xffD3D3D3;
	const bqColor ColorLightGreen = 0xff90EE90;
	const bqColor ColorLightPink = 0xffFFB6C1;
	const bqColor ColorLightSalmon = 0xffFFA07A;
	const bqColor ColorLightSeaGreen = 0xff20B2AA;
	const bqColor ColorLightSkyBlue = 0xff87CEFA;
	const bqColor ColorLightSlateGray = 0xff778899;
	const bqColor ColorLightSlateGrey = 0xff778899;
	const bqColor ColorLightSteelBlue = 0xffB0C4DE;
	const bqColor ColorLightYellow = 0xffFFFFE0;
	const bqColor ColorLime = 0xff00FF00;
	const bqColor ColorLimeGreen = 0xff32CD32;
	const bqColor ColorLinen = 0xffFAF0E6;
	const bqColor ColorMagenta = 0xffFF00FF;
	const bqColor ColorMaroon = 0xff800000;
	const bqColor ColorMediumAquaMarine = 0xff66CDAA;
	const bqColor ColorMediumBlue = 0xff0000CD;
	const bqColor ColorMediumOrchid = 0xffBA55D3;
	const bqColor ColorMediumPurple = 0xff9370DB;
	const bqColor ColorMediumSeaGreen = 0xff3CB371;
	const bqColor ColorMediumSlateBlue = 0xff7B68EE;
	const bqColor ColorMediumSpringGreen = 0xff00FA9A;
	const bqColor ColorMediumTurquoise = 0xff48D1CC;
	const bqColor ColorMediumVioletRed = 0xffC71585;
	const bqColor ColorMidnightBlue = 0xff191970;
	const bqColor ColorMintCream = 0xffF5FFFA;
	const bqColor ColorMistyRose = 0xffFFE4E1;
	const bqColor ColorMoccasin = 0xffFFE4B5;
	const bqColor ColorNavajoWhite = 0xffFFDEAD;
	const bqColor ColorNavy = 0xff000080;
	const bqColor ColorOldLace = 0xffFDF5E6;
	const bqColor ColorOlive = 0xff808000;
	const bqColor ColorOliveDrab = 0xff6B8E23;
	const bqColor ColorOrange = 0xffFFA500;
	const bqColor ColorOrangeRed = 0xffFF4500;
	const bqColor ColorOrchid = 0xffDA70D6;
	const bqColor ColorPaleGoldenRod = 0xffEEE8AA;
	const bqColor ColorPaleGreen = 0xff98FB98;
	const bqColor ColorPaleTurquoise = 0xffAFEEEE;
	const bqColor ColorPaleVioletRed = 0xffDB7093;
	const bqColor ColorPapayaWhip = 0xffFFEFD5;
	const bqColor ColorPeachPuff = 0xffFFDAB9;
	const bqColor ColorPeru = 0xffCD853F;
	const bqColor ColorPink = 0xffFFC0CB;
	const bqColor ColorPlum = 0xffDDA0DD;
	const bqColor ColorPowderBlue = 0xffB0E0E6;
	const bqColor ColorPurple = 0xff800080;
	const bqColor ColorRebeccaPurple = 0xff663399;
	const bqColor ColorRed = 0xffFF0000;
	const bqColor ColorRosyBrown = 0xffBC8F8F;
	const bqColor ColorRoyalBlue = 0xff4169E1;
	const bqColor ColorSaddleBrown = 0xff8B4513;
	const bqColor ColorSalmon = 0xffFA8072;
	const bqColor ColorSandyBrown = 0xffF4A460;
	const bqColor ColorSeaGreen = 0xff2E8B57;
	const bqColor ColorSeaShell = 0xffFFF5EE;
	const bqColor ColorSienna = 0xffA0522D;
	const bqColor ColorSilver = 0xffC0C0C0;
	const bqColor ColorSkyBlue = 0xff87CEEB;
	const bqColor ColorSlateBlue = 0xff6A5ACD;
	const bqColor ColorSlateGray = 0xff708090;
	const bqColor ColorSlateGrey = 0xff708090;
	const bqColor ColorSnow = 0xffFFFAFA;
	const bqColor ColorSpringGreen = 0xff00FF7F;
	const bqColor ColorSteelBlue = 0xff4682B4;
	const bqColor ColorTan = 0xffD2B48C;
	const bqColor ColorTeal = 0xff008080;
	const bqColor ColorThistle = 0xffD8BFD8;
	const bqColor ColorTomato = 0xffFF6347;
	const bqColor ColorTurquoise = 0xff40E0D0;
	const bqColor ColorViolet = 0xffEE82EE;
	const bqColor ColorWheat = 0xffF5DEB3;
	const bqColor ColorWhite = 0xffffffff;
	const bqColor ColorWhiteSmoke = 0xffF5F5F5;
	const bqColor ColorYellow = 0xffFFFF00;
	const bqColor ColorYellowGreen = 0xff9ACD32;
}

#endif

