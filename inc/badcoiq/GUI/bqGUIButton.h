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

#pragma once
#ifndef __BQ_GUIBTN_H__
#define __BQ_GUIBTN_H__
#ifdef BQ_WITH_GUI

// У всех элементов есть указатель на bqGUIDrawTextCallback
// Фреймворк должен иметь дефолтный bqGUIDrawTextCallback (возможно отдельные на каждый тип элемента)
// При создании нового элемента, будет установлен дефолтный m_textDrawCallback (bqGUICommon)

// кнопка
class bqGUIButton : public bqGUIElement
{
protected:
	bqString m_text;  // Этот текст рисуется
	bqVec2f m_textPosition; // на такой позиции
	void UpdateTextPosition(); // надо найти эту позицию

	bqTexture* m_texture = 0;
	bqVec4f m_uv = bqVec4f(0.f, 0.f, 1.f, 1.f);
public:
	bqGUIButton(bqGUIWindow*, const bqVec2f& position, const bqVec2f& size);
	virtual ~bqGUIButton();
	BQ_PLACEMENT_ALLOCATOR(bqGUIButton);
	BQ_DELETED_METHODS(bqGUIButton);


	virtual void Rebuild() override;
	virtual void Update() override;
	virtual void Draw(bqGS* gs, float dt) override;

	Alignment m_textAlign = Alignment::Center;

	virtual void SetText(const bqString&);

	void SetTexture(bqTexture* t);
	void SetUV(const bqVec4f&);
	void SetTCoords(float left, float top, float right, float bottom);
};

#endif
#endif

