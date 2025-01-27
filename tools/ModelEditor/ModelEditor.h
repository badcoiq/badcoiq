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

#include "badcoiq.h"
#ifndef _MODELEDITOR_H_
#define _MODELEDITOR_H_

BQ_LINK_LIBRARY("badcoiq");
#include "badcoiq/system/bqWindow.h"
#include "badcoiq/GUI/bqGUI.h"

#define APP_PRINT_ERROR bqLog::PrintError("%s %s %l\n", BQ_FILE, BQ_FUNCTION, BQ_LINE)


class FrameworkCallback : public bqFrameworkCallback
{
public:
	FrameworkCallback() {}
	virtual ~FrameworkCallback() {}
	virtual void OnMessage() {}
};

class MainWindowCallback : public bqWindowCallback
{
public:
    MainWindowCallback(){}
    virtual ~MainWindowCallback() {}

	virtual void OnClose(bqWindow*) override;
	virtual void OnSize(bqWindow* w) override;
    virtual void OnPopupMenu(bqWindow*, uint32_t /*id*/) override;
};

class DrawTextCallback : public bqGUIDrawTextCallback
{
    bqColor m_color;
    bqGUIFont* m_defaultFont = 0;
public:
    DrawTextCallback() {}
    virtual ~DrawTextCallback() {}

    virtual bqGUIFont* OnFont( char32_t)
    {
        return m_defaultFont;
    }

    virtual bqColor* OnColor( char32_t)
    {
        return &m_color;
    }

    void SetFont(bqGUIFont* f)
    {
        m_defaultFont = f;
    }

    const bqColor& GetColor() { return m_color; }
    void SetColor(const bqColor& c) { m_color = c; }
};

enum
{
    CommandID_MainMenuNew,
    CommandID_MainMenuOpen,
    CommandID_MainMenuSave,
    CommandID_MainMenuSaveAs,
    CommandID_MainMenuSaveCopy,
    CommandID_MainMenuImport,
    CommandID_MainMenuExport,
    CommandID_MainMenuExit,

    CommandID_ViewportViewPerspective,
    CommandID_ViewportViewTop,
    CommandID_ViewportViewBottom,
    CommandID_ViewportViewLeft,
    CommandID_ViewportViewRight,
    CommandID_ViewportViewFront,
    CommandID_ViewportViewBack,
    CommandID_ViewportToggleFullView,
    CommandID_ViewportToggleGrid,
    CommandID_ViewportDrawMaterial,
    CommandID_ViewportDrawMaterialWireframe,
    CommandID_ViewportDrawWireframe,
    CommandID_ViewportToggleDrawMaterial,
    CommandID_ViewportToggleDrawWireframe,
    CommandID_ViewportToggleDrawAABB,
    CommandID_CameraReset,
    CommandID_CameraMoveToSelection,
    /*CommandID_,
    CommandID_,
    CommandID_,
    CommandID_,
    CommandID_,
    CommandID_,
    CommandID_,
    CommandID_,*/
};

class GUIButton;
class ModelEditor
{
    friend class Viewport;
    friend class ViewportLayout;
    friend class ViewportView;

	FrameworkCallback m_frameworkCallback;
    MainWindowCallback m_mainWindowCallback;
    DrawTextCallback m_drawTextCallback;

	bqGS* m_gs = 0;
    float32_t* m_deltaTime = 0;

    bqInputData* m_inputData = 0;

	bool m_run = true;

    bqGUIStyle m_GUIStyle;
    bqTexture* m_GUITexture = 0;
    bqGUIWindow* m_GUIWindow_mainMenuBar = 0;
    bqArray<bqGUIElement*> m_GUIElements;
	void Shutdown();
    void GUI_rebuild();
    bqGUIButton* GUI_createButton(const bqVec2f& position, const bqVec2f& size, uint32_t id);
    bqGUIPictureBox* GUI_createPictureBox(const bqVec2f& position, const bqVec2f& size, uint32_t id);

    bqShortcutManager* m_shortcutMgr = 0;
    void _processShortcuts();

    bqVec4f m_mainMenuBarRect;
    bqVec4f m_editPanelRect;
    float32_t m_editPanelWidth = 150.f;
    const float32_t m_editPanelWidthMin = 50.f;

    Viewport* m_viewport = 0;
    bqGPUMesh* m_gridModel_perspective1 = 0;
    bqGPUMesh* m_gridModel_perspective2 = 0;
    bqGPUMesh* m_gridModel_top = 0;
    bqGPUMesh* m_gridModel_front = 0;
    bqGPUMesh* m_gridModel_left = 0;
    void _initGrid();
public:
	ModelEditor();
	~ModelEditor();

	bool Init();
	void Run();

	void OnWindowCallback_onClose(bqWindow*);
	void OnWindowCallback_onSize(bqWindow*);
    void OnWindowCallback_onPopupMenu(bqWindow* w, uint32_t id);
    void OnExit();

    void SetActiveViewportViewType(uint32_t);

    bqPopup* m_popupViewportOptions = 0;
    bqPopup* m_popupMainMenuOptions = 0;
    bqWindow* m_mainWindow = 0;
};

#endif
