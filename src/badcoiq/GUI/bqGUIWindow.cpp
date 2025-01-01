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

#ifdef BQ_WITH_GUI

#include "badcoiq/GUI/bqGUI.h"
#include "badcoiq/gs/bqGS.h"
#include "badcoiq/input/bqInputEx.h"
#include "badcoiq/system/bqWindow.h"

#include "../framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;

#include "bqGUIDefaultTextDrawCallbacks.h"

inline float
bqGUIWindow_lerp(float v0, float v1, float t)
{
	return (1.f - t) * v0 + t * v1;
}


bqGUIWindowTextDrawCallback::bqGUIWindowTextDrawCallback()
{
}

bqGUIWindowTextDrawCallback::~bqGUIWindowTextDrawCallback()
{
}


bqGUIFont* bqGUIWindowTextDrawCallback::OnFont(uint32_t r, char32_t c)
{
	return 0;// m_window->OnFont(r, c);
}

bqColor* bqGUIWindowTextDrawCallback::OnColor(uint32_t r, char32_t c)
{
	return 0;// m_window->OnColor(r, c);
}

class bqGUIWindowScrollbar : public bqGUIScrollbar
{
public:
	bqGUIWindowScrollbar(bqGUIWindow* w, const bqVec2f& position, const bqVec2f& size)
		:
		bqGUIScrollbar(w, position, size)
	{
		m_flags |= flag_disableParentScroll;
		m_isFull = false;
	}

	virtual ~bqGUIWindowScrollbar() {}
	BQ_PLACEMENT_ALLOCATOR(bqGUIWindowScrollbar);
	
	virtual void OnClickLMB() override
	{
//		m_window->m_rootElement->m_scrollTarget.y = m_value;
//		m_window->m_rootElement->m_scrollLimit.y = m_valueMax;
	}

	// из за работы lerp с m_scrollTarget при прокрутке через колесо мышки
	// прокрутка происходит без lerp, так как каждый раз устанавливается
	//  новый m_scrollTarget.
	// Если код задокументировать то m_scrollTarget.y так и останется на нуле
	//    но крутить мышкой будет ок.
	// Решение - добавить m_newTarget
	bool m_newTarget = true;

	virtual void OnScroll() override
	{
		//m_window->m_rootElement->m_scroll.y = m_value;
		//
		//if(m_newTarget)
		//	m_window->m_rootElement->m_scrollTarget.y = m_value;
		//
		//m_window->m_rootElement->m_scrollLimit.y = m_valueMax;
		//m_window->m_rootElement->m_scrollDelta.y = 0.f;
		//m_window->Rebuild();
	}
};

bqGUIWindow::bqGUIWindow(
	//bqWindow* systemWindow,
	const bqVec2f& position, 
	const bqVec2f& size)
	:
	bqGUICommon(position, size)
{
	// создаю корневой элемент
//	m_rootElement = dynamic_cast<bqGUIElement*>(bqCreate<bqGUIRootElement>(this, position, size));

	// установка дефолтного коллбэка
	m_textDrawCallback = g_framework->m_defaultTextDrawCallback_window;
	
	// Далее, там, где будет использован коллбэк (напрямую или где-то внутри других методов)
	// надо устанавливать указатель на текущее окно. Так
	// bqGUIWindowTextDrawCallback* wcb = (bqGUIWindowTextDrawCallback*)m_textDrawCallback;
	// wcb->m_window = this;
	// 
	
	// установка дефолтного шрифта.
	// перед использованием надо создать дефолтные шрифты
	//bqGUIWindowTextDrawCallback* cb = (bqGUIWindowTextDrawCallback*)g_framework->m_defaultTextDrawCallback_button;
	//cb->SetFont(bqFramework::GetDefaultFont(bqGUIDefaultFont::Text));

//	m_icons = bqFramework::GetDefaultFont(bqGUIDefaultFont::Icons);
//	m_onFont = &bqGUIWindow::_OnFont_active;
//	m_onColor = &bqGUIWindow::_OnColor_active;

	// думаю, обновлять значения скролбара нужно в методе Rebuild.
	// там же устанавливать видимость
//	bqGUIWindowScrollbar* scrlbr = new bqGUIWindowScrollbar(this, bqVec2f(), bqVec2f(20.f, 0.f));
//	m_scrollbar = scrlbr;
//	m_scrollbar->SetVisible(false);
//	Expand();
}

bqGUIWindow::~bqGUIWindow()
{
}

void bqGUIWindowBase::AddElement(bqGUIElement* el)
{
	BQ_ASSERT_ST(el);
	el->SetParent(this);
}

void bqGUIWindowBase::RemoveElement(bqGUIElement* el)
{
	BQ_ASSERT_ST(el);
	el->SetParent(0);
}

void bqGUIWindow::Rebuild()
{
}

void bqGUIWindow::Draw(bqGS* gs, float dt)
{
}


