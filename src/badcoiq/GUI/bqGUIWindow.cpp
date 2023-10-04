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
#include "badcoiq/input/bqInputEx.h"

#include "../framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;

#include "bqGUIDefaultTextDrawCallbacks.h"

bqGUIWindowTextDrawCallback::bqGUIWindowTextDrawCallback()
{
	m_color = bq::ColorBlack;
}

bqGUIWindowTextDrawCallback::~bqGUIWindowTextDrawCallback()
{
}


bqGUIFont* bqGUIWindowTextDrawCallback::OnFont(uint32_t r, char32_t)
{
	return m_window->GetStyle()->m_staticTextFont;
}

bqColor* bqGUIWindowTextDrawCallback::OnColor(uint32_t r, char32_t)
{
	return &m_window->GetStyle()->m_staticTextTextColor;
}

bqGUIWindow::bqGUIWindow(const bqVec2f& position, const bqVec2f& size)
	:
	bqGUICommon(position, size)
{
	// создаю корневой элемент
	m_rootElement = dynamic_cast<bqGUIElement*>(bqCreate<bqGUIRootElement>(this, position, size));

	// установка дефолтного коллбэка
	m_textDrawCallback = g_framework->m_defaultTextDrawCallback_window;
	
	// Далее, там, где будет использован коллбэк (напрямую или где-то внутри других методов)
	// надо устанавливать указатель на текущее окно. Так
	// bqGUIWindowTextDrawCallback* wcb = (bqGUIWindowTextDrawCallback*)m_textDrawCallback;
	// wcb->m_window = this;
	// 
	
	// установка дефолтного шрифта.
	// перед использованием надо создать дефолтные шрифты
	bqGUIWindowTextDrawCallback* cb = (bqGUIWindowTextDrawCallback*)g_framework->m_defaultTextDrawCallback_button;
	cb->SetFont(bqFramework::GetDefaultFont(bqGUIDefaultFont::Text));
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
	
	// "перестраиваю" root здесь
	m_rootElement->m_baseRect.x = m_baseRect.x;
	m_rootElement->m_baseRect.y = m_baseRect.y;
	m_rootElement->m_baseRect.z = m_baseRect.x + m_size.x;
	m_rootElement->m_baseRect.w = m_baseRect.y + m_size.y;

	m_clipRect = m_baseRect;
	m_activeRect = m_clipRect;

	// GUI элементы устанавливаются используя значения из m_rootElement
	// Если окно с title bar, то надо изменить верхнюю границу.
	// Пусть будет значение m_titlebarHeight для размера titlebar
	// 
	// Возможно нужно так-же изменить m_baseRect.y.
	// 
	if (m_windowFlags & windowFlag_withTitleBar)
	{
		// отодвину m_baseRect.y у m_rootElement
		m_rootElement->m_baseRect.y += m_titlebarHeight;

		// область titlebar
		// возможно в будущем нужно будет учитывать значение скроллинга.
		// например если окно находится в контейнере (пример из комментария у windowFlag_withCollapseButton)
		m_titlebarRect.x = m_baseRect.x;
		m_titlebarRect.y = m_baseRect.y;
		m_titlebarRect.z = m_baseRect.z;
		m_titlebarRect.w = m_titlebarRect.y + m_titlebarHeight;

		m_baseRect.y += m_titlebarHeight; // m_clipRect и m_activeRect зависят от m_baseRect
		                                  // пусть m_clipRect m_activeRect находится выше данного блока
	}

	if (m_baseRect.x > m_baseRect.z)
		m_baseRect.x = m_baseRect.z;
	if (m_baseRect.y > m_baseRect.w)
		m_baseRect.y = m_baseRect.w;

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
// сброс.
    m_windowCursorInfo = CursorInfo_out;

	if (bqMath::PointInRect(g_framework->m_input.m_mousePosition, m_activeRect))
{	g_framework->m_GUIState.m_windowUnderCursor = this;

// курсор в окне. значит 100% в области
   m_windowCursorInfo = CursorInfo_client;

// далее надо определить находится ли курсир в titlebar
// достаточно проверить по высоте
    if(m_mousePosition.y < m_titlebarRect.w)
    {
        m_windowCursorInfo = CursorInfo_titlebar;
    }

// остальные CursorInfo будут найдены позже 
}

	static float posX = 0;
	static float posXlerp = 0;
	static float posY = 0;
	static float posYlerp = 0;

	// перемещение окна
	// если курсор на titlebar
	//if (m_windowCursorInfo & CursorInfo_titlebar)
	//{
	//	// если флаг не установлен
	//	if (!(m_windowFlagsInternal & windowFlagInternal_isMove))
	//	{
	//		// и если было нажатие левой кнопки мыши
	//		if (g_framework->m_input.m_mouseButtonFlags & bq::MouseFlag_LMBDOWN)
	//		{
	//			// устанавливаем флаг
	//			m_windowFlagsInternal |= windowFlagInternal_isMove;


	//			posX = (float)m_position.x;
	//			posY = (float)m_position.y;
	//			posXlerp = posX;
	//			posYlerp = posY;
	//		}
	//	}
	//}

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
	
	if (m_windowFlags & windowFlag_withTitleBar)
	{
		bqGUIWindowTextDrawCallback* wcb = (bqGUIWindowTextDrawCallback*)m_textDrawCallback;
		wcb->m_window = this;

		gs->DrawGUIRectangle(m_titlebarRect, m_style->m_windowActiveTitleBGColor1, m_style->m_windowActiveTitleBGColor2, 0, 0);
		if (m_title.size())
		{

			bqVec2f tp;
			tp.x = m_titlebarRect.x + m_style->m_windowTitleIndent.x;
			tp.y = m_titlebarRect.y + m_style->m_windowTitleIndent.y;
			gs->DrawGUIText(m_title.c_str(), m_title.size(), tp, m_textDrawCallback);
		}
	}

	_bqGUIWindow_DrawElement(gs, m_rootElement, dt);
}


