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
#include "badcoiq/system/bqWindow.h"

bqGUIRootElement::bqGUIRootElement(bqGUIWindow* w, const bqVec2f& position, const bqVec2f& size) :
	bqGUIElement::bqGUIElement(w, position, size)
{
	m_flags |= bqGUICommon::flag_wheelScroll;

	// нужно вызвать так как свой Rebuild()
	Rebuild();
	// нужно вызвать так как свой Rebuild()
	m_buildRectOnCreation = m_baseRect;
}

bqGUIRootElement::~bqGUIRootElement() {}
void bqGUIRootElement::Rebuild()
{
	//bqGUIElement::Rebuild();
	m_baseRect.x = m_window->GetPosition().x;
	m_baseRect.y = m_window->GetPosition().y;

	m_baseRect.z = m_baseRect.x + m_window->GetSize().x;
	m_baseRect.w = m_baseRect.y + m_window->GetSize().y;
	//m_baseRect = m_window->m_buildRect;
	
	if (m_window->m_windowFlags & bqGUIWindow::windowFlag_withTitleBar)
	{
		m_baseRect.y += m_window->m_titlebarHeight;
	}


	m_buildRect = m_baseRect;
	m_activeRect = m_baseRect;
	m_clipRect = m_baseRect;
}
void bqGUIRootElement::Update()
{
	bqGUIElement::Update();
}
void bqGUIRootElement::Draw(bqGS* gs, float dt) {}

void bqGUIRootElement::UpdateContentSize()
{
	bqVec2f LB;

	if (GetChildren()->m_head)
	{
		auto children = GetChildren();
		if (children->m_head)
		{
			auto curr = children->m_head;
			auto last = curr->m_left;
			while (1)
			{
				bqGUIElement* child = dynamic_cast<bqGUIElement*>(curr->m_data);

				if (child->m_baseRect.z > LB.x) LB.x = child->m_baseRect.z;
				if (child->m_baseRect.w > LB.y) LB.y = child->m_baseRect.w;

				if (curr == last)
					break;
				curr = curr->m_right;
			}
		}
	}

	m_contentSize.x = LB.x - m_baseRect.x;
	m_contentSize.y = LB.y - m_baseRect.y;

	bqGUICommon::UpdateScrollLimit();
}
