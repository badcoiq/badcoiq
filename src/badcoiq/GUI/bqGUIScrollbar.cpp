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

#include "../framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;
#include "bqGUIDefaultTextDrawCallbacks.h"

bqGUIScrollbar::bqGUIScrollbar(bqGUIWindow* w, const bqVec2f& position, const bqVec2f& size)
	:
	bqGUIElement::bqGUIElement(w, position, size)
{
	m_textDrawCallback = g_framework->m_defaultTextDrawCallback_staticText;
}

bqGUIScrollbar::~bqGUIScrollbar()
{
}

void bqGUIScrollbar::Rebuild()
{
	m_textDrawCallback->m_element = this;
	bqGUIElement::Rebuild();
	bqGUIElement::Update();

	float size = 0.f;
	
	if(m_type == type_vertical)
		size = m_buildRect.w - m_buildRect.y;

	if (size <= 0.f)
		size = 1.f;

	if (m_valueMax <= 0.f)
		m_valueMax = 1.f;

	if (m_valueVisible <= 0.f)
		m_valueVisible = 1.f;


	//bqGUIElement* p = (bqGUIElement*)m_parent;
	//printf("%f p[%f]\n", m_buildRect.x, p->m_buildRect.x);

	m_controlRect.Set(0.f);

	/*
		float Dmax = 1.f / m_valueMax;    //0,0023809523809524
		float Dvis = 1.f / m_valueVisible;//0,0035714285714286

		// 280/420
		float Dcontr = m_valueVisible / m_valueMax; //0,6666666666666667

		// то есть получается это перевод в диапазон от 0 до 1
		// таким образом можно перевести значение в пиксели для построения m_controlRect

		float rectD = Dcontr / Dvis;
	*/

	// на примере текстового редактора
	// если количество строк более видимых
	// то появляется ползунок
	if (m_valueMax > m_valueVisible)  //420 > 280
	{
		float m_valueLimit = m_valueMax - m_valueVisible; // 140

		float Dlim = 1.f / m_valueLimit;    //0,0071428571428571
		float Dmax = 1.f / m_valueMax;    //0,0023809523809524
		float Dvis = 1.f / m_valueVisible;//0,0035714285714286

		 
		// 280/420  //0,6666666666666667
		// 280/140  //2
		float Dcontr = m_valueVisible / m_valueMax;


		// то есть получается это перевод в диапазон от 0 до 1
		// таким образом можно перевести значение в пиксели для построения m_controlRect

		float rectD = Dcontr / Dvis;
		//rectD /= Dlim;

		if (m_type == type_vertical)
		{
			m_controlRect.x = m_buildRect.x + m_controlRectIndent.x;
			m_controlRect.y = m_buildRect.y + m_controlRectIndent.y;
			m_controlRect.z = m_buildRect.z - m_controlRectIndent.z;
			m_controlRect.w = m_controlRect.y + rectD;// +m_controlSizeMinimum;

			if ((m_controlRect.w - m_controlRect.y) < m_controlSizeMinimum)
				m_controlRect.w = m_controlRect.y + m_controlSizeMinimum;
			

			// надо знать оставшиеся пиксели, между дном контрола и дном m_buildRect
			m_remainingPixels = m_buildRect.w - m_controlRect.w;
			if (m_remainingPixels <= 0.f)
				m_remainingPixels = 1.f;

			//printf("%f\n", m_remainingPixels);
		}
		
		// теперь надо передвинуть m_controlRect используя текущее значение

		if (m_value != 0.f)
		{
			// получаю магическое значение когда единичку делим на что-то
			float Dv = 1.f / m_remainingPixels;

			//   ниже комментарии относятся к m_isFull когда используется Dmax
			// m_value является частью m_valueMax, поэтому надо умножить на магическое значение Dmax
			// получаем m_value от 0 до 1 в таких же пределах как и m_valueMax
			// далее делится на магическое значение от m_remainingPixels - Dv
			//  хз как это работает. всё работает благодаря переводам в диапазон от 0 до 1.
			float v = 0.f;
			if(m_isFull)
				v = (m_value * Dmax) / Dv;
			else
				v = (m_value * Dlim) / Dv;

			// пока скроллит как в текстовых редакторах - когда всё уходит наверх, и экран остаётся пустым
			// надо сделать как например в проводнике (в папках)
		//	хз как float Dmax2 = 1.f / (m_valueMax - m_valueVisible);    //0,0023809523809524
		//	v = (m_value * Dmax2) / Dv;
			//  ... решил проблему используя m_valueLimit и Dlim


		//	printf("%f %f\n", m_value, v);

			if (m_type == type_vertical)
			{
				m_controlRect.y += v;
				m_controlRect.w += v;
			}
		}
	}
}

void bqGUIScrollbar::Update()
{
	bqGUIElement::Update();

	if (bqMath::PointInRect(g_framework->m_input.m_mousePosition, m_controlRect))
	{
		m_scrollbarFlags |= scrollbarFlag_cursorInControl;

		if (g_framework->m_input.m_mouseButtonFlags & bq::MouseFlag_LMBDOWN)
		{
			m_scrollbarFlags |= scrollbarFlag_drag;
		}
	}
	else
	{
		m_scrollbarFlags &= ~scrollbarFlag_cursorInControl;
	}

	if (g_framework->m_input.m_mouseButtonFlags & bq::MouseFlag_LMBUP)
	{
		m_scrollbarFlags &= ~scrollbarFlag_drag;
	}

	if (m_scrollbarFlags & scrollbarFlag_drag)
	{
		AddValue(g_framework->m_input.m_mouseMoveDelta.y);

		Rebuild();
	}
}

void bqGUIScrollbar::AddValue(float val)
{
	float size = m_remainingPixels;

	float m_valueLimit = m_valueMax - m_valueVisible;

	float limitValue = m_isFull ? m_valueMax : m_valueLimit;

	float v = limitValue / size;
	if (m_type == type_vertical)
		m_value += v * val;

	if (m_value < 0.f)
		m_value = 0.f;
	if (m_value > limitValue)
		m_value = limitValue;
	OnScroll();
}

void bqGUIScrollbar::Draw(bqGS* gs, float)
{
	gs->SetScissorRect(m_clipRect);
	if (IsDrawBG())
		gs->DrawGUIRectangle(m_buildRect, m_style->m_scrollbarBGColor, m_style->m_scrollbarBGColor, 0, 0);
	m_textDrawCallback->m_element = this;

	bqColor controlColor = m_style->m_scrollbarControlColor;
	if (m_scrollbarFlags & scrollbarFlag_cursorInControl)
	{
		controlColor = m_style->m_scrollbarControlMouseHoverColor;
	}
	else if (m_scrollbarFlags & scrollbarFlag_drag)
	{
		controlColor = m_style->m_scrollbarControlDragColor;
	}

	gs->DrawGUIRectangle(m_controlRect, controlColor, controlColor, 0, 0);
}

#endif
