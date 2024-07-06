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
#ifndef __BQ_GUIFONT_H__
#define __BQ_GUIFONT_H__

#ifdef BQ_WITH_GUI

#include "badcoiq/containers/bqArray.h"
#include "badcoiq/math/bqMath.h"

struct bqGUIFontGlyph
{
	// символ
	char32_t m_symbol = U' ';

	// текстурные координаты
	bqVec4f m_UV;

	// прямоугольник в пикселях
	bqVec4f m_rect;

	// отступ выступ
	int32_t m_underhang = 0;
	int32_t m_overhang = 0;

	// ширина
	uint32_t m_width = 0;
	// высота
	uint32_t m_height = 0;

	// номер текстуры в шрифте который содержит этот символ.
	uint32_t m_textureSlot = 0;
};

// Шрифт
class bqGUIFont
{
	// текстуры
	bqArray<bqTexture*> m_textures;

	// символы
	bqArray<bqGUIFontGlyph*> m_glyphs;

	// максимальный размер
	bqPoint m_maxSize;

	// для скоростного получения bqGUIFontGlyph по коду
	// индекс соответствует Unicode. Максимальное значение 0x32000-1
	bqGUIFontGlyph* m_glyphMap[0x32000];
public:
	bqGUIFont();
	~bqGUIFont();

	void AddTexture(bqTexture*);
	void AddGlyph(const bqGUIFontGlyph&);
	void AddGlyph(char32_t ch, const bqVec2f& leftTop, const bqPoint& charSz, uint32_t texture, const bqPoint& textureSize);

	const bqPoint& GetMaxSize() { return m_maxSize; }
	bqGUIFontGlyph** GetGlyphMap() { return m_glyphMap; }
	bqTexture* GetTexture(uint32_t i) { return m_textures.m_data[i]; }

	//size_t GetTextSize(const char32_t*, size_t len);

	int32_t m_characterSpacing = 1;
	int32_t m_spaceSize = 1;
	int32_t m_tabSize = 12;
};

enum class bqGUIDefaultFont
{
	Text,
	Icons
};

enum class bqGUIDefaultIconID
{
	CheckboxUncheck = 1,
	CheckboxCheck,
	RadioUncheck,
	RadioCheck,
	Plus,
	Minus,
	ArrowUp,
	ArrowDown,
	ArrowRight,
	ArrowLeft,
	CloseWindow
};

#endif
#endif
