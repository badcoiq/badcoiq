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
#pragma once
#ifndef _DEMOAPP_H_
#define _DEMOAPP_H_

#include "badcoiq.h"
#include "badcoiq/input/bqInput.h"
#include "badcoiq/gs/bqGS.h"
#include "badcoiq/gs/bqMaterial.h"
#include "badcoiq/common/bqImage.h"
#include "badcoiq/common/bqImageLoader.h"
#include "badcoiq/gs/bqTexture.h"
#include "badcoiq/scene/bqCamera.h"
#include "badcoiq/geometry/bqMeshCreator.h"
#include "badcoiq/containers/bqArray.h"
#include "badcoiq/archive/bqArchive.h"
#include "badcoiq/GUI/bqGUI.h"
#include "badcoiq/system/bqWindow.h"

#include <stdio.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


class FrameworkCallback;
class WindowCallback; 
class GUIDrawTextCallback;
class DemoApp;


class DemoExample
{
	friend class DemoApp;
protected:
	bqString m_name;
	bqString m_description;
	DemoApp* m_app = 0;
	bqGS* m_gs = 0;
public:
	DemoExample(DemoApp* app);
	virtual ~DemoExample() {}
	BQ_PLACEMENT_ALLOCATOR(DemoExample);

	virtual bool Init() = 0;
	virtual void Shutdown() = 0;
	virtual void OnDraw() = 0;

	virtual void OnWindowSize(bqWindow* w);

	bqGS* GetGS() { return m_gs; }
};

class DemoCategory
{
	friend class DemoApp;
	bqString m_name;

	uint32_t m_cursor = 0;

	DemoCategory* m_parentCategory = 0;
	bqArray<DemoCategory*> m_subCategories;
	bqArray<DemoExample*> m_examples;
	
	struct Element
	{
		enum
		{
			type_category,
			type_example,
			type_parentDir
		};
		uint32_t type = type_category;
		DemoCategory* m_cat = 0;
		DemoExample* m_ex = 0;
	};
	bqArray<Element> m_elements; // draw menu using this
	void findElements()
	{
		m_elements.clear();
		Element el;
		if (m_parentCategory)
		{
			el.type = el.type_parentDir;
			m_elements.push_back(el);
		}

		for (size_t i = 0; i < m_subCategories.m_size; ++i)
		{
			el.type = el.type_category;
			el.m_cat = m_subCategories.m_data[i];
			m_elements.push_back(el);
		}

		for (size_t i = 0; i < m_examples.m_size; ++i)
		{
			el.type = el.type_example;
			el.m_ex = m_examples.m_data[i];
			m_elements.push_back(el);
		}
	}
public:
	DemoCategory() {}
	virtual ~DemoCategory() 
	{
		for (size_t i = 0; i < m_subCategories.m_size; ++i)
		{
			delete m_subCategories.m_data[i];
		}
	}
	BQ_PLACEMENT_ALLOCATOR(DemoCategory);
};

class DemoApp
{
	friend class WindowCallback;
	friend class DemoExample;

	bqString m_drawingText;

	FrameworkCallback* m_frameworkCallback = 0;
	WindowCallback* m_windowCallback = 0;
	GUIDrawTextCallback* m_textDrawCallback = 0;
	bool m_isRun = true;
	
	bqWindow* m_window = 0;
	bqGS* m_gs = 0;
	//slCamera* m_camera = 0;
	bqGUIWindow* m_GUIWindow = 0;
	bqInputData* m_inputData = 0;
	bqGUIFont* m_fontDefault = 0;

	DemoCategory m_rootCategory;
	DemoCategory* m_currentCategory = 0;

	bqArray<DemoExample*> m_allExamples;
	DemoExample* m_activeExample = 0;

	bqGUIStaticText* m_staticTextDescription = 0;
	void findDescription();
	
	bqGPUMesh* _createMesh(bqPolygonMesh*);
public:
	DemoApp();
	~DemoApp();
	BQ_PLACEMENT_ALLOCATOR(DemoApp);

