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
#include "badcoiq/system/bqPopup.h"
#include "badcoiq/system/bqDLL.h"
#include "badcoiq/framework/bqShortcutManager.h"
#include "badcoiq/scene/bqCamera.h"

#include <filesystem>

ModelEditor* g_app = 0;

int main(int argc, char* argv[])
{
	ModelEditor app;
	if (app.Init())
	{
		app.Run();
	}

	return EXIT_SUCCESS;
}

enum GUI_BUTTON_ID
{
	ButtonID_MainMenu = 1
};

class GUIButton : public bqGUIButton
{
	uint32_t m_id = 0;
public:
	GUIButton(const bqVec2f& position, const bqVec2f& size, uint32_t id) :bqGUIButton(position, size) 
	{
		m_id = id;
	}
	virtual ~GUIButton() {}

	virtual void OnReleaseLMB()
	{
		bqFramework::ShowPopupAtCursor(g_app->m_popupMainMenuOptions, g_app->m_mainWindow);
	}
};

class GUIPictureBox : public bqGUIPictureBox{public:
	GUIPictureBox(const bqVec2f& position, const bqVec2f& size) :bqGUIPictureBox(position, size){}
	virtual ~GUIPictureBox() {}
};


ModelEditor::ModelEditor()
{
	g_app = this;
}

ModelEditor::~ModelEditor()
{
	Shutdown();
}

