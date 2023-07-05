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
#ifndef __BQ_WINDOW_H__
#define __BQ_WINDOW_H__

// Когда с окном что-то происходит, оно будет вызывать эти коллбэки
// При создании окна мы обязаны послать указатель на коллбэк
class bqWindowCallback
{
public:
	bqWindowCallback() {}
	virtual ~bqWindowCallback() {}

	// Когда переключились к этому окну
	virtual void OnActivate(bqWindow*) {}
	// Когда переключились к другому окну
	virtual void OnDeactivate(bqWindow*) {}

	// После изменении размера
	virtual void OnSize(bqWindow*) {}
	// Пока изменяем размер
	virtual void OnSizing(bqWindow*) {} // when resizing

	// Свернули окно
	virtual void OnMinimize(bqWindow*) {}
	// Развернули на весь монитор
	virtual void OnMaximize(bqWindow*) {}
	// Восстановили (из Maximized и Minimized в обычное)
	virtual void OnRestore(bqWindow*) {}

	// Когда окно перерисовывается
	virtual void OnDraw(bqWindow*) {}

	// Когда двигается
	virtual void OnMove(bqWindow*) {}

	// Когда кликается кнопка закрытия
	virtual void OnClose(bqWindow*) {}
};

struct bqWindowCommonData
{
	// тут хранится указатель на коллбэк
	bqWindowCallback* m_cb = 0;

	// размер рамки
	bqPoint m_borderSize;

	// минимальный размер окна
	bqPoint m_sizeMinimum;
	
	// текущий размер окна (клиентской области)
	bqPoint m_sizeCurrent;

	// окно видимо или нет
	bool m_isVisible = false;

	// данные специфичные для ОС
	void* m_implementation = 0;

	// Работает ли полноэкранный режим без рамки
	bool m_isFullscreen = false;
};

// Окно
class bqWindow
{
	bqWindowCommonData m_data;
public:
	BQ_PLACEMENT_ALLOCATOR(bqWindow);
	bqWindow(bqWindowCallback* cb);
	~bqWindow();

	// Установить заголовок
	void SetTitle(const char*);

	// Скрыть\показать
	void SetVisible(bool v);

	// Узнать, видимо ли окно.
	// Можно скипать рисование если окно например свёрнуто.
	bool IsVisible() { return m_data.m_isVisible; }

	// Развернуть в полный экран
	void Maximize();
	// Свернуть
	void Minimize();
	// Восстановить оконное состояние (ну типа оконное, не полный экран)
	void Restore();

	// Установить\убрать рамку
	void SetBorderless(bool);

	// Установить\убрать возможность изменять размер
	void SetNoResize(bool);

	// Показать\скрыть кнопку "свернуть окно"
	void SetNoMinimize(bool);

	// Тут, указатели для того чтобы получить их и сохранить где-то, и
	//  не вызывать эти методы каждый раз, так как в таком случае потребуется
	//    вызывать постоянно.
	// Получить минимальный размер окна.
	bqPoint* GetSizeMinimum() { return &m_data.m_sizeMinimum; }
	// Получить размер рамки
	bqPoint* GetBorderSize() { return &m_data.m_borderSize; }

	// Получить данные
	bqWindowCommonData* GetData() { return &m_data; }

	// Установить позицию и размер
	void SetPositionAndSize(int x, int y, int sx, int sy);

	void ToFullscreenMode();
	void ToWindowMode();
};


#endif

