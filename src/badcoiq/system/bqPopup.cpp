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

#ifdef BQ_WITH_POPUP

#include "badcoiq/system/bqWindow.h"
#include "badcoiq/system/bqWindowWin32.h"
#include "badcoiq/system/bqPopup.h"

#include "../framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;


#ifdef BQ_PLATFORM_WINDOWS
#include "badcoiq/system/bqPopupWin32.h"
#endif


bqPopupWin32::bqPopupWin32()
{
	m_hPopupMenu = CreatePopupMenu();
}

bqPopupWin32::~bqPopupWin32()
{
	for (size_t i = 0, sz = m_subMenus.size(); i < sz; ++i)
	{
		delete m_subMenus[i];
	}
	DestroyMenu(m_hPopupMenu);
}


bqPopup* bqPopupWin32::CreateSubMenu(const wchar_t* text)
{
	bqPopupWin32* newSubMenu = new bqPopupWin32;
	m_subMenus.push_back(newSubMenu);
	AppendMenu(m_hPopupMenu, MF_POPUP | MF_BYPOSITION | MF_STRING, (UINT_PTR)newSubMenu->m_hPopupMenu, text);
	return newSubMenu;
}

void bqPopupWin32::AddItem(const wchar_t* _text, uint32_t id, const wchar_t* shortcut)
{
	bqStringW text = _text;
	if (shortcut)
	{
		text += L"\t";
		text += shortcut;
	}
	AppendMenu(m_hPopupMenu, MF_BYPOSITION | MF_STRING, id, text.data());
}

void bqPopupWin32::AddSeparator()
{
	AppendMenu(m_hPopupMenu, MF_SEPARATOR, 0, 0);
}

void bqPopupWin32::Show(bqWindow* activeWindow, uint32_t x, uint32_t y)
{
	auto wd = (bqWindowWin32*)activeWindow->GetData()->m_implementation;
	HWND hWnd = wd->m_hWnd;
	SetForegroundWindow(hWnd);
	POINT pt;
	pt.x = x;
	pt.y = y;
	ClientToScreen(hWnd, &pt);
	TrackPopupMenu(m_hPopupMenu, TPM_TOPALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hWnd, NULL);
}


#endif