bool ModelEditor::Init()
{
	bqFramework::Start(&m_frameworkCallback);
	m_deltaTime = bqFramework::GetDeltaTime();
	m_inputData = bqInput::GetData();

	m_mainWindowCallback.SetUserData(this);
	m_mainWindow = bqFramework::CreateSystemWindow(&m_mainWindowCallback);
	if (m_mainWindow && bqFramework::GetGSNum())
	{
		m_mainWindow->SetPositionAndSize(10, 10, 800, 600);
		m_mainWindow->SetVisible(true);
	}
	else
	{
		APP_PRINT_ERROR;
		return false;
	}

	m_gs = bqFramework::CreateGS(bqFramework::GetGSUID(0));
	if (!m_gs)
	{
		APP_PRINT_ERROR;
		return false;
	}

	if (!m_gs->Init(m_mainWindow, 0))
	{
		APP_PRINT_ERROR;
		return false;
	}

	m_shortcutMgr = new bqShortcutManager(100);
	m_shortcutMgr->SetCommand(CommandID_MainMenuNew, L"New", 0, 0);
	m_shortcutMgr->SetCommand(CommandID_MainMenuExit, L"Exit", bq::KeyboardMod_Alt, bqInput::KEY_F4);
	m_shortcutMgr->SetCommand(CommandID_MainMenuSave, L"Save", bq::KeyboardMod_Ctrl, bqInput::KEY_S);
	m_shortcutMgr->SetCommand(CommandID_MainMenuSaveAs, L"Save As...", bq::KeyboardMod_CtrlShift, bqInput::KEY_S);
	m_shortcutMgr->SetCommand(CommandID_ViewportViewPerspective, L"Perspective", 0, bqInput::KEY_P);
	m_shortcutMgr->SetCommand(CommandID_ViewportViewLeft, L"Left", 0, bqInput::KEY_L);
	m_shortcutMgr->SetCommand(CommandID_ViewportViewTop, L"Top", 0, bqInput::KEY_T);
	m_shortcutMgr->SetCommand(CommandID_ViewportViewFront, L"Front", 0, bqInput::KEY_F);
	m_shortcutMgr->SetCommand(CommandID_ViewportToggleFullView, L"TFV", bq::KeyboardMod_Alt, bqInput::KEY_W);
	m_shortcutMgr->SetCommand(CommandID_CameraMoveToSelection, L"Z", 0, bqInput::KEY_Z);
	m_shortcutMgr->SetCommand(CommandID_ViewportToggleGrid, L"G", 0, bqInput::KEY_G);

	m_GUITexture = bqFramework::CreateTexture(m_gs,
		bqFramework::GetPath("../data/model_editor/gui.png").c_str(),
		false, false);
	

	bqFramework::InitDefaultFonts(m_gs);
	m_drawTextCallback.SetFont(bqFramework::GetDefaultFont(bqGUIDefaultFont::Text));

	m_GUIStyle = *bqFramework::GetGUIStyle(bqGUIStyleTheme::Light);
	m_GUIStyle.m_buttonBGColor1 = 0xFFFFFFFF;
	m_GUIStyle.m_buttonBGColor2 = 0xFFFFFFFF;
	m_GUIStyle.m_buttonMouseHoverBGColor1 = 0xFFFFFFFF;
	m_GUIStyle.m_buttonMouseHoverBGColor2 = 0xFFFFFFFF;

	m_GUIWindow_mainMenuBar = m_mainWindow->CreateNewGUIWindow(bqVec2f(0.f, 0.f),
		bqVec2f(800.f, 32.f));
	m_GUIWindow_mainMenuBar->SetDrawBG(true);
	m_GUIWindow_mainMenuBar->m_windowFlags |= bqGUIWindowBase::windowFlag_disableToTop;
	m_GUIWindow_mainMenuBar->Activate();


	auto pictureBox = GUI_createPictureBox(bqVec2f(), bqVec2f(800.f, 32.f), 0);
	pictureBox->SetDrawBG(true);
	pictureBox->SetTexture(m_GUITexture);
	pictureBox->SetTCoords(32.f, 0.f, 36.f, 31.f);
	pictureBox->SetName("menubar_bg");
	m_GUIWindow_mainMenuBar->AddElement(pictureBox);

	auto button = GUI_createButton(bqVec2f(), bqVec2f(32.f, 32.f), GUI_BUTTON_ID::ButtonID_MainMenu);
	button->SetDrawBG(true);
	button->SetTexture(m_GUITexture);
	button->SetTCoords(0.f, 0.f, 31.f, 31.f);
	m_GUIWindow_mainMenuBar->AddElement(button);

	m_GUIWindow_editPanel = m_mainWindow->CreateNewGUIWindow(bqVec2f(0.f, 0.f),
		bqVec2f(10.f, 10.f));
	m_GUIWindow_editPanel->SetDrawBG(true);
	m_GUIWindow_editPanel->Activate();
	m_GUIWindow_editPanel->m_windowFlags |= bqGUIWindowBase::windowFlag_disableToTop;
	m_GUIWindow_editPanel->m_windowFlags |= m_GUIWindow_addObject->windowFlag_withShadow;

	m_GUIWindow_addObject = m_mainWindow->CreateNewGUIWindow(bqVec2f(0.f, 0.f),
		bqVec2f(300.f, 300.f));
	m_GUIWindow_addObject->SetDrawBG(true);
	m_GUIWindow_addObject->m_windowFlags |= m_GUIWindow_addObject->windowFlag_withTitleBar;
	m_GUIWindow_addObject->m_windowFlags |= m_GUIWindow_addObject->windowFlag_withShadow;
	m_GUIWindow_addObject->m_windowFlags |= m_GUIWindow_addObject->windowFlag_canMove;
	m_GUIWindow_addObject->m_windowFlags |= m_GUIWindow_addObject->windowFlag_withCloseButton;
	m_GUIWindow_addObject->GetTitleText().assign(U"Add object");
	m_GUIWindow_addObject->Activate();

	_initGrid();

	m_viewport = new Viewport;

	m_sdk = new bqME;

	_initPlugins();

	GUI_rebuild();

	m_popupViewportOptions = bqFramework::CreatePopup();
	m_popupViewportOptions->AddItem(L"Perspective", CommandID_ViewportViewPerspective, m_shortcutMgr->GetTextW(CommandID_ViewportViewPerspective));
	m_popupViewportOptions->AddItem(L"Top", CommandID_ViewportViewTop, m_shortcutMgr->GetTextW(CommandID_ViewportViewTop));
	m_popupViewportOptions->AddItem(L"Bottom", CommandID_ViewportViewBottom, m_shortcutMgr->GetTextW(CommandID_ViewportViewBottom));
	m_popupViewportOptions->AddItem(L"Left", CommandID_ViewportViewLeft, m_shortcutMgr->GetTextW(CommandID_ViewportViewLeft));
	m_popupViewportOptions->AddItem(L"Right", CommandID_ViewportViewRight, m_shortcutMgr->GetTextW(CommandID_ViewportViewRight));
	m_popupViewportOptions->AddItem(L"Front", CommandID_ViewportViewFront, m_shortcutMgr->GetTextW(CommandID_ViewportViewFront));
	m_popupViewportOptions->AddItem(L"Back", CommandID_ViewportViewBack, m_shortcutMgr->GetTextW(CommandID_ViewportViewBack));
	m_popupViewportOptions->AddSeparator();
	m_popupViewportOptions->AddItem(L"Toggle full view", CommandID_ViewportToggleFullView, m_shortcutMgr->GetTextW(CommandID_ViewportToggleFullView));
	m_popupViewportOptions->AddItem(L"Toggle grid", CommandID_ViewportToggleGrid, m_shortcutMgr->GetTextW(CommandID_ViewportToggleGrid));
	m_popupViewportOptions->AddSeparator();
	m_popupViewportOptions->AddItem(L"Material", CommandID_ViewportDrawMaterial, m_shortcutMgr->GetTextW(CommandID_ViewportDrawMaterial));
	m_popupViewportOptions->AddItem(L"Material+Wireframe", CommandID_ViewportDrawMaterialWireframe, m_shortcutMgr->GetTextW(CommandID_ViewportDrawMaterialWireframe));
	m_popupViewportOptions->AddItem(L"Wireframe", CommandID_ViewportDrawWireframe, m_shortcutMgr->GetTextW(CommandID_ViewportDrawWireframe));
	m_popupViewportOptions->AddItem(L"Toggle draw material", CommandID_ViewportToggleDrawMaterial, m_shortcutMgr->GetTextW(CommandID_ViewportToggleDrawMaterial));
	m_popupViewportOptions->AddItem(L"Toggle draw wireframe", CommandID_ViewportToggleDrawWireframe, m_shortcutMgr->GetTextW(CommandID_ViewportToggleDrawWireframe));
	m_popupViewportOptions->AddSeparator();
	m_popupViewportOptions->AddItem(L"Toggle draw AABB", CommandID_ViewportToggleDrawAABB, m_shortcutMgr->GetTextW(CommandID_ViewportToggleDrawAABB));
	m_popupViewportOptions->AddSeparator();
	m_popupViewportOptions->AddItem(L"Camera Reset", CommandID_CameraReset, m_shortcutMgr->GetTextW(CommandID_CameraReset));
	m_popupViewportOptions->AddItem(L"Camera Move to selection", CommandID_CameraMoveToSelection, m_shortcutMgr->GetTextW(CommandID_CameraMoveToSelection));

	m_popupMainMenuOptions = bqFramework::CreatePopup();
	m_popupMainMenuOptions->AddItem(L"New", CommandID_MainMenuNew, m_shortcutMgr->GetTextW(CommandID_MainMenuNew));
	m_popupMainMenuOptions->AddItem(L"Open", CommandID_MainMenuOpen, m_shortcutMgr->GetTextW(CommandID_MainMenuOpen));
	m_popupMainMenuOptions->AddSeparator();
	m_popupMainMenuOptions->AddItem(L"Save", CommandID_MainMenuSave, m_shortcutMgr->GetTextW(CommandID_MainMenuSave));
	m_popupMainMenuOptions->AddItem(L"Save As...", CommandID_MainMenuSaveAs, m_shortcutMgr->GetTextW(CommandID_MainMenuSaveAs));
	m_popupMainMenuOptions->AddItem(L"Save Copy", CommandID_MainMenuSaveCopy, m_shortcutMgr->GetTextW(CommandID_MainMenuSaveCopy));
	m_popupMainMenuOptions->AddSeparator();
	m_popupMainMenuOptions->AddItem(L"Import", CommandID_MainMenuImport, m_shortcutMgr->GetTextW(CommandID_MainMenuImport));
	m_popupMainMenuOptions->AddItem(L"Export", CommandID_MainMenuExport, m_shortcutMgr->GetTextW(CommandID_MainMenuExport));
	m_popupMainMenuOptions->AddSeparator();
	m_popupMainMenuOptions->AddItem(L"Exit", CommandID_MainMenuExit, m_shortcutMgr->GetTextW(CommandID_MainMenuExit));


	m_mainWindow->ToFullscreenMode();
	m_mainWindow->SetBorderless(false);

	m_cubeView = new CubeView;
	if (!m_cubeView->Init())
		BQ_SAFEDESTROY(m_cubeView);

	return true;
}

