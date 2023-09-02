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

#ifdef BQ_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "badcoiq/input/bqInputEx.h"

#include "bqGUIDefaultTextDrawCallbacks.h"

#include "../string/bqStringInternal.h"

bqGUITextEditorTextDrawCallback::bqGUITextEditorTextDrawCallback()
{
	m_color = bq::ColorBlack;
}

bqGUITextEditorTextDrawCallback::~bqGUITextEditorTextDrawCallback()
{
}

bqGUIFont* bqGUITextEditorTextDrawCallback::OnFont(uint32_t r, char32_t)
{
	return m_font;
}

bqColor* bqGUITextEditorTextDrawCallback::OnColor(uint32_t r, char32_t)
{
	return &m_color;
}

bqGUITextEditor::bqGUITextEditor(bqGUIWindow* w, const bqVec2f& position, const bqVec2f& size)
	:
	bqGUIElement::bqGUIElement(w, position, size)
{
	m_flags |= bqGUICommon::flag_wheelScroll;

	bqGUITextEditorTextDrawCallback* cb = dynamic_cast<bqGUITextEditorTextDrawCallback*>(g_framework->m_defaultTextDrawCallback_textEditor);
	cb->SetFont(bqFramework::GetDefaultFont(bqGUIDefaultFont::Text));

	m_textDrawCallback = cb;

	Clear(true);
	findTextCursorRect();
}

bqGUITextEditor::~bqGUITextEditor()
{
	if (m_textBuffer)
		bqMemory::free(m_textBuffer);
}

void bqGUITextEditor::reallocate(size_t newSize)
{
	char32_t* newBuffer = (char32_t*)bqMemory::malloc((newSize + 1) * sizeof(char32_t));
	for (uint32_t i = 0; i < m_textBufferLen; ++i)
	{
		newBuffer[i] = m_textBuffer[i];
	}
	newBuffer[m_textBufferLen] = 0;
	m_textBufferAllocated = newSize;
	bqMemory::free(m_textBuffer);
	m_textBuffer = newBuffer;
}

void bqGUITextEditor::findNumberOfLines()
{
	m_numberOfLines = 1; // минимум 1 линия доже если нет текста
	m_lines.clear();
	m_lines.push_back(LineInfo(0, 1, 0));

	LineInfo* li = &m_lines.m_data[0];
	++li->m_size;

	size_t ln = 1;
	for (uint32_t i = 0; i < m_textBufferLen; ++i)
	{
		// если имеем \n тогда след.символ является первым в новой строке
		if (m_textBuffer[i] == U'\n')
		{
			++m_numberOfLines;
			m_lines.push_back(LineInfo(i + 1, ++ln, 0));
			li = &m_lines.m_data[m_lines.m_size - 1];
		}
		++li->m_size;
	}
}

void bqGUITextEditor::Clear(bool freeMemory)
{
	if (freeMemory)
	{
		if (m_textBuffer)
			bqMemory::free(m_textBuffer);

		m_textBufferAllocated = 10;
		m_textBuffer = (char32_t*)bqMemory::malloc((m_textBufferAllocated + 1) * sizeof(char32_t));
	}

	m_textBufferLen = 0;
	m_textBuffer[0] = 0;
	findNumberOfLines();
	findTextCursorRect();
	findHScroll();
	findVScroll();
}

void bqGUITextEditor::SetText(const bqString& text)
{
	size_t len = text.size();
	if (len)
	{
		if (len > m_textBufferAllocated)
			reallocate(len);
		Clear(false);
		for (uint32_t i = 0; i < len; ++i)
		{
			m_textBuffer[i] = text.c_str()[i];
		}
		m_textBufferLen = len;
		m_textBuffer[len] = 0;
	}
	else
	{
		m_textBufferLen = 0;
		m_textBuffer[0] = 0;
	}
	findNumberOfLines();
	findTextCursorRect();
	findHScroll();
	findVScroll();
}

void bqGUITextEditor::GoLeft()
{
	drawTextCursor();
	auto input = bqInput::GetData();
	if (m_textCursor)
	{
		if (input->m_keyboardModifier == bq::KeyboardMod_Shift)
		{
			if ((m_selectionStart == 0) && (m_selectionEnd == 0))
			{
				m_textEditorFlags |= textEditorFlag_isSelected;
				m_selectionStart = m_textCursor;
			}
			m_selectionEnd = m_textCursor - 1;
		}
		else
		{
			if (IsTextSelected())
			{
				if (m_selectionStart < m_selectionEnd)
				{
					m_textCursor = m_selectionStart + 1;
					findColAndLineByTextCursor();
				}
				else
					++m_textCursor;
				Deselect();
			}
		}

		--m_textCursor;
		if (m_col > 1)
			--m_col;
		else
		{
			if (m_line > 1)
			{
				--m_line;
				m_col = m_lines[m_line - 1].m_size;
			}
		}
	}
	else
	{
		if (IsTextSelected() && (input->m_keyboardModifier != bq::KeyboardMod_Shift))
			Deselect();
	}

	if (input->m_keyboardModifier == bq::KeyboardMod_Shift)
	{
		if (IsTextSelected() && (m_selectionStart == m_selectionEnd))
			Deselect();
	}

	m_colFix = m_col;
	findTextCursorRect();
	findVScroll();
	findHScroll();
}

