/*
BSD 2-Clause License

Copyright (c) 2025, badcoiq
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
#ifndef __BQ_GUIMENU_H__
/// \cond
#define __BQ_GUIMENU_H__
/// \endcond
#ifdef BQ_WITH_GUI

#include <stack>
#include <vector>

class bqGUIPopup;

struct bqGUIMenuItemInfo
{
	const char32_t* text = 0;
	uint32_t id = 0;
	bqGUIPopup* popup = 0;
};

struct bqGUIMenuItem
{
	bqGUIMenuItemInfo info;
	size_t textLen = 0;
	uint32_t width = 0;
	bqRect rect;
	uint32_t isEnabled = 0;
	void* userData = 0;
};

class bqGUIMenu
{
	struct _menuPopupItemInfo
	{
		bqGUIMenuItem m_item;

		bqString title;
		bqString shortcut_text;
		bqGUIIcons* icons = 0;
		uint32_t icon_index = 0;

	};
	struct _menuTreeNode
	{
		_menuTreeNode* children = 0;
		_menuTreeNode* siblings = 0;
		_menuTreeNode* parent = 0;

		_menuPopupItemInfo itemInfo;
	};
	struct _menuTree
	{
		_menuTreeNode* m_root = 0;
	}m_menuTree;
	std::vector<_menuTreeNode*> m_menuNodes; // for easy delete
	_menuTreeNode* _menu_getLastSibling(_menuTreeNode*, int* num);
	_menuTreeNode* m_menuNodeCurr = 0;
	std::stack<_menuTreeNode*> m_menuNodeCurrPrev;

	void _menu_addMenuItem(bool isSub, const char32_t* title, uint32_t id, const char32_t* shortcut_text, bool enabled);

public:
	bqGUIMenu();
	~bqGUIMenu();
	void Clear();
	void BeginMenu(const char32_t* title, uint32_t id = 0);
	// for separator use 0 for title
	// for Enabled/Disabled use OnIsMenuItemEnabled
	void AddMenuItem(const char32_t* title, uint32_t id, const char32_t* shortcut_text = 0);
	void BeginSubMenu(const char32_t* title, uint32_t id, bool enabled = true);
	void EndSubMenu();
	void EndMenu();

	// ниже код из старых наработок на СИ
	//bqGUIMenuItem* items = 0;
	//uint32_t itemsSize = 0;

	bqGUIDrawTextCallback* textProcessor = 0;

	uint32_t height = 0;
	uint32_t currentHeight = 0;

	/*indent from left side. for logo for expamle*/
	uint32_t indent = 0;

	uint32_t textIndent = 0;

	bqPoint textOffset;

	bqGUIMenuItem* activeItem = 0;
	bqGUIMenuItem* hoverItem = 0;

	bqGUIStyle* userStyle = 0;

	int(*onIsItemEnabled)(bqGUIMenuItem*) = 0;
};


#endif
#endif