bqGUIButton* ModelEditor::GUI_createButton(const bqVec2f& p, const bqVec2f& s, uint32_t id)
{
	GUIButton* e = new GUIButton(p,s,id);
	e->SetStyle(&m_GUIStyle);
	m_GUIElements.push_back(e);
	return e;
}

bqGUIPictureBox* ModelEditor::GUI_createPictureBox(const bqVec2f& p, const bqVec2f& s, uint32_t id)
{
	GUIPictureBox* e = new GUIPictureBox(p, s);
	e->SetStyle(&m_GUIStyle);
	m_GUIElements.push_back(e);
	return e;
}

void ModelEditor::Run()
{
	while (m_run)
	{
		bqFramework::Update();
		m_mainWindow->UpdateGUI();

		//m_gs->BeginDraw();
		//m_gs->ClearAll();
		//m_gs->EndDraw();

		m_cursorInGUIWindow = bqFramework::GetGUIState().m_windowUnderCursor;

		_processShortcuts();
		m_viewport->Update();
		m_viewport->Draw();

		m_gs->BeginGUI(false);
		//m_gs->DrawGUIRectangle(m_editPanelRect, bq::ColorRed, bq::ColorYellow, 0, 0);
		
		m_mainWindow->DrawGUI(m_gs);

		m_gs->SetScissorRect(bqVec4f(0.f, 0.f,
			(float)m_mainWindow->GetCurrentSize()->x, (float)m_mainWindow->GetCurrentSize()->y));
	////	m_gs->DrawGUIRectangle(bqVec4f(0.f, 0.f, 100.f, 20.f), bq::ColorRed, bq::ColorYellow, 0, 0);
	////	m_gs->DrawGUIText(U"Hello!!!", 9, bqVec2f(10.f), &m_drawTextCallback);

		m_gs->EndGUI();

		m_gs->SetRenderTargetDefault();
		m_gs->BeginDraw();
		m_gs->ClearAll();
		m_gs->EndDraw();
		m_gs->SwapBuffers();
	}
}

