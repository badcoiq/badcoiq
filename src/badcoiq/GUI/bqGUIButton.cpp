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

#include "framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;

#include "bqGUIDefaultTextDrawCallbacks.h"

bqGUIButtonTextDrawCallback::bqGUIButtonTextDrawCallback()
{
	m_color = bq::ColorBlack;
}

bqGUIButtonTextDrawCallback::~bqGUIButtonTextDrawCallback()
{
}


bqGUIFont* bqGUIButtonTextDrawCallback::OnFont(uint32_t, char32_t)
{
	return m_font;
}

bqColor* bqGUIButtonTextDrawCallback::OnColor(uint32_t, char32_t)
{
	return &m_color;
}

bqGUIButton::bqGUIButton(bqGUIWindow* w, const bqVec2f& position, const bqVec2f& size)
	:
	bqGUIElement::bqGUIElement(w, position, size)
{
	// установка дефолтного коллбэка
	m_textDrawCallback = g_framework->m_defaultTextDrawCallback_button;
	
	// установка дефолтного шрифта.
	// перед использованием надо создать дефолтные шрифты
	bqGUIButtonTextDrawCallback* cb = (bqGUIButtonTextDrawCallback*)g_framework->m_defaultTextDrawCallback_button;
	cb->SetFont(bqFramework::GetDefaultFont(bqGUIDefaultFont::Text));
}

bqGUIButton::~bqGUIButton() {}

void bqGUIButton::Rebuild()
{
	bqGUIElement::Rebuild();
	UpdateTextPosition();
}

void bqGUIButton::Update()
{
	bqGUIElement::Update();
	if (m_window->m_rootElement->m_scrollDelta.y)
		bqGUIButton::Rebuild();
}

void bqGUIButton::Draw(bqGS* gs, float /*dt*/)
{
	gs->SetScissorRect(m_clipRect.GetRect());
	if (IsEnabled())
	{
		if (IsClickedLMB())
		{
			if (IsDrawBG())
				gs->DrawGUIRectangle(m_buildRect, m_style->m_buttonMousePressBGColor1, m_style->m_buttonMousePressBGColor2, m_texture, &m_uv);
			m_textDrawCallback->m_reason = bqGUIDrawTextCallback::Reason_pressed;
			gs->DrawGUIText(m_text.c_str(), m_text.size(), m_textPosition, m_textDrawCallback);
		}
		else
		{
			if (IsCursorInRect())
			{
				if (IsDrawBG())
					gs->DrawGUIRectangle(m_buildRect, m_style->m_buttonMouseHoverBGColor1, m_style->m_buttonMouseHoverBGColor2, m_texture, &m_uv);
				m_textDrawCallback->m_reason = bqGUIDrawTextCallback::Reason_mouseAbove;
				gs->DrawGUIText(m_text.c_str(), m_text.size(), m_textPosition, m_textDrawCallback);
			}
			else
			{
				if (IsDrawBG())
					gs->DrawGUIRectangle(m_buildRect, m_style->m_buttonBGColor1, m_style->m_buttonBGColor2, m_texture, &m_uv);
				m_textDrawCallback->m_reason = bqGUIDrawTextCallback::Reason_default;
				gs->DrawGUIText(m_text.c_str(), m_text.size(), m_textPosition, m_textDrawCallback);
			}
		}
	}
	else
	{
		if (IsDrawBG())
			gs->DrawGUIRectangle(m_buildRect, m_style->m_buttonDisabledBGColor1, m_style->m_buttonDisabledBGColor2, m_texture, &m_uv);
		m_textDrawCallback->m_reason = bqGUIDrawTextCallback::Reason_disabled;
		gs->DrawGUIText(m_text.c_str(), m_text.size(), m_textPosition, m_textDrawCallback);
	}
}

void bqGUIButton::UpdateTextPosition()
{

	m_textPosition.x = m_buildRect.x;
	m_textPosition.y = m_buildRect.y;

	auto tsz = m_textDrawCallback->GetTextSize(m_text.c_str());
	m_textPosition.y = m_buildRect.y + tsz.y;

	bqVec2f buttonCenter;
	buttonCenter.x = (m_buildRect.z - m_buildRect.x) * 0.5f;
	buttonCenter.y = (m_buildRect.w - m_buildRect.y) * 0.5f;

	bqVec2f textHalfLen;
	textHalfLen.x = tsz.x * 0.5f;
	textHalfLen.y = tsz.y * 0.5f;

	switch (m_textAlign)
	{
	case bqGUIElement::LeftTop:
		m_textPosition.x = m_buildRect.x;
		m_textPosition.y = m_buildRect.y;
		break;
	case bqGUIElement::Left:
	case bqGUIElement::Top:
	case bqGUIElement::Right:
	case bqGUIElement::Bottom:
	case bqGUIElement::RightTop:
	case bqGUIElement::LeftBottom:
	case bqGUIElement::RightBottom:
	case bqGUIElement::Center:
		m_textPosition.x = m_buildRect.x + buttonCenter.x - textHalfLen.x;
		m_textPosition.y = m_buildRect.y + buttonCenter.y - textHalfLen.y;
		break;
	}
}

void bqGUIButton::SetText(const bqString& s)
{
	m_text = s;
	UpdateTextPosition();
}


void bqGUIButton::SetTexture(bqTexture* t)
{
	m_texture = t;
}

void bqGUIButton::SetUV(const bqVec4f& uv)
{
	m_uv = uv;
}

void bqGUIButton::SetTCoords(float left, float top, float right, float bottom)
{
	if (m_texture)
	{
		float h = (float)m_texture->GetInfo().m_imageInfo.m_height;
		float w = (float)m_texture->GetInfo().m_imageInfo.m_width;
		m_uv.x = bqMath::CoordToUV(left, w);
		m_uv.y = bqMath::CoordToUV(top, h);
		m_uv.z = bqMath::CoordToUV(right, w);
		m_uv.w = bqMath::CoordToUV(bottom, h);
	}
}
