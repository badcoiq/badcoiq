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
#ifndef __BQ_GUITEXTEDTR_H__
#define __BQ_GUITEXTEDTR_H__

class bqGUITextEditor : public bqGUIElement
{
	char32_t* m_textBuffer = 0;
	size_t m_textBufferLen = 0;
	size_t m_textBufferAllocated = 0; //in char32_t
	void reallocate(size_t newSize);

	//size_t m_textBeginDrawIndex = 0;

	// this points to place where to put new char.
	// from 0 to m_textBufferLen
	// 0 - put in front
	// m_textBufferLen - push_back
	size_t m_textCursor = 0;
	float m_textCursorTimer = 0.f;
	float m_textCursorTimerLimit = 0.5f;
	bqVec4f m_textCursorRect; // save it in Draw
	bool m_drawTextCursor = false;
	void drawTextCursor() { m_drawTextCursor = true; m_textCursorTimer = 0.f; }

	size_t m_line = 1;
	size_t m_col = 1;

	// it will use when press up or down.
	// I need to set m_col in position that was before
	/*
	* Like this. cursor in 3rd line, between 2 and 3
	* I want press up up and cursor will in line 1 and between 2 and 3
	*
	* 012[c]3456
	* 0
	* 012[c]3
	*/
	size_t m_colFix = 1;

	bool m_skipDraw = false;

	bool m_isLMBHit = false;

	void findColAndLineByTextCursor();

	size_t m_firstItemIndexForDraw = 0;

	size_t m_numberOfLines = 0;
	size_t m_numberOfVisibleLines = 0;
	float m_lineHeight = 0.f;
	struct LineInfo
	{
		LineInfo(size_t ind, size_t line, size_t sz) :m_index(ind), m_line(line), m_size(sz) {}
		size_t m_index = 0; // индекс символа в главном буфере
		size_t m_line = 1;  // линия. начинается с 1
		size_t m_size = 0;  // количество символов
	};
	bqArray<LineInfo> m_lines;
	void findNumberOfLines();

	void findTextCursorRect();
	void findVScroll();
	void findHScroll();

	float m_h_scroll = 0.f;
	float m_v_scroll = 0.f;

	void GoLeft();
	void GoRight();
	void GoUp();
	void _GoUp();
	void GoDown();
	void _GoDown();
	void GoHome();
	void GoEnd();
	void GoPageUp();
	void GoPageDown();
	void Delete();
	void Backspace();
	void Type(char32_t);
	void _PutText(const char32_t*, size_t);
	void PutText(const char32_t*, size_t);

	enum
	{
		textEditorFlag_isActivated = 0x1,
		textEditorFlag_isSelected = 0x2
	};
	uint32_t m_textEditorFlags = 0;

	size_t m_selectionStart = 0;
	size_t m_selectionEnd = 0;

public:
	bqGUITextEditor(bqGUIWindow*, const bqVec2f& position, const bqVec2f& size);
	virtual ~bqGUITextEditor();

	bool m_useRN = false; // \r\n on Enter

	bool IsActivated() { return (m_textEditorFlags & textEditorFlag_isActivated); }
	bool IsTextSelected() { return (m_textEditorFlags & textEditorFlag_isSelected); }
	void Activate(bool);

	size_t GetTextLen() { return m_textBufferLen; }
	const char32_t* GetText() { return m_textBuffer; }

	virtual void Rebuild() final;
	virtual void Update() final;
	virtual void Draw(bqGS* gs, float dt) final;

	virtual void UpdateContentSize() final;

	virtual void Clear(bool freeMemory);
	virtual void SetText(const bqString&);
	virtual void Deselect();

	virtual void Select(size_t s1, size_t s2);
	virtual void SelectAll();
	virtual void DeleteSelected();
	virtual void Copy();
	virtual void Paste();
	virtual void Cut();

	virtual bool OnChar(char32_t) { return true; }
};

#endif

