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
#include "badcoiq/scene/bqCamera.h"
#include "badcoiq/input/bqInputEx.h"

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
		m_views.push_back(new ViewportView(this, ViewportView::type_perspective));
	}break;
	case ViewportLayout::type_4views:
	{
		m_views.push_back(new ViewportView(this, ViewportView::type_perspective));
		m_views.push_back(new ViewportView(this, ViewportView::type_top));
		m_views.push_back(new ViewportView(this, ViewportView::type_left));
		m_views.push_back(new ViewportView(this, ViewportView::type_front));
	}break;
	}
	m_activeView = m_views.m_data[0];
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

		m_resizeRect.x = m_views.m_data[0]->m_rectangle.z - 3.f;
		m_resizeRect.y = m_views.m_data[0]->m_rectangle.w - 3.f;
		m_resizeRect.z = m_resizeRect.x + 6.f;
		m_resizeRect.w = m_resizeRect.y + 6.f;

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
	
	g_app->m_gs->BeginGUI(false);
	g_app->m_gs->SetScissorRect(m_resizeRect);
	g_app->m_gs->DrawGUIRectangle(m_resizeRect, bq::ColorBlack, bq::ColorBlack, 0, 0);
	g_app->m_gs->EndGUI();

}


ViewportView::ViewportView(ViewportLayout* l, uint32_t type)
	: m_layout(l)
{

	m_camera = new bqCamera;
	m_camera->SetType(bqCamera::Type::Editor);
	m_camera->EditorReset();
	SetCameraType(type);

	

	m_camera->m_position = bqVec3(0.f, 0.f, 0.f);
	m_camera->m_positionPlatform.w = 20.f;
	//m_camera->Rotate(0, 90, 0.f);
	m_camera->m_aspect = (float)g_app->m_mainWindow->GetCurrentSize()->x / (float)g_app->m_mainWindow->GetCurrentSize()->y;
	m_camera->Update(0.f);
	m_camera->m_viewProjectionMatrix = m_camera->GetMatrixProjection() * m_camera->GetMatrixView();

	bqTextureInfo ti;
	m_rtt = g_app->m_gs->CreateRTT(bqPoint(2,2), ti);
}

ViewportView::~ViewportView()
{
	BQ_SAFEDESTROY(m_rtt);
	BQ_SAFEDESTROY(m_camera);
}

void ViewportView::Rebuild()
{
	m_camera->m_aspect = (float)g_app->m_mainWindow->GetCurrentSize()->x / (float)g_app->m_mainWindow->GetCurrentSize()->y;
	m_camera->Update(0.f);
	BQ_SAFEDESTROY(m_rtt);
	bqTextureInfo ti;
	m_rtt = g_app->m_gs->CreateRTT(bqPoint(m_rectangle.z - m_rectangle.x, m_rectangle.w - m_rectangle.y), ti);
}

void ViewportView::Update()
{
	m_camera->Update(0.1);
	m_camera->UpdateFrustum();

	if (bqMath::PointInRect(bqInput::GetMousePosition(), m_rectangle))
	{
		if (bqInput::IsLMBHit())
		{
			SetActiveView();
		}

		if (bqInput::IsRMBHit())
		{
			SetActiveView();
		}

		if (bqInput::IsMMBHit())
		{
			SetActiveView();
		}

		if (bqInput::IsX1MBHit())
		{
			SetActiveView();
		}

		if (bqInput::IsX2MBHit())
		{
			SetActiveView();
		}
	}
}

void ViewportView::SetActiveView()
{
	m_layout->m_activeView = this;
}

void ViewportView::Draw()
{
	bool isActiveView = (m_layout->m_activeView == this);
	bqFramework::SetMatrix(bqMatrixType::View, &m_camera->m_viewMatrix);
	bqFramework::SetMatrix(bqMatrixType::Projection, &m_camera->m_projectionMatrix);
	bqFramework::SetMatrix(bqMatrixType::ViewProjection, &m_camera->m_viewProjectionMatrix);

	g_app->m_gs->SetRenderTarget(m_rtt);
	g_app->m_gs->SetScissorRect(bqVec4f(0.f, 0.f,
		(float)m_rtt->GetInfo().m_imageInfo.m_width,
		(float)m_rtt->GetInfo().m_imageInfo.m_height));
	g_app->m_gs->SetViewport(0.f, 0.f,
		(float)m_rtt->GetInfo().m_imageInfo.m_width,
		(float)m_rtt->GetInfo().m_imageInfo.m_height);

	//g_app->m_gs->BeginDraw();
	g_app->m_gs->ClearDepth();
	_DrawScene(this);
//	g_app->m_gs->EndDraw();


	g_app->m_gs->SetRenderTargetDefault();
	/*g_app->m_gs->SetViewport(0.f, 0.f,
		(float)m_rectangle.z- m_rectangle.x,
		(float)m_rectangle.w - m_rectangle.y);*/
	g_app->m_gs->BeginGUI(false);
	if (isActiveView)
	{
		auto rectangle_copy = m_rectangle;
		rectangle_copy.x -= 1.f;
		rectangle_copy.y -= 1.f;
		rectangle_copy.z += 1.f;
		rectangle_copy.w += 1.f;

		g_app->m_gs->SetScissorRect(rectangle_copy);
		g_app->m_gs->DrawGUIRectangle(rectangle_copy, bq::ColorRed, bq::ColorRed, 0, 0);
	}
	g_app->m_gs->SetScissorRect(m_rectangle);

	bqColor bgcolor = 0xFFF4F4EE;

	g_app->m_gs->DrawGUIRectangle(m_rectangle, bgcolor, bgcolor, 0, 0);
	g_app->m_gs->DrawGUIRectangle(m_rectangle, bq::ColorWhite, bq::ColorWhite, m_rtt, 0);

	const char32_t* viewportTypeText = U"Perspective";
	switch (m_type)
	{
	case type_back:viewportTypeText = U"Back"; break;
	case type_bottom:viewportTypeText = U"Bottom"; break;
	case type_front:viewportTypeText = U"Front"; break;
	case type_left:viewportTypeText = U"Left"; break;
	case type_right:viewportTypeText = U"Right"; break;
	case type_top:viewportTypeText = U"Top"; break;
	}

	g_app->m_gs->DrawGUIText(viewportTypeText, bqStringLen(viewportTypeText), bqVec2f(m_rectangle.x, m_rectangle.y), &g_app->m_drawTextCallback);

	g_app->m_gs->EndGUI();

	
}

