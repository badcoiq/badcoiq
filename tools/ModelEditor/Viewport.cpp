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
#include "CubeView.h"

#include "badcoiq/gs/bqGS.h"
#include "badcoiq/scene/bqCamera.h"
#include "badcoiq/input/bqInputEx.h"
#include "badcoiq/system/bqPopup.h"

extern ModelEditor* g_app;

Viewport::Viewport()
{
	m_layouts.push_back(new ViewportLayout(this, ViewportLayout::type_full));
	m_layouts.push_back(new ViewportLayout(this, ViewportLayout::type_4views));


	m_fullViewLayout = m_layouts.m_data[0];
	m_beforeToggleFullViewLayout = m_layouts.m_data[1];
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
	/*for (size_t i = 0; i < m_layouts.m_size; ++i)
	{
		m_layouts.m_data[i]->Update();
	}*/
	m_activeLayout->Update();
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

void Viewport::SetActiveViewportViewType(uint32_t t)
{
	m_activeLayout->GetActiveView()->SetCameraType(t);
}

void Viewport::ToggleFullView()
{
	/*if (m_activeLayout == m_fullViewLayout)
	{
		auto old = m_activeLayout;
		m_activeLayout = m_beforeToggleFullViewLayout;
		m_activeLayout->GetActiveView()->CopyDataFrom(old->GetActiveView());
		m_activeLayout->Rebuild();
	}
	else
	{
		m_beforeToggleFullViewLayout = m_activeLayout;
		m_activeLayout = m_fullViewLayout;
		m_activeLayout->GetActiveView()->CopyDataFrom(m_beforeToggleFullViewLayout->GetActiveView());
		m_activeLayout->Rebuild();
	}*/
	m_activeLayout->ToggleFullView();
}

void Viewport::ToggleGrid()
{
	m_activeLayout->GetActiveView()->ToggleGrid();
}

void Viewport::CameraReset()
{
	m_activeLayout->GetActiveView()->ResetCamera();
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
		m_activeView = m_views.m_data[0];
	}break;
	case ViewportLayout::type_4views:
	{
		m_views.push_back(new ViewportView(this, ViewportView::type_top));
		m_views.push_back(new ViewportView(this, ViewportView::type_front));
		m_views.push_back(new ViewportView(this, ViewportView::type_left));
		m_views.push_back(new ViewportView(this, ViewportView::type_perspective));
		m_activeView = m_views.m_data[3];
	}break;
	}
	m_type = type;
}

ViewportLayout::~ViewportLayout()
{
}

void ViewportLayout::ToggleFullView()
{
	m_activeView->m_fullview = m_activeView->m_fullview ? false : true;
	Rebuild();
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

	if (m_activeView->m_fullview)
	{
		m_activeView->m_rectangle = m_viewport->m_rectangle;
		m_activeView->m_rectangle.x += border;
		m_activeView->m_rectangle.y += border;
		m_activeView->m_rectangle.z -= border;
		m_activeView->m_rectangle.w -= border;
	}

	for (size_t i = 0; i < m_views.m_size; ++i)
	{
		m_views.m_data[i]->Rebuild();
	}
}

void ViewportLayout::Update()
{
	if (m_activeView->m_fullview)
	{
		m_activeView->Update();
	}
	else
	{
		for (size_t i = 0; i < m_views.m_size; ++i)
		{
			m_views.m_data[i]->Update();
		}
	}
}

void ViewportLayout::Draw()
{
	if (m_activeView->m_fullview)
	{
		m_activeView->Draw();
	}
	else
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

}


