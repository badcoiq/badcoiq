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
#include "badcoiq/gs/bqGS.h"
#include "badcoiq/input/bqInputEx.h"

#include "../framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;
#include "bqGUIDefaultTextDrawCallbacks.h"

bqGUIStaticTextTextDrawCallback::bqGUIStaticTextTextDrawCallback()
{
}

bqGUIStaticTextTextDrawCallback::~bqGUIStaticTextTextDrawCallback()
{
}

bqGUIFont* bqGUIStaticTextTextDrawCallback::OnFont(uint32_t, char32_t)
{
	return m_element->GetStyle()->m_staticTextFont;
}

bqColor* bqGUIStaticTextTextDrawCallback::OnColor(uint32_t, char32_t)
{
	return &m_element->GetStyle()->m_staticTextTextColor;
}

bqGUIStaticText::bqGUIStaticText(bqGUIWindow* w, const bqVec2f& position, const bqVec2f& size)
	:
	bqGUIElement::bqGUIElement(w, position, size)
{
	m_textDrawCallback = g_framework->m_defaultTextDrawCallback_staticText;
}

bqGUIStaticText::~bqGUIStaticText()
{
}

void bqGUIStaticText::SetText(const bqString& text)
{
	m_text = text;
}

void bqGUIStaticText::Rebuild()
{
	m_textDrawCallback->m_element = this;
	bqGUIElement::Rebuild();
	m_lineHeight = 0.f;
	const char32_t b[] = { U"X#$@WHM_" };
	for (int i = 0; i < 9; ++i)
	{
		bqGUIFont* font = m_textDrawCallback->OnFont(0, b[i]);
		if (font->GetMaxSize().y > m_lineHeight)
			m_lineHeight = (float)font->GetMaxSize().y;
	}
}

void bqGUIStaticText::Update()
{
	bqGUIElement::Update();
}

void bqGUIStaticText::Draw(bqGS* gs, float)
{
	gs->SetScissorRect(m_clipRect);
	if (IsDrawBG())
		gs->DrawGUIRectangle(m_buildRect, m_style->m_staticTextBGColor, m_style->m_staticTextBGColor, 0, 0);
	m_textDrawCallback->m_element = this;

	auto sz = m_text.size();
	if (sz)
	{
		bqVec2f pos;
		pos.x = m_buildRect.x;
		pos.y = m_buildRect.y;

		bqVec2f textPosition = pos;

		bool draw = true;
		for (size_t i = 0; i < sz; ++i)
		{
			if (m_useDrawLimit)
			{
				if (i >= m_drawLimit)
					draw = false;
			}

			char32_t ch = m_text[i];
			bqGUIFont* font = m_textDrawCallback->OnFont(0, ch);
			bqGUIFontGlyph* g = font->GetGlyphMap()[ch];
			
			bqVec4f chrct;
			chrct.x = textPosition.x;
			chrct.y = textPosition.y;
			chrct.z = chrct.x + g->m_width + g->m_overhang + g->m_underhang;
			chrct.w = chrct.y + g->m_height;

			if (chrct.z > m_buildRect.z)
			{
				textPosition.y += m_lineHeight;
				textPosition.x = pos.x;

				// надо заново вычислить chrct
				// просто дубликат кода
				chrct.x = textPosition.x;
				chrct.y = textPosition.y;
				chrct.z = chrct.x + g->m_width + g->m_overhang + g->m_underhang;
				chrct.w = chrct.y + g->m_height;
			}


			if (draw)
			{
				bqColor* clr = m_textDrawCallback->OnColor(0, ch);
				gs->DrawGUIRectangle(chrct,
					*clr, *clr,
					font->GetTexture(g->m_textureSlot),
					&g->m_UV);
				/*gs->DrawGUICharacter(
					ch,
					font,
					textPosition,
					*m_textDrawCallback->OnColor(0, ch));*/
			}

			textPosition.x += g->m_width + g->m_overhang + g->m_underhang + font->m_characterSpacing;

			switch (ch)
			{
			case U' ':
				textPosition.x += font->m_spaceSize;
				break;
			case U'\t':
				textPosition.x += font->m_tabSize;
				break;
			case U'\n':
				textPosition.x = pos.x;
				textPosition.y += m_lineHeight;
				break;
			}
		}
	}
}

#endif
