﻿/*
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
#ifndef __BQ_GUIWINDOW_H__
/// \cond
#define __BQ_GUIWINDOW_H__
/// \endcond

#ifdef BQ_WITH_GUI

class bqGUIWindowCallback
{
public:
	bqGUIWindowCallback() {}
	virtual ~bqGUIWindowCallback() {}
	virtual void OnSizeChange(bqGUIWindow*) {}
};

/// ПЕРЕРАБОТКА
/// Нужен базовый класс который будет помогать при создании
/// окна и подобным окнам элементам. Что это означает?
/// 1. В окне есть другие элементы. То есть базовый класс
///    будет контейнером.
/// 2. Содержимое должно прокручиваться колёсиком мыши.
///
/// Реализовав данный класс, можно будет реализовать (переписать)
/// класс окна.
/// Необходимо отключить весь гуи, произвести изменения, и
/// включить элементы гуя поочерёдно.
class bqGUIWindowBase : public bqHierarchy
{
protected:
	// для чего пока не помню
	//bqWindow* m_systemWindow = 0;
public:
	bqGUIWindowBase() {}
	virtual ~bqGUIWindowBase() {}

	/// \brief Добавить элемент.
	void AddElement(bqGUIElement* el);

	/// \brief Убрать элемент.
	void RemoveElement(bqGUIElement* el);
};

class bqGUIWindow : public bqGUICommon, public bqGUIWindowBase
{
	friend class bqFramework;
	
public:
	bqGUIWindow(/*bqWindow* systemWindow, */const bqVec2f& position, const bqVec2f& size);
	BQ_PLACEMENT_ALLOCATOR(bqGUIWindow);
	virtual ~bqGUIWindow();

	/// bqGUICommon
	virtual void Rebuild() override;
	virtual void Draw(bqGS* gs, float dt) override;

	// Не надо. потому что наследуется bqHierarchy
	// bqGUIElement* m_rootElement = 0;

