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
#ifndef __BQ_GUISTYLE_H__
#define __BQ_GUISTYLE_H__

#include "badcoiq/common/bqColor.h"

enum class bqGUIStyleTheme
{
	Light,
	Dark
};

struct bqGUIStyle
{
	bqColor m_colorWhite = bq::ColorWhite;

	bqColor m_windowActiveBGColor1;
	bqColor m_windowActiveBGColor2;
	bqColor m_windowActiveBorderColor;
	bqColor m_windowActiveTitleBGColor1;
	bqColor m_windowActiveTitleBGColor2;
	bqColor m_windowActiveTitleTextColor;

	bqColor m_buttonBGColor1;
	bqColor m_buttonBGColor2;
	bqColor m_buttonBorderColor;
	bqColor m_buttonTextColor;
	bqColor m_buttonDisabledBGColor1;
	bqColor m_buttonDisabledBGColor2;
	bqColor m_buttonDisabledBorderColor;
	bqColor m_buttonDisabledTextColor;
	bqColor m_buttonMouseHoverBGColor1;
	bqColor m_buttonMouseHoverBGColor2;
	bqColor m_buttonMouseHoverTextColor;
	bqColor m_buttonMousePressBGColor1;
	bqColor m_buttonMousePressBGColor2;
	bqColor m_buttonMousePressTextColor;

	bqColor m_chkradioTextColor;
	bqColor m_chkradioDisabledTextColor;
	bqColor m_chkradioMouseHoverTextColor;
	bqColor m_chkradioMousePressTextColor;

	bqColor m_textEditorBGColor;
	bqColor m_textEditorLine1BGColor;
	bqColor m_textEditorLine2BGColor;
	bqColor m_textEditorTextColor;
	bqColor m_textEditorSelectedTextColor;
	bqColor m_textEditorSelectedTextBGColor;
	bqColor m_textEditorCursorColor;

	bqColor m_staticTextBGColor;
	bqColor m_staticTextTextColor;
	bqGUIFont* m_staticTextFont = 0;

	bqColor m_listboxBGColor;
	bqColor m_listboxLine1BGColor;
	bqColor m_listboxLine2BGColor;
	bqColor m_listboxSelectedLineBGColor;

	bqColor m_sliderTextColor;
	bqColor m_sliderAxisEmtpyColor;
	bqColor m_sliderAxisFillColor;
	bqColor m_sliderControlColor;
};

#endif

