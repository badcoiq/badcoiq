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

#include "DemoApp.h"

#include "badcoiq/geometry/bqPolygonMesh.h"

BQ_LINK_LIBRARY("badcoiq");

class MyStaticText : public bqGUIStaticText
{
public:
	MyStaticText(bqGUIWindow* w, const bqVec2f& position, const bqVec2f& size) :
		bqGUIStaticText(w, position, size)
	{
	}
	virtual ~MyStaticText() {}
};

void DemoExample::OnWindowSize(bqWindow* w)
{
	DemoApp* app = (DemoApp*)w->GetUserData();
	if (app)
	{
		//app->m_camera->m_aspect = w->GetCurrentSize()->x / w->GetCurrentSize()->y;
		app->m_gs->OnWindowSize();
		app->m_gs->SetViewport(0, 0, (uint32_t)w->GetCurrentSize()->x, (uint32_t)w->GetCurrentSize()->y);
		app->m_gs->SetScissorRect(bqVec4f(0.f, 0.f, (float)w->GetCurrentSize()->x, (float)w->GetCurrentSize()->y));
		app->m_GUIWindow->Rebuild();
	}
}

DemoExample::DemoExample(DemoApp* app) 
	:
	m_app(app), 
	m_gs(app->GetGS()) 
{}


DemoApp::DemoApp()
{
}

DemoApp::~DemoApp()
{
	for (size_t i = 0; i < m_allExamples.m_size; ++i)
	{
		delete m_allExamples.m_data[i];
	}

	BQ_SAFEDESTROY(m_whiteTexture);
	BQ_SAFEDESTROY(m_textDrawCallback);
	BQ_SAFEDESTROY(m_frameworkCallback);
	BQ_SAFEDESTROY(m_windowCallback);
	BQ_SAFEDESTROY(m_window);
	bqFramework::Stop();
}

