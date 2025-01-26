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
#include "badcoiq/system/bqPopup.h"
#include "badcoiq/framework/bqShortcutManager.h"
#include "badcoiq/scene/bqCamera.h"

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

	m_GUITexture = bqFramework::CreateTexture(m_gs,
		bqFramework::GetPath("../data/model_editor/gui.png").c_str(),
		false, false);
	
	m_gs->SetClearColor(0.89f, 0.89f, 0.89f, 1.f);

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
	//m_GUIWindow_mainMenuBar->m_windowFlags |= bqGUIWindowBase::windowFlag_withTitleBar;
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

	_initGrid();

	m_viewport = new Viewport;

	GUI_rebuild();

	m_popupViewportOptions = bqFramework::CreatePopup();
	m_popupViewportOptions->AddItem(L"Perspective", CommandID_ViewportViewPerspective, L"");
	m_popupViewportOptions->AddItem(L"Top", CommandID_ViewportViewTop, L"");
	m_popupViewportOptions->AddItem(L"Bottom", CommandID_ViewportViewBottom, L"");
	m_popupViewportOptions->AddItem(L"Left", CommandID_ViewportViewLeft, L"");
	m_popupViewportOptions->AddItem(L"Right", CommandID_ViewportViewRight, L"");
	m_popupViewportOptions->AddItem(L"Front", CommandID_ViewportViewFront, L"");
	m_popupViewportOptions->AddItem(L"Back", CommandID_ViewportViewBack, L"");
	m_popupViewportOptions->AddSeparator();
	m_popupViewportOptions->AddItem(L"Toggle full view", CommandID_ViewportToggleFullView, L"");
	m_popupViewportOptions->AddItem(L"Toggle grid", CommandID_ViewportToggleGrid, L"");
	m_popupViewportOptions->AddSeparator();
	m_popupViewportOptions->AddItem(L"Material", CommandID_ViewportDrawMaterial, L"");
	m_popupViewportOptions->AddItem(L"Material+Wireframe", CommandID_ViewportDrawMaterialWireframe, L"");
	m_popupViewportOptions->AddItem(L"Wireframe", CommandID_ViewportDrawWireframe, L"");
	m_popupViewportOptions->AddItem(L"Toggle draw material", CommandID_ViewportToggleDrawMaterial, L"");
	m_popupViewportOptions->AddItem(L"Toggle draw wireframe", CommandID_ViewportToggleDrawWireframe, L"");
	m_popupViewportOptions->AddSeparator();
	m_popupViewportOptions->AddItem(L"Toggle draw AABB", CommandID_ViewportToggleDrawAABB, L"");
	m_popupViewportOptions->AddSeparator();
	m_popupViewportOptions->AddItem(L"Camera Reset", CommandID_CameraReset, L"");
	m_popupViewportOptions->AddItem(L"Camera Move to selection", CommandID_CameraMoveToSelection, L"");

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

		_processShortcuts();
		m_viewport->Update();
		m_viewport->Draw();

		m_gs->BeginGUI(false);
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
	}
}

void ModelEditor::OnExit()
{
	m_run = false;
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
		m_GUIWindow_mainMenuBar->Rebuild();
	}

	m_editPanelRect.x = 0.f;
	m_editPanelRect.y = m_mainMenuBarRect.w;
	m_editPanelRect.z = m_editPanelWidth;
	m_editPanelRect.w = m_mainWindow->GetCurrentSize()->y;

	if(m_viewport)
		m_viewport->Rebuild();
}

void ModelEditor::_processShortcuts()
{
	if (m_shortcutMgr->IsShortcutActive(CommandID_MainMenuExit))OnExit();
	//if (m_shortcutMgr->IsShortcutActive(CommandID_MainMenuExit))OnExit();
}

