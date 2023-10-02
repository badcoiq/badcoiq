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

bqGUISliderTextDrawCallback::bqGUISliderTextDrawCallback()
{
	m_color = bq::ColorBlack;
}

bqGUISliderTextDrawCallback::~bqGUISliderTextDrawCallback()
{
}

bqGUIFont* bqGUISliderTextDrawCallback::OnFont(uint32_t r, char32_t c)
{
	return m_font;
}

bqColor* bqGUISliderTextDrawCallback::OnColor(uint32_t r, char32_t c)
{
	return &m_color;
}

bqGUISlider::bqGUISlider(bqGUIWindow* w, const bqVec2f& position, const bqVec2f& size)
	:
	bqGUIElement(w, position, size)
{
	bqGUISliderTextDrawCallback* cb = (bqGUISliderTextDrawCallback*)g_framework->m_defaultTextDrawCallback_slider;
	cb->SetFont(bqFramework::GetDefaultFont(bqGUIDefaultFont::Text));

	m_textDrawCallback = cb;

	SetDrawBG(false);
}

bqGUISlider::~bqGUISlider() {}


void bqGUISlider::Update()
{
	bqGUIElement::Update();
	if (m_window->m_rootElement->m_scrollDelta.y)
		Rebuild();

	if (!m_isClicked)
	{
		if (bqMath::PointInRect(bqInput::GetData()->m_mousePosition, m_controlRect))
		{
			if (bqInput::IsLMBHit())
				m_isClicked = true;
		}
	}
	else
	{
		auto id = bqInput::GetData();

		if (bqInput::IsKeyHit(bqInput::KEY_ESCAPE))
			m_isClicked = false;
		else if (bqInput::IsLMBRelease())
			m_isClicked = false;
		else if (id->m_mouseMoveDelta.x != 0.f)
		{
			float value = 0.f;
			if (id->m_mousePosition.x <= m_axisRect.x)
				value = 0.f;
			else if (id->m_mousePosition.x >= m_axisRect.z)
				value = 1.f;
			else
			{
				float mouseWidth = id->m_mousePosition.x - m_axisRect.x;
				if (m_axisWidth != 0.f)
				{
					float M = 1.f / m_axisWidth;
					value = mouseWidth * M;
				}
			}

			//printf("%f\n", value);
			if (m_morePrecise)
			{
				auto V = (double)bqInput::GetData()->m_mouseMoveDelta.x * m_multiplerNormal;

				if (id->m_keyboardModifier == bq::KeyboardMod_Alt)
					V *= m_multiplerSlow;
				if (id->m_keyboardModifier == bq::KeyboardMod_Shift)
					V *= m_multiplerFast;

				m_valueFloat += V;
				m_valueInt += (int32_t)bqInput::GetData()->m_mouseMoveDelta.x;
				m_valueUint += (uint32_t)bqInput::GetData()->m_mouseMoveDelta.x;
			}
			else
				m_valueFloat = bqMath::Lerp1(m_valueMinFloat, m_valueMaxFloat, (double)value);

			this->OnChangeValue();


			if (m_valueFloat < m_valueMinFloat) m_valueFloat = m_valueMinFloat;
			if (m_valueFloat > m_valueMaxFloat) m_valueFloat = m_valueMaxFloat;

			if (m_valueInt < m_valueMinInt) m_valueInt = m_valueMinInt;
			if (m_valueInt > m_valueMaxInt) m_valueInt = m_valueMaxInt;

			if (m_valueUint < m_valueMinUint) m_valueUint = m_valueMinUint;
			if (m_valueUint > m_valueMaxUint) m_valueUint = m_valueMaxUint;

			findAxisRectFill();
		}
	}
}


