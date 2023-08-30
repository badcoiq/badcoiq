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

#include "badcoiq.h"

#include "badcoiq/GUI/bqGUI.h"
#include "badcoiq/gs/bqGS.h"

#include "../framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;
#include "bqGUIDefaultTextDrawCallbacks.h"

bqGUIListBoxTextDrawCallback::bqGUIListBoxTextDrawCallback()
{
	m_color = bq::ColorBlack;
	m_colorRed = bq::ColorRed;
}

bqGUIListBoxTextDrawCallback::~bqGUIListBoxTextDrawCallback()
{
}

bqGUIFont* bqGUIListBoxTextDrawCallback::OnFont(uint32_t r, char32_t c)
{
	return m_font;
}

bqColor* bqGUIListBoxTextDrawCallback::OnColor(uint32_t r, char32_t c)
{
	switch (r)
	{
	case bqGUIDrawTextCallback::Reason_selected:
		return &m_colorRed;
	}
	return &m_color;
}

bqGUIListBox::bqGUIListBox(bqGUIWindow* w, const bqVec2f& position, const bqVec2f& size)
	:
	bqGUIElement::bqGUIElement(w, position, size)
{
	m_flags |= bqGUICommon::flag_wheelScroll;
	bqGUIListBoxTextDrawCallback* cb = (bqGUIListBoxTextDrawCallback*)g_framework->m_defaultTextDrawCallback_listbox;
	cb->SetFont(bqFramework::GetDefaultFont(bqGUIDefaultFont::Text));

	m_textDrawCallback = cb;
}

bqGUIListBox::~bqGUIListBox()
{
	for (size_t i = 0; i < m_items.m_size; ++i)
	{
		bqDestroy(m_items.m_data[i]);
	}
}

void bqGUIListBox::Rebuild()
{
	bqGUIElement::Rebuild();
	m_lineHeight = 0.f;
	const char32_t b[] = { U"X#$@WHM_" };
	for (int i = 0; i < 9; ++i)
	{
		bqGUIFont* font = m_textDrawCallback->OnFont(0, b[i]);
		if (font->GetMaxSize().y > m_lineHeight)
			m_lineHeight = (float)font->GetMaxSize().y;
	}
	m_numberOfVisibleLines = 0;
	if (m_lineHeight && (m_buildRect.w - m_buildRect.y))
	{
		m_numberOfVisibleLines = (size_t)ceilf((m_buildRect.w - m_buildRect.y) / m_lineHeight);
	}
}

void bqGUIListBox::UpdateContentSize()
{
	m_contentSize.x = m_baseRect.z - m_baseRect.x;
	m_contentSize.y = 0.f;

	if (m_items.m_size)
	{
		m_contentSize.y = (float)(m_items.m_size) * m_lineHeight;

		// + when scroll to bottom there is must 1 line be visible
		m_contentSize.y += (m_baseRect.w - m_baseRect.y) - m_lineHeight;
	}

	UpdateScrollLimit();
}

void bqGUIListBox::Update()
{
	bqGUIElement::Update();

	// find m_firstItemIndexForDraw, use only m_v_scroll
	bqVec2f pos;
	pos.x = m_buildRect.x;
	pos.y = m_buildRect.y - 0.f;
	pos.y += (m_lineHeight * (float)m_firstItemIndexForDraw) - m_scroll.y;
	uint32_t itemsSize = m_items.m_size + m_numberOfVisibleLines;

	if (pos.y < (m_buildRect.y - m_lineHeight - m_lineHeight))
	{
		float d = (m_buildRect.y - m_lineHeight - m_lineHeight) - pos.y;
		int dd = (int)ceilf(d / m_lineHeight);

		m_firstItemIndexForDraw += dd;

		uint32_t lastIndex = itemsSize - m_items.m_size;
		if (m_numberOfVisibleLines > lastIndex)
			m_numberOfVisibleLines = lastIndex;
	}
	else if (pos.y > (m_buildRect.y - m_lineHeight - m_lineHeight))
	{
		if (m_firstItemIndexForDraw)
		{
			float d = pos.y - (m_buildRect.y - m_lineHeight - m_lineHeight);
			int dd = (int)ceilf(d / m_lineHeight);

			if ((size_t)dd < m_firstItemIndexForDraw)
				m_firstItemIndexForDraw -= dd;
			else
				m_firstItemIndexForDraw = 0;
		}
	}
	printf("m_firstItemIndexForDraw %i\n", m_firstItemIndexForDraw);
}