ViewportView::ViewportView(ViewportLayout* l, uint32_t type)
	: m_layout(l)
{
	m_cubeViewCamera = new bqCamera;
	m_cubeViewCamera->SetType(bqCamera::Type::Editor);
	//m_cubeViewCamera->m_forceOrtho = true;

	for (int i = 0; i < type__size; ++i)
	{
		m_cameras[i] = new bqCamera;
		m_cameras[i]->SetType(bqCamera::Type::Editor);
		m_cameras[i]->m_editorCameraType = bqCamera::CameraEditorType::Perspective;
	
		switch (i)
		{
		default:
		case type_perspective:m_cameras[i]->m_editorCameraType = bqCamera::CameraEditorType::Perspective; break;
		case type_back:m_cameras[i]->m_editorCameraType= bqCamera::CameraEditorType::Back; break;
		case type_bottom:m_cameras[i]->m_editorCameraType = bqCamera::CameraEditorType::Bottom; break;
		case type_front:m_cameras[i]->m_editorCameraType = bqCamera::CameraEditorType::Front; break;
		case type_left:m_cameras[i]->m_editorCameraType = bqCamera::CameraEditorType::Left; break;
		case type_right:m_cameras[i]->m_editorCameraType = bqCamera::CameraEditorType::Right; break;
		case type_top:m_cameras[i]->m_editorCameraType = bqCamera::CameraEditorType::Top; break;
		}
	}
	m_activeCamera = m_cameras[type];

	m_cubeViewCamera->m_editorCameraType = m_activeCamera->m_editorCameraType;

	ResetCamera();
	SetCameraType(type);

	//m_camera->m_position = bqVec3(0.f, 0.f, 0.f);
	//m_camera->m_positionPlatform.w = 20.f;

	//m_camera->Rotate(0, 90, 0.f);
	m_activeCamera->Update(0.f);
	m_cubeViewCamera->Update(0.f);
	m_activeCamera->m_viewProjectionMatrix = m_activeCamera->GetMatrixProjection() * m_activeCamera->GetMatrixView();

	bqTextureInfo ti;
	m_rtt = g_app->m_gs->CreateRTT(bqPoint(2,2), ti);

	m_rttCubeView = g_app->m_gs->CreateRTT(bqPoint(m_cubeViewSize, m_cubeViewSize), ti);
}

ViewportView::~ViewportView()
{
	BQ_SAFEDESTROY(m_rtt);
	BQ_SAFEDESTROY(m_rttCubeView);
	for (int i = 0; i < type__size; ++i)
	{
		BQ_SAFEDESTROY(m_cameras[i]);
	}
}

void ViewportView::Rebuild()
{
	for (int i = 0; i < type__size; ++i)
	{
		m_cameras[i]->m_aspect = (float)g_app->m_mainWindow->GetCurrentSize()->x / (float)g_app->m_mainWindow->GetCurrentSize()->y;
		m_cameras[i]->Update(0.f);
	}
	BQ_SAFEDESTROY(m_rtt);
	bqTextureInfo ti;
	m_rtt = g_app->m_gs->CreateRTT(bqPoint(m_rectangle.z - m_rectangle.x, m_rectangle.w - m_rectangle.y), ti);

	m_viewportTypeText = U"Perspective";
	switch (m_type)
	{
	case type_back:m_viewportTypeText = U"Back"; break;
	case type_bottom:m_viewportTypeText = U"Bottom"; break;
	case type_front:m_viewportTypeText = U"Front"; break;
	case type_left:m_viewportTypeText = U"Left"; break;
	case type_right:m_viewportTypeText = U"Right"; break;
	case type_top:m_viewportTypeText = U"Top"; break;
	}

	auto textSize = g_app->m_drawTextCallback.GetTextSize(m_viewportTypeText);

	m_viewportOptionsRectangle.x = m_rectangle.x;
	m_viewportOptionsRectangle.y = m_rectangle.y;
	m_viewportOptionsRectangle.z = m_rectangle.x + textSize.x;
	m_viewportOptionsRectangle.w = m_rectangle.y + textSize.y;

	switch (m_type)
	{
	case type_back:
	case type_bottom:
	case type_front:
	case type_left:
	case type_right:
	case type_top:
	case type_perspective:
		m_cubeViewCamera->m_fov = 0.3f;
		m_cubeViewCamera->m_positionPlatform.w = 14.75f;
		break;
	}

	m_cubeViewRectangle.z = m_rectangle.z;
	m_cubeViewRectangle.w = m_rectangle.w;
	m_cubeViewRectangle.x = m_cubeViewRectangle.z - m_cubeViewSize;
	m_cubeViewRectangle.y = m_cubeViewRectangle.w - m_cubeViewSize;
}