//bqGUIFont* bqGUIWindow::_OnFont_active(uint32_t, char32_t)
//{
//	return m_style->m_windowActiveTitleTextFont;
//}
//
//bqColor* bqGUIWindow::_OnColor_active(uint32_t, char32_t)
//{
//	return &m_style->m_windowActiveTitleTextColor;
//}
//
//bqGUIFont* bqGUIWindow::_OnFont_Nactive(uint32_t, char32_t)
//{
//	return m_style->m_windowNActiveTitleTextFont;
//}
//
//bqColor* bqGUIWindow::_OnColor_Nactive(uint32_t, char32_t)
//{
//	return &m_style->m_windowNActiveTitleTextColor;
//}
//
//
//bqGUIFont* bqGUIWindow::OnFont(uint32_t r, char32_t c)
//{
//	return (this->*m_onFont)(r, c);
//}
//
//bqColor* bqGUIWindow::OnColor(uint32_t r, char32_t c)
//{
//	return (this->*m_onColor)(r, c);
//}
//
//void bqGUIWindow::Activate()
//{
//	// тут надо вырубить предидущее активированное окно
//	// ...
//
//	m_onFont = &bqGUIWindow::_OnFont_active;
//	m_onColor = &bqGUIWindow::_OnColor_active;
//}
//
//void bqGUIWindow::Deactivate()
//{
//	m_onFont = &bqGUIWindow::_OnFont_Nactive;
//	m_onColor = &bqGUIWindow::_OnColor_Nactive;
//}
//
//void bqGUIWindow::SetTitle(const char32_t* t)
//{
//	m_title = t;
//}
//
//void bqGUIWindow::SetPositionAndSize(const bqVec2f& p, const bqVec2f& sz)
//{
//	m_position = p;
//	m_size = sz;
//}
//
//// перестроение элементов
//// рекурсивная функция
//// старт в bqGUIWindow::Rebuild()
//void _bqGUIWindow_RebuildElement(bqGUIElement* e)
//{
//	e->Rebuild();
//
//	if (e->GetChildren()->m_head)
//	{
//		auto children = e->GetChildren();
//		if (children->m_head)
//		{
//			auto curr = children->m_head;
//			auto last = curr->m_left;
//			while (1)
//			{
//				_bqGUIWindow_RebuildElement(dynamic_cast<bqGUIElement*>(curr->m_data));
//				if (curr == last)
//					break;
//				curr = curr->m_right;
//			}
//		}
//	}
//
//	e->UpdateContentSize();
//	e->UpdateScroll();
//}
//
//void bqGUIWindow::Rebuild()
//{
//
//
//	// вычисляю rects
//	m_baseRect.x = m_position.x;
//	m_baseRect.y = m_position.y;
//	m_baseRect.z = m_baseRect.x + m_size.x;
//	m_baseRect.w = m_baseRect.y + m_size.y;
//	
//	// "перестраиваю" root здесь
//    // ... надо проверить...
//	// возможно тут ненадо, так как реализовал перестроение в bqGUIRootElement::Rebuild()
//	m_rootElement->m_baseRect.x = m_baseRect.x;
//	m_rootElement->m_baseRect.y = m_baseRect.y;
//	m_rootElement->m_baseRect.z = m_baseRect.x + m_size.x;
//	m_rootElement->m_baseRect.w = m_baseRect.y + m_size.y;
//
//	m_clipRect = m_baseRect;
//	m_activeRect = m_clipRect;
//
//	// GUI элементы устанавливаются используя значения из m_rootElement
//	// Если окно с title bar, то надо изменить верхнюю границу.
//	// Пусть будет значение m_titlebarHeight для размера titlebar
//	// 
//	// Возможно нужно так-же изменить m_baseRect.y.
//	// 
//	if (m_windowFlags & windowFlag_withTitleBar)
//	{
//		// отодвину m_baseRect.y у m_rootElement
//		m_rootElement->m_baseRect.y += m_titlebarHeight;
//
//		// область titlebar
//		// возможно в будущем нужно будет учитывать значение скроллинга.
//		// например если окно находится в контейнере (пример из комментария у windowFlag_withCollapseButton)
//		m_titlebarRect.x = m_baseRect.x;
//		m_titlebarRect.y = m_baseRect.y;
//		m_titlebarRect.z = m_baseRect.z;
//		m_titlebarRect.w = m_titlebarRect.y + m_titlebarHeight;
//
//		auto rectForBorder = m_titlebarRect;
//
//		if (m_windowFlagsInternal & windowFlagInternal_isExpand)
//			rectForBorder = m_baseRect;
//
//		// вычисление рамки перед m_baseRect.y += m_titlebarHeight;
//		m_borderRectLeft.x = rectForBorder.x;
//		m_borderRectLeft.y = rectForBorder.y;
//		m_borderRectLeft.z = m_borderRectLeft.x + m_borderSize;
//		m_borderRectLeft.w = rectForBorder.w;
//		m_borderRectTop.x = rectForBorder.x;
//		m_borderRectTop.y = rectForBorder.y;
//		m_borderRectTop.z = rectForBorder.z;
//		m_borderRectTop.w = rectForBorder.y + m_borderSize;
//		m_borderRectRight.x = rectForBorder.z - m_borderSize;
//		m_borderRectRight.y = rectForBorder.y;
//		m_borderRectRight.z = rectForBorder.z;
//		m_borderRectRight.w = rectForBorder.w;
//		m_borderRectBottom.x = rectForBorder.x;
//		m_borderRectBottom.y = rectForBorder.w - m_borderSize;
//		m_borderRectBottom.z = rectForBorder.z;
//		m_borderRectBottom.w = rectForBorder.w;
//		m_borderRectLeftTop.x = rectForBorder.x;
//		m_borderRectLeftTop.y = rectForBorder.y;
//		m_borderRectLeftTop.z = m_borderRectLeftTop.x + m_borderSize + m_borderSize;
//		m_borderRectLeftTop.w = m_borderRectLeftTop.y + m_borderSize + m_borderSize;
//		m_borderRectLeftBottom.x = rectForBorder.x;
//		m_borderRectLeftBottom.y = rectForBorder.w - m_borderSize - m_borderSize;
//		m_borderRectLeftBottom.z = m_borderRectLeftBottom.x + m_borderSize + m_borderSize;
//		m_borderRectLeftBottom.w = rectForBorder.w;
//		m_borderRectRightTop.x = rectForBorder.z - m_borderSize - m_borderSize;
//		m_borderRectRightTop.y = rectForBorder.y;
//		m_borderRectRightTop.z = rectForBorder.z;
//		m_borderRectRightTop.w = m_borderRectRightTop.y + m_borderSize + m_borderSize;
//		m_borderRectRightBottom.x = rectForBorder.z - m_borderSize - m_borderSize;
//		m_borderRectRightBottom.y = rectForBorder.w - m_borderSize - m_borderSize;
//		m_borderRectRightBottom.z = rectForBorder.z;
//		m_borderRectRightBottom.w = rectForBorder.w;
//		
//
//		m_baseRect.y += m_titlebarHeight; // m_clipRect и m_activeRect зависят от m_baseRect
//		                                  // пусть m_clipRect m_activeRect находится выше данного блока
//	
//		if (m_icons)
//		{
//			auto g = m_icons->GetGlyphMap()[(uint32_t)bqGUIDefaultIconID::CloseWindow];
//			if (g)
//			{
//				m_closeButtonRect.x = m_titlebarRect.z - g->m_width - m_borderSize;
//				m_closeButtonRect.z = m_closeButtonRect.x + g->m_width;
//				m_closeButtonRect.y = m_titlebarRect.y + m_borderSize;
//				m_closeButtonRect.w = m_closeButtonRect.y + g->m_height;
//			}
//
//			uint32_t iconid = (uint32_t)bqGUIDefaultIconID::ArrowRight;
//
//			if(m_windowFlagsInternal & windowFlagInternal_isExpand)
//			{
//				iconid = (uint32_t)bqGUIDefaultIconID::ArrowDown;
//			}
//
//			g = m_icons->GetGlyphMap()[iconid];
//			if(g)
//			{
//				m_collapseButtonRect.x = m_titlebarRect.x + m_borderSize;
//				m_collapseButtonRect.z = m_collapseButtonRect.x + g->m_width;
//				m_collapseButtonRect.y = m_titlebarRect.y + m_borderSize;
//				m_collapseButtonRect.w = m_collapseButtonRect.y + g->m_height;
//			}
//		}
//
//		
//	}
//
//	if (m_baseRect.x > m_baseRect.z)
//		m_baseRect.x = m_baseRect.z;
//	if (m_baseRect.y > m_baseRect.w)
//		m_baseRect.y = m_baseRect.w;
//
//	m_rootElement->m_clipRect = m_rootElement->m_baseRect;
//	m_rootElement->m_activeRect = m_rootElement->m_clipRect;
//	
//
//	// потом перестраиваю другие элементы
//	_bqGUIWindow_RebuildElement(m_rootElement);
//	
//	if (!(m_windowFlags & windowFlag_disableScrollbar))
//	{
//		// наверное обновлять скролл бар нужно после обновления всех элементов.
//		// для скроллбара нужно знать content size окна
//
//		// Всё равно вычисление control rect не правильно
//		// это видно по поведению в badcoiqConsoleApplication1.exe
//		m_scrollbar->m_valueVisible = GetSize().y;
//		m_scrollbar->m_valueMax = m_rootElement->m_contentSize.y;
//		if (m_windowFlags & windowFlag_withTitleBar)
//		{
//			m_scrollbar->m_valueVisible -= m_titlebarHeight;
//		}
//
//		if (m_scrollbar->m_valueMax > m_scrollbar->m_valueVisible)
//		{
//			m_scrollbar->SetVisible(true);
//		}
//		else
//		{
//			m_scrollbar->SetVisible(false);
//		}
//		//m_scrollbar->SetVisible(true);
//		// так-же надо установить rects
//		m_scrollbar->SetPosition(
//			(m_rootElement->m_baseRect.z - m_rootElement->m_baseRect.x) - m_scrollbar->GetSize().x, 0.f);
//		m_scrollbar->SetSize(m_scrollbar->GetSize().x, m_rootElement->m_baseRect.w - m_rootElement->m_baseRect.y);
//		m_scrollbar->Rebuild();
//		
//		/*... проблема в том что скролбар перестраивается не правильно
//			controlRect не перестраивается
//			Если сунуть вызов m_scrollbar->Rebuild(); ниже в Update
//			то всё будет окей, но вызываться будет каждый кадр.
//			Надо понять, почему там всё окей, а тут нет.*/
//		// проблема была в том что окно скроллится благодоря скроллу root элемента
//		// root элемент не вызывал Rebuild() окна
//		// Добавил проверку на m_scrollDelta. если не 0 то нужен Rebuild
//		// Думаю что так и нужно оставить, делать отдельный скроллинг для окна бесмысленно
//		//  изначально скроллинг был добавлен в GUI элемент из за идеи что многие элементы
//		//   скроллят свои "внутренности" сами. Если элемент нужно перестраивать в зависимости от скроллинга,
//		//   надо делать проверку, есть ли у родителя m_scrollDelta. Я не уверен на счёт прадедов и т.д..
//	}
//}
//
//// действия с вводом\реакция на мышь
//// рекурсивная функция
//// старт в bqGUIWindow::Update()
//// проход списка в обратном порядке
//void _bqGUIWindow_UpdateElement(bqGUIElement* e)
//{
//	e->Update();
//
//	if (e->GetChildren()->m_head)
//	{
//		auto children = e->GetChildren();
//		if (children->m_head)
//		{
//			auto last = children->m_head;
//			auto curr = last->m_left;
//			while (1)
//			{
//				_bqGUIWindow_UpdateElement(dynamic_cast<bqGUIElement*>(curr->m_data));
//				if (curr == last)
//					break;
//				curr = curr->m_left;
//			}
//		}
//	}
//	e->UpdateScroll();
//}
//
//void bqGUIWindow::Update()
//{
//	// сброс.
//    m_windowCursorInfo = CursorInfo_out;
//
//	// отступ сверху. Должен содержать высоту titlebar, возможно в будущем полосу меню и прочие вещи
//	int topIndent = 0;
//	if (m_windowFlags & windowFlag_withTitleBar)
//	{
//		// нужно чтобы topIndent определялся в том числе когда курсор вне m_activeRect
//		topIndent += (int)m_titlebarHeight;
//
//		if (!(m_windowFlags & windowFlag_disableScrollbar))
//		{
//			if (m_rootElement->m_scrollDelta.y != 0.f)
//			{
//				//m_scrollbar->m_value += m_rootElement->m_scrollDelta.y;
//				((bqGUIWindowScrollbar*) m_scrollbar)->m_newTarget = false;
//				m_scrollbar->AddValue(m_rootElement->m_scrollDelta.y);
//				((bqGUIWindowScrollbar*)m_scrollbar)->m_newTarget = true;
//			}
//		}
//	}
//
//	bqFramework::SetActiveCursor(bqFramework::GetDefaultCursor(bqCursorType::Arrow));
//
//	if (bqMath::PointInRect(g_framework->m_input.m_mousePosition, m_activeRect))
//	{	
//		g_framework->m_GUIState.m_windowUnderCursor = this;
//
//	// курсор в окне. значит 100% в области
//	   m_windowCursorInfo = CursorInfo_client;
//
//	// далее надо определить находится ли курсор в titlebar
//	// достаточно проверить по высоте
//	   if (m_windowFlags & windowFlag_withTitleBar)
//	   {
//		   if (g_framework->m_input.m_mousePosition.y < m_titlebarRect.w)
//		   {
//			   if (bqMath::PointInRect(g_framework->m_input.m_mousePosition, m_closeButtonRect))
//			   {
//				   m_windowCursorInfo = CursorInfo_closeButton;
//			   }
//			   else if (bqMath::PointInRect(g_framework->m_input.m_mousePosition, m_collapseButtonRect))
//			   {
//				   m_windowCursorInfo = CursorInfo_collapseButton;
//			   }
//			   else
//				   m_windowCursorInfo = CursorInfo_titlebar;
//		   }
//
//
//		   // пусть рамка будет только когда включен titlebar
//		   if (bqMath::PointInRect(g_framework->m_input.m_mousePosition, m_borderRectLeft))
//		   {
//			   m_windowCursorInfo = CursorInfo_resizeL;
//			   bqFramework::SetActiveCursor(bqFramework::GetDefaultCursor(bqCursorType::SizeWE));
//		   }
//		   else if (bqMath::PointInRect(g_framework->m_input.m_mousePosition, m_borderRectTop))
//		   {
//			   m_windowCursorInfo = CursorInfo_resizeT;
//			   bqFramework::SetActiveCursor(bqFramework::GetDefaultCursor(bqCursorType::SizeNS));
//		   }
//		   else if (bqMath::PointInRect(g_framework->m_input.m_mousePosition, m_borderRectRight))
//		   {
//			   m_windowCursorInfo = CursorInfo_resizeR;
//			   bqFramework::SetActiveCursor(bqFramework::GetDefaultCursor(bqCursorType::SizeWE));
//		   }
//		   else if (bqMath::PointInRect(g_framework->m_input.m_mousePosition, m_borderRectBottom))
//		   {
//			   m_windowCursorInfo = CursorInfo_resizeB;
//			   bqFramework::SetActiveCursor(bqFramework::GetDefaultCursor(bqCursorType::SizeNS));
//		   }
//		   
//		   // углы имеют приоритет. для этого отдельная группа if else
//		   if (bqMath::PointInRect(g_framework->m_input.m_mousePosition, m_borderRectLeftTop))
//		   {
//			   m_windowCursorInfo = CursorInfo_resizeLT;
//			   bqFramework::SetActiveCursor(bqFramework::GetDefaultCursor(bqCursorType::SizeNWSE));
//		   }
//		   else if (bqMath::PointInRect(g_framework->m_input.m_mousePosition, m_borderRectRightTop))
//		   {
//			   m_windowCursorInfo = CursorInfo_resizeRT;
//			   bqFramework::SetActiveCursor(bqFramework::GetDefaultCursor(bqCursorType::SizeNESW));
//		   }
//		   else if (bqMath::PointInRect(g_framework->m_input.m_mousePosition, m_borderRectLeftBottom))
//		   {
//			   m_windowCursorInfo = CursorInfo_resizeLB;
//			   bqFramework::SetActiveCursor(bqFramework::GetDefaultCursor(bqCursorType::SizeNESW));
//		   }
//		   else if (bqMath::PointInRect(g_framework->m_input.m_mousePosition, m_borderRectRightBottom))
//		   {
//			   m_windowCursorInfo = CursorInfo_resizeRB;
//			   bqFramework::SetActiveCursor(bqFramework::GetDefaultCursor(bqCursorType::SizeNWSE));
//		   }
//	   }
//	}
//
//	static float posX = 0;
//	static float posXlerp = 0;
//	static float posY = 0;
//	static float posYlerp = 0;
//
//	bool needRebuild = false;
//
//	if (m_rootElement->m_scrollDelta != 0.f)
//		needRebuild = true;
//
//	// перемещение окна
//	// если курсор на titlebar
//	// над определить первое нажатие по titlebar, запомнить  позицию 
//	if (m_windowCursorInfo == CursorInfo_titlebar)
//	{
//		// если флаг не установлен
//		if (!(m_windowFlagsInternal & windowFlagInternal_isMove))
//		{
//			// и если было нажатие левой кнопки мыши
//			if (g_framework->m_input.m_mouseButtonFlags & bq::MouseFlag_LMBDOWN)
//			{
//				// устанавливаем флаг только если это окно можно перемещать
//				if(m_windowFlags & windowFlag_canMove)
//					m_windowFlagsInternal |= windowFlagInternal_isMove;
//
//				posX = (float)m_position.x;
//				posY = (float)m_position.y;
//				posXlerp = posX;
//				posYlerp = posY;
//			}
//		}
//	}
//
//	// перемещение окна
//	if (m_windowFlagsInternal & windowFlagInternal_isMove)
//	{
//		// если удерживается левая кнопка мыши
//		if (g_framework->m_input.m_mouseButtonFlags & bq::MouseFlag_LMBHOLD)
//		{
//			// изменяю значение цели для lerp
//			posXlerp += g_framework->m_input.m_mouseMoveDelta.x;
//			float lf = g_framework->m_deltaTime * 30.f;
//			if (lf > 1.f)
//				lf = 1.f;
//
//		//	printf("a\n");
//			// вызов lerp
//			// и окно перемещается
//			m_position.x = bqGUIWindow_lerp(m_position.x, posXlerp, lf);
//
//			// для y оси
//			posYlerp += g_framework->m_input.m_mouseMoveDelta.y;
//			m_position.y = bqGUIWindow_lerp(m_position.y, posYlerp, lf);
//
//			needRebuild = true;
//		}
//
//		// прекращение перемещения
//		if (g_framework->m_input.m_mouseButtonFlags & bq::MouseFlag_LMBUP)
//		{
//			m_windowFlagsInternal ^= windowFlagInternal_isMove;
//		}
//	}
//
//
//	//printf("%f %f [%i]\n", m_position.x, m_position.y, topIndent);
//
//	// только если есть кнопка закрытия
//	if (m_windowFlags & windowFlag_withCloseButton)
//	{
//		// закрытие (надо просто изменить видимость)
//		if (m_windowCursorInfo == CursorInfo_closeButton)
//		{
//			// надо запомнить первый щёлк
//			// установлю флаг
//			if (g_framework->m_input.m_mouseButtonFlags & bq::MouseFlag_LMBDOWN)
//			{
//				// так-же, если нажали кнопку, нужно сделать так, чтобы она оставалась
//				// красной (ну или как она там будет выглядеть когда наводим курсор)
//				// может быть даже можно указать специальный цвет для нажатия.
//				// пока просто нужно учеть этот факт в рисовании иконки
//				m_windowFlagsInternal |= windowFlagInternal_closeBtn;
//			}
//		
//		}
//		// на отжатии кнопки
//		if (g_framework->m_input.m_mouseButtonFlags & bq::MouseFlag_LMBUP)
//		{
//			// если флаг установлен (то есть ранее нажимали кнопку) - то закрываем окно
//			if (m_windowCursorInfo == CursorInfo_closeButton)
//			{
//				if (m_windowFlagsInternal & windowFlagInternal_closeBtn)
//				{
//					SetVisible(false);
//				}
//			}
//			m_windowFlagsInternal &= ~windowFlagInternal_closeBtn;
//		}
//	}
//
//	if (m_windowFlags & windowFlag_withCollapseButton)
//	{
//		if (m_windowCursorInfo == CursorInfo_collapseButton)
//		{
//			// будет работать при первом нажатии
//			if (g_framework->m_input.m_mouseButtonFlags & bq::MouseFlag_LMBDOWN)
//			{
//				if (m_windowFlagsInternal & windowFlagInternal_isExpand)
//				{
//					Collapse();
//				}
//				else
//					Expand();
//				needRebuild = true;
//			}
//		}
//	}
//
//	if (m_windowFlags & windowFlag_canResize)
//	{
//		if (g_framework->m_input.m_mouseButtonFlags & bq::MouseFlag_LMBDOWN)
//		{
//			if (m_windowCursorInfo == CursorInfo_resizeL)
//			{
//				m_windowFlagsInternal |= windowFlagInternal_resize;
//				m_windowFlagsInternal |= windowFlagInternal_resizeL;
//			}
//			else if (m_windowCursorInfo == CursorInfo_resizeT)
//			{
//				m_windowFlagsInternal |= windowFlagInternal_resize;
//				m_windowFlagsInternal |= windowFlagInternal_resizeT;
//			}
//			else if (m_windowCursorInfo == CursorInfo_resizeR)
//			{
//				m_windowFlagsInternal |= windowFlagInternal_resize;
//				m_windowFlagsInternal |= windowFlagInternal_resizeR;
//			}
//			else if (m_windowCursorInfo == CursorInfo_resizeB)
//			{
//				m_windowFlagsInternal |= windowFlagInternal_resize;
//				m_windowFlagsInternal |= windowFlagInternal_resizeB;
//			}
//			else if (m_windowCursorInfo == CursorInfo_resizeLT)
//			{
//				m_windowFlagsInternal |= windowFlagInternal_resize;
//				m_windowFlagsInternal |= windowFlagInternal_resizeLT;
//			}
//			else if (m_windowCursorInfo == CursorInfo_resizeRT)
//			{
//				m_windowFlagsInternal |= windowFlagInternal_resize;
//				m_windowFlagsInternal |= windowFlagInternal_resizeRT;
//			}
//			else if (m_windowCursorInfo == CursorInfo_resizeLB)
//			{
//				m_windowFlagsInternal |= windowFlagInternal_resize;
//				m_windowFlagsInternal |= windowFlagInternal_resizeLB;
//			}
//			else if (m_windowCursorInfo == CursorInfo_resizeRB)
//			{
//				m_windowFlagsInternal |= windowFlagInternal_resize;
//				m_windowFlagsInternal |= windowFlagInternal_resizeRB;
//			}
//		}
//		
//		if ((m_windowFlagsInternal & windowFlagInternal_resize))
//		{
//			if (g_framework->m_input.m_mouseButtonFlags & bq::MouseFlag_LMBHOLD)
//			{
//			//	posXlerp += g_framework->m_input.m_mouseMoveDelta.x;
//			
//				if (m_windowFlagsInternal & windowFlagInternal_resizeL)
//				{
//					_resizeL();
//					bqFramework::SetActiveCursor(bqFramework::GetDefaultCursor(bqCursorType::SizeWE));
//				}
//
//				if (m_windowFlagsInternal & windowFlagInternal_resizeR)
//				{
//					_resizeR();
//					bqFramework::SetActiveCursor(bqFramework::GetDefaultCursor(bqCursorType::SizeWE));
//				}
//
//				if (m_windowFlagsInternal & windowFlagInternal_resizeT)
//				{
//					if((m_windowFlagsInternal & windowFlagInternal_isExpand))
//						_resizeT();
//					bqFramework::SetActiveCursor(bqFramework::GetDefaultCursor(bqCursorType::SizeNS));
//				}
//
//				if (m_windowFlagsInternal & windowFlagInternal_resizeB)
//				{
//					if ((m_windowFlagsInternal & windowFlagInternal_isExpand))
//						_resizeB();
//					bqFramework::SetActiveCursor(bqFramework::GetDefaultCursor(bqCursorType::SizeNS));
//				}
//
//				if (m_windowFlagsInternal & windowFlagInternal_resizeLT)
//				{
//					if ((m_windowFlagsInternal & windowFlagInternal_isExpand))
//					{
//						_resizeL();
//						_resizeT();
//					}
//					bqFramework::SetActiveCursor(bqFramework::GetDefaultCursor(bqCursorType::SizeNWSE));
//				}
//
//				if (m_windowFlagsInternal & windowFlagInternal_resizeRT)
//				{
//					if ((m_windowFlagsInternal & windowFlagInternal_isExpand))
//					{
//						_resizeR();
//						_resizeT();
//					}
//					bqFramework::SetActiveCursor(bqFramework::GetDefaultCursor(bqCursorType::SizeNESW));
//				}
//
//				if (m_windowFlagsInternal & windowFlagInternal_resizeLB)
//				{
//					if ((m_windowFlagsInternal & windowFlagInternal_isExpand))
//					{
//						_resizeL();
//						_resizeB();
//					}
//					bqFramework::SetActiveCursor(bqFramework::GetDefaultCursor(bqCursorType::SizeNESW));
//				}
//
//				if (m_windowFlagsInternal & windowFlagInternal_resizeRB)
//				{
//					if ((m_windowFlagsInternal & windowFlagInternal_isExpand))
//					{
//						_resizeR();
//						_resizeB();
//					}
//					bqFramework::SetActiveCursor(bqFramework::GetDefaultCursor(bqCursorType::SizeNWSE));
//				}
//
//				needRebuild = true;
//			}
//		}
//
//		if (g_framework->m_input.m_mouseButtonFlags & bq::MouseFlag_LMBUP)
//		{
//			if (m_windowFlagsInternal & windowFlagInternal_resize)
//			{
//				// снять все resize флаги
//				m_windowFlagsInternal &= ~0xFF800000;
//			}
//		}
//	}
//	
//	// надо передвинуть окно если оно за пределами системного окна
//	if (m_position.x + m_size.x < 30)
//		m_position.x += 30 - (m_position.x + m_size.x);
//
//	if (m_position.x > m_systemWindow->GetCurrentSize()->x - 30)
//		m_position.x = (float)(m_systemWindow->GetCurrentSize()->x - 30);
//
//	if (m_position.y < 0)
//		m_position.y = 0;
//
//	if (m_position.y >= m_systemWindow->GetCurrentSize()->y - topIndent)
//		m_position.y = (float)(m_systemWindow->GetCurrentSize()->y - topIndent);
//
//	if (needRebuild)
//	{
//		Rebuild();
//	}
//
//
//	if(g_framework->m_GUIState.m_activePopup)
//	{
//		return;
//	}
//
//	if(m_windowFlagsInternal & windowFlagInternal_isExpand)
//	{	
//		_bqGUIWindow_UpdateElement(m_rootElement);
//	}
//}
//
//void bqGUIWindow::_resizeL()
//{
//	float oldPos = m_position.x;
//	float oldSz = m_size.x;
//
//	m_position.x += g_framework->m_input.m_mouseMoveDelta.x;
//	m_size.x -= g_framework->m_input.m_mouseMoveDelta.x;
//
//	if (m_size.x < 30.f)
//	{
//		m_position.x = oldPos;
//		m_size.x = oldSz;
//	}
//	if (m_windowCallback)m_windowCallback->OnSizeChange(this);
//}
//
//void bqGUIWindow::_resizeT()
//{
//	float oldPos = m_position.y;
//	float oldSz = m_size.y;
//
//	m_position.y += g_framework->m_input.m_mouseMoveDelta.y;
//	m_size.y -= g_framework->m_input.m_mouseMoveDelta.y;
//
//	if (m_size.y < 30.f)
//	{
//		m_position.y = oldPos;
//		m_size.y = oldSz;
//	}
//	
//	if (m_position.y < 0)
//	{
//		m_position.y = oldPos;
//		m_size.y = oldSz;
//	}
//	if (m_windowCallback)m_windowCallback->OnSizeChange(this);
//}
//
//void bqGUIWindow::_resizeR()
//{
//	float oldSz = m_size.x;
//	m_size.x += g_framework->m_input.m_mouseMoveDelta.x;
//	if (m_size.x < 30.f)
//	{
//		m_size.x = oldSz;
//	}
//	if (m_windowCallback)m_windowCallback->OnSizeChange(this);
//}
//
//void bqGUIWindow::_resizeB()
//{
//	float oldSz = m_size.y;
//	m_size.y += g_framework->m_input.m_mouseMoveDelta.y;
//	if (m_size.y < 30.f)
//	{
//		m_size.y = oldSz;
//	}
//	if(m_windowCallback)m_windowCallback->OnSizeChange(this);
//}
//
//// рисование
//// рекурсивная функция
//// старт в bqGUIWindow::Draw()
//void _bqGUIWindow_DrawElement(bqGS* gs, bqGUIElement* e, float dt)
//{
//	if (e->IsVisible())
//	{
//		if ((e->m_clipRect.x < e->m_clipRect.z) && (e->m_clipRect.y < e->m_clipRect.w)) // непонимаю это
//			e->Draw(gs, dt);
//
//		if (e->GetChildren()->m_head)
//		{
//			auto children = e->GetChildren();
//			if (children->m_head)
//			{
//				auto curr = children->m_head;
//				auto last = curr->m_left;
//				while (1)
//				{
//					_bqGUIWindow_DrawElement(gs, dynamic_cast<bqGUIElement*>(curr->m_data), dt);
//					if (curr == last)
//						break;
//					curr = curr->m_right;
//				}
//			}
//		}
//	}
//}
//
//void bqGUIWindow::Draw(bqGS* gs, float dt)
//{
//	gs->SetScissorRect(m_clipRect);
//
//	if (IsDrawBG() && (m_windowFlagsInternal & windowFlagInternal_isExpand))
//	{
//		gs->DrawGUIRectangle(m_baseRect, m_style->m_windowActiveBGColor1, m_style->m_windowActiveBGColor2, 0, 0);
//	}
//
//	if (m_windowFlags & windowFlag_withTitleBar)
//	{
//		bqGUIWindowTextDrawCallback* wcb = (bqGUIWindowTextDrawCallback*)m_textDrawCallback;
//		wcb->m_window = this;
//
//		gs->DrawGUIRectangle(m_titlebarRect, m_style->m_windowActiveTitleBGColor1, m_style->m_windowActiveTitleBGColor2, 0, 0);
//		if (m_title.size())
//		{
//
//			bqVec2f tp;
//			tp.x = m_titlebarRect.x + m_style->m_windowTitleIndent.x;
//			tp.y = m_titlebarRect.y + m_style->m_windowTitleIndent.y;
//			gs->DrawGUIText(m_title.c_str(), m_title.size(), tp, m_textDrawCallback);
//		}
//
//		// для рисовании кнопки закрыть нужен дефолтный шрифт с иконками
//		if (m_windowFlags & windowFlag_withCloseButton)
//		{
//			bqColor cbc = bq::ColorWhite;
//			
//			if (m_windowCursorInfo == CursorInfo_closeButton
//				|| (m_windowFlagsInternal & windowFlagInternal_closeBtn) )
//				cbc = bq::ColorRed;
//
//			auto g = m_icons->GetGlyphMap()[(uint32_t)bqGUIDefaultIconID::CloseWindow];
//			if (g)
//			{
//				gs->DrawGUIRectangle(
//					m_closeButtonRect,
//					cbc,
//					cbc,
//					m_icons->GetTexture(0),
//					&g->m_UV);
//			}
//		}
//
//		if (m_windowFlags & windowFlag_withCollapseButton)
//		{
//			bqColor cbc = bq::ColorWhite;
//			uint32_t iconid = (uint32_t)bqGUIDefaultIconID::ArrowRight;
//
//			if(m_windowFlagsInternal & windowFlagInternal_isExpand)
//			{
//				iconid = (uint32_t)bqGUIDefaultIconID::ArrowDown;
//			}
//			
//			auto g = m_icons->GetGlyphMap()[iconid];
//			if (g)
//			{
//				gs->DrawGUIRectangle(
//					m_collapseButtonRect,
//					cbc,
//					cbc,
//					m_icons->GetTexture(0),
//					&g->m_UV);
//			}
//		}
//
//		/*gs->DrawGUIRectangle(m_borderRectLeft, bq::ColorBlue, bq::ColorBlue, 0, 0);
//		gs->DrawGUIRectangle(m_borderRectTop, bq::ColorRed, bq::ColorRed, 0, 0);
//		gs->DrawGUIRectangle(m_borderRectRight, bq::ColorGreenYellow, bq::ColorGreenYellow, 0, 0);
//		gs->DrawGUIRectangle(m_borderRectBottom, bq::ColorSeaGreen, bq::ColorSeaGreen, 0, 0);
//		gs->DrawGUIRectangle(m_borderRectLeftTop, bq::ColorDarkViolet, bq::ColorDarkViolet, 0, 0);
//		gs->DrawGUIRectangle(m_borderRectRightTop, bq::ColorFuchsia, bq::ColorFuchsia, 0, 0);
//		gs->DrawGUIRectangle(m_borderRectLeftBottom, bq::ColorGoldenRod, bq::ColorGoldenRod, 0, 0);
//		gs->DrawGUIRectangle(m_borderRectRightBottom, bq::ColorHotPink, bq::ColorHotPink, 0, 0);*/
//	}
//	
//	if(m_windowFlagsInternal & windowFlagInternal_isExpand)
//	{
//		_bqGUIWindow_DrawElement(gs, m_rootElement, dt);
//	}
//
//	bqVec4f cl;
//	cl.z = (float)m_systemWindow->GetCurrentSize()->x;
//	cl.w = (float)m_systemWindow->GetCurrentSize()->y;
//	gs->SetScissorRect(cl);
//}
//
//void bqGUIWindow::Expand()
//{
//	m_windowFlagsInternal |= windowFlagInternal_isExpand;
//}
//
//void bqGUIWindow::Collapse()
//{
//	m_windowFlagsInternal &= ~windowFlagInternal_isExpand;
//}

#endif
