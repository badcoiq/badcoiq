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
#include "badcoiq/gs/bqGS.h"
#include "badcoiq/framework/bqPopupData.h"

int main(int argc, char* argv[])
{
	ModelEditor app;
	if (app.Init())
	{
		app.Run();
	}

	return EXIT_SUCCESS;
}

ModelEditor::ModelEditor()
{
}

ModelEditor::~ModelEditor()
{
	Shutdown();
}

bool ModelEditor::Init()
{
	bqFramework::Start(&m_frameworkCallback);
	m_deltaTime = bqFramework::GetDeltaTime();

	m_mainWindowCallback.SetUserData(this);
	m_mainWindow = bqFramework::SummonWindow(&m_mainWindowCallback);
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

	m_gs = bqFramework::SummonGS(bqFramework::GetGSUID(0));
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
	
	m_gs->SetClearColor(0.3f, 0.3f, 0.3f, 1.f);

	bqFramework::InitDefaultFonts(m_gs);
	m_drawTextCallback.SetFont(bqFramework::GetDefaultFont(bqGUIDefaultFont::Text));

	return true;
}

void ModelEditor::Run()
{
	while (m_run)
	{
		bqFramework::Update();
		bqFramework::UpdateGUI();
		m_gs->BeginGUI();
		m_gs->DrawGUIRectangle(bqVec4f(0.f, 0.f, 100.f, 20.f), bq::ColorRed, bq::ColorYellow, 0, 0);
		m_gs->DrawGUIText(U"Hello!!!", 9, bqVec2f(10.f), &m_drawTextCallback);

		bqFramework::DrawGUI(m_gs);
		m_gs->EndGUI();
		m_gs->EndDraw();
		m_gs->SwapBuffers();
	}
}

void ModelEditor::Shutdown()
{
}

void MainWindowCallback::OnClose(bqWindow* w)
{
	ModelEditor* editor = (ModelEditor*)(GetUserData());
	if (editor)
		editor->OnWindowCallback_onClose(w);
}

void MainWindowCallback::OnSize(bqWindow* w)
{
	ModelEditor* editor = (ModelEditor*)(GetUserData());
	if (editor)
		editor->OnWindowCallback_onSize(w);
}

void ModelEditor::OnWindowCallback_onClose(bqWindow* w)
{
	m_run = false;
}

void ModelEditor::OnWindowCallback_onSize(bqWindow* w)
{
	if (m_gs)
		m_gs->OnWindowSize();
}
