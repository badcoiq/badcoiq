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
#ifndef __BQ_GUIWINDOW_H__
#define __BQ_GUIWINDOW_H__

class bqGUIWindow : public bqGUICommon
{
	friend class bqFramework;
	

public:
	enum
	{
		windowFlag_withCloseButton = 0x1,

		// нахер оно надо?
		// хотя...
		// если есть такой контейнер GUI, который принимает окна...
		// _________________________________________________________
		//                                     |_v_____Окно_1_____x|
		//                                     |                   |
		//                                     |   содержимое      |
		//                                     |       окна        |
		//                                     |                   |
		//                                     |                   |
		//                                     |_v_____Окно_2_____x|
		//                                     |                   |
		//                                     |   содержимое      |
		//                                     |       окна        |
		//                                     |                   |
		//                                     |_v_____Окно_3_____x|
		//                                     | и т.д.  их можно  |
		//                                     | будет сворачивать |
		//                                     |                   |
		//                                     |_>____Окно_4______x|
		//                                     |_>____Окно_5______x|
		//                                     |_>____Окно_6______x|
		// ____________________________________|___________________|
		windowFlag_withCollapseButton = 0x2,

		windowFlag_withTitleBar = 0x4,
		windowFlag_canMove = 0x8,
		windowFlag_canResize = 0x10,
		windowFlag_canDock = 0x20,
		windowFlag_canToTop = 0x40,
	};

	// нужно знать, где находится курсор
	enum
	{
		CursorInfo_out,         // за пределами окна
		CursorInfo_titlebar,    // в titlebar
		CursorInfo_client,       // в клиентской области
		CursorInfo_closeButton,   // на кнопке закрытия
		CursorInfo_collapseButton, // на кнопке сворачивания
		
		CursorInfo_resizeRB,       // далее на границах и в  углах
		CursorInfo_resizeB,
		CursorInfo_resizeLB,
		CursorInfo_resizeL,
		CursorInfo_resizeLT,
		CursorInfo_resizeT,
		CursorInfo_resizeRT,
		CursorInfo_resizeR,
	};
	uint32_t m_windowCursorInfo = 0;

private:

	bqString m_title;
	bqVec4f m_titlebarRect;
	bqVec4f m_closeButtonRect;

	enum
	{
		windowFlagInternal_isMove = 0x1
	};
	uint32_t m_windowFlagsInternal = 0;

	bqWindow* m_systemWindow = 0;

public:
	bqGUIWindow(const bqVec2f& position, const bqVec2f& size);
	virtual ~bqGUIWindow();
	BQ_PLACEMENT_ALLOCATOR(bqGUIWindow);

	// Надо будет вызвать Rebuild
	void SetPositionAndSize(const bqVec2f& p, const bqVec2f& sz);

	virtual void Rebuild() override;
	virtual void Update() override;
	virtual void Draw(bqGS* gs, float dt) override;

	void SetTitle(const char32_t*);

	bqWindow* GetSystemWindow() { return m_systemWindow; }

	// элементы открыты так как не нужно производить какие-то дополнительные действия
	// например при установке значения. Нефиг плодить глупые get/set методы.
	bqGUIElement* m_rootElement = 0;
	bqVec2f m_sizeMinimum = bqVec2f(100.f, 30.f);
	uint32_t m_windowFlags = 0;
	float m_titlebarHeight = 20.f;
	
public:
	bqGUIFont* OnFont(uint32_t r, char32_t);
	bqColor* OnColor(uint32_t r, char32_t);

	void Activate();
	void Deactivate();

	bqGUIFont* m_icons = 0;

private:
	bqGUIFont* _OnFont_active(uint32_t r, char32_t);
	bqColor* _OnColor_active(uint32_t r, char32_t);
	bqGUIFont* _OnFont_Nactive(uint32_t r, char32_t);
	bqColor* _OnColor_Nactive(uint32_t r, char32_t);
	
	bqGUIFont* (bqGUIWindow::* m_onFont)(uint32_t, char32_t) = 0;
	bqColor* (bqGUIWindow::* m_onColor)(uint32_t, char32_t) = 0;
};


#endif

