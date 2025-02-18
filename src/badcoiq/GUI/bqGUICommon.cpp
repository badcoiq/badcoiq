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
#include "badcoiq/math/bqMath.h"

#include "../framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;

void bqGUICommon_buildRectOnCreation(bqVec4f& buildRectOnCreation, const bqVec2f& position, const bqVec2f& size)
{
	buildRectOnCreation.x = position.x;
	buildRectOnCreation.y = position.y;
	buildRectOnCreation.z = position.x + size.x;
	buildRectOnCreation.w = position.y + size.y;
}

// усановка позиции, размера и создание m_buildRectOnCreation
bqGUICommon::bqGUICommon(const bqVec2f& position, const bqVec2f& size)
{
	m_position = position;
	m_size = size;
	bqGUICommon_buildRectOnCreation(m_buildRectOnCreation, m_position, m_size);
	m_baseRect = m_buildRectOnCreation;
}

void bqGUICommon::SetPosition(float x, float y)
{
	m_position.x = x;
	m_position.y = y;
	bqGUICommon_buildRectOnCreation(m_buildRectOnCreation, m_position, m_size);
	m_baseRect = m_buildRectOnCreation;
}

void bqGUICommon::SetSize(float x, float y)
{
	m_size.x = x;
	m_size.y = y;
	bqGUICommon_buildRectOnCreation(m_buildRectOnCreation, m_position, m_size);
	m_baseRect = m_buildRectOnCreation;
}

bqGUICommon::~bqGUICommon() 
{
	BQ_SAFEDESTROY(m_name);
}

void bqGUICommon::OnMouseEnter() {}
void bqGUICommon::OnMouseLeave() {}
void bqGUICommon::OnClickLMB() {}
void bqGUICommon::OnClickRMB() {}
void bqGUICommon::OnClickMMB() {}
void bqGUICommon::OnClickX1MB() {}
void bqGUICommon::OnClickX2MB() {}
void bqGUICommon::OnReleaseLMB() {}
void bqGUICommon::OnReleaseRMB() {}
void bqGUICommon::OnReleaseMMB() {}
void bqGUICommon::OnReleaseX1MB() {}
void bqGUICommon::OnReleaseX2MB() {}

// Базовый размер содержимого это размер прямоугольника.
// Для окна нужен отдельный метод.
void bqGUICommon::UpdateContentSize()
{
	m_contentSize.x = m_baseRect.z - m_baseRect.x;
	m_contentSize.y = m_baseRect.w - m_baseRect.y;
	UpdateScrollLimit();
}

// Если размер содержимого больше прямоугольной области ко которой будет строиться элемент
// то вычисляется лимит. Если нет то лимит 0 и ничего скроллить нельзя.
void bqGUICommon::UpdateScrollLimit()
{
	m_scrollLimit.x = 0.f;
	m_scrollLimit.y = 0.f;

	bqVec2f buildRectSize;
	buildRectSize.x = m_baseRect.z - m_baseRect.x;
	buildRectSize.y = m_baseRect.w - m_baseRect.y;

	if (m_contentSize.x > buildRectSize.x)
		m_scrollLimit.x = m_contentSize.x - buildRectSize.x;
	if (m_contentSize.y > buildRectSize.y)
		m_scrollLimit.y = m_contentSize.y - buildRectSize.y;
}

// Тут происходит скроллинг
void bqGUICommon::UpdateScroll()
{
	if (IsCursorInRect()
		&& !g_framework->m_GUIState.m_scrollBlock
		&& (m_flags & bqGUICommon::flag_wheelScroll))
	{
		g_framework->m_GUIState.m_scrollBlock = true;

		if (g_framework->m_input.m_mouseWheelDelta < 0.f)
		{
			m_scrollTarget.y += 10.f;
			if (m_scrollTarget.y > m_scrollLimit.y)
				m_scrollTarget.y = m_scrollLimit.y;
		}
		else if (g_framework->m_input.m_mouseWheelDelta > 0.f)
		{
			m_scrollTarget.y -= 10.f;

			if (m_scrollTarget.y < 0.f)
				m_scrollTarget.y = 0.f;
		}
	}

	m_scrollOld = m_scroll;

	if(m_scroll.y != m_scrollTarget.y)
		m_scroll.y = bqMath::Lerp1(m_scroll.y, m_scrollTarget.y, 0.111111f);

	m_scrollDelta = m_scroll - m_scrollOld;
	if (m_scrollDelta.y < 0.0001f && m_scrollDelta.y > -0.0001f)
	{
		m_scrollDelta.y = 0.f;
		m_scroll.y = m_scrollTarget.y;
	}
}

