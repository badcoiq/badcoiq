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
#ifndef __BQ_GS_H__
#define __BQ_GS_H__

#include "badcoiq/string/bqString.h"
#include "badcoiq/common/bqColor.h"
#include "badcoiq/gs/bqShader.h"
#include "badcoiq/gs/bqGPUMesh.h"
#include "badcoiq/gs/bqMaterial.h"

// Используется при рисовании
enum class bqGSRasterizerState
{
	SolidCull,     // обычная растеризация + не рисовать заднюю сторону треугольника
	WireframeCull, // сетка + не рисовать заднюю сторону треугольника

	// то же самое только задняя сторона рисуется
	Solid,
	Wireframe
};

// Графическая система
class bqGS
{
public:
	bqGS() {}
	virtual ~bqGS() {}

	// Инициализировать GPU вещи
	// `parameters` опциональны, зависит от запускаемой библиотеки
	virtual bool Init(bqWindow*, const char* parameters) = 0;
	virtual void Shutdown() = 0;

	// Инициализировать больше окон. 
	virtual bool InitWindow(bqWindow*) = 0;
	// Чтобы рисовать во множество окон. Пока не реализовано.
	virtual void SetActiveWindow(bqWindow*) = 0;

	// Получить имя
	virtual bqString GetName() = 0;
	// Получить остальную информацию в виде текста
	virtual bqString GetTextInfo() = 0;
	// Получить ID
	virtual bqUID GetUID() = 0;

	// Цвет фона
	virtual void SetClearColor(float r, float g, float b, float a) = 0;

	// Вызвать перед рисованием
	virtual void BeginDraw() = 0;
	// Очистить буфер с глубиной
	virtual void ClearDepth() = 0;
	// Очистить фон (закрасить фон)
	virtual void ClearColor() = 0;
	// ClearDepth и ClearColor
	virtual void ClearAll() = 0;
	// Вызвать после завершения рисования
	virtual void EndDraw() = 0;
	// Вызвать после EndDraw
	virtual void SwapBuffers() = 0;

	// Нарисовать линию в 3D.
	// Обязательно нужно указать на ViewProjection матрицу
	virtual void DrawLine3D(const bqVec4& p1, const bqVec4& p2, const bqColor& c) = 0;

	// Установить текущий шейдер.
	// Если bqShaderType::User то необходимо указать индекс шейдера
	virtual void SetShader(bqShaderType, uint32_t userShaderIndex) = 0;

	virtual bqGPUMesh* SummonMesh(bqMesh* m) = 0;

	virtual void SetMesh(bqGPUMesh* m) = 0;
	virtual void SetMaterial(bqMaterial* m) = 0;
	virtual void Draw() = 0;

	virtual void SetRasterizerState(bqGSRasterizerState) = 0;
};

#endif