void bqGUISlider::Rebuild()
{
	bqGUIElement::Rebuild();

	auto H = m_buildRect.w - m_buildRect.y;

	m_axisRect.x = m_buildRect.x + m_axisIndent;
	m_axisRect.z = m_buildRect.z - m_axisIndent;

	m_axisRect.y = m_buildRect.y + (H * 0.5f);
	m_axisRect.y -= m_axisHeight * 0.5f;
	m_axisRect.w = m_axisRect.y + m_axisHeight;

	m_axisWidth = m_axisRect.z - m_axisRect.x;

	findAxisRectFill();
}

void bqGUISlider::Draw(bqGS* gs, float dt)
{
	gs->SetScissorRect(m_clipRect);

	gs->DrawGUIRectangle(m_axisRect, m_style->m_sliderAxisEmtpyColor, m_style->m_sliderAxisEmtpyColor, 0, 0);
	gs->DrawGUIRectangle(m_axisRectFill, m_style->m_sliderAxisFillColor, m_style->m_sliderAxisFillColor, 0, 0);
	gs->DrawGUIRectangle(m_controlRect, m_style->m_sliderControlColor, m_style->m_sliderControlColor, 0, 0);

	if (m_drawText)
	{
		bqGUIElement* parent = dynamic_cast<bqGUIElement*>(GetParent());
		if (parent && m_text.size())
		{
			gs->SetScissorRect(parent->m_clipRect);

			bqVec2f textPosition;
			textPosition.x = m_buildRect.z;
			textPosition.y = m_axisRect.y - m_axisHeight;

			gs->DrawGUIText(
				m_text.c_str(),
				m_text.size(),
				textPosition,
				m_textDrawCallback);
		}
	}
}

void bqGUISlider::findAxisRectFill()
{
	m_axisRectFill = m_axisRect;

	m_axisWidth = m_axisRect.z - m_axisRect.x;

	float D = 0.f;

	m_text.clear();

	switch (m_valueType)
	{
	case bqGUISlider::ValueType::Float:
	{
		m_text.append(m_valueFloat);
		for (uint32_t i = 0; i < m_truncateFloatByThisNumOfChars; ++i)
		{
			m_text.pop_back();
		}
		double len = m_valueMaxFloat - m_valueMinFloat;
		if (len != 0.0)
		{
			double M = 1.f / len;
			double V = abs(m_valueMinFloat - m_valueFloat);
			D = (float)(V * M);
		}
	}break;
	case bqGUISlider::ValueType::Uint:
	{
		m_text.append(m_valueUint);
		uint32_t len = m_valueMaxUint - m_valueMinUint;

	}break;
	case bqGUISlider::ValueType::Int:
	{
		m_text.append(m_valueInt);
		int32_t len = m_valueMaxInt - m_valueMinInt;

	}break;
	}

	m_axisRectFill.z = m_axisRectFill.x + (m_axisWidth * D);

	m_controlRect.x = m_buildRect.x + m_axisIndent - (m_controlWidth * 0.5f);
	m_controlRect.y = m_buildRect.y;
	m_controlRect.z = m_controlRect.x + m_controlWidth;
	m_controlRect.w = m_buildRect.w;

	m_controlRect.x += m_axisRectFill.z - m_axisRectFill.x;
	m_controlRect.z += m_axisRectFill.z - m_axisRectFill.x;
}

void bqGUISlider::SetMinMaxFloat(double mn, double mx)
{
	m_valueMinFloat = mn;
	m_valueMaxFloat = mx;
	m_valueFloat = m_valueMinFloat;
	Rebuild();
}

void bqGUISlider::SetMinMaxUint(uint32_t mn, uint32_t mx)
{
	m_valueMinUint = mn;
	m_valueMaxUint = mx;
	m_valueUint = m_valueMinUint;
	Rebuild();
}

void bqGUISlider::SetMinMaxInt(int32_t mn, int32_t mx)
{
	m_valueMinInt = mn;
	m_valueMaxInt = mx;
	m_valueInt = m_valueMinInt;
	Rebuild();
}

