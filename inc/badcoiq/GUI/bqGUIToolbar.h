/*
BSD 2-Clause License

Copyright (c) 2025, badcoiq
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
#ifndef __BQ_TOOLBAR_H__
/// \cond
#define __BQ_TOOLBAR_H__
/// \endcond
#ifdef BQ_WITH_GUI

/// \brief Полоска с кнопками
class bqGUIToolbar : public bqGUIElement
{
	struct node
	{
		bqGUIElement* m_element = 0;
		bool m_ownElement = false;
	};
	bqArray<node> m_nodes;
	float32_t m_buttonSize = 0.f;
	float32_t m_addPosition = 0.f;
public:
	bqGUIToolbar(const bqVec2f& position, const bqVec2f& size, float32_t btnSz);
	virtual ~bqGUIToolbar();
	BQ_PLACEMENT_ALLOCATOR(bqGUIToolbar);
	BQ_DELETED_METHODS(bqGUIToolbar);

	virtual void Rebuild() override;
	virtual void Update() override;
	virtual void Draw(bqGS* gs, float dt) override;

	virtual void OnButton(uint32_t id, bqGUIButton*) = 0;


	float32_t m_elementsDistance = 3.f;
	bqVec2f m_indent = bqVec2f(3.f, 3.f);

	void AddButton(uint32_t id, uint32_t iconID);
	void AddSeparator();
	void AddGUIElement(bqGUIElement*);
	void RemoveAllElements();
};

#endif
#endif
