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
#ifndef __BQ_GS_H__
#define __BQ_GS_H__

#ifdef BQ_WITH_GS

#include "badcoiq/string/bqString.h"
#include "badcoiq/common/bqColor.h"
#include "badcoiq/gs/bqShader.h"
#include "badcoiq/gs/bqGPUMesh.h"
#include "badcoiq/gs/bqMaterial.h"
#include "badcoiq/gs/bqTexture.h"

// Используется при рисовании
enum class bqGSRasterizerState
{
	SolidCull,     // обычная растеризация + не рисовать заднюю сторону треугольника
	WireframeCull, // сетка + не рисовать заднюю сторону треугольника

	// то же самое только задняя сторона рисуется
	Solid,
	Wireframe
};


class bqGPUOcclusionObject
{
public:
	bqGPUOcclusionObject() {}
	virtual ~bqGPUOcclusionObject() {}
	BQ_PLACEMENT_ALLOCATOR(bqGPUOcclusionObject);

	bool m_visible = true;
};

/// \brief Графическая система
class bqGS
{
public:
	bqGS() {}
	virtual ~bqGS() {}

	// Инициализировать GPU вещи
	// `parameters` опциональны, зависит от запускаемой библиотеки
	virtual bool Init(bqWindow*, const char* parameters) = 0;
	virtual void Shutdown() = 0;

	/// Получить имя
	virtual bqString GetName() = 0;
	/// Получить остальную информацию в виде текста
	virtual bqString GetTextInfo() = 0;
	/// Получить ID
	virtual bqUID GetUID() = 0;

	/// Цвет фона
	virtual void SetClearColor(float r, float g, float b, float a) = 0;

	/// Вызвать перед рисованием
	virtual void BeginDraw() = 0;
	/// Очистить буфер с глубиной
	virtual void ClearDepth() = 0;
	/// Очистить фон (закрасить фон)
	virtual void ClearColor() = 0;
	/// ClearDepth и ClearColor
	virtual void ClearAll() = 0;
	/// Вызвать после завершения рисования
	virtual void EndDraw() = 0;
	/// Вызвать после EndDraw
	virtual void SwapBuffers() = 0;

	/// Нарисовать линию в 3D.
	/// Обязательно нужно указать на ViewProjection матрицу
	virtual void DrawLine3D(const bqVec3& p1, const bqVec3& p2, const bqColor& c) = 0;

	/// Установить текущий шейдер.
	/// Если bqShaderType::User то необходимо указать индекс шейдера
	virtual void SetShader(bqShaderType, uint32_t userShaderIndex) = 0;

#ifdef BQ_WITH_MESH
	/// Создать GPU модель для рисования
	virtual bqGPUMesh* CreateMesh(bqMesh* m) = 0;

	/// Установить текущую модель
	virtual void SetMesh(bqGPUMesh* m) = 0;
#endif

	/// Установить текущий материал
	virtual void SetMaterial(bqMaterial* m) = 0;

	/// Установить как рисовать треугольники
	virtual void SetRasterizerState(bqGSRasterizerState) = 0;

	/// Нарисовать
	virtual void Draw() = 0;

#ifdef BQ_WITH_IMAGE
	
	/// Создать текстуру
	virtual bqTexture* CreateTexture(bqImage*, const bqTextureInfo&) = 0;
	virtual bqTexture* CreateTexture(bqImage* i)
	{
		bqTextureInfo ti;
		return CreateTexture(i, ti);
	}

	/// Создать Render Target Texture/Frame Buffer Object
	virtual bqTexture* CreateRTT(const bqPoint& size, const bqTextureInfo&) = 0;

	/// Установить текстуру для рисования.
	virtual void SetRenderTarget(bqTexture* t) = 0;

#endif
	virtual void SetRenderTargetDefault() = 0;

	virtual void UseVSync(bool) = 0;
	virtual void EnableDepth() = 0;
	virtual void DisableDepth() = 0;
	virtual void EnableBlend() = 0;
	virtual void DisableBlend() = 0;

	virtual bqVec2f GetDepthRange() = 0;
	virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
	virtual void SetScissorRect(const bqRect&) = 0;
	virtual void SetScissorRect(const bqVec4f&) = 0;

	/// Надо вызвать когда был изменён размер окна
	/// Будет вычислена матрица для GUI, и обновятся буферы у SwapChain
	virtual void OnWindowSize() = 0;

	virtual void SetMainTargetSize(const bqPoint&) = 0;


	virtual void EnableBackFaceCulling() = 0;
	virtual void DisableBackFaceCulling() = 0;

#ifdef BQ_WITH_SPRITE
	virtual void DrawSprite(bqSprite*) = 0;
#endif

#ifdef BQ_WITH_GUI
	virtual void BeginGUI(bool clear = true) = 0;
	virtual void EndGUI() = 0;
	virtual void DrawGUIRectangle(const bqVec4f& rect, const bqColor& color1, const bqColor& color2,
		bqTexture* t, bqVec4f* UVs) = 0;
	virtual void DrawGUIText(const char32_t* text, uint32_t textSz, const bqVec2f& position,
		bqGUIDrawTextCallback*) = 0;
#ifdef BQ_WITH_SPRITE
	// textSizeInPixels - опционально. Он определяет точку вокруг которой будет вращаться текст.
	//                    чисто технически (по умолчанию), текст крутится так как будто точка
	//                    находится в левом нижнем углу. Решение - надо просто сдвинуть текст
	//                    на половину длинны. Постоянно вычислять длинну внутри метода думаю
	//                    не верно. Проще указать эту длинну вручную. bqGUIFont должен иметь метод
	//                    который вернёт длинну в пикселях. Если метода нет то его надо написать.
	virtual void DrawText3D(const bqVec3& pos, const char32_t* text, size_t textLen,
		bqGUIFont* font, const bqColor& color, float sizeMultipler, size_t textSizeInPixels) = 0;
#endif
#endif

	// 
	virtual bqGPUOcclusionObject* SummonOcclusionObject() = 0;
	
	/// Все действия связанные с Occlusion culling надо делать
	///  между вызовами этих методов
	virtual void OcclusionBegin() = 0;
	virtual void OcclusionEnd() = 0;
	
	/// Перед рисованием надо установить модель используя SetMesh
	virtual void OcclusionDraw(bqGPUOcclusionObject*) = 0;
	virtual void OcclusionResult(bqGPUOcclusionObject*) = 0;

};

#endif
#endif

