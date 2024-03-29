﻿/*
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
#ifndef _EXmshgnbox_H_
#define _EXmshgnbox_H_

class TestGUIScrollbar;
class ExampleMshGnBox_WindowCallback : public bqGUIWindowCallback
{
public:
	ExampleMshGnBox_WindowCallback() {}
	virtual ~ExampleMshGnBox_WindowCallback() {}
	virtual void OnSizeChange(bqGUIWindow*) override;

	TestGUIScrollbar* m_scrollbar = 0;
};

class DemoExample;
class DemoApp;
class ExampleMshGnBox : public DemoExample
{
	bqCamera* m_camera = 0;
	bqGPUMesh* m_meshBox = 0;
	bqMat4 m_worldBox, m_wvp;

	ExampleMshGnBox_WindowCallback m_guiWindowCallback;
	bqGUIWindow* m_guiWindow = 0;
public:
	ExampleMshGnBox(DemoApp*);
	virtual ~ExampleMshGnBox();
	BQ_PLACEMENT_ALLOCATOR(ExampleMshGnBox);

	virtual bool Init() override;
	virtual void Shutdown() override;
	virtual void OnDraw() override;
};

#endif