// Работа с мышкой
void bqGUICommon::Update()
{
	if (bqMath::PointInRect(g_framework->m_GUIState.m_mousePosition, m_activeRect))
	{
		if ((m_flags & bqGUICommon::flag_cursorInRect) == 0)
			OnMouseEnter();

		m_flags |= bqGUICommon::flag_cursorInRect;
	}
	else
	{
		if (IsCursorInRect())
			OnMouseLeave();

		m_flags &= ~bqGUICommon::flag_cursorInRect;
	}

	if (IsEnabled())
	{
		if (IsCursorInRect())
		{
			if (bqInput::IsLMBHit())
			{
				m_flags |= bqGUICommon::flag_clickedLMB;
				g_framework->m_GUIState.m_clickedElement = this;
				OnClickLMB();
			}
			else if (bqInput::IsLMBRelease())
			{
				if (IsClickedLMB())
					OnReleaseLMB();
			}

			if (bqInput::IsRMBHit())
			{
				m_flags |= bqGUICommon::flag_clickedRMB;
				g_framework->m_GUIState.m_clickedElement = this;
				OnClickRMB();
			}
			else if (bqInput::IsRMBRelease())
			{
				if (IsClickedRMB())
					OnReleaseRMB();
			}

			if (bqInput::IsMMBHit())
			{
				m_flags |= bqGUICommon::flag_clickedMMB;
				g_framework->m_GUIState.m_clickedElement = this;
				OnClickMMB();
			}
			else if (bqInput::IsMMBRelease())
			{
				if (IsClickedMMB())
					OnReleaseMMB();
			}

			if (bqInput::IsX1MBHit())
			{
				m_flags |= bqGUICommon::flag_clickedX1MB;
				g_framework->m_GUIState.m_clickedElement = this;
				OnClickX1MB();
			}
			else if (bqInput::IsX1MBRelease())
			{
				if (IsClickedX1MB())
					OnReleaseX1MB();
			}

			if (bqInput::IsX2MBHit())
			{
				m_flags |= bqGUICommon::flag_clickedX2MB;
				g_framework->m_GUIState.m_clickedElement = this;
				OnClickX2MB();
			}
			else if (bqInput::IsX2MBRelease())
			{
				if (IsClickedX2MB())
					OnReleaseX2MB();
			}
		}
		else 
		{
			if (bqInput::IsLMBRelease())
			{
				if (IsClickedLMB())
					OnReleaseLMB();
			}
			if (bqInput::IsRMBRelease())
			{
				if (IsClickedRMB())
					OnReleaseRMB();
			}
			if (bqInput::IsMMBRelease())
			{
				if (IsClickedMMB())
					OnReleaseMMB();
			}
			if (bqInput::IsX1MBRelease())
			{
				if (IsClickedX1MB())
					OnReleaseX1MB();
			}
			if (bqInput::IsX2MBRelease())
			{
				if (IsClickedX2MB())
					OnReleaseX2MB();
			}
		}

	}

	if (bqInput::IsLMBRelease())
	{
		g_framework->m_GUIState.m_clickedElement = 0;
		m_flags &= ~bqGUICommon::flag_clickedLMB;
	}
	if (bqInput::IsRMBRelease())
	{
		g_framework->m_GUIState.m_clickedElement = 0;
		m_flags &= ~bqGUICommon::flag_clickedRMB;
	}
	if (bqInput::IsMMBRelease())
	{
		g_framework->m_GUIState.m_clickedElement = 0;
		m_flags &= ~bqGUICommon::flag_clickedMMB;
	}
	if (bqInput::IsX1MBRelease())
	{
		g_framework->m_GUIState.m_clickedElement = 0;
		m_flags &= ~bqGUICommon::flag_clickedX1MB;
	}
	if (bqInput::IsX2MBRelease())
	{
		g_framework->m_GUIState.m_clickedElement = 0;
		m_flags &= ~bqGUICommon::flag_clickedX2MB;
	}
}

void bqGUICommon::SetName(const char* n)
{
	if (n)
	{
		if (strlen(n))
		{
			if (!m_name)
				m_name = new bqStringA;
			m_name->assign(n);
		}
	}
	else
	{
		if (m_name)
		{
			BQ_SAFEDESTROY(m_name);
		}
	}
}

const char* bqGUICommon::GetName()
{
	if (m_name)
	{
		return m_name->c_str();
	}
	return 0;
}

#endif
