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
#ifndef __BQ_GUISCRLBAR_H__
#define __BQ_GUISCRLBAR_H__

// сам по себе этот элемент должен представлять ползунок, вертикальный или коризонтальный
// как bqGUISlider. Но здесь я вижу bqGUIScrollbar чуть иначе чем слайдер.
// У bqGUIScrollbar должно быть значение. Это значение будет брать окно для значения m_scroll
//    таким образом можно будет прикрутить скролбар к окну.
class bqGUIScrollbar : public bqGUIElement
{
	bqVec4f m_controlRect; // та самая часть за которую тянем
	float m_remainingPixels = 0.f;
public:
	bqGUIScrollbar(bqGUIWindow*, const bqVec2f& position, const bqVec2f& size);
	virtual ~bqGUIScrollbar();
	BQ_PLACEMENT_ALLOCATOR(bqGUIScrollbar);

	virtual void Rebuild() final;
	virtual void Update() final;
	virtual void Draw(bqGS* gs, float dt) final;

	virtual void OnScroll() {}

	enum
	{
		type_vertical,
		//type_horizontal // реализуй меня
	};
	uint32_t m_type = type_vertical;

	// Текущее значение и максимальное.
	// Отрицательное быть не может (думаю что не нужно).
	// При изменении этих значений надо перестроить m_controlRect
	// Для этого надо вызвать Rebuild()
	// Если прикрутить ползунок к текстовому редактору, то например
	//   при увеличении количества строк, скорее всего нужно будет
	//   увеличить значение m_maxValue. Так-же должны быть какие-то лимиты,
	//    по крайней мере лимит на m_controlRect, чтобы он не стал слишком маленьким.
	float m_value = 0.f;         // например текущая строка
	float m_valueMax = 100.f;    // например общее количество строк
	// чтобы настроить размер ползунка например как в текстовых редакторах или как в проводнике Windows
	// нужно знать ещё количество видимых элементов. Пусть это будет переменная m_valueVisible
	float m_valueVisible = 50.f; // например количество видимых строк

	// чтобы ползунок не занимал всю широту фона, надо его уменьшить
	// отступы left top right
	bqVec3f m_controlRectIndent = bqVec3f(2.f, 1.f, 2.f);
	float m_controlSizeMinimum = 17.f;

	enum
	{
		scrollbarFlag_cursorInControl = 0x1,
		scrollbarFlag_drag = 0x2
	};
	uint32_t m_scrollbarFlags = 0;
};

#endif

