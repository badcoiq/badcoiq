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

#include "ModelEditor.h"
#include "Viewport.h"

extern ModelEditor* g_app;

Viewport::Viewport()
{
	m_layouts.push_back(new ViewportLayout(ViewportLayout::type_full));
	m_layouts.push_back(new ViewportLayout(ViewportLayout::type_4views));
}

Viewport::~Viewport()
{
	for (size_t i = 0; i < m_layouts.m_size; ++i)
	{
		delete m_layouts.m_data[i];
	}
}

void Viewport::Rebuild()
{
	for (size_t i = 0; i < m_layouts.m_size; ++i)
	{
		m_layouts.m_data[i]->Rebuild();
	}
}

void Viewport::Update()
{
	for (size_t i = 0; i < m_layouts.m_size; ++i)
	{
		m_layouts.m_data[i]->Update();
	}
}


void Viewport::Draw()
{
	for (size_t i = 0; i < m_layouts.m_size; ++i)
	{
		m_layouts.m_data[i]->Draw();
	}
}

ViewportLayout::ViewportLayout(uint32_t type)
{
	switch (type)
	{
	default:
	case ViewportLayout::type_full:
	{
		m_views.push_back(new ViewportView);
	}break;
	case ViewportLayout::type_4views:
	{
		m_views.push_back(new ViewportView);
		m_views.push_back(new ViewportView);
		m_views.push_back(new ViewportView);
		m_views.push_back(new ViewportView);
	}break;
	}
}

ViewportLayout::~ViewportLayout()
{
}

void ViewportLayout::Rebuild()
{
	for (size_t i = 0; i < m_views.m_size; ++i)
	{
		m_views.m_data[i]->Rebuild();
	}
}

void ViewportLayout::Update()
{
	for (size_t i = 0; i < m_views.m_size; ++i)
	{
		m_views.m_data[i]->Update();
	}
}

void ViewportLayout::Draw()
{
	for (size_t i = 0; i < m_views.m_size; ++i)
	{
		m_views.m_data[i]->Draw();
	}
}


ViewportView::ViewportView()
{
}

ViewportView::~ViewportView()
{
}

void ViewportView::Rebuild()
{
}

void ViewportView::Update()
{
}

void ViewportView::Draw()
{
}