void bqGUITextEditor::GoRight()
{
	drawTextCursor();

	auto prev = m_textCursor;
	auto input = bqInput::GetData();

	if (input->m_keyboardModifier == bq::KeyboardMod_Shift)
	{
		if ((m_selectionStart == 0) && (m_selectionEnd == 0))
		{
			m_textEditorFlags |= textEditorFlag_isSelected;
			m_selectionStart = m_textCursor;
		}
		m_selectionEnd = m_textCursor + 1;
	}
	else
	{
		if (IsTextSelected())
		{
			if (m_selectionStart > m_selectionEnd)
			{
				m_textCursor = m_selectionStart - 1;
				findColAndLineByTextCursor();
			}
			else
				--m_textCursor;
			Deselect();
		}
	}

	++m_textCursor;
	if (m_textCursor > m_textBufferLen)
	{
		m_textCursor = m_textBufferLen;
		/*if (IsTextSelected() && (input->m_keyboardModifier != bq::KeyboardMod_Shift))
			Deselect();*/
	}
	else
	{
		++m_col;
		if (m_textBuffer[prev] == U'\n')
		{
			m_col = 1;
			++m_line;
		}
	}

	if (input->m_keyboardModifier == bq::KeyboardMod_Shift)
	{
		if (IsTextSelected() && (m_selectionStart == m_selectionEnd))
			Deselect();
	}

	m_colFix = m_col;
	findTextCursorRect();
	findVScroll();
	findHScroll();
}

void bqGUITextEditor::GoHome()
{
	drawTextCursor();
	auto input = bqInput::GetData();

	if (m_col > 1)
	{
		if (input->m_keyboardModifier == bq::KeyboardMod_Shift || m_isLMBHit)
		{
			if ((m_selectionStart == 0) && (m_selectionEnd == 0))
			{
				if (!IsTextSelected())
				{
					m_textEditorFlags |= textEditorFlag_isSelected;
					m_selectionStart = m_textCursor;
				}
			}
		}

		m_textCursor = m_lines[m_line - 1].m_index;

		if (input->m_keyboardModifier == bq::KeyboardMod_Shift || m_isLMBHit)
		{
			m_selectionEnd = m_textCursor;
		}

		m_col = 1;
	}
	

	{
		if (IsTextSelected() && (input->m_keyboardModifier != bq::KeyboardMod_Shift))
			Deselect();
	}

	m_colFix = m_col;
	findTextCursorRect();
	findHScroll();
}

void bqGUITextEditor::GoEnd()
{
	drawTextCursor();
	auto input = bqInput::GetData();

	if (input->m_keyboardModifier == bq::KeyboardMod_Shift || m_isLMBHit)
	{
		if ((m_selectionStart == 0) && (m_selectionEnd == 0))
		{
			if (!IsTextSelected())
			{
				m_textEditorFlags |= textEditorFlag_isSelected;
				m_selectionStart = m_textCursor;
			}
		}
	}

	m_col = m_lines[m_line - 1].m_size;
	m_textCursor = m_lines[m_line - 1].m_index + (m_col - 1);

	if (input->m_keyboardModifier == bq::KeyboardMod_Shift || m_isLMBHit)
	{
		m_selectionEnd = m_textCursor;
	}
	{
		if (IsTextSelected() && (input->m_keyboardModifier != bq::KeyboardMod_Shift))
			Deselect();
	}

	m_colFix = m_col;

	findTextCursorRect();
	findHScroll();
}

void bqGUITextEditor::GoPageUp()
{
	drawTextCursor();

	if (m_textBufferLen && m_numberOfVisibleLines && m_lineHeight != 0.f)
	{
		for (size_t i = 0; i < m_numberOfVisibleLines; ++i)
		{
			m_v_scroll -= m_lineHeight;
		}

		if (m_v_scroll < 0.f)
			m_v_scroll = 0.f;

		uint32_t index = m_firstItemIndexForDraw;

		if (index >= m_numberOfVisibleLines)
			m_firstItemIndexForDraw -= m_numberOfVisibleLines;
		else
			m_firstItemIndexForDraw = 0;

		for (size_t i = 0; i < m_numberOfVisibleLines; ++i)
		{
			_GoUp();
		}
	}

	findTextCursorRect();
	findHScroll();
	findVScroll();
}