void ViewportView::_DrawScene(ViewportView* view)
{
	_DrawGrid(14.f);
}

void ViewportView::_DrawGrid(int gridSize)
{
	static bqMat4 WVP;
	WVP = m_camera->m_projectionMatrix * m_camera->m_viewMatrix * bqEmptyMatrix;
	bqFramework::SetMatrix(bqMatrixType::WorldViewProjection, &WVP);

	g_app->m_gs->SetShader(bqShaderType::LineModel, 0);
	
	bool isCameraLowerThanWorld = false;
	if (m_camera->m_position.y < 0.f)
		isCameraLowerThanWorld = true;

	switch (m_type)
	{
	case type_perspective:
	{
		/*bqColor gridColor = bq::ColorLightGrey;
		if (positionCameraY < 0.f)
			gridColor = bq::ColorBlack;

		for (int i = 0, z = 7; i <= gridSize; ++i, --z) 
		{
			g_app->m_gs->DrawLine3D(bqVec3(((float)-gridSize) * 0.5f, 0.f, -z), bqVec3(((float)gridSize) * 0.5f, 0.f, -z), gridColor);
			g_app->m_gs->DrawLine3D(bqVec3(-z, 0.f, ((float)-gridSize) * 0.5f), bqVec3(-z, 0.f, ((float)gridSize) * 0.5f), gridColor);
		}

		if (positionCameraY > 0.f)
		{
			g_app->m_gs->DrawLine3D(bqVec3(((float)-gridSize) * 0.5f, 0.f, 0), bqVec3(((float)gridSize) * 0.5f, 0.f, 0), bq::ColorRed);
			g_app->m_gs->DrawLine3D(bqVec3(0, 0.f, ((float)-gridSize) * 0.5f), bqVec3(0, 0.f, ((float)gridSize) * 0.5f), bq::ColorLime);
		}*/
		if (isCameraLowerThanWorld)
			g_app->m_gs->SetMesh(g_app->m_gridModel_perspective2);
		else
			g_app->m_gs->SetMesh(g_app->m_gridModel_perspective1);
	}break;
	case type_left:
	{
		bool front = ((m_camera->m_rotationPlatform.y < PIPI) &&
			(m_camera->m_rotationPlatform.y > PI));
		//if (m_activeCamera->m_positionPlatform.w < 40.f)
		front ? g_app->m_gs->SetMesh(g_app->m_gridModel_left1) : g_app->m_gs->SetMesh(g_app->m_gridModel_left2);
	}break;
	case type_right:
	{
		bool front = ((m_camera->m_rotationPlatform.y > 0.f) &&
			(m_camera->m_rotationPlatform.y < PI));
		front ? g_app->m_gs->SetMesh(g_app->m_gridModel_left1) : g_app->m_gs->SetMesh(g_app->m_gridModel_left2);
	}break;
	case type_bottom:
	case type_top:
		isCameraLowerThanWorld ? g_app->m_gs->SetMesh(g_app->m_gridModel_top2) : g_app->m_gs->SetMesh(g_app->m_gridModel_top1);
		break;
	case type_front: 
		{
		bool front = ((m_camera->m_rotationPlatform.y < PIHalf) &&
			(m_camera->m_rotationPlatform.y > -PIPlusHalf));
		front ? g_app->m_gs->SetMesh(g_app->m_gridModel_front1) : g_app->m_gs->SetMesh(g_app->m_gridModel_front2);
	}break;
	case type_back: 
		{
		bool front = ((m_camera->m_rotationPlatform.y > PIHalf) &&
			(m_camera->m_rotationPlatform.y < PI + PIHalf));
		front ? g_app->m_gs->SetMesh(g_app->m_gridModel_front1) : g_app->m_gs->SetMesh(g_app->m_gridModel_front2);
	}break;
	}

	static bqMaterial material;
	material.m_shaderType = bqShaderType::LineModel;
	g_app->m_gs->SetMaterial(&material);
	g_app->m_gs->EnableDepth();
	g_app->m_gs->Draw();
}

void ViewportView::SetCameraType(uint32_t ct)
{
	m_type = ct;

	m_camera->m_editorCameraType = bqCamera::CameraEditorType::Perspective;
	switch (m_type)
	{
	case type_back:
		m_camera->m_editorCameraType = bqCamera::CameraEditorType::Back;
		break;
	case type_bottom:m_camera->m_editorCameraType = bqCamera::CameraEditorType::Bottom;
		break;
	case type_front:m_camera->m_editorCameraType = bqCamera::CameraEditorType::Front;
		break;
	case type_left:m_camera->m_editorCameraType = bqCamera::CameraEditorType::Left;
		break;
	case type_right:m_camera->m_editorCameraType = bqCamera::CameraEditorType::Right;
		break;
	case type_top:m_camera->m_editorCameraType = bqCamera::CameraEditorType::Top;
		break;
	}
}