void bqGUIListBox::Draw(bqGS* gs, float dt)
{
	if (IsEnabled())
	{
		gs->SetScissorRect(m_clipRect);
		if (IsDrawBG())
			gs->DrawGUIRectangle(m_buildRect, m_style->m_staticTextBGColor, m_style->m_staticTextBGColor, 0, 0);
	}

	if (m_items.m_size)
	{
		uint32_t index = m_firstItemIndexForDraw;

		bqVec2f pos;
		pos.x = m_buildRect.x;
		pos.y = m_buildRect.y - m_scroll.y;
		pos.y += (m_lineHeight * (float)index);
		uint32_t itemsSize = m_items.m_size + m_numberOfVisibleLines;

		if (pos.y < (m_baseRect.y - m_lineHeight - m_lineHeight))
		{
			++m_firstItemIndexForDraw;

			uint32_t lastIndex = itemsSize - m_items.m_size;
			if (m_numberOfVisibleLines > lastIndex)
				m_numberOfVisibleLines = lastIndex;
		}
		else if (pos.y > (m_baseRect.y - m_lineHeight))
		{
			if (m_firstItemIndexForDraw)
				--m_firstItemIndexForDraw;
		}

		for (uint32_t i = 0; i < m_numberOfVisibleLines + 4; ++i)
		{
			bqVec4f r;
			r.x = pos.x;
			r.y = pos.y;
			r.z = m_buildRect.z;
			r.w = r.y + m_lineHeight;


			bqVec4f rClip = r;
			if (rClip.y < m_clipRect.y)
				rClip.y = m_clipRect.y;
			if (m_drawItemBG)
			{
				bqColor cc;
				cc = m_style->m_listboxLine1BGColor;
				if (index)
				{
					if ((index % 2) != 0)
						cc = m_style->m_listboxLine2BGColor;
				}

				if (m_items.m_data[index]->m_isSelected)
					cc = m_style->m_listboxSelectedLineBGColor;

				gs->DrawGUIRectangle(r, cc, cc, 0, 0);
			}

			if (m_items.m_data[index]->m_text.size())
			{
				gs->DrawGUIText(
					m_items.m_data[index]->m_text.c_str(),
					m_items.m_data[index]->m_text.size(),
					pos,
					m_textDrawCallback);
			}

			if (bqMath::PointInRect(bqInput::GetData()->m_mousePosition, r))
			{
				if (bqInput::IsLMBHit())
				{
					if (m_items.m_data[index]->m_isSelected)
					{
						if (OnDeSelect(m_items.m_data[index]))
						{
							m_items.m_data[index]->m_isSelected = false;
						}
					}
					else
					{
						if (OnSelect(m_items.m_data[index]))
						{
							if (!m_multiSelect)
							{
								for (size_t i2 = 0; i2 < m_items.m_size; ++i2)
								{
									m_items.m_data[i2]->m_isSelected = false;
								}
							}

							m_items.m_data[index]->m_isSelected = true;
						}
					}
				}
			}

			pos.y = pos.y + m_lineHeight;

			++index;
			if (index >= itemsSize)
				break;
			else if (index == m_items.m_size)
				break;
		}
	}
}

bqGUIListBoxItem* bqGUIListBox::AddItem(const char32_t* t, uint32_t id, void* data)
{
	bqGUIListBoxItem* newItem = bqCreate<bqGUIListBoxItem>();

	newItem->m_text = t;
	newItem->m_data = data;
	newItem->m_id = id;

	m_items.push_back(newItem);
	return newItem;
}

void bqGUIListBox::RemoveItem(bqGUIListBoxItem* it)
{
	BQ_ASSERT_ST(it);
	m_items.erase_first(it);
	bqDestroy(it);
}