bool DemoApp::Init()
{
	m_frameworkCallback = new FrameworkCallback();
	m_windowCallback = new WindowCallback();
	
	bqFramework::Start(m_frameworkCallback);
	
	m_window = bqFramework::SummonWindow(m_windowCallback);
	m_window->SetPositionAndSize(0, 0, 800, 600);
	m_window->SetVisible(true);	

	bqArchiveSystem::ZipAdd(bqFramework::GetPath("..\\data\\data.zip").c_str());
	m_inputData = bqInput::GetData();

	// m_gs will be auto destroyed, do not call slDestroy(m_gs);
	m_gs = bqFramework::SummonGS(bqFramework::GetGSUID(0));
	if (m_gs)
	{
		if (!m_gs->Init(m_window, 0))
			return false;
		m_gs->SetClearColor(0.f, 0.f, 1.f, 1.f);
		m_window->SetUserData(this);
	}
	else
	{
		return false;
	}

	bqFramework::InitDefaultFonts(m_gs);
	m_fontDefault = bqFramework::GetDefaultFont(bqGUIDefaultFont::Text);

	m_textDrawCallback = new GUIDrawTextCallback(m_fontDefault);

	m_GUIWindow = bqFramework::SummonGUIWindow(bqVec2f(300.f, 0.f),
		bqVec2f(300.f, 400.f));
	m_GUIWindow->SetDrawBG(false);
	auto staticText = new MyStaticText(m_GUIWindow, bqVec2f(0.f, 0.f), bqVec2f(300.f, 100.f));
	m_staticTextDescription = staticText;
	m_staticTextDescription->SetText(U" ");

	static bqGUIStyle styleInMenu;
	styleInMenu = *bqFramework::GetGUIStyle(bqGUIStyleTheme::Light);
	styleInMenu.m_staticTextTextColor = bq::ColorWhite;
	
	staticText->SetStyle(&styleInMenu);
	staticText->SetDrawBG(false);

	bqFramework::RebuildGUI();

	m_dt = bqFramework::GetDeltaTime();

	m_gs->SetClearColor(0.2f, 0.2f, 0.2f, 1.f);

	{
		bqImage img;
		img.Create(2, 2);
		img.Fill(bqImageFillType::Solid, bq::ColorWhite, bq::ColorWhite);
		bqTextureInfo tinf;
		m_whiteTexture = m_gs->SummonTexture(&img, tinf);
	}


	m_currentCategory = &m_rootCategory;
	AddExample(new ExampleBasics3DLineAndCamera(this));
	/*AddExample(new ExampleBasicsMouseAndKeyboard(this), U"Mouse and keyboard", "basics/", U"Show information about keyboard and mouse.");
	AddExample(new ExampleBasicsImageAndTexture(this), U"Image and texture", "basics/", U"Load image and create texture.");
	AddExample(new ExampleBasics3DModel(this), U"Load model", "basics/", U"Load model data from file. File can contain many objects, need to use callback. Better to create special class for this.");
	AddExample(new ExampleBasicsRenderTargetTexture(this), U"Render Target Texture", "basics/", U"...");
	AddExample(new ExampleBasicsRayFromScreen(this), U"Ray from screen", "basics/", U"Click and move the camera (WASDQE)");
	AddExample(new ExampleSkinnedMesh1(this), U"Skinned mesh", "basics/", U"Low level skining.");

	AddExample(new ExampleSceneEditorCamera(this), U"Editor camera", "scene/", U"Camera like in 3D editor. Hold Alt and RMB for rotation");
	AddExample(new ExampleSceneFrustumCulling(this), U"Frustum culling", "scene/culling", U"Draw only visible objects. Use QWEASD.");
	AddExample(new ExampleSceneOcclusionCulling(this), U"Occlusion culling", "scene/culling", U"Frustum and Occlusion culling");
	AddExample(new ExampleSceneSprite(this), U"Sprite/Billboard", "scene/sprite", U"Basic sprite");
	AddExample(new ExampleSceneSprite10000(this), U"10000", "scene/sprite", U"10000 sprites");

	AddExample(new ExampleBasicsScreenResolution(this), U"Screen Resolution", "other/", U"Change main target size. Use WASDQE");
	AddExample(new ExamplePolygonMesh(this), U"Creating the mesh", "other/PolygonMesh", U"Create mesh by hands, generate some objects, generate UV coordinates for them.");
	AddExample(new ExampleImageGeneration(this), U"Generate texture", "other/Image", U"Generate image and create texture");*/
	//AddExample(new ExampleSprite, U"Physics", "physics/kinematic");
	//AddExample(new ExampleSprite, U"Demo", "/");

	m_rootCategory.findElements();

	m_windowCallback->OnSize(m_window);
	return true;
}

void DemoApp::Run()
{
	float* dt = bqFramework::GetDeltaTime();
	while (m_isRun)
	{
		bqFramework::Update();

	//	Sleep(1);

		m_gs->EnableBlend();

		if (m_activeExample)
			m_activeExample->OnDraw();
		else
			OnDraw();
	}
}

void DemoApp::AddExample(DemoExample* e)
{
	m_allExamples.push_back(e);
	e->m_name = e->GetName();
	e->m_description = e->GetDescription();
	auto cat = GetCategory(e->GetCategory());
	if (cat)
	{
		cat->m_examples.push_back(e);
		cat->findElements();
	}
}

