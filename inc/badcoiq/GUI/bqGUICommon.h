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
#ifndef __BQ_GUICMN_H__
#define __BQ_GUICMN_H__


// Данные которые есть и у GUI элементов и у GUI окна
class bqGUICommon : public bqUserData, public bqHierarchy
{
public:
	enum : uint32_t
	{
		flag_visible = 0x1,
		flag_enabled = 0x2,
		flag_drawBG = 0x4,

		// для внутреннего использования. можно читать.
		flag_wheelScroll = 0x2000000,
		flag_clickedX2MB = 0x4000000,
		flag_clickedX1MB = 0x8000000,
		flag_clickedMMB = 0x10000000,
		flag_clickedRMB = 0x20000000,
		flag_clickedLMB = 0x40000000,
		flag_cursorInRect = 0x80000000,
	};

protected:
	bqGUIStyle* m_style = 0;
	bqGUIDrawTextCallback* m_textDrawCallback = 0;

	uint32_t m_flags = flag_visible | flag_enabled | flag_drawBG;

	bqVec2f m_position;
	bqVec2f m_size;
	bqVec4f m_buildRectOnCreation;

public:
	bqGUICommon(const bqVec2f& position, const bqVec2f& size);
	virtual ~bqGUICommon();
	BQ_PLACEMENT_ALLOCATOR(bqGUICommon);

	const bqVec4f& GetBuildRectOnCreation() { return m_buildRectOnCreation; }
	void SetBuildRectOnCreation(const bqVec4f& r) { m_buildRectOnCreation = r; }

	virtual void SetDrawTextCallback(bqGUIDrawTextCallback* cb) { m_textDrawCallback = cb; }

	virtual void SetStyle(bqGUIStyle* s)
	{
		s ? m_style = s : m_style = bqFramework::GetGUIStyle(bqGUIStyleTheme::Light);
	}

	virtual bqGUIStyle* GetStyle()
	{
		return m_style;
	}

	virtual const bqVec2f& GetSize()
	{
		return m_size;
	}

	virtual const bqVec2f& GetPosition()
	{
		return m_position;
	}

	// Установить видимость. Если элемент не видим, то обработка и рисование потомков будет пропущено
	virtual void SetVisible(bool v) {
		if (v)
			m_flags |= bqGUICommon::flag_visible;
		else
			m_flags &= ~bqGUICommon::flag_visible;
	}
	virtual bool IsVisible() { return (m_flags & flag_visible); }

	virtual void SetEnabled(bool v) {
		if (v)
			m_flags |= bqGUICommon::flag_enabled;
		else
			m_flags &= ~bqGUICommon::flag_enabled;
	}
	virtual bool IsEnabled() { return (m_flags & flag_enabled); }

	virtual bool IsCursorInRect() { return (m_flags & flag_cursorInRect); }
	virtual bool IsClickedLMB() { return (m_flags & flag_clickedLMB); }
	virtual bool IsClickedRMB() { return (m_flags & flag_clickedRMB); }
	virtual bool IsClickedMMB() { return (m_flags & flag_clickedMMB); }
	virtual bool IsClickedX1MB() { return (m_flags & flag_clickedX1MB); }
	virtual bool IsClickedX2MB() { return (m_flags & flag_clickedX2MB); }

	virtual void SetDrawBG(bool v) {
		if (v)
			m_flags |= bqGUICommon::flag_drawBG;
		else
			m_flags &= ~bqGUICommon::flag_drawBG;
	}
	virtual bool IsDrawBG() { return (m_flags & flag_drawBG); }

	uint32_t m_id = 0;

	// Вычислить m_baseRect и прочие ...Rect
	// Надо вызывать это после создания GUI и после того как размер окна изменится
	virtual void Rebuild() = 0;

	// Нарисовать
	virtual void Draw(bqGS* gs, float dt) = 0;

	// Работа с определением была ли нажатие мышкой и т.д.
	// Базовые вещи (напр. курсор в области элемента/окна) корректны и для элементов и для окна
	virtual void Update();

	// Коллбэки
	virtual void OnMouseEnter();
	virtual void OnMouseLeave();
	virtual void OnClickLMB();
	virtual void OnClickRMB();
	virtual void OnClickMMB();
	virtual void OnClickX1MB();
	virtual void OnClickX2MB();
	virtual void OnReleaseLMB();
	virtual void OnReleaseRMB();
	virtual void OnReleaseMMB();
	virtual void OnReleaseX1MB();
	virtual void OnReleaseX2MB();

	bqVec4f m_baseRect; // Вычисляется в Rebuild. Позиция плюс размер (с учётом родителя)
	bqVec4f m_buildRect;  // Рисуется на основе этого.
	bqVec4f m_clipRect;   // Если элемент ушёл за пределы окна или ещё чего, то не нужно рисовать
	                      //   то что ушло за пределы. Это m_buildRect с учётом некоторых факторов.
	bqVec4f m_activeRect; // Область в которой эелемент/окно реагирует на курсор мышки

	// для скроллинга (прокрутка, когда крутишь колесо мышки)
	bqVec2f m_scroll = 0.f;
	bqVec2f m_scrollOld = 0.f;
	bqVec2f m_scrollDelta = 0.f;
	bqVec2f m_scrollTarget = 0.f; // для lerp
	bqVec2f m_scrollLimit = 0.f;  // лимит. надо знать когда прекращать прокрутку

	// Размер элемента + все дети (не внуки). Типа AABB. Найти максимумы x и y.
	// размер окна со всеми элементами.
	// bqVec2f используется потому что скроллинг использует bqVec2f
	bqVec2f m_contentSize;

	// Вычислить размер содержимого
	virtual void UpdateContentSize();

	// Вычислить лимиты для скроллинга
	virtual void UpdateScrollLimit();

	// 
	virtual void UpdateScroll(); // должно вызываться от от правнуков до прародителей
};

#endif