	bool Init();
	void Run();
	void Quit() { m_isRun = false; }

	bqGS* GetGS() { return m_gs; }
	bqWindow* GetWindow() { return m_window; }
	GUIDrawTextCallback* GetTextDrawCallback() { return m_textDrawCallback; }

	void AddExample(DemoExample*, const char32_t* name, const char* category,
		const char32_t* description);
	void OnDraw();
	DemoCategory* GetCategory(const char* category);
	DemoCategory* FindCategory(DemoCategory*, const char* name);

	void StartExample(DemoExample*);
	void StopExample();
	
	float* m_dt = 0;
	bqTexture* m_whiteTexture = 0;

	bqTexture* LoadTexture(const char*);
	bqGPUMesh* CreateMeshSphere(uint32_t segments, float radius, bool smooth);
	bqGPUMesh* CreateMeshBox(const bqAabb&);
};

class FrameworkCallback : public bqFrameworkCallback
{
public:
	FrameworkCallback() {}
	virtual ~FrameworkCallback() {}
	BQ_PLACEMENT_ALLOCATOR(FrameworkCallback);
	
	virtual void OnMessage() override
	{
	}
};

class WindowCallback : public bqWindowCallback
{
public:
	WindowCallback() {}
	virtual ~WindowCallback() {}
	BQ_PLACEMENT_ALLOCATOR(WindowCallback);

	virtual void OnSizing(bqWindow* w) override 
	{
	}

	virtual void OnSize(bqWindow* w) override
	{
		DemoApp* app = (DemoApp*)w->GetUserData();
		if (app)
		{
			//if (w->GetCurrentSize()->x && w->GetCurrentSize()->y)
				//app->m_camera->m_aspect = w->GetCurrentSize()->x / w->GetCurrentSize()->y;
			app->m_gs->OnWindowSize();
			app->m_gs->SetViewport(0, 0, (uint32_t)w->GetCurrentSize()->x, (uint32_t)w->GetCurrentSize()->y);
			app->m_gs->SetScissorRect(bqVec4f(0.f, 0.f, (float)w->GetCurrentSize()->x, (float)w->GetCurrentSize()->y));
		//	app->m_GUIWindow->SetPositionAndSize(slVec2f(100.f), slVec2f(w->GetCurrentSize()->x - 400.f, w->GetCurrentSize()->y - 300.f));
			app->m_GUIWindow->Rebuild();

			if (app->m_activeExample)
				app->m_activeExample->OnWindowSize(w);
		}
	}

	virtual void OnClose(bqWindow* w) override 
	{
		DemoApp* app = (DemoApp*)w->GetUserData();
		if (app)
		{
			w->SetVisible(false);
			app->Quit();
		}
	}
};

class GUIDrawTextCallback : public bqGUIDrawTextCallback
{
public:
	GUIDrawTextCallback(bqGUIFont* f):m_font(f) {}
	virtual ~GUIDrawTextCallback() {}
	BQ_PLACEMENT_ALLOCATOR(GUIDrawTextCallback);

	virtual bqGUIFont* OnFont(uint32_t reason, char32_t c) final
	{
		return m_font;
	}
	virtual bqColor* OnColor(uint32_t reason, char32_t c) final
	{
		switch (reason)
		{
		case bqGUIDrawTextCallback::Reason_default:
			return m_curColor;
		case bqGUIDrawTextCallback::Reason_mouseAbove:
			return &m_colorYellow;
		}
		return m_curColor;
	}

	bqGUIFont* m_font = 0;
	bqColor m_colorWhite = bq::ColorWhite;
	bqColor m_colorRed = bq::ColorRed;
	bqColor m_colorYellow = bq::ColorGold;
	bqColor* m_curColor = &m_colorYellow;
};

#include "examples/basic/ex_basic_3DLineAndCamera.h"
#include "examples/scene/camera/ex_sc_camera_fly.h"

#endif