void DemoApp::OnDraw()
{
	m_gs->SetClearColor(0.2f, 0.2f, 0.2f, 1.f);

	m_gs->BeginGUI();
	bqFramework::DrawGUI(m_gs);
	m_gs->SetScissorRect(bqVec4f(0.f, 0.f,
		(float)m_window->GetCurrentSize()->x, (float)m_window->GetCurrentSize()->y));

	float Y = 0.f;
	const float lineHeight = 14.f;

	if (m_currentCategory->m_elements.m_size)
	{
		if (bqInput::IsKeyHit(bqInput::KEY_UP))
		{
			if (m_currentCategory->m_cursor)
				--m_currentCategory->m_cursor;
			else
				m_currentCategory->m_cursor = m_currentCategory->m_elements.m_size - 1;
			findDescription();
		}
		else if (bqInput::IsKeyHit(bqInput::KEY_DOWN))
		{
			++m_currentCategory->m_cursor;
			if (m_currentCategory->m_cursor == m_currentCategory->m_elements.m_size)
				m_currentCategory->m_cursor = 0;
			findDescription();
		}
		else if (bqInput::IsKeyHit(bqInput::KEY_ENTER))
		{
			auto & el = m_currentCategory->m_elements.m_data[m_currentCategory->m_cursor];
			switch (el.type)
			{
			case DemoCategory::Element::type_parentDir:
				m_currentCategory = m_currentCategory->m_parentCategory;
				findDescription();
				return;
			case DemoCategory::Element::type_category:
				m_currentCategory = el.m_cat;
				findDescription();
				return;
			case DemoCategory::Element::type_example:
				StartExample(el.m_ex);
				findDescription();
				return;
			}
		}
		else if (bqInput::IsKeyHit(bqInput::KEY_RIGHT))
		{
			auto& el = m_currentCategory->m_elements.m_data[m_currentCategory->m_cursor];
			switch (el.type)
			{
			case DemoCategory::Element::type_parentDir:
				break;
			case DemoCategory::Element::type_category:
				m_currentCategory = el.m_cat;
				findDescription();
				return;
			case DemoCategory::Element::type_example:
				break;
			}
		}
		else if (bqInput::IsKeyHit(bqInput::KEY_LEFT))
		{
			if(m_currentCategory->m_parentCategory)
				m_currentCategory = m_currentCategory->m_parentCategory;
			findDescription();
			return;
		}
		else if (bqInput::IsKeyHit(bqInput::KEY_ESCAPE))
		{
			if (m_currentCategory->m_parentCategory)
				m_currentCategory = m_currentCategory->m_parentCategory;
			findDescription();
			return;
		}
		

		for (size_t i = 0; i < m_currentCategory->m_elements.m_size; ++i)
		{
			m_textDrawCallback->m_curColor = &m_textDrawCallback->m_colorWhite;
			if (i == m_currentCategory->m_cursor)
				m_textDrawCallback->m_curColor = &m_textDrawCallback->m_colorYellow;

			m_drawingText.clear();
			switch (m_currentCategory->m_elements.m_data[i].type)
			{
				case DemoCategory::Element::type_parentDir:
					m_drawingText.append(U"[...]");
					break;
				case DemoCategory::Element::type_category:
					m_drawingText.append(U"[");
					m_drawingText.append(m_currentCategory->m_elements.m_data[i].m_cat->m_name);
					m_drawingText.append(U"]");
					break;
				case DemoCategory::Element::type_example:
					m_drawingText.append(m_currentCategory->m_elements.m_data[i].m_ex->m_name);
					//m_drawingText.append(U"*");
					break;
			}
			m_gs->DrawGUIText(m_drawingText.c_str(), m_drawingText.size(), bqVec2f(0.f, Y), m_textDrawCallback);
			Y += lineHeight;
		}
	}
	m_gs->EndGUI();

	m_gs->BeginDraw();
	m_gs->ClearAll();

	m_gs->EndDraw();
	m_gs->SwapBuffers();
}

DemoCategory* DemoApp::GetCategory(const char* category)
{
	bqStringA stra;
	stra.assign(category);

	const char* delimiters = "/";

	DemoCategory* curCat = &m_rootCategory;
	
	char* next_token1 = NULL;

	char* token = strtok_s((char*)stra.data(), delimiters, &next_token1);
	while (token) 
	{
		auto findCat = FindCategory(curCat, token);
		if (findCat)
		{
			curCat = findCat;
		}
		else
		{
			DemoCategory* newCategory = new DemoCategory;
			newCategory->m_parentCategory = curCat;
			newCategory->m_name = token;
			curCat->m_subCategories.push_back(newCategory);
			curCat->findElements();
			curCat = newCategory;
		}
		curCat->findElements();

		token = strtok_s(nullptr, delimiters, &next_token1);
	}
	curCat->findElements();
	return curCat;
}

