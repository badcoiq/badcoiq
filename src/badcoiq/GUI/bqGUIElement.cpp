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
#include "badcoiq/math/bqMath.h"

#include "../framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;

bqGUIElement::bqGUIElement(bqGUIWindow* w, const bqVec2f& position, const bqVec2f& size) :
	bqGUICommon(position, size)
{
	BQ_ASSERT_ST(w);
	m_window = w;
	SetParent(w->m_rootElement);
	SetStyle(bqFramework::GetGUIStyle(bqGUIStyleTheme::Light));
}
bqGUIElement::~bqGUIElement() {}

void bqGUIElement::ToTop() 
{
	// забыл написать
}

void bqGUIElement::Rebuild()
{
	bqGUIElement* parent = dynamic_cast<bqGUIElement*>(GetParent());
	if (parent)
	{
		m_baseRect.x = parent->m_baseRect.x + m_position.x;
		m_baseRect.y = parent->m_baseRect.y + m_position.y;
		m_baseRect.z = m_baseRect.x + m_size.x;
		m_baseRect.w = m_baseRect.y + m_size.y;

		if (m_baseRect.x > m_baseRect.z)
			m_baseRect.x = m_baseRect.z;
		if (m_baseRect.y > m_baseRect.w)
			m_baseRect.y = m_baseRect.w;

		bqVec2f parentSize(parent->m_baseRect.z - parent->m_baseRect.x, parent->m_baseRect.w - parent->m_baseRect.y);
		if (parentSize.x && parentSize.y)
		{
			// Это вычисление не верное
			// Достаточно брать высоту и длинну, центр искать не надо
		//	bqVec2f parentCreationCenter;
		//	parentCreationCenter.x = parent->m_buildRectOnCreation.x + ((parent->m_buildRectOnCreation.z - parent->m_buildRectOnCreation.x) * 0.5f);
		//	parentCreationCenter.y = parent->m_buildRectOnCreation.y + ((parent->m_buildRectOnCreation.w - parent->m_buildRectOnCreation.y) * 0.5f);
		//	bqVec2f parentCurrentCenter;
		//	parentCurrentCenter.x = parent->m_buildRect.x + ((parent->m_buildRect.z - parent->m_buildRect.x) * 0.5f);
		//	parentCurrentCenter.y = parent->m_buildRect.y + ((parent->m_buildRect.w - parent->m_buildRect.y) * 0.5f);
			//parentCurrentCenter.x = parent->m_position.x + (parent->m_size.x * 0.5f);
			//parentCurrentCenter.y = parent->m_position.y + (parent->m_size.y * 0.5f);
		//	float parentRectSizeDiff_X = parentCurrentCenter.x - parentCreationCenter.x;
		//	float parentRectSizeDiff_Y = parentCurrentCenter.y - parentCreationCenter.y;

			bqVec2f parentCrSz;
			parentCrSz.x = parent->m_buildRectOnCreation.z - parent->m_buildRectOnCreation.x;
			parentCrSz.y = parent->m_buildRectOnCreation.w - parent->m_buildRectOnCreation.y;

			bqVec2f parentCurSz;
			parentCurSz.x = parent->m_buildRect.z - parent->m_buildRect.x;
			parentCurSz.y = parent->m_buildRect.w - parent->m_buildRect.y;

			float parentRectSizeDiff_X = (parentCurSz.x - parentCrSz.x) * 0.5f;
			float parentRectSizeDiff_Y = (parentCurSz.y - parentCrSz.y) * 0.5f;

			// выравнивание
			switch (m_alignment)
			{
			case bqGUIElement::Left:
				m_baseRect.y += parentRectSizeDiff_Y;
				m_baseRect.w += parentRectSizeDiff_Y;
				break;
			case bqGUIElement::Top:
				m_baseRect.x += parentRectSizeDiff_X;
				m_baseRect.z += parentRectSizeDiff_X;
				break;
			case bqGUIElement::Center:
				m_baseRect.y += parentRectSizeDiff_Y;
				m_baseRect.w += parentRectSizeDiff_Y;
				m_baseRect.x += parentRectSizeDiff_X;
				m_baseRect.z += parentRectSizeDiff_X;
				break;
			case bqGUIElement::Right:
				m_baseRect.y += parentRectSizeDiff_Y;
				m_baseRect.w += parentRectSizeDiff_Y;

				m_baseRect.x = parent->m_baseRect.z -
					(parent->m_buildRectOnCreation.z - m_buildRectOnCreation.x) + parent->m_buildRectOnCreation.x;

				m_baseRect.z = parent->m_baseRect.z -
					(parent->m_buildRectOnCreation.z - m_buildRectOnCreation.z) + parent->m_buildRectOnCreation.x;
				break;
			case bqGUIElement::Bottom:
				m_baseRect.x += parentRectSizeDiff_X;
				m_baseRect.z += parentRectSizeDiff_X;

				m_baseRect.y = parent->m_baseRect.w -
					(parent->m_buildRectOnCreation.w - m_buildRectOnCreation.y) + parent->m_buildRectOnCreation.y;

				m_baseRect.w = parent->m_baseRect.w -
					(parent->m_buildRectOnCreation.w - m_buildRectOnCreation.w) + parent->m_buildRectOnCreation.y;
				break;
			case bqGUIElement::LeftTop:
				break;
			case bqGUIElement::RightTop:
				m_baseRect.x = parent->m_baseRect.z -
					(parent->m_buildRectOnCreation.z - m_buildRectOnCreation.x) + parent->m_buildRectOnCreation.x;

				m_baseRect.z = parent->m_baseRect.z -
					(parent->m_buildRectOnCreation.z - m_buildRectOnCreation.z) + parent->m_buildRectOnCreation.x;
				break;
			case bqGUIElement::LeftBottom:
				m_baseRect.y = parent->m_baseRect.w -
					(parent->m_buildRectOnCreation.w - m_buildRectOnCreation.y) + parent->m_buildRectOnCreation.y;

				m_baseRect.w = parent->m_baseRect.w -
					(parent->m_buildRectOnCreation.w - m_buildRectOnCreation.w) + parent->m_buildRectOnCreation.y;
				break;
			case bqGUIElement::RightBottom:
				m_baseRect.x = parent->m_baseRect.z -
					(parent->m_buildRectOnCreation.z - m_buildRectOnCreation.x) + parent->m_buildRectOnCreation.x;

				m_baseRect.z = parent->m_baseRect.z -
					(parent->m_buildRectOnCreation.z - m_buildRectOnCreation.z) + parent->m_buildRectOnCreation.x;
				m_baseRect.y = parent->m_baseRect.w -
					(parent->m_buildRectOnCreation.w - m_buildRectOnCreation.y) + parent->m_buildRectOnCreation.y;

				m_baseRect.w = parent->m_baseRect.w -
					(parent->m_buildRectOnCreation.w - m_buildRectOnCreation.w) + parent->m_buildRectOnCreation.y;
				break;
			default:
				break;
			}
		}
	}


	// else
	// только root element без родителя
	// перестроить root в bqGUIWindow::Rebuild();

	bqGUIElement::Update(); // не надо вызывать -переполнение стека
	                         // но если написать bqGUIElement::Update() то ошибка пропадёт
}

void bqGUIElement::Update()
{
	bqGUICommon::Update();

	// m_buildRect строится каждый раз заного на основе m_baseRect

	m_buildRect = m_baseRect;
	// обычно m_clipRect это == m_baseRect;
	m_clipRect = m_buildRect;

	bqGUIElement* parent = dynamic_cast<bqGUIElement*>(GetParent());
	if (parent && !(m_flags & flag_disableParentScroll))
	{
		m_buildRect.x -= parent->m_scroll.x;
		m_buildRect.y -= parent->m_scroll.y;
		m_buildRect.z -= parent->m_scroll.x;
		m_buildRect.w -= parent->m_scroll.y;

		m_clipRect = m_buildRect;

		if (m_clipRect.x < parent->m_clipRect.x)
			m_clipRect.x = parent->m_clipRect.x;
		if (m_clipRect.y < parent->m_clipRect.y)
			m_clipRect.y = parent->m_clipRect.y;
		if (m_clipRect.z > parent->m_clipRect.z)
			m_clipRect.z = parent->m_clipRect.z;
		if (m_clipRect.w > parent->m_clipRect.w)
			m_clipRect.w = parent->m_clipRect.w;
	}
	m_activeRect = m_clipRect;
}
