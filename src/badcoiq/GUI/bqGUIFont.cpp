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

#ifdef BQ_WITH_GUI


#include "badcoiq/GUI/bqGUI.h"
#include "badcoiq/math/bqMath.h"

bqGUIFont::bqGUIFont()
{
	// добавляю символ, просто нолик, вызываю AddGlyph
	bqGUIFontGlyph g;
	g.m_symbol = 0;
	AddGlyph(g);

	// теперь можно дать указатели на весь m_glyphMap
	for (uint32_t i = 0; i < 0x32000; ++i)
	{
		m_glyphMap[i] = m_glyphs.m_data[0];
	}
}

bqGUIFont::~bqGUIFont()
{
	for (size_t i = 0; i < m_glyphs.m_size; ++i)
	{
		bqDestroy(m_glyphs.m_data[i]);
	}
}


void bqGUIFont::AddTexture(bqTexture* t)
{
	BQ_ASSERT_ST(t);
	m_textures.push_back(t);
}

void bqGUIFont::AddGlyph(const bqGUIFontGlyph& g)
{
	// создать новый
	bqGUIFontGlyph* newG = bqCreate<bqGUIFontGlyph>();
	// копировать данные
	*newG = g;
	// сохранить их
	m_glyphs.push_back(newG);
	// сохранить адрес
	m_glyphMap[g.m_symbol] = newG;

	if ((int32_t)g.m_height > m_maxSize.y) m_maxSize.y = g.m_height;
	if ((int32_t)g.m_width > m_maxSize.x) m_maxSize.x = g.m_width;
}

void bqGUIFont::AddGlyph(char32_t ch, const bqVec2f& leftTop, const bqPoint& charSz, uint32_t texture, const bqPoint& texSz)
{
	bqGUIFontGlyph glyph;
	glyph.m_symbol = ch;
	glyph.m_width = charSz.x;
	glyph.m_height = charSz.y;
	glyph.m_textureSlot = texture;
	glyph.m_UV.x = bqMath::CoordToUV(leftTop.x, (float)texSz.x);
	glyph.m_UV.y = bqMath::CoordToUV(leftTop.y, (float)texSz.y);
	glyph.m_UV.z = bqMath::CoordToUV(leftTop.x + (float)charSz.x, (float)texSz.x);
	glyph.m_UV.w = bqMath::CoordToUV(leftTop.y + (float)charSz.y, (float)texSz.y);
	AddGlyph(glyph);
}

#endif