void bqGUITextEditor::GoPageDown()
{
	drawTextCursor();

	if (m_textBufferLen && m_numberOfVisibleLines && m_lineHeight != 0.f)
	{
		auto novl = m_numberOfVisibleLines;
		bool scroll = true;
		if (m_numberOfLines > m_numberOfVisibleLines)
		{
			if (m_firstItemIndexForDraw > (m_numberOfLines - m_numberOfVisibleLines - 4))
			{
				novl = m_numberOfLines - m_firstItemIndexForDraw;
				scroll = false;
			}
		}

		if (scroll)
		{
			for (size_t i = 0; i < novl; ++i)
			{
				m_v_scroll += m_lineHeight;
			}
		}

		/*if (m_v_scroll < 0.f)
			m_v_scroll = 0.f;*/

		uint32_t index = m_firstItemIndexForDraw;

		if (index >= novl)
			m_firstItemIndexForDraw += novl;
		//else
		//	m_firstItemIndexForDraw = 0;
		if (m_firstItemIndexForDraw >= m_lines.m_size)
			m_firstItemIndexForDraw = m_lines.m_size - 1;

		for (size_t i = 0; i < novl; ++i)
		{
			_GoDown();
		}
	}

	findTextCursorRect();
	findHScroll();
	findVScroll();
}

void bqGUITextEditor::_GoUp()
{
	auto input = bqInput::GetData();

	if (m_line > 1)
	{
		--m_line;
		//printf("sz %i col %i colFix %i\n", m_lines[m_line - 1].m_size, m_col, m_colFix);

		if (input->m_keyboardModifier == bq::KeyboardMod_Shift || m_isLMBHit)
		{
			if ((m_selectionStart == 0) && (m_selectionEnd == 0))
			{
				if (!IsTextSelected())
				{
					m_textEditorFlags |= textEditorFlag_isSelected;
					m_selectionStart = m_textCursor;
				}
			}
		}
		else
		{
			if (IsTextSelected())
			{
				auto s1 = m_selectionStart;
				auto s2 = m_selectionEnd;
				if (s1 > s2)
				{
					s1 = s2;
					s2 = m_selectionStart;
				}
				m_textCursor = s1;
				findColAndLineByTextCursor();
				Deselect();
			}
		}

		if (m_lines[m_line - 1].m_size < m_colFix)
			m_col = m_lines[m_line - 1].m_size;
		else
			m_col = m_colFix;
		m_textCursor = m_lines[m_line - 1].m_index + (m_col - 1);

		if (input->m_keyboardModifier == bq::KeyboardMod_Shift || m_isLMBHit)
			m_selectionEnd = m_textCursor;
	}
}

void bqGUITextEditor::GoUp()
{
	drawTextCursor();
	_GoUp();
	findTextCursorRect();
	findVScroll();
	findHScroll();
}

void bqGUITextEditor::_GoDown()
{
	auto input = bqInput::GetData();

	if (m_line - 1 < m_lines.m_size - 1)
	{
		++m_line;
		//printf("sz %i col %i colFix %i\n", m_lines[m_line - 1].m_size, m_col, m_colFix);
		if (input->m_keyboardModifier == bq::KeyboardMod_Shift || m_isLMBHit)
		{
			if ((m_selectionStart == 0) && (m_selectionEnd == 0))
			{
				if (!IsTextSelected())
				{
					m_textEditorFlags |= textEditorFlag_isSelected;
					m_selectionStart = m_textCursor;
				}
			}
		}
		else
		{
			if (IsTextSelected())
			{
				auto s1 = m_selectionStart;
				auto s2 = m_selectionEnd;
				if (s1 > s2)
				{
					s1 = s2;
					s2 = m_selectionStart;
				}
				m_textCursor = s2;
				findColAndLineByTextCursor();
				Deselect();
			}
		}

		if (m_lines[m_line - 1].m_size < m_colFix)
			m_col = m_lines[m_line - 1].m_size;
		else
			m_col = m_colFix;
		m_textCursor = m_lines[m_line - 1].m_index + (m_col - 1);

		if (input->m_keyboardModifier == bq::KeyboardMod_Shift || m_isLMBHit)
			m_selectionEnd = m_textCursor;
	}
}

void bqGUITextEditor::GoDown()
{
	drawTextCursor();
	_GoDown();
	findTextCursorRect();
	findVScroll();
	findHScroll();
}

void bqGUITextEditor::UpdateContentSize()
{
	m_contentSize.x = m_baseRect.z - m_baseRect.x;
	m_contentSize.y = 0.f;

	if (m_numberOfLines)
	{
		m_contentSize.y = (float)(m_numberOfLines)*m_lineHeight;

		// + when scroll to bottom there is must 1 line be visible
		m_contentSize.y += (m_baseRect.w - m_baseRect.y) - m_lineHeight;
	}
	UpdateScrollLimit();
}

void bqGUITextEditor::Rebuild()
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
	//findVScroll();
	//findHScroll();
//	printf("Num of lines: %i\n", m_numberOfLines);
//	printf("Num of visible lines: %i\n", m_numberOfVisibleLines);
}

