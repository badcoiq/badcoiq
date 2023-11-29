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
#ifndef __BQ_GUI_H__
#define __BQ_GUI_H__

#include "badcoiq/GUI/bqGUIFont.h"
#include "badcoiq/GUI/bqGUIText.h"
#include "badcoiq/GUI/bqGUIStyle.h"
#include "badcoiq/GUI/bqGUICommon.h"
#include "badcoiq/GUI/bqGUIElement.h"
#include "badcoiq/GUI/bqGUIRootElement.h"
#include "badcoiq/GUI/bqGUIPopup.h"
#include "badcoiq/GUI/bqGUIWindow.h"

// Надо знать текущее состояние GUI
// Тут состояние об окнах. Окна хранят своё состояние сами.
struct bqGUIState
{
	bqGUIWindow* m_windowUnderCursor = 0;
	bqGUIWindow* m_activeWindow = 0;
	bqGUITextEditor* m_activeTextEditor = 0;
	uint32_t m_scrollBlock = 0;
	bqGUIPopup* m_activePopup = 0;
};

#include "badcoiq/GUI/bqGUIButton.h"
#include "badcoiq/GUI/bqGUICheckRadioBox.h"
#include "badcoiq/GUI/bqGUITextEditor.h"
#include "badcoiq/GUI/bqGUIListBox.h"
#include "badcoiq/GUI/bqGUISlider.h"
#include "badcoiq/GUI/bqGUIStaticText.h"
#include "badcoiq/GUI/bqGUIPictureBox.h"
#include "badcoiq/GUI/bqGUIScrollbar.h"

#endif