//	enum
//	{
//		windowFlag_withCloseButton = 0x1,
//
//		// нахер оно надо?
//		// хотя...
//		// если есть такой контейнер GUI, который принимает окна...
//		// _________________________________________________________
//		//                                     |_v_____Окно_1_____x|
//		//                                     |                   |
//		//                                     |   содержимое      |
//		//                                     |       окна        |
//		//                                     |                   |
//		//                                     |                   |
//		//                                     |_v_____Окно_2_____x|
//		//                                     |                   |
//		//                                     |   содержимое      |
//		//                                     |       окна        |
//		//                                     |                   |
//		//                                     |_v_____Окно_3_____x|
//		//                                     | и т.д.  их можно  |
//		//                                     | будет сворачивать |
//		//                                     |                   |
//		//                                     |_>____Окно_4______x|
//		//                                     |_>____Окно_5______x|
//		//                                     |_>____Окно_6______x|
//		// ____________________________________|___________________|
//		windowFlag_withCollapseButton = 0x2,
//
//		windowFlag_withTitleBar = 0x4,
//		windowFlag_canMove = 0x8,
//		windowFlag_canResize = 0x10,
//		windowFlag_canDock = 0x20, // на будущее
//
//// если я правильно помню
//// я добавил это для того
//// чтобы окно можно было разместить в
//// качестве фона. и кликая по этому окну,
//// оно не должно становится самым верхним
//// не понимаю названия windowFlag_canToTop
//// надо изменить 
//		windowFlag_disableToTop = 0x40, // надо реализовать 
//windowFlag_disableScrollbar = 0x80,
//	};
//
//	// нужно знать, где находится курсор
//	enum
//	{
//		CursorInfo_out,         // за пределами окна
//		CursorInfo_titlebar,    // в titlebar
//		CursorInfo_client,       // в клиентской области
//		CursorInfo_closeButton,   // на кнопке закрытия
//		CursorInfo_collapseButton, // на кнопке сворачивания
//		
//		CursorInfo_resizeRB,       // далее на границах и в  углах
//		CursorInfo_resizeB,
//		CursorInfo_resizeLB,
//		CursorInfo_resizeL,
//		CursorInfo_resizeLT,
//		CursorInfo_resizeT,
//		CursorInfo_resizeRT,
//		CursorInfo_resizeR,
//	};
//	uint32_t m_windowCursorInfo = 0;
//
//private:
//
//	bqString m_title;
//	bqVec4f m_titlebarRect;
//	bqVec4f m_closeButtonRect;
//	bqVec4f m_collapseButtonRect;
//
//	enum
//	{
//		windowFlagInternal_isMove = 0x1,
//		windowFlagInternal_isExpand = 0x2,
//		windowFlagInternal_closeBtn = 0x4, // когда нажимается кнопка для закрытия, нужно запонить первый щёлк мышки
//		windowFlagInternal_reserved1 = 0x8,
//		windowFlagInternal_reserved2 = 0x10,
//		windowFlagInternal_reserved3 = 0x20,
//		windowFlagInternal_reserved4 = 0x40,
//		windowFlagInternal_reserved5 = 0x80,
//		windowFlagInternal_reserved6 = 0x100,
//		windowFlagInternal_reserved7 = 0x200,
//		windowFlagInternal_reserved8 = 0x400,
//		windowFlagInternal_reserved9 = 0x800,
//		windowFlagInternal_reserved10 = 0x1000,
//		windowFlagInternal_reserved11 = 0x2000,
//		windowFlagInternal_reserved12 = 0x4000,
//		windowFlagInternal_reserved13 = 0x8000,
//		windowFlagInternal_reserved14 = 0x10000,
//		windowFlagInternal_reserved15 = 0x20000,
//		windowFlagInternal_reserved16 = 0x40000,
//		windowFlagInternal_reserved17 = 0x80000,
//		windowFlagInternal_reserved18 = 0x100000,
//		windowFlagInternal_reserved19 = 0x200000,
//		windowFlagInternal_reserved20 = 0x400000,
//
//		windowFlagInternal_resize = 0x800000,
//		windowFlagInternal_resizeB = 0x1000000,   // пусть флаги будут с конца
//		windowFlagInternal_resizeR = 0x2000000,   // будет проще проверять if(m_windowFlagsInternal > windowFlagInternal_resize)
//		windowFlagInternal_resizeT = 0x4000000,
//		windowFlagInternal_resizeL = 0x8000000,
//		windowFlagInternal_resizeRB = 0x10000000,
//		windowFlagInternal_resizeRT = 0x20000000,
//		windowFlagInternal_resizeLB = 0x40000000,
//		windowFlagInternal_resizeLT = 0x80000000,
//	};
//	uint32_t m_windowFlagsInternal = 0;
//
//	void _resizeL();
//	void _resizeT();
//	void _resizeR();
//	void _resizeB();
//
//	bqWindow* m_systemWindow = 0;
//	
//	// пока лишь используется как отступ для иконок на titlebar
//	// и для определения попал ли курсор на рамку, которой нет.
//	float m_borderSize = 3.f;
//	// можно добавить прямоугольные области для рамки. почему бы и нет?
//	// так будет проще определять куда вошёл курсор
//	// + возможно можно будет использовать картинки для рисования рамки
//	bqVec4f m_borderRectLeft;
//	bqVec4f m_borderRectTop;
//	bqVec4f m_borderRectRight;
//	bqVec4f m_borderRectBottom;
//	bqVec4f m_borderRectLeftTop;
//	bqVec4f m_borderRectRightTop;
//	bqVec4f m_borderRectLeftBottom;
//	bqVec4f m_borderRectRightBottom;
//
//
//public:
//	bqGUIWindow(const bqVec2f& position, const bqVec2f& size);
//	virtual ~bqGUIWindow();
//	BQ_PLACEMENT_ALLOCATOR(bqGUIWindow);
//	BQ_DELETED_METHODS(bqGUIWindow);
//
//	// Надо будет вызвать Rebuild
//	void SetPositionAndSize(const bqVec2f& p, const bqVec2f& sz);
//
//	virtual void Rebuild() override;
//	virtual void Update() override;
//	virtual void Draw(bqGS* gs, float dt) override;
//
//	void SetTitle(const char32_t*);
//
//	bqWindow* GetSystemWindow() { return m_systemWindow; }
//
//	// элементы открыты так как не нужно производить какие-то дополнительные действия
//	// например при установке значения. Нефиг плодить глупые get/set методы.
//	bqGUIElement* m_rootElement = 0;
//	bqVec2f m_sizeMinimum = bqVec2f(100.f, 30.f);
//	uint32_t m_windowFlags = 0;
//	float m_titlebarHeight = 20.f;
//	
//public:
//	bqGUIFont* OnFont(uint32_t r, char32_t);
//	bqColor* OnColor(uint32_t r, char32_t);
//	
//	void Activate();
//	void Deactivate();
//	void Expand();
//	void Collapse();
//
//	bqGUIFont* m_icons = 0;
//
//	void SetCallback(bqGUIWindowCallback* cb) { m_windowCallback = cb; }
//
//private:
//	bqGUIFont* _OnFont_active(uint32_t r, char32_t);
//	bqColor* _OnColor_active(uint32_t r, char32_t);
//	bqGUIFont* _OnFont_Nactive(uint32_t r, char32_t);
//	bqColor* _OnColor_Nactive(uint32_t r, char32_t);
//	
//	bqGUIFont* (bqGUIWindow::* m_onFont)(uint32_t, char32_t) = 0;
//	bqColor* (bqGUIWindow::* m_onColor)(uint32_t, char32_t) = 0;
//
//	bqGUIWindowCallback* m_windowCallback = 0;
//
//bqGUIScrollbar* m_scrollbar = 0;
//
//public:
//bqGUIScrollbar* GetScrollbar(){ return m_scrollbar; }
};


#endif
#endif
