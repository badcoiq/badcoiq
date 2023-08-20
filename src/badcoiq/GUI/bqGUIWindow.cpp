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

bqGUIWindow::bqGUIWindow(const bqVec2f& position, const bqVec2f& size)
	:
	bqGUICommon(position, size)
{
	// создаю корневой элемент
	m_rootElement = dynamic_cast<bqGUIElement*>(bqCreate<bqGUIRootElement>(this, position, size));
}

bqGUIWindow::~bqGUIWindow()
{
}

void bqGUIWindow::SetTitle(const char32_t* t)
{
	m_title = t;
}

void bqGUIWindow::SetPositionAndSize(const bqVec2f& p, const bqVec2f& sz)
{
	m_position = p;
	m_size = sz;
}

// перестроение элементов
// рекурсивная функция
// старт в bqGUIWindow::Rebuild()
void _bqGUIWindow_RebuildElement(bqGUIElement* e)
{
	e->Rebuild();

	if (e->GetChildren()->m_head)
	{
		auto children = e->GetChildren();
		if (children->m_head)
		{
			auto curr = children->m_head;
			auto last = curr->m_left;
			while (1)
			{
				_bqGUIWindow_RebuildElement(dynamic_cast<bqGUIElement*>(curr->m_data));
				if (curr == last)
					break;
				curr = curr->m_right;
			}
		}
	}

	e->UpdateContentSize();
	e->UpdateScroll();
}

void bqGUIWindow::Rebuild()
{
	// вычисляю rects
	m_baseRect.x = m_position.x;
	m_baseRect.y = m_position.y;
	m_baseRect.z = m_baseRect.x + m_size.x;
	m_baseRect.w = m_baseRect.y + m_size.y;
	if (m_baseRect.x > m_baseRect.z)
		m_baseRect.x = m_baseRect.z;
	if (m_baseRect.y > m_baseRect.w)
		m_baseRect.y = m_baseRect.w;

	m_clipRect = m_baseRect;
	m_activeRect = m_clipRect;

	// "перестраиваю" root здесь
	m_rootElement->m_baseRect.x = m_baseRect.x;
	m_rootElement->m_baseRect.y = m_baseRect.y;
	m_rootElement->m_baseRect.z = m_baseRect.x + m_size.x;
	m_rootElement->m_baseRect.w = m_baseRect.y + m_size.y;
	m_rootElement->m_clipRect = m_rootElement->m_baseRect;
	m_rootElement->m_activeRect = m_rootElement->m_clipRect;

	// потом перестраиваю другие элементы
	_bqGUIWindow_RebuildElement(m_rootElement);
}

// действия с вводом\реакция на мышь
// рекурсивная функция
// старт в bqGUIWindow::Update()
// проход списка в обратном порядке
void _bqGUIWindow_UpdateElement(bqGUIElement* e)
{
	e->Update();

	if (e->GetChildren()->m_head)
	{
		auto children = e->GetChildren();
		if (children->m_head)
		{
			auto last = children->m_head;
			auto curr = last->m_left;
			while (1)
			{
				_bqGUIWindow_UpdateElement(dynamic_cast<bqGUIElement*>(curr->m_data));
				if (curr == last)
					break;
				curr = curr->m_left;
			}
		}
	}
	e->UpdateScroll();
}

void bqGUIWindow::Update()
{
	if (bqMath::PointInRect(g_framework->m_input.m_mousePosition, m_activeRect))
		g_framework->m_GUIState.m_windowUnderCursor = this;

	_bqGUIWindow_UpdateElement(m_rootElement);
}

// рисование
// рекурсивная функция
// старт в bqGUIWindow::Draw()
void _bqGUIWindow_DrawElement(bqGS* gs, bqGUIElement* e, float dt)
{
	if (e->IsVisible())
	{
		if ((e->m_clipRect.x < e->m_clipRect.z) && (e->m_clipRect.y < e->m_clipRect.w))
			e->Draw(gs, dt);

		if (e->GetChildren()->m_head)
		{
			auto children = e->GetChildren();
			if (children->m_head)
			{
				auto curr = children->m_head;
				auto last = curr->m_left;
				while (1)
				{
					_bqGUIWindow_DrawElement(gs, dynamic_cast<bqGUIElement*>(curr->m_data), dt);
					if (curr == last)
						break;
					curr = curr->m_right;
				}
			}
		}
	}
}

void bqGUIWindow::Draw(bqGS* gs, float dt)
{
	if (IsDrawBG())
		gs->DrawGUIRectangle(m_baseRect, m_style->m_windowActiveBGColor1, m_style->m_windowActiveBGColor2, 0, 0);

	_bqGUIWindow_DrawElement(gs, m_rootElement, dt);
}