void ViewportView::Update()
{
	m_activeCamera->Update(0.1);
	m_activeCamera->UpdateFrustum();
	m_cubeViewCamera->Update(0.1);

	if (!g_app->m_cursorInGUIWindow)
	{
		if (g_app->m_cubeView->m_isMouseMove && (m_layout->m_mouseFocusView == this))
		{
			g_app->m_cubeView->Update(this);
		}

		bqPointf& mousePosition = bqInput::GetMousePosition();

		if (bqMath::PointInRect(mousePosition, m_viewportOptionsRectangle))
		{
			if (bqInput::IsRMBHit())
			{
				g_app->m_popupViewportOptions->Show(g_app->m_mainWindow, m_viewportOptionsRectangle.x, m_viewportOptionsRectangle.y);
			}
		}

		if (bqMath::PointInRect(mousePosition, m_rectangle))
		{
			if (bqInput::IsLMBHit())
			{
				m_layout->m_mouseFocusView = this;
				SetActiveView();
			}

			if (bqInput::IsRMBHit())
			{
				m_layout->m_mouseFocusView = this;
				SetActiveView();
			}

			if (bqInput::IsMMBHit())
			{
				m_layout->m_mouseFocusView = this;
				SetActiveView();
			}

			if (bqInput::IsX1MBHit())
			{
				m_layout->m_mouseFocusView = this;
				SetActiveView();
			}

			if (bqInput::IsX2MBHit())
			{
				m_layout->m_mouseFocusView = this;
				SetActiveView();
			}

			if (bqInput::IsLMBRelease())
			{
				m_layout->m_mouseFocusView = 0;
			}

			if (bqInput::IsRMBRelease())
			{
				m_layout->m_mouseFocusView = 0;
			}

			if (bqInput::IsMMBRelease())
			{
				m_layout->m_mouseFocusView = 0;
			}

			if (bqInput::IsX1MBRelease())
			{
				m_layout->m_mouseFocusView = 0;
			}

			if (bqInput::IsX2MBRelease())
			{
				m_layout->m_mouseFocusView = 0;
			}

			if (g_app->m_inputData->m_mouseWheelDelta)
				m_activeCamera->EditorZoom(g_app->m_inputData->m_mouseWheelDelta);

			if (bqMath::PointInRect(mousePosition, m_cubeViewRectangle)
				&& !g_app->m_cubeView->m_isMouseMove)
			{
				g_app->m_cubeView->Update(this);

				auto meshID = g_app->m_cubeView->IsMouseRayIntersect(m_cubeViewCamera, m_cubeViewRectangle);
				if (meshID != CubeView::meshID__size)
				{
					/*switch (meshID)
					{
					case CubeView::meshID_front:
						printf("meshID_front\n");
						break;
					case CubeView::meshID_back:
						printf("meshID_back\n");
						break;
					case CubeView::meshID_top:
						printf("meshID_top\n");
						break;
					case CubeView::meshID_bottom:
						printf("meshID_bottom\n");
						break;
					case CubeView::meshID_left:
						printf("meshID_left\n");
						break;
					case CubeView::meshID_right:
						printf("meshID_right\n");
						break;

					case CubeView::meshID_TB:
						printf("meshID_TB\n");
						break;
					case CubeView::meshID_TL:
						printf("meshID_TL\n");
						break;
					case CubeView::meshID_TF:
						printf("meshID_TF\n");
						break;
					case CubeView::meshID_TR:
						printf("meshID_TR\n");
						break;
					case CubeView::meshID_BckR:
						printf("meshID_BckR\n");
						break;
					case CubeView::meshID_BckL:
						printf("meshID_BckL\n");
						break;
					case CubeView::meshID_FL:
						printf("meshID_FL\n");
						break;
					case CubeView::meshID_FR:
						printf("meshID_FR\n");
						break;
					case CubeView::meshID_BR:
						printf("meshID_BR\n");
						break;
					case CubeView::meshID_BB:
						printf("meshID_BB\n");
						break;
					case CubeView::meshID_BL:
						printf("meshID_BL\n");
						break;
					case CubeView::meshID_BF:
						printf("meshID_BF\n");
						break;

					case CubeView::meshID_TRB:
						printf("meshID_TRB\n");
						break;
					case CubeView::meshID_TLB:
						printf("meshID_TLB\n");
						break;
					case CubeView::meshID_TLF:
						printf("meshID_TLF\n");
						break;
					case CubeView::meshID_TRF:
						printf("meshID_TRF\n");
						break;
					case CubeView::meshID_BRF:
						printf("meshID_BRF\n");
						break;
					case CubeView::meshID_BRB:
						printf("meshID_BRB\n");
						break;
					case CubeView::meshID_BLB:
						printf("meshID_BLB\n");
						break;
					case CubeView::meshID_BLF:
						printf("meshID_BLF\n");
						break;
					}*/
				}
			}
		}


		if (m_layout->m_mouseFocusView == this)
		{
			if (bqInput::IsRMBHold() && !m_cubeViewNowRotating)
			{
				if (bqInput::IsCtrl())
				{
					m_activeCamera->EditorRotate(&g_app->m_inputData->m_mouseMoveDelta, *g_app->m_deltaTime);
					m_cubeViewCamera->EditorRotate(&g_app->m_inputData->m_mouseMoveDelta, *g_app->m_deltaTime);
				}
				else
				{
					m_activeCamera->EditorPanMove(&g_app->m_inputData->m_mouseMoveDelta, *g_app->m_deltaTime);
				}
			}
		}
	}

	if (m_cubeViewNowRotating)
	{
		bqVec3f lerpResult;
		bqMath::Lerp1(m_activeCamera->m_rotationPlatform, m_cubeViewRotationTarget, m_cubeViewRotatingLerpTime, lerpResult);

		//printf("Y:%f T:%f\n", m_activeCamera->m_rotationPlatform.y, m_cubeViewRotationTarget.y);

		m_cubeViewRotatingLerpTime += *g_app->m_deltaTime;
		if (m_cubeViewRotatingLerpTime > m_cubeViewRotatingLerpTimeLimit)
		{
			m_cubeViewRotatingLerpTime = 0.f;
			m_cubeViewNowRotating = false;

			if (lerpResult.y > PIPIf)
			{
				lerpResult.y -= PIPIf;
				if (lerpResult.y > PIPIf)
					lerpResult.y = 0.f;
			}
			if (lerpResult.y < 0)
				lerpResult.y = lerpResult.y + PIPIf;

			//printf("\n");
		}

		m_activeCamera->m_rotationPlatform = lerpResult;
		m_cubeViewCamera->m_rotationPlatform = lerpResult;
	}
	else
	{
	}
}

