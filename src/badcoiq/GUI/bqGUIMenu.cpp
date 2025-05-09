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

#include "badcoiq.h"

#ifdef BQ_WITH_GUI


#include "badcoiq/GUI/bqGUI.h"
#include "badcoiq/gs/bqGS.h"
#include "badcoiq/input/bqInputEx.h"

#include "../framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;

bqGUIMenu::bqGUIMenu()
{
}

bqGUIMenu::~bqGUIMenu()
{
	Clear();
}

void bqGUIMenu::Clear()
{
	for (size_t i = 0; i < m_menuNodes.size(); ++i)
	{
		delete m_menuNodes[i];
	}
	m_menuNodes.clear();
	m_menuTree.m_root = 0;
}

bqGUIMenu::_menuTreeNode* bqGUIMenu::_menu_getLastSibling(_menuTreeNode* node, int* num)
{
	*num = 1;

	if (node->siblings)
	{
		_menuTreeNode* sib = node->siblings;
	bgn:;
		*num += 1;
		if (sib == node)
			return node;

		if (!sib->siblings)
			return sib;

		sib = sib->siblings;
		goto bgn;
	}
	return node;
}

void bqGUIMenu::BeginMenu(const char32_t* title, uint32_t id)
{
	_menuTreeNode* newNode = new _menuTreeNode;
	newNode->itemInfo.m_item.info.id = id;
	if (title)
	{
		newNode->itemInfo.title = title;
	}

	if (!m_menuTree.m_root)
	{
		m_menuTree.m_root = newNode;
	}
	else
	{
		int num = 0;
		_menuTreeNode* lastSib = _menu_getLastSibling(m_menuTree.m_root, &num);
		lastSib->siblings = newNode;
	}
	m_menuNodeCurr = newNode;
	m_menuNodes.push_back(m_menuNodeCurr);
}

void bqGUIMenu::_menu_addMenuItem(bool isSub, 
	const char32_t* title, 
	uint32_t id, 
	const char32_t* shortcut_text,
	bool enabled)
{
	_menuPopupItemInfo info;
	info.m_item.isEnabled = enabled;
	info.icons = 0;
	info.icon_index = 0;
	info.m_item.info.id = id;
	if (shortcut_text)
		info.shortcut_text = shortcut_text;
	if (title)
		info.title = title;

	_menuTreeNode* newNode = new _menuTreeNode;
	newNode->itemInfo = info;

	if (!m_menuNodeCurr->children)
	{
		m_menuNodeCurr->children = newNode;
	}
	else
	{
		int num = 0;
		_menuTreeNode* lastSib = _menu_getLastSibling(m_menuNodeCurr->children, &num);
		lastSib->siblings = newNode;
	}

	if (isSub)
	{
		m_menuNodeCurrPrev.push(m_menuNodeCurr);
		m_menuNodeCurr = newNode;
	}

	m_menuNodes.push_back(newNode);
}

void bqGUIMenu::AddMenuItem(const char32_t* title, uint32_t id, const char32_t* shortcut_text)
{
	_menu_addMenuItem(false, title, id, shortcut_text, true);
}

void bqGUIMenu::BeginSubMenu(const char32_t* title, uint32_t id, bool enabled)
{
	_menu_addMenuItem(true, title, id, 0, enabled);
}

void bqGUIMenu::EndSubMenu()
{
	m_menuNodeCurr = m_menuNodeCurrPrev.top();
	m_menuNodeCurrPrev.pop();
}

void bqGUIMenu::EndMenu()
{
	// логично что должно быть EndMenu
	// но тут делать нечего
}


#endif
