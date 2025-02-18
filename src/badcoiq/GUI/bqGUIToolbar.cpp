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
#include "badcoiq/gs/bqGS.h"
#include "badcoiq/input/bqInputEx.h"

#include "../framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;

class bqGUIToolbarButton : public bqGUIButton
{
	bqGUIToolbar* m_toolbar = 0;
public:
	bqGUIToolbarButton(const bqVec2f& position, const bqVec2f& size, bqGUIToolbar* tb):
	bqGUIButton::bqGUIButton(position, size)
	{
		m_toolbar = tb;
	}
	virtual ~bqGUIToolbarButton() {}

	virtual void OnReleaseLMB() override 
	{
		if (m_flags & flag_cursorInRect)
		{
			if (m_toolbar)
				m_toolbar->OnButton(m_id, this);

			m_flags &= ~flag_cursorInRect;
			//printf("m_flags\n");
		}
	}
};

bqGUIToolbar::bqGUIToolbar(const bqVec2f& position, const bqVec2f& size, float32_t btnSz)
	:
	bqGUIElement::bqGUIElement(position, size)
{
	m_buttonSize = btnSz;
}

bqGUIToolbar::~bqGUIToolbar()
{
	RemoveAllElements();
}

void bqGUIToolbar::Rebuild()
{
	bqGUIElement::Rebuild();

	for (size_t i = 0; i < m_nodes.m_size; ++i)
	{
		auto n = m_nodes.m_data[i];
		n.m_element->Rebuild();
	}
}

void bqGUIToolbar::Update()
{
	bqGUIElement::Update();

	for (size_t i = 0; i < m_nodes.m_size; ++i)
	{
		auto n = m_nodes.m_data[i];
		n.m_element->Update();
	}
}

void bqGUIToolbar::Draw(bqGS* gs, float dt)
{
	gs->SetScissorRect(m_clipRect);
	if (m_flags & flag_drawBG)
	{
		gs->DrawGUIRectangle(m_buildRect,
			m_style->m_pictureBoxBGColor,
			m_style->m_pictureBoxBGColor,
			0,
			0);
	}

	for (size_t i = 0; i < m_nodes.m_size; ++i)
	{
		auto n = m_nodes.m_data[i];
		n.m_element->Draw(gs, dt);
	}
}

void bqGUIToolbar::AddButton(uint32_t id, uint32_t iconID)
{
	bqVec2f pos;
	bqVec2f sz;

	pos.x = m_indent.x + m_addPosition;
	pos.y = m_indent.y;
	
	sz.x = m_buttonSize;
	sz.y = m_buttonSize;

	m_addPosition += m_buttonSize + m_elementsDistance;

	bqGUIToolbarButton* newButton = new bqGUIToolbarButton(pos, sz, this);
	newButton->SetID(id);
	node n;
	n.m_element = newButton;
	n.m_ownElement = true;
	m_nodes.push_back(n);
}

void bqGUIToolbar::AddSeparator()
{
}

void bqGUIToolbar::AddGUIElement(bqGUIElement* e)
{
	BQ_ASSERT(e);
	node n;
	n.m_element = e;
	n.m_ownElement = false;
	m_nodes.push_back(n);
}

void bqGUIToolbar::RemoveAllElements()
{
	if(m_nodes.size())
	{
		for(size_t i = 0; i < m_nodes.m_size; ++i)
		{
			auto n = m_nodes.m_data[i];
			if(n.m_ownElement)
			{
				BQ_SAFEDESTROY(n.m_element);
			}
		}
		m_nodes.clear();
	}
}


#endif