void ModelEditor::Shutdown()
{
	BQ_SAFEDESTROY(m_popupViewportOptions);
	BQ_SAFEDESTROY(m_popupMainMenuOptions);
	BQ_SAFEDESTROY(m_cubeView);
	BQ_SAFEDESTROY(m_shortcutMgr);
	BQ_SAFEDESTROY(m_gridModel_perspective1);
	BQ_SAFEDESTROY(m_gridModel_perspective2);
	BQ_SAFEDESTROY(m_gridModel_top);
	BQ_SAFEDESTROY(m_gridModel_front);
	BQ_SAFEDESTROY(m_gridModel_left);

	for (size_t i = 0; i < m_GUIElements.m_size; ++i)
	{
		delete m_GUIElements.m_data[i];
	}
	m_GUIElements.clear();
}

void MainWindowCallback::OnClose(bqWindow* w)
{
	g_app->OnWindowCallback_onClose(w);
}

void MainWindowCallback::OnSize(bqWindow* w)
{
	g_app->OnWindowCallback_onSize(w);
}

void MainWindowCallback::OnPopupMenu(bqWindow* w, uint32_t id)
{
	g_app->OnWindowCallback_onPopupMenu(w, id);
}

void ModelEditor::OnWindowCallback_onClose(bqWindow* w)
{
	OnExit();
}

void ModelEditor::OnWindowCallback_onSize(bqWindow* w)
{
	if (m_gs)
		m_gs->OnWindowSize();
	GUI_rebuild();
}