void ViewportView::SetActiveView()
{
	m_layout->m_activeView = this;
}

void ViewportView::Draw()
{
	bool isActiveView = (m_layout->m_activeView == this);
	bqFramework::SetMatrix(bqMatrixType::View, &m_activeCamera->m_viewMatrix);
	bqFramework::SetMatrix(bqMatrixType::Projection, &m_activeCamera->m_projectionMatrix);
	bqFramework::SetMatrix(bqMatrixType::ViewProjection, &m_activeCamera->m_viewProjectionMatrix);

	g_app->m_gs->SetRenderTarget(m_rtt);
	g_app->m_gs->SetScissorRect(bqVec4f(0.f, 0.f,
		(float)m_rtt->GetInfo().m_imageInfo.m_width,
		(float)m_rtt->GetInfo().m_imageInfo.m_height));
	g_app->m_gs->SetViewport(0.f, 0.f,
		(float)m_rtt->GetInfo().m_imageInfo.m_width,
		(float)m_rtt->GetInfo().m_imageInfo.m_height);

	g_app->m_gs->ClearAll();
	_DrawScene(this);

	// CUBE VIEW
	g_app->m_gs->SetRenderTarget(m_rttCubeView);
	g_app->m_gs->SetScissorRect(bqVec4f(0.f, 0.f, m_cubeViewSize, m_cubeViewSize));
	g_app->m_gs->SetViewport(0.f, 0.f, m_cubeViewSize, m_cubeViewSize);
	g_app->m_gs->SetClearColor(0.f,0.f,0.f,0.f);
	g_app->m_gs->ClearAll();

	g_app->m_cubeView->Draw(m_cubeViewCamera);
	g_app->m_gs->DisableDepth();
	g_app->m_gs->SetShader(bqShaderType::Line3D, 0);
	bqFramework::SetMatrix(bqMatrixType::ViewProjection, &m_cubeViewCamera->m_viewProjectionMatrix);
	g_app->m_gs->DrawLine3D(bqVec3(0, 0.f, 0), bqVec3(1, 0.f, 0), bq::ColorRed);
	g_app->m_gs->DrawLine3D(bqVec3(0, 0.f, 0), bqVec3(0, 1, 0), bq::ColorBlue);
	g_app->m_gs->DrawLine3D(bqVec3(0, 0.f, 0), bqVec3(0, 0, 1), bq::ColorLime);


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

	bqColor bgcolor = 0xFFFFF4EE;

	g_app->m_gs->DisableDepth();
	g_app->m_gs->EnableBlend();
	g_app->m_gs->DrawGUIRectangle(m_rectangle, bgcolor, bgcolor, 0, 0);
	g_app->m_gs->DrawGUIRectangle(m_rectangle, bq::ColorWhite, bq::ColorWhite, m_rtt, 0);
	g_app->m_gs->DrawGUIRectangle(m_cubeViewRectangle, bq::ColorWhite, bq::ColorWhite, m_rttCubeView, 0);

	
	
	bqPointf& mousePosition = bqInput::GetMousePosition();
	bqColor oldColor = g_app->m_drawTextCallback.GetColor();
	if (bqMath::PointInRect(mousePosition, m_viewportOptionsRectangle))
	{
		g_app->m_drawTextCallback.SetColor(bq::ColorYellow);
	}
	else
	{
		g_app->m_drawTextCallback.SetColor(bq::ColorBlack);
	}
	g_app->m_gs->DrawGUIText(m_viewportTypeText, bqStringLen(m_viewportTypeText), bqVec2f(m_rectangle.x, m_rectangle.y), &g_app->m_drawTextCallback);
	g_app->m_drawTextCallback.SetColor(oldColor);

	g_app->m_gs->EndGUI();

	
}

