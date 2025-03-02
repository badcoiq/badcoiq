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
#include "badcoiq/gs/bqTexture.h"

#include "badcoiq/common/bqFile.h"

bqGUIFont::bqGUIFont()
{
	// добавляю символ, просто нолик, вызываю AddGlyph
	bqGUIFontGlyph g;
	g.m_symbol = 0;
	AddGlyph(g);

	// теперь можно дать указатели на весь m_glyphMap
	for (uint32_t i = 0; i < BQ_FONT_GLYPH_MAP_SIZE; ++i)
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
	for (size_t i = 0, sz = m_textures.m_size; i < sz; ++i)
	{
		if (m_textures.m_data[i] == t)
			return;
	}

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

void bqGUIFont::AddGlyph(char32_t ch, const bqVec2f& leftTop, const bqPoint& charSz, bqTexture* texture)
{
	size_t textureIndex = 0xFFFFFFFF;
	for (size_t i = 0, sz = m_textures.m_size; i < sz; ++i)
	{
		if (m_textures.m_data[i] == texture)
		{
			textureIndex = i;
			break;
		}
	}

	if (textureIndex == 0xFFFFFFFF)
	{
		AddTexture(texture);
		textureIndex = 0;
	}

	bqPoint textureSize;
	textureSize.x = texture->GetInfo().m_imageInfo.m_width;
	textureSize.y = texture->GetInfo().m_imageInfo.m_height;

	AddGlyph(ch, leftTop, charSz, textureIndex, textureSize);
}

bqFont::bqFont()
{
	_freeAll();
}
bqFont::~bqFont()
{
}

bool bqFont::CreateFromFile(const char* fontFile)
{
	bqFile file;
	if (file.Open(bqFile::_open::Open, fontFile))
	{
		uint8_t b4[4] = { 0, 0, 0, 0 };
		file.ReadByte(&b4[0]);
		file.ReadByte(&b4[1]);
		file.ReadByte(&b4[2]);
		file.ReadByte(&b4[3]);

		const uint32_t tag_dsig = BQ_MAKEFOURCC('D', 'S', 'I', 'G');
		const uint32_t tag_fftm = BQ_MAKEFOURCC('F', 'F', 'T', 'M');
		const uint32_t tag_gdef = BQ_MAKEFOURCC('G', 'D', 'E', 'F');
		const uint32_t tag_gpos = BQ_MAKEFOURCC('G', 'P', 'O', 'S');
		const uint32_t tag_gsub = BQ_MAKEFOURCC('G', 'S', 'U', 'B');
		const uint32_t tag_os2 = BQ_MAKEFOURCC('O', 'S', '/', '2');
		const uint32_t tag_cmap = BQ_MAKEFOURCC('c', 'm', 'a', 'p');
		const uint32_t tag_gasp = BQ_MAKEFOURCC('g', 'a', 's', 'p');
		const uint32_t tag_glyf = BQ_MAKEFOURCC('g', 'l', 'y', 'f');
		const uint32_t tag_head = BQ_MAKEFOURCC('h', 'e', 'a', 'd');
		const uint32_t tag_hmtx = BQ_MAKEFOURCC('h', 'm', 't', 'x');
		const uint32_t tag_loca = BQ_MAKEFOURCC('l', 'o', 'c', 'a');
		const uint32_t tag_maxp = BQ_MAKEFOURCC('m', 'a', 'x', 'p');
		const uint32_t tag_name = BQ_MAKEFOURCC('n', 'a', 'm', 'e');
		const uint32_t tag_post = BQ_MAKEFOURCC('p', 'o', 's', 't');
		const uint32_t tag_prep = BQ_MAKEFOURCC('p', 'r', 'e', 'p');
		uint32_t sfntVersion = BQ_MAKEFOURCC(b4[3], b4[2], b4[1], b4[0]);
		if (sfntVersion == 0x00010000)
		{
			file.ReadByte(&b4[0]);
			file.ReadByte(&b4[1]);
			uint32_t numTables = BQ_MAKEFOURCC(b4[1], b4[0], 0,0);
			// skip searchRange entrySelector rangeShift
			for (int i = 0; i < 6; ++i)
			{
				file.ReadByte(&b4[0]);
			}
			// tableRecords
			for (int i = 0; i < numTables; ++i)
			{
				file.ReadByte(&b4[3]);
				file.ReadByte(&b4[2]);
				file.ReadByte(&b4[1]);
				file.ReadByte(&b4[0]);
				uint32_t tableTag = BQ_MAKEFOURCC(b4[3], b4[2], b4[1], b4[0]);

				file.ReadByte(&b4[0]);
				file.ReadByte(&b4[1]);
				file.ReadByte(&b4[2]);
				file.ReadByte(&b4[3]);
				uint32_t checksum = BQ_MAKEFOURCC(b4[3], b4[2], b4[1], b4[0]);

				file.ReadByte(&b4[0]);
				file.ReadByte(&b4[1]);
				file.ReadByte(&b4[2]);
				file.ReadByte(&b4[3]);
				uint32_t offset = BQ_MAKEFOURCC(b4[3], b4[2], b4[1], b4[0]);

				file.ReadByte(&b4[0]);
				file.ReadByte(&b4[1]);
				file.ReadByte(&b4[2]);
				file.ReadByte(&b4[3]);
				uint32_t length = BQ_MAKEFOURCC(b4[3], b4[2], b4[1], b4[0]);

				switch (tableTag)
				{
				case tag_dsig: break;
				case tag_fftm: break;
				case tag_gdef: break;
				case tag_gpos: break;
				case tag_gsub: break;
				case tag_os2: break;
				case tag_cmap: break;
				case tag_gasp: break;
				case tag_glyf: break;
				case tag_head: break;
				case tag_hmtx: break;
				case tag_loca: break;
				case tag_maxp: break;
				case tag_name: break;
				case tag_post: break;
				case tag_prep: break;
				/*default:
					return false;*/
				}

				printf("a");
			}
		}
	}

	return false;
}

void bqFont::_addGlyph(const bqGUIFontGlyph& g)
{
	bqGUIFontGlyph* newG = bqCreate<bqGUIFontGlyph>();
	*newG = g;
	m_glyphs.push_back(newG);
	m_glyphMap[g.m_symbol] = newG;
}

void bqFont::_freeAll()
{
	for (size_t i = 0; i < m_glyphs.m_size; ++i)
	{
		delete m_glyphs.m_data[i];
	}
	m_glyphs.clear();

	for (size_t i = 0; i < m_images.m_size; ++i)
	{
		delete m_images.m_data[i];
	}
	m_images.clear();

	bqGUIFontGlyph g;
	g.m_symbol = 0;
	_addGlyph(g);

	for (uint32_t i = 0; i < BQ_FONT_GLYPH_MAP_SIZE; ++i)
	{
		m_glyphMap[i] = m_glyphs.m_data[0];
	}
}

#endif
