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
#ifndef __BQ_TEXTURE_H__
#define __BQ_TEXTURE_H__

#include "badcoiq/common/bqImage.h"

// Тип текстуры
enum class bqTextureType : uint32_t
{
	Texture2D,
	RTT       // Render Target Texture / FBO
};

// Если фильтр стоит CMP_...
enum class bqTextureComparisonFunc : uint32_t
{
	Never,
	Less,
	Equal,
	LessEqual,
	Greater,
	NotEqual,
	GreaterEqual,
	Always
};

// Так как текстурные координаты от 0 до 1, можно решить, как рисовать то, что за
// пределами этой области
enum class bqTextureAddressMode : uint32_t
{
	Wrap,   // текстура будет повторяться
	Mirror, // отразиться
	Clamp,  // будет растягиваться крайними пикселями
	Border, // будет залито указанным цветом (D3D11->D3D11_SAMPLER_DESC::BorderColor)
	MirrorOnce // сначала Mirror потом Clamp
};

// Фильтрация. 
// PPP самый простой, пиксельный
enum class bqTextureFilter : uint32_t
{
	// min mag mip / point linear
	PPP,
	PPL,
	PLP,
	PLL,
	LPP,
	LPL,
	LLP,
	LLL,
	ANISOTROPIC,
	
	// comparison, для PCF
	CMP_PPP,
	CMP_PPL,
	CMP_PLP,
	CMP_PLL,
	CMP_LPP,
	CMP_LPL,
	CMP_LLP,
	CMP_LLL,
	CMP_ANISOTROPIC,
};

// Вся информация о текстуре.
// Так-же заполняется при создании новой текстуры
struct bqTextureInfo
{
	// Шейдеру может понадобиться знать размер текстуры
	bqImageInfo m_imageInfo;

	bqTextureType m_type = bqTextureType::Texture2D;
	bqTextureComparisonFunc m_cmpFnc = bqTextureComparisonFunc::Always;
	bqTextureAddressMode m_adrMode = bqTextureAddressMode::Wrap;
	bqTextureFilter m_filter = bqTextureFilter::PPP;
	uint32_t m_anisotropicLevel = 1;
	bool m_generateMipmaps = false;
};

// GPU текстура
class bqTexture
{
protected:
	bqTextureInfo m_info;
public:
	bqTexture() {}
	virtual ~bqTexture() {}
	BQ_PLACEMENT_ALLOCATOR(bqTexture);

	const bqTextureInfo& GetInfo() { return m_info; }
	void SetInfo(const bqTextureInfo& ti) { m_info = ti; }
};


#endif