void ViewportView::_DrawScene(ViewportView* view)
{
	if(m_drawGrid)
		_DrawGrid(14.f);

	
}

void ViewportView::_DrawGrid(int gridSize)
{
	static bqMat4 WVP;
	static bqMat4 W;
	WVP = m_activeCamera->m_projectionMatrix * m_activeCamera->m_viewMatrix * W;
	bqFramework::SetMatrix(bqMatrixType::WorldViewProjection, &WVP);
	bqFramework::SetMatrix(bqMatrixType::World, &W);

	g_app->m_gs->SetShader(bqShaderType::LineModel, 0);
	
	bool isCameraLowerThanWorld = false;
	if (m_activeCamera->m_position.y < 0.f)
		isCameraLowerThanWorld = true;

	switch (m_type)
	{
	default:
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
	case type_right:
		g_app->m_gs->SetMesh(g_app->m_gridModel_left);
	break;
	case type_bottom:
	case type_top:
		g_app->m_gs->SetMesh(g_app->m_gridModel_top);
		break;
	case type_back:
	case type_front:		
		g_app->m_gs->SetMesh(g_app->m_gridModel_front);
	break;
	}

	static bqMaterial material;
	material.m_shaderType = bqShaderType::LineModel;
	g_app->m_gs->SetMaterial(&material);
	g_app->m_gs->EnableDepth();
	g_app->m_gs->Draw();
}

void ViewportView::ResetCamera()
{
	m_activeCamera->EditorReset();
	m_activeCamera->m_aspect = (m_rectangle.z - m_rectangle.x) / (m_rectangle.w - m_rectangle.y);

	// Надо временно установить m_editorCameraType
	// для EditorReset()
	switch (m_type)
	{
	case type_back:
		m_cubeViewCamera->m_editorCameraType = bqCamera::CameraEditorType::Back;
		break;
	case type_bottom:
		m_cubeViewCamera->m_editorCameraType = bqCamera::CameraEditorType::Bottom;
		break;
	case type_front:
		m_cubeViewCamera->m_editorCameraType = bqCamera::CameraEditorType::Front;
		break;
	case type_left:
		m_cubeViewCamera->m_editorCameraType = bqCamera::CameraEditorType::Left;
		break;
	case type_right:
		m_cubeViewCamera->m_editorCameraType = bqCamera::CameraEditorType::Right;
		break;
	case type_top:
		m_cubeViewCamera->m_editorCameraType = bqCamera::CameraEditorType::Top;
		break;
	case type_perspective:
		m_cubeViewCamera->m_editorCameraType = bqCamera::CameraEditorType::Perspective;
		break;
	}	
	m_cubeViewCamera->EditorReset();
	m_cubeViewCamera->m_aspect = m_activeCamera->m_aspect;
	m_cubeViewCamera->m_editorCameraType = bqCamera::CameraEditorType::Perspective;


	m_activeCamera->Update(0.f);
	m_cubeViewCamera->Update(0.f);

	Rebuild();
}

