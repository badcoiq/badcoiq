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

#ifndef _VIEWPORT_H_
#define _VIEWPORT_H_

#include "badcoiq/containers/bqArray.h"

/// Viewport в данном случае выступает менеджером к различным viewport
/// 
/// Viewport содержит ViewportLayout
/// ViewportLayout содержит ViewportView
/// ViewportLayout это макет/планировка
///  он может содержать 1 или более ViewportView
/// ViewportView это есть то во что рисуется сцена
class ViewportView;
class ViewportLayout;
class Viewport
{
	friend class ViewportLayout;
	bqVec4f m_rectangle;
	bqArray<ViewportLayout*> m_layouts;
	ViewportLayout* m_activeLayout = 0;

	ViewportLayout* m_fullViewLayout = 0;

	// нужно будет следить за этой переменной.
	// в будущем могут быть иные layout
	// при изменении layout, будет нужно изменять
	// эту переменную
	ViewportLayout* m_beforeToggleFullViewLayout = 0;

public:
	Viewport();
	~Viewport();

	void Rebuild();
	void Update();
	void Draw();
	void SetActiveViewportViewType(uint32_t);
	void ToggleFullView();
	void ToggleGrid();
	void CameraReset();
};

class ViewportView
{
public:
	enum
	{
		type_perspective,
		type_right,
		type_left,
		type_top,
		type_bottom,
		type_front,
		type_back,

		type__size
	};

private:
	friend class ViewportLayout;
	friend class CubeView;
	friend class ModelEditor;

	void _DrawScene(ViewportView* view);
	void _DrawGrid(int gridSize);

	const char32_t* m_viewportTypeText = U" ";

	bool m_drawGrid = true;
	bool m_fullview = false;
	bqVec4f m_rectangle = bqVec4f(0.f,0.f,1.f,1.f);
	bqVec4f m_viewportOptionsRectangle;
	bqVec4f m_cubeViewRectangle = bqVec4f(0.f, 0.f, 1.f, 1.f);
	float32_t m_cubeViewSize = 128.f;
	ViewportLayout* m_layout = 0;
	
	bqCamera* m_cameras[type__size];
	bqCamera* m_cubeViewCamera = 0;
	bqCamera* m_activeCamera = 0;
	bqTexture* m_rtt = 0;
	
	bqTexture* m_rttCubeView = 0;

	bqVec3f m_cubeViewRotationTarget;
	bool m_cubeViewNowRotating = false;
	float32_t m_cubeViewRotatingLerpTime = 0.f;
	float32_t m_cubeViewRotatingLerpTimeLimit = 0.5f;
public:
	ViewportView(ViewportLayout*, uint32_t type);
	~ViewportView();

	uint32_t m_type = type_perspective;

	void Rebuild();
	void Update();
	void Draw();
	void SetActiveView();
	void SetCameraType(uint32_t);
	void ResetCamera();
	
	void CubeViewOnClick(uint32_t meshID);
	void CubeViewOnMouseMove();

	//void CopyDataFrom(ViewportView*);
	void ToggleGrid();
};

class ViewportLayout
{
	friend class ViewportView;		
	Viewport* m_viewport = 0;
	bqArray<ViewportView*> m_views;
	ViewportView* m_activeView = 0;
	ViewportView* m_mouseFocusView = 0;
	bqVec4f m_resizeRect;
public:
	ViewportLayout(Viewport* viewport, uint32_t type);
	~ViewportLayout();
	
	void ToggleFullView();

	enum
	{
		type_full,
		type_4views
	};
	uint32_t m_type = 0;

	void Rebuild();
	void Update();
	void Draw();

	ViewportView* GetActiveView() { return m_activeView; }
};

#endif
