/*
BSD 2-Clause License

Copyright (c) 2023-2025, badcoiq
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

#ifdef BQ_WITH_GUI

#include "badcoiq/containers/bqArray.h"

#include "badcoiq/GUI/bqGUIIcons.h"
#include "badcoiq/GUI/bqGUIFont.h"
#include "badcoiq/GUI/bqGUIText.h"
#include "badcoiq/GUI/bqGUIStyle.h"
#include "badcoiq/GUI/bqGUICommon.h"
#include "badcoiq/GUI/bqGUIElement.h"
#include "badcoiq/GUI/bqGUIRootElement.h"
#include "badcoiq/GUI/bqGUIPopup.h"
#include "badcoiq/GUI/bqGUIWindow.h"

/// \brief Состояние GUI
/// Надо знать такие вещи как, например, какое окно находится под курсором, активен ли popup (он не реализован), активен ли ввод текста и т.д.
struct bqGUIState
{
	bqGUIWindow* m_windowUnderCursor = 0;
	uint32_t m_numWindowsUnderCursor = 0;

	// это копия позиции курсора из bqInputData
	// это значение нужно использовать в GUI
	// так как надо будет временно изменять значение
	// чтобы всё работало как надо.
	bqPointf m_mousePosition;

	// Если нажимаем на меню (это которое FILE EDIT VIEW и т.д.)
	// то, надо запомнить этот факт, чтобы потом блокировать
	// реакцию GUI элементов (чтобы, выбирая пункт меню, тот GUI
	// элемент который находится под меню под курсором не реагировал).
	bqGUIMenu* m_menu = 0;

	bqGUIWindow* m_activeWindow = 0;
	bqGUITextEditor* m_activeTextEditor = 0;
	uint32_t m_scrollBlock = 0;
	//bqGUIPopup* m_activePopup = 0;
	bqGUICommon* m_clickedElement = 0;
};

#include "badcoiq/GUI/bqGUIStaticText.h"

#include "badcoiq/GUI/bqGUIButton.h"
#include "badcoiq/GUI/bqGUICheckRadioBox.h"
#include "badcoiq/GUI/bqGUITextEditor.h"
#include "badcoiq/GUI/bqGUIListBox.h"
#include "badcoiq/GUI/bqGUISlider.h"
#include "badcoiq/GUI/bqGUIPictureBox.h"
#include "badcoiq/GUI/bqGUIScrollbar.h"
#include "badcoiq/GUI/bqGUIToolbar.h"
#include "badcoiq/GUI/bqGUIMenu.h"

#endif
#endif
