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
#ifndef __BQ_GUIWINDOW_H__
#define __BQ_GUIWINDOW_H__

class bqGUIWindow : public bqGUICommon
{
public:
	enum
	{
		windowFlag_withCloseButton = 0x1,
		windowFlag_withCollapseButton = 0x2,
		windowFlag_withTitleBar = 0x4,
		windowFlag_canMove = 0x8,
		windowFlag_canResize = 0x10,
		windowFlag_canDock = 0x20,
		windowFlag_canToTop = 0x40,
	};

private:
	bqGUIElement* m_rootElement = 0;

	bqVec2f m_sizeMinimum = bqVec2f(100.f, 30.f);

	bqString m_title;
	uint32_t m_windowFlags = 0;
public:
	bqGUIWindow(const bqVec2f& position, const bqVec2f& size);
	virtual ~bqGUIWindow();
	BQ_PLACEMENT_ALLOCATOR(bqGUIWindow);

	// Надо будет вызвать Rebuild
	void SetPositionAndSize(const bqVec2f& p, const bqVec2f& sz);

	void SetSizeMinimum(const bqVec2f& sz) { m_sizeMinimum = sz; }

	bqGUIElement* GetRootElement() { return m_rootElement; }

	virtual void Rebuild() override;
	virtual void Update() override;
	virtual void Draw(bqGS* gs, float dt) override;

	void SetTitle(const char32_t*);
};


#endif