void bqGUITextEditor::Update()
{
	bqGUIElement::Update();

	if (bqInput::IsLMBRelease())
		m_isLMBHit = false;

	if (!m_lineHeight)
		return;

	if (IsActivated())
	{
		if (bqInput::IsKeyHit(bqInput::KEY_LEFT))
			GoLeft();
		if (bqInput::IsKeyHit(bqInput::KEY_RIGHT))
			GoRight();
		if (bqInput::IsKeyHit(bqInput::KEY_UP))
			GoUp();
		if (bqInput::IsKeyHit(bqInput::KEY_DOWN))
			GoDown();
		if (bqInput::IsKeyHit(bqInput::KEY_HOME))
			GoHome();
		if (bqInput::IsKeyHit(bqInput::KEY_END))
			GoEnd();
		if (bqInput::IsKeyHit(bqInput::KEY_PGUP))
			GoPageUp();
		if (bqInput::IsKeyHit(bqInput::KEY_PGDOWN))
			GoPageDown();
		if (bqInput::IsKeyHit(bqInput::KEY_DELETE))
			Delete();

		if (bqInput::IsKeyHit(bqInput::KEY_BACKSPACE))
			Backspace();
		else if ((bqInput::GetData()->m_keyboardModifier & bq::KeyboardMod_Ctrl)
			|| (bqInput::GetData()->m_keyboardModifier & bq::KeyboardMod_Shift))
		{
			if (bqInput::GetData()->m_keyboardModifier & bq::KeyboardMod_Ctrl)
			{
				if (bqInput::IsKeyHit(bqInput::KEY_A))
					SelectAll();
				else if (bqInput::IsKeyHit(bqInput::KEY_C))
					Copy();
				else if (bqInput::IsKeyHit(bqInput::KEY_INSERT))
					Copy();
				else if (bqInput::IsKeyHit(bqInput::KEY_V))
					Paste();
				else if (bqInput::IsKeyHit(bqInput::KEY_X))
					Cut();
			}
			else if (bqInput::GetData()->m_keyboardModifier & bq::KeyboardMod_Shift)
			{
				if (bqInput::IsKeyHit(bqInput::KEY_INSERT))
					Paste();
				else if (bqInput::IsKeyHit(bqInput::KEY_DELETE))
					Cut();
				else if (bqInput::GetData()->m_character)
					Type(bqInput::GetData()->m_character);
			}
		}
		else if (bqInput::GetData()->m_character)
			Type(bqInput::GetData()->m_character);
	}

	if (IsCursorInRect())
	{
		if (bqInput::IsLMBHit())
		{
			Deselect();
			m_isLMBHit = true;
			m_skipDraw = true;
			Draw(0, 0.f);
		}

		if (!IsActivated())
		{
			if (IsClickedLMB()
				|| IsClickedRMB()
				|| IsClickedMMB()
				|| IsClickedX1MB()
				|| IsClickedX2MB())
				Activate(true);
		}

		if (g_framework->m_input.m_mouseWheelDelta < 0.f)
		{
			m_v_scroll += 10.f;
			if (m_v_scroll > m_scrollLimit.y)
				m_v_scroll = m_scrollLimit.y;
		}
		else if (g_framework->m_input.m_mouseWheelDelta > 0.f)
		{
			m_v_scroll -= 10.f;
			if (m_v_scroll < 0.f)
				m_v_scroll = 0.f;
		}

	}
	else
	{
		if (IsActivated())
		{
			if (bqInput::IsLMBHit()
				|| bqInput::IsRMBHit()
				|| bqInput::IsMMBHit()
				|| bqInput::IsX1MBHit()
				|| bqInput::IsX2MBHit())
			{
				Activate(false);
			}
		}
	}

	bqVec2f pos;
	pos.x = m_buildRect.x;
	pos.y = m_buildRect.y - m_v_scroll;
	pos.y += (m_lineHeight * (float)m_firstItemIndexForDraw);
	uint32_t itemsSize = m_numberOfLines + m_numberOfVisibleLines;


	if (pos.y < (m_buildRect.y - m_lineHeight - m_lineHeight))
	{
		float d = (m_buildRect.y - m_lineHeight - m_lineHeight) - pos.y;
		int dd = (int)ceilf(d / m_lineHeight);

		m_firstItemIndexForDraw += dd;

		uint32_t lastIndex = itemsSize - m_numberOfLines;
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

	bqGUIElement* parent = dynamic_cast<bqGUIElement*>(GetParent());

	if (parent->m_scrollDelta.y != 0.f)
	{
		findTextCursorRect();
	}
}

void bqGUITextEditor::Activate(bool v)
{
	if (g_framework->m_GUIState.m_activeTextEditor && g_framework->m_GUIState.m_activeTextEditor != this)
	{
		g_framework->m_GUIState.m_activeTextEditor->Activate(false);
	}

	if (v)
	{
		g_framework->m_GUIState.m_activeTextEditor = this;
		m_textEditorFlags |= textEditorFlag_isActivated;
		drawTextCursor();
		findTextCursorRect();
		findVScroll();
		findHScroll();
	}
	else
	{
		g_framework->m_GUIState.m_activeTextEditor = 0;
		m_textEditorFlags &= ~textEditorFlag_isActivated;
		m_drawTextCursor = false;
	}
}

void bqGUITextEditor::findHScroll()
{
	if (IsActivated())
	{
		bqVec4f buildRectWithScroll = m_buildRect;
		buildRectWithScroll.x += m_h_scroll;
		buildRectWithScroll.z += m_h_scroll;
		buildRectWithScroll.y += m_v_scroll;
		buildRectWithScroll.w += m_v_scroll;

		if (m_textCursorRect.x < buildRectWithScroll.x)
			m_h_scroll -= buildRectWithScroll.x - m_textCursorRect.x;
		else if (m_textCursorRect.z > buildRectWithScroll.z)
			m_h_scroll += m_textCursorRect.z - buildRectWithScroll.z;

		if (m_h_scroll < 0.f)
			m_h_scroll = 0.f;
	}
}

void bqGUITextEditor::findVScroll()
{
	if (IsActivated())
	{
		bqVec4f buildRectWithScroll = m_buildRect;
		buildRectWithScroll.x += m_h_scroll;
		buildRectWithScroll.z += m_h_scroll;
		buildRectWithScroll.y += m_v_scroll;
		buildRectWithScroll.w += m_v_scroll;

		if (m_textCursorRect.y < buildRectWithScroll.y)
			m_v_scroll -= buildRectWithScroll.y - m_textCursorRect.y;
		else if (m_textCursorRect.w > buildRectWithScroll.w)
			m_v_scroll += m_textCursorRect.w - buildRectWithScroll.w;

		if (m_v_scroll < 0.f)
			m_v_scroll = 0.f;
	}
	UpdateContentSize();
}

void bqGUITextEditor::findTextCursorRect()
{
	m_textCursorRect.Set(0.f);
	bqVec2f pos;
	pos.x = m_buildRect.x;
	pos.y = m_buildRect.y;

	bqVec2f textPosition = pos;
	float fontMaxSizeY = 0.f;
	for (size_t i = 0; i < m_textBufferLen; ++i)
	{
		char32_t ch = m_textBuffer[i];
		bqGUIFont* font = m_textDrawCallback->OnFont(0, ch);
		bqGUIFontGlyph* g = font->GetGlyphMap()[ch];
		if (font->GetMaxSize().y > fontMaxSizeY)
			fontMaxSizeY = (float)font->GetMaxSize().y;
		if (i == m_textCursor)
		{
			m_textCursorRect.x = textPosition.x;
			m_textCursorRect.y = textPosition.y;
			m_textCursorRect.z = m_textCursorRect.x + 2.f;
			m_textCursorRect.w = m_textCursorRect.y + fontMaxSizeY;
			return;
		}
		textPosition.x += g->m_width + g->m_overhang + g->m_underhang + font->m_characterSpacing;
		switch (m_textBuffer[i])
		{
		case U' ':
			textPosition.x += font->m_spaceSize;
			break;
		case U'\t':
			textPosition.x += font->m_tabSize;
			break;
		case U'\n':
			textPosition.y += m_lineHeight;
			textPosition.x = pos.x;
			break;
		}
	}

	if (m_textCursor == m_textBufferLen)
	{
		m_textCursorRect.x = textPosition.x;
		m_textCursorRect.y = textPosition.y;
		m_textCursorRect.z = m_textCursorRect.x + 2.f;
		m_textCursorRect.w = m_textCursorRect.y + fontMaxSizeY;
	}
}

void bqGUITextEditor::Draw(bqGS* gs, float dt)
{
	if (IsActivated())
	{
		m_textCursorTimer += dt;
		if (m_textCursorTimer > m_textCursorTimerLimit)
		{
			m_textCursorTimer = 0.f;
			m_drawTextCursor = m_drawTextCursor ? false : true;
		}
	}

	if (IsEnabled() && !m_skipDraw)
	{
		gs->SetScissorRect(m_clipRect.GetRect());
		if (IsDrawBG())
			gs->DrawGUIRectangle(m_buildRect, m_style->m_textEditorBGColor, m_style->m_textEditorBGColor, 0, 0);
	}

	if (m_textBufferLen)
	{
		uint32_t index = m_firstItemIndexForDraw;
		auto input = bqInput::GetData();
		auto& mp = input->m_mousePosition;

		bool mouseMove = false;
		if (input->m_mouseMoveDelta.x != 0.f || input->m_mouseMoveDelta.y != 0.f)
			mouseMove = true;

		bqVec2f pos;
		pos.x = m_buildRect.x - m_h_scroll;
		pos.y = m_buildRect.y - m_v_scroll;
		pos.y += (m_lineHeight * (float)index);

		uint32_t itemsSize = m_numberOfLines;

		// or not under. if cursor is out of last char in the line (cursor.x > charRect.z)
		//   then index must be last char in this line
		// if cursor is not in the line (below text), then index is m_textBufferLen
		size_t charIndexUnderCursor = 0;
		size_t nearestChar = 0;
		bool isCharIndexUnderCursor = false;

		auto s1 = m_selectionStart;
		auto s2 = m_selectionEnd;
		if (s1 > s2)
		{
			s1 = s2;
			s2 = m_selectionStart;
		}

		float distance = 99999.f;

		for (uint32_t i = 0; i < m_numberOfVisibleLines + 4; ++i)
		{
			bqVec2f textPosition = pos;

			bqVec4f r;
			r.x = pos.x;
			r.y = pos.y;
			r.z = m_buildRect.z;
			r.w = r.y + m_lineHeight;


			if (IsDrawBG() && !m_skipDraw)
			{
				bqColor cc;
				cc = GetStyle()->m_textEditorLine1BGColor;
				if (index)
				{
					if ((index % 2) != 0)
						cc = GetStyle()->m_textEditorLine2BGColor;
				}

				gs->DrawGUIRectangle(r, cc, cc, 0, 0);
			}

			size_t last = m_textBufferLen + 1;
			for (size_t o = m_lines.m_data[index].m_index; o < last; ++o)
			{
				char32_t ch = m_textBuffer[o];
				bqGUIFont* font = m_textDrawCallback->OnFont(0, ch);
				bqGUIFontGlyph* g = font->GetGlyphMap()[ch];

				bqVec4f chrct;
				chrct.x = textPosition.x;
				chrct.y = textPosition.y;

				chrct.z = chrct.x + g->m_width + g->m_overhang + g->m_underhang;
				chrct.w = chrct.y + g->m_height;

				auto d = bqMath::Distance(bqVec4f(chrct.x, chrct.y, 0.f, 0.f),
					bqVec4f(mp.x, mp.y, 0.f, 0.f));
				if (d < distance)
				{
					distance = d;
					nearestChar = o;
				}

				if ((mp.y >= chrct.y) && (mp.y <= chrct.w))
				{
					if (mp.x >= chrct.x)
					{
						if (mp.x > chrct.x + ((float)g->m_width * 0.5f))
						{
							if (ch == U'\n')
								charIndexUnderCursor = o;
							else
								charIndexUnderCursor = o + 1;
						}
						else
							charIndexUnderCursor = o;
						isCharIndexUnderCursor = true;
					}
					else if (mp.x < m_buildRect.x)
					{
						charIndexUnderCursor = m_lines.m_data[index].m_index;
						isCharIndexUnderCursor = true;
					}
				}

				if (ch && !m_skipDraw)
				{
					if (IsTextSelected())
					{
						if (o >= s1 && o < s2)
						{
							bqColor cc = GetStyle()->m_textEditorSelectedTextBGColor;
							auto slrcht = chrct;
							slrcht.x -= 1.f;
							slrcht.z += 1.f;
							gs->DrawGUIRectangle(slrcht, cc, cc, 0, 0);
						}
					}
					
					bqColor* clr = m_textDrawCallback->OnColor(0, m_textBuffer[o]);
					gs->DrawGUIRectangle(chrct,
						*clr, *clr,
						font->GetTexture(g->m_textureSlot),
						&g->m_UV);

					/*gs->DrawGUICharacter(
						m_textBuffer[o],
						font,
						textPosition,
						*m_textDrawCallback->OnColor(0, m_textBuffer[o]));*/

				}

				if (m_drawTextCursor)
				{
					if (o == m_textCursor)
					{
						if (!m_skipDraw)
						{
							auto fnl = m_textCursorRect;
							fnl.y -= m_v_scroll;
							fnl.w -= m_v_scroll;
							fnl.x -= m_h_scroll;
							fnl.z -= m_h_scroll;

							gs->DrawGUIRectangle(fnl, GetStyle()->m_textEditorCursorColor, GetStyle()->m_textEditorCursorColor, 0, 0);
						}
					}
				}

				textPosition.x += g->m_width + g->m_overhang + g->m_underhang + font->m_characterSpacing;

				switch (m_textBuffer[o])
				{
				case U' ':
					textPosition.x += font->m_spaceSize;
					break;
				case U'\t':
					textPosition.x += font->m_tabSize;
					break;
				case U'\n':
					textPosition.x = pos.x;

					o = m_textBufferLen;
					break;
				}
			}

			pos.y = pos.y + m_lineHeight;

			++index;
			if (index >= itemsSize)
				break;
		}

		if (!isCharIndexUnderCursor)
		{
			if (mouseMove && m_isLMBHit && bqInput::IsLMBHold())
			{
				isCharIndexUnderCursor = true;
				charIndexUnderCursor = nearestChar;
			}
		}

		if (isCharIndexUnderCursor)
		{
			if (charIndexUnderCursor > m_textBufferLen)
				charIndexUnderCursor = m_textBufferLen;

			bool needupdate = false;
			if (IsCursorInRect())
			{
				if (bqInput::IsLMBHit())
				{
					m_textCursor = charIndexUnderCursor;
					needupdate = true;
				}
			}

			if (mouseMove && m_isLMBHit && bqInput::IsLMBHold())
			{
				m_textCursor = charIndexUnderCursor;
				if (!IsTextSelected())
				{
					m_textEditorFlags |= textEditorFlag_isSelected;
					m_selectionStart = m_textCursor;
					m_selectionEnd = m_textCursor;
				}
				else
				{
					m_selectionEnd = m_textCursor;
				}

				if (mp.y < m_buildRect.y)
				{
					if (mouseMove)
					{
						m_v_scroll -= m_lineHeight;
						if (m_v_scroll < 0.f)
							m_v_scroll = 0.f;
					}
				}
				else if (mp.y > m_buildRect.w)
				{
					if (mouseMove)
					{
						m_v_scroll += m_lineHeight;

						float contentSize_y = (float)(m_numberOfLines)*m_lineHeight;
						contentSize_y -= (m_baseRect.w - m_baseRect.y);

						if (m_v_scroll > contentSize_y)
							m_v_scroll = contentSize_y;
					}
				}
				needupdate = true;
			}
			if (needupdate)
			{
				findTextCursorRect();
				findColAndLineByTextCursor();
				drawTextCursor();
				findHScroll();
			}
		}
	}
	else
	{
		// draw empty line
		bqVec2f pos;
		pos.x = m_buildRect.x - m_h_scroll;
		pos.y = m_buildRect.y - m_v_scroll;

		bqVec4f r;
		r.x = pos.x;
		r.y = pos.y;
		r.z = m_buildRect.z;
		r.w = r.y + m_lineHeight;

		if (IsDrawBG() && !m_skipDraw)
		{
			bqColor cc;
			cc = GetStyle()->m_textEditorLine1BGColor;

			gs->DrawGUIRectangle(r, cc, cc, 0, 0);
		}

		if (m_drawTextCursor && !m_skipDraw)
		{
			auto fnl = r;
			fnl.z = fnl.x + 2.f;
			fnl.w = fnl.y + m_lineHeight;
			fnl.y -= m_v_scroll;
			fnl.w -= m_v_scroll;
			fnl.x -= m_h_scroll;
			fnl.z -= m_h_scroll;

			gs->DrawGUIRectangle(fnl, GetStyle()->m_textEditorCursorColor, GetStyle()->m_textEditorCursorColor, 0, 0);
		}
	}

	m_skipDraw = false;
}

void bqGUITextEditor::findColAndLineByTextCursor()
{
	m_line = 1;
	m_col = 1;

	for (uint32_t i = 0; i < m_textBufferLen; ++i)
	{
		if (i == m_textCursor)
			break;

		++m_col;

		if (m_textBuffer[i] == U'\n')
		{
			m_col = 1;
			++m_line;
		}

	}

	m_colFix = m_col;
}

void bqGUITextEditor::Deselect()
{
	if (IsTextSelected())
	{
		m_textEditorFlags &= ~textEditorFlag_isSelected;
		m_selectionStart = 0;
		m_selectionEnd = 0;
	}
}

void bqGUITextEditor::DeleteSelected()
{
	auto s1 = m_selectionStart;
	auto s2 = m_selectionEnd;
	if (s1 > s2)
	{
		s1 = s2;
		s2 = m_selectionStart;
	}

	auto num_to_delete = s2 - s1;
	auto str_len = m_textBufferLen;
	auto buf = m_textBuffer;
	for (size_t i = s2, i2 = s1; i < str_len; ++i, ++i2)
	{
		buf[i2] = buf[i];
	}
	buf[str_len - num_to_delete] = 0;

	Deselect();
	m_textCursor = s1;
	m_textBufferLen -= num_to_delete;

	findNumberOfLines();
	findColAndLineByTextCursor();
	findTextCursorRect();
	findHScroll();
	findVScroll();
}

void bqGUITextEditor::Delete()
{
	this->drawTextCursor();
	if (IsTextSelected())
	{
		DeleteSelected();
		return;
	}

	if (m_textCursor < m_textBufferLen)
	{
		bool ok = false;
		auto buf = m_textBuffer;
		auto str_len = m_textBufferLen;

		for (size_t i = m_textCursor; i < str_len; ++i)
		{
			ok = true;
			if (i + 1 == str_len)
				break;
			buf[i] = buf[i + 1];
		}
		if (ok)
		{
			buf[str_len - 1] = 0;
			m_textBufferLen -= 1;
			findNumberOfLines();
			findColAndLineByTextCursor();
			findHScroll();
			findVScroll();
		}
	}

}

void bqGUITextEditor::Backspace()
{
	this->drawTextCursor();
	if (IsTextSelected())
	{
		DeleteSelected();
		return;
	}

	bool ok = false;
	auto buf = m_textBuffer;
	auto str_len = m_textBufferLen;
	for (size_t i = m_textCursor; i < str_len; ++i)
	{
		if (i == 0)
			break;
		ok = true;
		buf[i - 1] = buf[i];
	}
	if (m_textCursor == str_len && !ok)
		ok = true;
	if (ok)
	{
		if (str_len - 1 >= 0)
		{
			if (m_textCursor)
			{
				--m_textCursor;
				buf[str_len - 1] = 0;
				m_textBufferLen -= 1;
			}
		}
		findNumberOfLines();
		findColAndLineByTextCursor();
		findTextCursorRect();
		findHScroll();
		findVScroll();
	}
}

void bqGUITextEditor::Type(char32_t c)
{
	if (IsTextSelected())
		DeleteSelected();

	char32_t b[2] = { c, 0 };

	if (c == U'\r')
	{
		if (m_useRN)
			_PutText(b, 1);

		b[0] = U'\n';
		_PutText(b, 1);
	}
	else
	{
		_PutText(b, 1);
	}

	findNumberOfLines();
	findColAndLineByTextCursor();
	findTextCursorRect();
	findHScroll();
	findVScroll();
}

void bqGUITextEditor::_PutText(const char32_t* text, size_t len)
{
	size_t newLen = m_textBufferLen + len;

	if (newLen > m_textBufferAllocated)
		reallocate(newLen);

	size_t i = m_textBufferLen;
	while (i >= m_textCursor)
	{
		size_t next = i + len;
		if (next < m_textBufferAllocated)
			m_textBuffer[next] = m_textBuffer[i];

		if (i == 0)
			break;
		--i;
	}
	for (size_t i2 = 0; i2 < len; ++i2)
	{
		m_textBuffer[m_textCursor] = text[i2];
		++m_textCursor;
	}

	m_textBufferLen = newLen;
	m_textBuffer[newLen] = 0;
}

void bqGUITextEditor::PutText(const char32_t* text, size_t len)
{
	if (IsTextSelected())
		DeleteSelected();

	_PutText(text, len);

	findNumberOfLines();
	findColAndLineByTextCursor();
	findTextCursorRect();
	findHScroll();
	findVScroll();
}

void bqGUITextEditor::Select(size_t s1, size_t s2)
{
	if (m_textBufferLen)
	{
		Deselect();

		auto _s1 = s1;
		auto _s2 = s2;
		if (_s2 < _s1)
		{
			_s1 = s2;
			_s2 = s1;
		}

		if (_s1 <= m_textBufferLen)
		{
			m_textEditorFlags |= this->textEditorFlag_isSelected;
			m_selectionStart = _s1;
			if (_s2 <= m_textBufferLen)
				m_selectionEnd = _s2;
			else
				m_selectionEnd = m_textBufferLen;
		}

		findTextCursorRect();
		findHScroll();
		findVScroll();
	}
}

void bqGUITextEditor::SelectAll()
{
	if (m_textBufferLen)
	{
		m_textEditorFlags |= this->textEditorFlag_isSelected;
		m_selectionStart = 0;
		m_selectionEnd = m_textBufferLen;

		findTextCursorRect();
		findHScroll();
		findVScroll();
	}
}

void bqGUITextEditor::Copy()
{
	if (IsTextSelected())
	{
		uint32_t s1 = m_selectionStart;
		uint32_t s2 = m_selectionEnd;
		if (s1 > s2)
		{
			s1 = s2;
			s2 = m_selectionStart;
		}
		uint32_t num_to_select = s2 - s1;

#ifdef BQ_PLATFORM_WINDOWS
		if (!OpenClipboard(0))
			return;

		uint32_t len = num_to_select;
		EmptyClipboard();
		HGLOBAL clipbuffer = GlobalAlloc(GMEM_DDESHARE, ((len + len) + 1) * sizeof(WCHAR));
		if (clipbuffer)
		{
			wchar_t* buffer = (wchar_t*)GlobalLock(clipbuffer);
			if (buffer)
			{
				memset(buffer, 0, (len + len + 1) * sizeof(wchar_t));
				wchar_t* wchar_ptr = buffer;
				//memcpy(buffer, &impl->text[s1], len * sizeof(wchar_t));
				UC uc;
				for (size_t i = 0; i < len; ++i)
				{
					char32_t c = m_textBuffer[s1 + i];

					if (c >= 0x32000)
						c = '?';

					uc.integer = g_UnicodeChars[c].m_utf16;

					if (uc.shorts[1])
					{
						*wchar_ptr = uc.shorts[1];
						++wchar_ptr;
					}
					if (uc.shorts[0])
					{
						*wchar_ptr = uc.shorts[0];
						++wchar_ptr;
					}
				}
				*wchar_ptr = 0;
				++wchar_ptr;

				buffer[len] = 0;
			}
			GlobalUnlock(clipbuffer);
			SetClipboardData(CF_UNICODETEXT, clipbuffer);
		}

		CloseClipboard();
#else
#error Need implementation....
#endif
	}
}

void bqGUITextEditor::Cut()
{
	Copy();
	DeleteSelected();
}

void bqGUITextEditor::Paste()
{
	if (IsTextSelected())
		DeleteSelected();

#ifdef BQ_PLATFORM_WINDOWS
	if (!OpenClipboard(0))
		return;

	char32_t b[2] = { U'?', 0 };

	HANDLE hData = GetClipboardData(CF_UNICODETEXT);
	if (hData)
	{
		wchar_t* buffer = (wchar_t*)GlobalLock(hData);
		if (buffer)
		{
			uint32_t len = (uint32_t)wcslen(buffer);
			if (len)
			{
				char32_t* utf32 = (char32_t*)malloc(len * sizeof(char32_t));
				size_t utf32sz = 0;

				bqString str;
				str.assign(buffer);

				for (size_t i = 0; i < str.size(); ++i)
				{
					if (OnChar(str[i]))
					{
						b[0] = str[i];
						_PutText(b, 1);
					}
				}
				free(utf32);
			}
		}
		GlobalUnlock(hData);
	}
	CloseClipboard();
#else
#error Need implementation....
#endif

	findNumberOfLines();
	findColAndLineByTextCursor();
	findTextCursorRect();
	findHScroll();
	findVScroll();
}