void ViewportView::ToggleGrid()
{
	m_drawGrid = m_drawGrid ? false : true;
}

void ViewportView::SetCameraType(uint32_t ct)
{
	m_type = ct;

	switch (m_type)
	{
	case type_back:
		m_activeCamera = m_cameras[type_back];
		break;
	case type_bottom:
		m_activeCamera = m_cameras[type_bottom];
		break;
	case type_front:
		m_activeCamera = m_cameras[type_front];
		break;
	case type_left:
		m_activeCamera = m_cameras[type_left];
		break;
	case type_right:
		m_activeCamera = m_cameras[type_right];
		break;
	case type_top:
		m_activeCamera = m_cameras[type_top];
		break;
	case type_perspective:
		m_activeCamera = m_cameras[type_perspective];
		break;
	}


	m_cubeViewCamera->m_editorCameraType = bqCamera::CameraEditorType::Perspective;
	m_cubeViewCamera->m_rotationPlatform = m_activeCamera->m_rotationPlatform;
	ResetCamera();
}



void ViewportView::CubeViewOnClick(uint32_t meshID)
{
	//if (!m_cubeViewNowRotating)
	{
		m_cubeViewNowRotating = true;
		m_cubeViewRotatingLerpTime = 0.f;

		switch (meshID)
		{
		case CubeView::meshID_front:
			if (m_activeCamera->m_rotationPlatform.y > PIf)
				m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-90.f), PIPIf, 0.f);
			else
				m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-90.f), bqMath::DegToRad(0.f), 0.f);
			break;
		case CubeView::meshID_back:
			m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-90.f), bqMath::DegToRad(180.f), 0.f);
			break;
		case CubeView::meshID_top:
			m_cubeViewRotationTarget = bqVec3f(0, PIf, 0);
			break;
		case CubeView::meshID_bottom:
			if (m_activeCamera->m_rotationPlatform.y > PIf)
				m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-180.f), PIPIf, 0.f);
			else
				m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-180.f), 0.f, 0.f);
			break;
		case CubeView::meshID_left:
			m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-90.f), bqMath::DegToRad(270.f), 0.f);
			break;
		case CubeView::meshID_right:
			m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-90.f), bqMath::DegToRad(90.f), 0.f);
			break;

		case CubeView::meshID_TB:
			m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-45.f), PIf, 0.f);
			break;
		case CubeView::meshID_TL:
			if (m_activeCamera->m_rotationPlatform.y > PIHalf)
				m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-45.f), bqMath::DegToRad(270.f), 0.f);
			else
				m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-45.f), bqMath::DegToRad(-90.f), 0.f);
			break;
		case CubeView::meshID_TF:
			if (m_activeCamera->m_rotationPlatform.y > PIf)
				m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-45.f), PIPIf, 0);
			else
				m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-45.f), 0, 0);
			break;
		case CubeView::meshID_TR:
			if (m_activeCamera->m_rotationPlatform.y > PIf + PIfHalf)
				m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-45.f), PIPIf + PIfHalf, 0.f);
			else
				m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-45.f), bqMath::DegToRad(90.f), 0.f);
			break;
		case CubeView::meshID_BckR:
			m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-90.f), bqMath::DegToRad(-45.f) + PIf, 0.f);
			break;
		case CubeView::meshID_BckL:
			m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-90.f), bqMath::DegToRad(45.f) + PIf, 0.f);
			break;
		case CubeView::meshID_FL:
			if (m_activeCamera->m_rotationPlatform.y < PIfHalf)
				m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-90.f), bqMath::DegToRad(-45.f), 0.f);
			else
				m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-90.f), bqMath::DegToRad(-45.f) + PIPIf, 0.f);
			break;
		case CubeView::meshID_FR:
			if (m_activeCamera->m_rotationPlatform.y > PIf + PIfHalf)
				m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-90.f), bqMath::DegToRad(-45.f) + PIHalf + PIPIf, 0.f);
			else if (m_activeCamera->m_rotationPlatform.y > PIfHalf)
				m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-90.f), bqMath::DegToRad(-45.f) + PIHalf, 0.f);
			else
				m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-90.f), bqMath::DegToRad(-45.f) + PIHalf, 0.f);
			break;
		case CubeView::meshID_BR:
			if (m_activeCamera->m_rotationPlatform.y > PIf + PIfHalf)
				m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-45.f - 90.f), PIPIf + PIfHalf, 0.f);
			else
				m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-45.f - 90.f), bqMath::DegToRad(90.f), 0.f);
			break;
		case CubeView::meshID_BB:
			m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-45.f - 90.f), PIf, 0.f);
			break;
		case CubeView::meshID_BL:
			if (m_activeCamera->m_rotationPlatform.y > PIHalf)
				m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-45.f - 90.f), bqMath::DegToRad(270.f), 0.f);
			else
				m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-45.f - 90.f), bqMath::DegToRad(-90.f), 0.f);
			break;
		case CubeView::meshID_BF:
			if (m_activeCamera->m_rotationPlatform.y > PIf)
				m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-45.f - 90.f), PIPIf, 0);
			else
				m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-45.f - 90.f), 0, 0);
			break;

		case CubeView::meshID_TRB:
			m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-45.f), bqMath::DegToRad(-45.f) + PIf, 0.f);
			break;
		case CubeView::meshID_TLB:
			m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-45.f), bqMath::DegToRad(45.f) + PIf, 0.f);
			break;
		case CubeView::meshID_TLF:
			if (m_activeCamera->m_rotationPlatform.y < PIfHalf)
				m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-45.f), bqMath::DegToRad(-45.f), 0.f);
			else
				m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-45.f), bqMath::DegToRad(-45.f) + PIPIf, 0.f);
			break;
		case CubeView::meshID_TRF:
			if (m_activeCamera->m_rotationPlatform.y > PIf + PIfHalf)
				m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-45.f), bqMath::DegToRad(-45.f) + PIHalf + PIPIf, 0.f);
			else if (m_activeCamera->m_rotationPlatform.y > PIfHalf)
				m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-45.f), bqMath::DegToRad(-45.f) + PIHalf, 0.f);
			else
				m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-45.f), bqMath::DegToRad(-45.f) + PIHalf, 0.f);
			break;
		case CubeView::meshID_BRF:
			if (m_activeCamera->m_rotationPlatform.y > PIf + PIfHalf)
				m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-45.f - 90.f), bqMath::DegToRad(-45.f) + PIHalf + PIPIf, 0.f);
			else if (m_activeCamera->m_rotationPlatform.y > PIfHalf)
				m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-45.f - 90.f), bqMath::DegToRad(-45.f) + PIHalf, 0.f);
			else
				m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-45.f - 90.f), bqMath::DegToRad(-45.f) + PIHalf, 0.f);
			break;
		case CubeView::meshID_BRB:
			m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-45.f - 90.f), bqMath::DegToRad(-45.f) + PIf, 0.f);
			break;
		case CubeView::meshID_BLB:
			m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-45.f - 90.f), bqMath::DegToRad(45.f) + PIf, 0.f);
			break;
		case CubeView::meshID_BLF:
			if (m_activeCamera->m_rotationPlatform.y < PIfHalf)
				m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-45.f - 90.f), bqMath::DegToRad(-45.f), 0.f);
			else
				m_cubeViewRotationTarget = bqVec3f(bqMath::DegToRad(-45.f - 90.f), bqMath::DegToRad(-45.f) + PIPIf, 0.f);
			break;
		}
		//m_cubeViewCamera->m_rotationPlatform = m_activeCamera->m_rotationPlatform;
	}
}

void ViewportView::CubeViewOnMouseMove()
{
	m_activeCamera->EditorRotate(&g_app->m_inputData->m_mouseMoveDelta, *g_app->m_deltaTime);
	m_cubeViewCamera->m_rotationPlatform = m_activeCamera->m_rotationPlatform;
}