void ModelEditor::OnWindowCallback_onPopupMenu(bqWindow* w, uint32_t id)
{
	switch (id)
	{
	case CommandID_MainMenuNew:
		break;
	case CommandID_MainMenuExit:
		OnExit();
		break;
	case CommandID_MainMenuExport:
		break;
	case CommandID_MainMenuImport:
		break;
	case CommandID_MainMenuOpen:
		break;
	case CommandID_MainMenuSave:
		break;
	case CommandID_MainMenuSaveAs:
		break;
	case CommandID_MainMenuSaveCopy:
		break;

	case CommandID_ViewportViewPerspective:
		g_app->SetActiveViewportViewType(ViewportView::type_perspective);
		break;
	case CommandID_ViewportViewTop:
		g_app->SetActiveViewportViewType(ViewportView::type_top);
		break;
	case CommandID_ViewportViewBottom:
		g_app->SetActiveViewportViewType(ViewportView::type_bottom);
		break;
	case CommandID_ViewportViewLeft:
		g_app->SetActiveViewportViewType(ViewportView::type_left);
		break;
	case CommandID_ViewportViewRight:
		g_app->SetActiveViewportViewType(ViewportView::type_right);
		break;
	case CommandID_ViewportViewFront:
		g_app->SetActiveViewportViewType(ViewportView::type_front);
		break;
	case CommandID_ViewportViewBack:
		g_app->SetActiveViewportViewType(ViewportView::type_back);
		break;
	case CommandID_ViewportToggleFullView:
		g_app->ToggleFullView();
		break;
	case CommandID_ViewportToggleGrid:
		g_app->ToggleGrid();
		break;
	case CommandID_ViewportDrawMaterial:
		break;
	case CommandID_ViewportDrawMaterialWireframe:
		break;
	case CommandID_ViewportDrawWireframe:
		break;
	case CommandID_ViewportToggleDrawMaterial:
		break;
	case CommandID_ViewportToggleDrawWireframe:
		break;
	case CommandID_ViewportToggleDrawAABB:
		break;
	case CommandID_CameraReset:
		g_app->CameraReset();
		break;
	case CommandID_CameraMoveToSelection:
		break;
	}
}

void ModelEditor::OnExit()
{
	m_run = false;
}

void ModelEditor::SetActiveViewportViewType(uint32_t t)
{
	m_viewport->SetActiveViewportViewType(t);
}

void ModelEditor::ToggleFullView()
{
	m_viewport->ToggleFullView();
}

void ModelEditor::ToggleGrid()
{
	m_viewport->ToggleGrid();
}

void ModelEditor::CameraReset()
{
	m_viewport->CameraReset();
}

void ModelEditor::GUI_rebuild()
{
	m_mainMenuBarRect.x = 0.f;
	m_mainMenuBarRect.y = 0.f;
	m_mainMenuBarRect.z = (float)m_mainWindow->GetCurrentSize()->x;
	m_mainMenuBarRect.w = 32.f;

	if (m_GUIWindow_mainMenuBar)
	{
		auto el = m_GUIWindow_mainMenuBar->GetGUIElement("menubar_bg");
		if (el)
		{
			bqGUIPictureBox* pb = dynamic_cast<bqGUIPictureBox*>(el);
			if (pb)
			{
				pb->SetSize(m_mainWindow->GetCurrentSize()->x, pb->GetSize().y);
			}
		}

		m_GUIWindow_mainMenuBar->SetSize(m_mainMenuBarRect.z, m_mainMenuBarRect.w);
	//	m_GUIWindow_mainMenuBar->Rebuild();
	}

	m_editPanelRect.x = 0.f;
	m_editPanelRect.y = m_mainMenuBarRect.w;
	m_editPanelRect.z = m_editPanelWidth;
	m_editPanelRect.w = m_mainWindow->GetCurrentSize()->y;
	if (m_GUIWindow_editPanel)
	{
		m_GUIWindow_editPanel->SetPosition(0.f, m_editPanelRect.y);
		m_GUIWindow_editPanel->SetSize(m_editPanelRect.z, m_editPanelRect.w);
		m_GUIWindow_editPanel->Rebuild();
	}

	m_mainWindow->RebuildGUI();

	if(m_viewport)
		m_viewport->Rebuild();
}

