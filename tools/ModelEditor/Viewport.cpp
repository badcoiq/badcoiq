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

#include "badcoiq/gs/bqGS.h"

extern ModelEditor* g_app;

Viewport::Viewport()
{
	m_layouts.push_back(new ViewportLayout(this, ViewportLayout::type_full));
	m_layouts.push_back(new ViewportLayout(this, ViewportLayout::type_4views));

	m_activeLayout = m_layouts.m_data[1];
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
	m_rectangle.x = g_app->m_editPanelRect.z;
	m_rectangle.y = g_app->m_mainMenuBarRect.w;
	m_rectangle.z = g_app->m_mainMenuBarRect.z;
	m_rectangle.w = g_app->m_mainWindow->GetCurrentSize()->y;

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
	g_app->m_gs->BeginGUI(true);
	g_app->m_gs->SetScissorRect(m_rectangle);
	g_app->m_gs->DrawGUIRectangle(m_rectangle, bq::ColorAqua, bq::ColorAqua, 0, 0);
	g_app->m_gs->EndGUI();

	m_activeLayout->Draw();
	/*for (size_t i = 0; i < m_layouts.m_size; ++i)
	{
		m_layouts.m_data[i]->Draw();
	}*/
}

ViewportLayout::ViewportLayout(Viewport* viewport, uint32_t type)
{
	m_viewport = viewport;
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
	m_type = type;
}

ViewportLayout::~ViewportLayout()
{
}

void ViewportLayout::Rebuild()
{
	float border = 1.f;

	switch (m_type)
	{
	case type_full:
	{
		m_views.m_data[0]->m_rectangle = m_viewport->m_rectangle;
		m_views.m_data[0]->m_rectangle.x += border;
		m_views.m_data[0]->m_rectangle.y += border;
		m_views.m_data[0]->m_rectangle.z -= border;
		m_views.m_data[0]->m_rectangle.w -= border;
	}break;
	case type_4views:
	{
		m_views.m_data[0]->m_rectangle = m_viewport->m_rectangle;
		m_views.m_data[0]->m_rectangle.x += border;
		m_views.m_data[0]->m_rectangle.y += border;
		m_views.m_data[0]->m_rectangle.z -= border;
		m_views.m_data[0]->m_rectangle.w -= border;
		
		m_views.m_data[1]->m_rectangle = m_views.m_data[0]->m_rectangle;
		m_views.m_data[2]->m_rectangle = m_views.m_data[0]->m_rectangle;
		m_views.m_data[3]->m_rectangle = m_views.m_data[0]->m_rectangle;

		bqVec2f halfSize;
		halfSize.x = m_views.m_data[0]->m_rectangle.z - m_views.m_data[0]->m_rectangle.x;
		halfSize.y = m_views.m_data[0]->m_rectangle.w - m_views.m_data[0]->m_rectangle.y;
		halfSize.x *= 0.5f;
		halfSize.y *= 0.5f;

		m_views.m_data[0]->m_rectangle.z =
			m_views.m_data[0]->m_rectangle.x + halfSize.x - border;
		m_views.m_data[0]->m_rectangle.w =
			m_views.m_data[0]->m_rectangle.y + halfSize.y - border;

		m_views.m_data[1]->m_rectangle.x = m_views.m_data[0]->m_rectangle.z + border;
		m_views.m_data[1]->m_rectangle.w =
			m_views.m_data[1]->m_rectangle.y + halfSize.y - border;

		m_views.m_data[2]->m_rectangle.z =
			m_views.m_data[2]->m_rectangle.x + halfSize.x - border;
		m_views.m_data[2]->m_rectangle.y =
			m_views.m_data[0]->m_rectangle.w + border;
		
		m_views.m_data[3]->m_rectangle.x = m_views.m_data[2]->m_rectangle.z + border;
		m_views.m_data[3]->m_rectangle.y =
			m_views.m_data[1]->m_rectangle.w + border;

	}break;
	}

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
	g_app->m_gs->BeginGUI(false);
	g_app->m_gs->SetScissorRect(m_rectangle);
	g_app->m_gs->DrawGUIRectangle(m_rectangle, bq::ColorLightSalmon, bq::ColorLightSalmon, 0, 0);
	g_app->m_gs->EndGUI();
}