DemoCategory* DemoApp::FindCategory(DemoCategory* cat, const char* name)
{
	bqString N;
	N = name;
	for (size_t i = 0; i < cat->m_subCategories.m_size; ++i)
	{
		if (cat->m_subCategories.m_data[i]->m_name == N)
			return cat->m_subCategories.m_data[i];
	}

	return nullptr;
}

void DemoApp::StopExample()
{
	if (m_activeExample)
	{
		m_activeExample->Shutdown();
		m_activeExample = 0;
	}
	m_GUIWindow->SetVisible(true);
}

void DemoApp::StartExample(DemoExample* ex)
{
	StopExample();

	if (ex->Init())
	{
		m_activeExample = ex;
	}
	else
	{
		ex->Shutdown();
	}

	m_GUIWindow->SetVisible(false);
}

void DemoApp::findDescription()
{
	auto i = m_currentCategory->m_cursor;

	switch (m_currentCategory->m_elements.m_data[i].type)
	{
	case DemoCategory::Element::type_parentDir:
	case DemoCategory::Element::type_category:
		m_staticTextDescription->SetText(U" ");
		break;
	case DemoCategory::Element::type_example:
		m_staticTextDescription->SetText(m_currentCategory->m_elements.m_data[i].m_ex->m_description.c_str());
		break;
	}
}

bqTexture* DemoApp::LoadTexture(const char* f)
{
	bqTexture* t = 0;
	auto image = bqFramework::SummonImage(bqFramework::GetPath(f).c_str());
	if (image)
	{
		bqTextureInfo ti;
		t = m_gs->SummonTexture(image, ti);

		BQ_SAFEDESTROY(image);

		if (!t)
		{
			bqLog::PrintError("%s: can't create texture\n", BQ_FUNCTION);
			BQ_ASSERT_ST(t);
			return 0;
		}
	}
	return t;
}

bqGPUMesh* DemoApp::CreateMeshSphere(uint32_t segments, float radius, bool smooth)
{
	bqGPUMesh* m_GPUMesh = 0;

	/*bqPolygonMesh* polygonMesh = bqFramework::SummonPolygonMesh();
	polygonMesh->AddSphere(radius, segments, bqMat4());
	polygonMesh->GenerateNormals(smooth);*/

	//m_GPUMesh = _createMesh(polygonMesh);
	//BQ_SAFEDESTROY(polygonMesh);
	return m_GPUMesh;
}

bqGPUMesh* DemoApp::_createMesh(bqPolygonMesh* polygonMesh)
{
	bqGPUMesh* m_GPUMesh = 0;
	bqMesh* mesh = polygonMesh->SummonMesh();

	if (mesh)
	{
		mesh->GenerateTangents();
		m_GPUMesh = m_gs->SummonMesh(mesh);
		BQ_SAFEDESTROY(mesh);
		if (!m_GPUMesh)
			bqLog::PrintError("Don't know why [%s : %i]\n", BQ_FUNCTION, BQ_LINE);
	}
	else
	{
		bqLog::PrintError("Don't know why [%s : %i]\n", BQ_FUNCTION, BQ_LINE);
	}
	return m_GPUMesh;
}

bqGPUMesh* DemoApp::CreateMeshBox(const bqAabb& b)
{
	bqGPUMesh* m_GPUMesh = 0;

	/*bqPolygonMesh* polygonMesh = bqFramework::SummonPolygonMesh();
	polygonMesh->AddBox(b, bqMat4());

	m_GPUMesh = _createMesh(polygonMesh);
	BQ_SAFEDESTROY(polygonMesh);*/
	return m_GPUMesh;
}

