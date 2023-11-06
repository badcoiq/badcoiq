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
	BQ_SAFEDESTROY(m_texture4x4);
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

	m_GUIWindow = bqFramework::SummonGUIWindow(m_window, bqVec2f(300.f, 0.f),
		bqVec2f(300.f, 400.f));
	m_GUIWindow->SetDrawBG(false);
	auto staticText = new MyStaticText(m_GUIWindow, bqVec2f(0.f, 0.f), bqVec2f(300.f, 300.f));
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

	bqImage* image = bqFramework::SummonImage(bqFramework::GetPath("../data/images/4x4.png").c_str());
	if (image)
	{
		bqTextureInfo tinf;
		m_texture4x4 = m_gs->SummonTexture(image, tinf);
		delete image;
	}

	m_currentCategory = &m_rootCategory;
	AddExample(new ExampleBasics3DLineAndCamera(this), U"3D линия и камера", "basics/", U"Основы. Надо хотьчто-то нарисовать и уметь перемещаться нажимая кнопки. Используй WASDQE");
	AddExample(new ExampleBasicsRayFromCursor(this), U"Луч от курсора", "basics/", U"Кликаем ЛКМ, перемещаемся WASDQE, и видим созданные лучи");
	AddExample(new ExampleBasicsMshGnrtr(this), U"Генератор моделей", "basics/", U"Основа работы с генератором моделей. Вместо мучений с загрузкой файлов проще что нибудь сгенерировать.");
	AddExample(new ExampleBasics3DModel(this), U"Загрузка моделей", "basics/", U"Обычно файл с моделью содержит множество моделей. Будет правильнее грузить эти модели по отдельности. Для этого нужно будет передать коллбэк. Использовать конечно будет не так просто, правильнее сделать базовый класс для моделей.");
	AddExample(new ExampleBasicsRayTri(this), U"Пересечение луча и треугольника", "basics/", U"Для того чтобы понять пересёк ли луч модель, нужно хранить эту модель в памяти. В этом примере используется bqPolygonMesh");
	AddExample(new ExampleBasicsRayTri2(this), U"Пересечение луча и треугольника / 2", "basics/", U"Почти тоже самое но модель грузится из файла, и треугольники берутся из bqMesh. Так же есть оптимизация, проверка на пересечение AABB");
	AddExample(new ExampleBasicsSprite(this), U"Спрайты", "basics/", U"Создание спрайта, установка анимации.");
	AddExample(new ExampleBasicsText3D(this), U"Текст в 3D", "basics/", U"Плоский текст в 3D. Просто рисуем спрайты.");
	AddExample(new ExampleBasicsRotations(this), U"Вращение", "basics/", U"Работа вспомогательного класса bqSceneObject. Жми кнопки 1 2 3 4 5 6 для вращения");
	AddExample(new ExampleBasicsOclCul(this), U"Occlusion Culling", "basics/", U"Создаём особый объект который будет использоваться GPU. Рисуем специальным способом с указанием этого объекта. Обязательно надо отсортировать объекты по дальности. ");
	AddExample(new ExampleBasicsSkeletalAnimation(this), U"Скелетная анимация 1/2", "basics/", U"Всё создаём вручную, и анимируем тоже.");
	AddExample(new ExampleBasicsSkeletalAnimation2(this), U"Скелетная анимация 2/2", "basics/", U"Загрузка SMD. Множество файлов, отдельно геометрия отдельно анимации.");
	
	AddExample(new ExampleMshGnBox(this), U"Box", "generators/mesh", U"Всё о создании коробки");
	
	AddExample(new ExampleSceneCameraFly(this), U"Летающая камера", "scene/camera", U"Камера перемещается туда куда смотрит. Просто при перемещении крутим вектр (умножаем на матрицу вращения), получается нужный вектр. Используй WASDQE, пробел и мышь.");
	
	AddExample(new ExampleGameMario(this), U"Mario", "games/", U"Тот самый Марио");
	
	// что-то не работает код хотя 99 процентов копипаста
	//AddExample(new ExampleSceneCameraEdtr(this), U"Камера для 3D редактора", "scene/camera", U"Как в 3Ds Max");

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

		m_gs->EnableBlend();

		if (m_activeExample)
			m_activeExample->OnDraw();
		else
			OnDraw();
	}
}

void DemoApp::AddExample(DemoExample* e, const char32_t* name, const char* category,
	const char32_t* description)
{
	m_allExamples.push_back(e);
	e->m_name = name;
	e->m_description = description;
	auto cat = GetCategory(category);
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

void DemoApp::DrawAABB(const bqAabb& aabb, const bqColor& color, const bqVec4& position)
{
	auto& p1 = aabb.m_min;
	auto& p2 = aabb.m_max;

	bqVec4 v1 = p1;
	bqVec4 v2 = p2;

	bqVec4 v3(p1.x, p1.y, p2.z, 1.f);
	bqVec4 v4(p2.x, p1.y, p1.z, 1.f);
	bqVec4 v5(p1.x, p2.y, p1.z, 1.f);
	bqVec4 v6(p1.x, p2.y, p2.z, 1.f);
	bqVec4 v7(p2.x, p1.y, p2.z, 1.f);
	bqVec4 v8(p2.x, p2.y, p1.z, 1.f);

	m_gs->DrawLine3D(v1 + position, v4 + position, color);
	m_gs->DrawLine3D(v5 + position, v8 + position, color);
	m_gs->DrawLine3D(v1 + position, v5 + position, color);
	m_gs->DrawLine3D(v4 + position, v8 + position, color);
	m_gs->DrawLine3D(v3 + position, v7 + position, color);
	m_gs->DrawLine3D(v6 + position, v2 + position, color);
	m_gs->DrawLine3D(v3 + position, v6 + position, color);
	m_gs->DrawLine3D(v7 + position, v2 + position, color);
	m_gs->DrawLine3D(v2 + position, v8 + position, color);
	m_gs->DrawLine3D(v4 + position, v7 + position, color);
	m_gs->DrawLine3D(v5 + position, v6 + position, color);
	m_gs->DrawLine3D(v1 + position, v3 + position, color);
}

void DemoApp::DrawGrid(int gridSize, float positionCameraY)
{
	m_gs->SetShader(bqShaderType::Line3D, 0);
	bqColor gridColor = bq::ColorLightGrey;
	if (positionCameraY < 0.f)	gridColor = bq::ColorBlack;
	for (int i = 0, z = 7; i <= gridSize; ++i, --z) {
		m_gs->DrawLine3D(bqVec3(((float)-gridSize) * 0.5f, 0.f, -z), bqVec3(((float)gridSize) * 0.5f, 0.f, -z), gridColor);
		m_gs->DrawLine3D(bqVec3(-z, 0.f, ((float)-gridSize) * 0.5f), bqVec3(-z, 0.f, ((float)gridSize) * 0.5f), gridColor);
	}
}