void ModelEditor::_processShortcuts()
{
	if (m_shortcutMgr->IsShortcutActive(CommandID_MainMenuExit))
		OnExit();
	if (m_shortcutMgr->IsShortcutActive(CommandID_ViewportViewPerspective))
		g_app->SetActiveViewportViewType(ViewportView::type_perspective);
	if (m_shortcutMgr->IsShortcutActive(CommandID_ViewportViewBack))
		g_app->SetActiveViewportViewType(ViewportView::type_back);
	if (m_shortcutMgr->IsShortcutActive(CommandID_ViewportViewBottom))
		g_app->SetActiveViewportViewType(ViewportView::type_bottom);
	if (m_shortcutMgr->IsShortcutActive(CommandID_ViewportViewFront))
		g_app->SetActiveViewportViewType(ViewportView::type_front);
	if (m_shortcutMgr->IsShortcutActive(CommandID_ViewportViewLeft))
		g_app->SetActiveViewportViewType(ViewportView::type_left);
	if (m_shortcutMgr->IsShortcutActive(CommandID_ViewportViewRight))
		g_app->SetActiveViewportViewType(ViewportView::type_right);
	if (m_shortcutMgr->IsShortcutActive(CommandID_ViewportViewTop))
		g_app->SetActiveViewportViewType(ViewportView::type_top);
	if (m_shortcutMgr->IsShortcutActive(CommandID_ViewportToggleFullView))
		g_app->ToggleFullView();
	if (m_shortcutMgr->IsShortcutActive(CommandID_ViewportToggleGrid))
		g_app->ToggleGrid();

	//if (m_shortcutMgr->IsShortcutActive(CommandID_MainMenuExit))OnExit();
}

void ModelEditor::_initPlugins()
{
	std::filesystem::create_directory(bqFramework::GetPath("plugins/").c_str());

	for (auto& entry : std::filesystem::directory_iterator(L"plugins/"))
	{
		auto path = entry.path();
		if (!path.has_extension())
			continue;

		auto ex = path.extension();
		if (ex != ".dll")
			continue;

		auto lib_str = path.generic_string();

		auto module = bqDLL::Load(lib_str.c_str());
		if (!module)
			continue;

		bqLog::PrintInfo("Load plugin: %s...\n", lib_str.data());

		bqMECreatePlugin_t CreatePlugin = (bqMECreatePlugin_t)bqDLL::GetProc(module, "MECreatePlugin");
		if (!CreatePlugin)
		{
			bqLog::PrintWarning("FAIL (function %s not found)\n", "MECreatePlugin");
			continue;
		}

		bqMEDestroyPlugin_t DestroyPlugin = (bqMEDestroyPlugin_t)bqDLL::GetProc(module, "MEDestroyPlugin");
		if (!DestroyPlugin)
		{
			bqLog::PrintWarning("FAIL (function %s not found)\n", "MEDestroyPlugin");
			continue;
		}



		auto newPlugin = CreatePlugin(m_sdk);
		if (!newPlugin)
		{
			bqLog::PrintWarning("FAIL (newPlugin is nullptr)\n");
			continue;
		}

		bool isDebug = false;
#ifdef BQ_DEBUG
		isDebug = true;
#endif
		if (newPlugin->IsDebug() && isDebug != true)
		{
			DestroyPlugin(newPlugin);
			bqLog::PrintWarning("FAIL (debug version)\n");
			continue;
		}

		if (newPlugin->CheckVersion() != BQ_ME_SDK_VERSION)
		{
			DestroyPlugin(newPlugin);
			bqLog::PrintWarning("FAIL (bad version)\n");
			continue;
		}
		/*newPlugin->Init(m_sdk);
		bqLog::PrintInfo(L"DONE (%s)\n", newPlugin->GetName());

		plugin_info pi;
		pi.m_plugin = newPlugin;
		pi.m_path = path.filename().generic_string();
		m_plugins.push_back(pi);*/

		PluginInfo pi;
		pi.m_destroyFunction = DestroyPlugin;
		pi.m_plugin = newPlugin;
		m_plugins.push_back(pi);
	}

	/*m_pluginForApp = (miApplicationPlugin*)miMalloc(sizeof(miApplicationPlugin));
	new(m_pluginForApp)miApplicationPlugin();

	plugin_info pi;
	pi.m_plugin = m_pluginForApp;
	pi.m_path = "appplugin";
	m_plugins.push_back(pi);

	m_plugins.shrink_to_fit();*/

}
