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
#ifndef __BQ_GUICBS_H_
#define __BQ_GUICBS_H_

#include "badcoiq.h"

#include "badcoiq/GUI/bqGUI.h"


class bqGUIButtonTextDrawCallback : public bqGUIDrawTextCallback
{
	bqGUIFont* m_font = 0;
	bqColor m_color;
public:
	bqGUIButtonTextDrawCallback();
	virtual ~bqGUIButtonTextDrawCallback();
	BQ_PLACEMENT_ALLOCATOR(bqGUIButtonTextDrawCallback);

	void SetFont(bqGUIFont* f) { m_font = f; }

	virtual bqGUIFont* OnFont(uint32_t r, char32_t) override;
	virtual bqColor* OnColor(uint32_t r, char32_t) override;
};

class bqGUICheckRadioBoxTextDrawCallback : public bqGUIDrawTextCallback
{
	bqGUIFont* m_font = 0;
	bqColor m_colorDefault;
	bqColor m_colorMouseHover;
	bqColor m_colorDisabled;
public:
	bqGUICheckRadioBoxTextDrawCallback();
	virtual ~bqGUICheckRadioBoxTextDrawCallback();
	BQ_PLACEMENT_ALLOCATOR(bqGUIButtonTextDrawCallback);

	void SetFont(bqGUIFont* f) { m_font = f; }

	virtual bqGUIFont* OnFont(uint32_t r, char32_t) override;
	virtual bqColor* OnColor(uint32_t r, char32_t) override;
};

class bqGUITextEditorTextDrawCallback : public bqGUIDrawTextCallback
{
	bqGUIFont* m_font = 0;
	bqColor m_color;
public:
	bqGUITextEditorTextDrawCallback();
	virtual ~bqGUITextEditorTextDrawCallback();
	BQ_PLACEMENT_ALLOCATOR(bqGUITextEditorTextDrawCallback);

	void SetFont(bqGUIFont* f) { m_font = f; }

	virtual bqGUIFont* OnFont(uint32_t r, char32_t) override;
	virtual bqColor* OnColor(uint32_t r, char32_t) override;
};

class bqGUIListBoxTextDrawCallback : public bqGUIDrawTextCallback
{
	bqGUIFont* m_font = 0;
	bqColor m_color;
	bqColor m_colorRed;
public:
	bqGUIListBoxTextDrawCallback();
	virtual ~bqGUIListBoxTextDrawCallback();
	BQ_PLACEMENT_ALLOCATOR(bqGUIListBoxTextDrawCallback);

	void SetFont(bqGUIFont* f) { m_font = f; }

	virtual bqGUIFont* OnFont(uint32_t r, char32_t) override;
	virtual bqColor* OnColor(uint32_t r, char32_t) override;
};

class bqGUISliderTextDrawCallback : public bqGUIDrawTextCallback
{
	bqGUIFont* m_font = 0;
	bqColor m_color;
public:
	bqGUISliderTextDrawCallback();
	virtual ~bqGUISliderTextDrawCallback();
	BQ_PLACEMENT_ALLOCATOR(bqGUISliderTextDrawCallback);

	void SetFont(bqGUIFont* f) { m_font = f; }

	virtual bqGUIFont* OnFont(uint32_t r, char32_t) override;
	virtual bqColor* OnColor(uint32_t r, char32_t) override;
};


#endif
