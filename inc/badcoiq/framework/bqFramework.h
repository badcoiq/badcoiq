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
#ifndef __BQ_FRAMEWORK_H__
#define __BQ_FRAMEWORK_H__

#include "badcoiq/common/bqUID.h"

template<class T>
const T& bqMax(const T& a, const T& b)
{
	return (a < b) ? b : a;
}
template<class T>
const T& bqMin(const T& a, const T& b)
{
	return (b < a) ? b : a;
}

enum class bqMatrixType : uint32_t
{
	// Это матрица описывающая трансформацию объекта
	World,

	// Эти матрицы вычисляются в классе камеры
	View,           // так называемая видовая. Матрица вращения которая крутит мир вокруг зрителя.
	Projection,     // проекционная. в ней настраиваются FOV. Может быть без перспективы вообще (ортогоналная проекция)
	ViewProjection, // Для 3D линии. View * Projection
	ViewInvert,     // Инвертированная View

	WorldViewProjection, // Как ViewProjection только ещё учитывается World. Для 3D объектов.

	_count
};

// Предполагаю что фреймворк должен посылать какие-то сообщения.
// Я это ещё не реализовал
class bqFrameworkCallback
{
public:
	bqFrameworkCallback() {}
	virtual ~bqFrameworkCallback() {}

	virtual void OnMessage() = 0;
};

// API для фреймворка.
class bqFramework
{
public:
	// Когда нам нужно запустить ДВИЖЁК, вызываем Start
	// Он выделит память для внутреннего класса
	static void Start(bqFrameworkCallback*);

	// для завершения работы, он освободит ту память
	static void Stop();

	// Например, он вычислит DeltaTime, обновит состояния ввода, GUI, окна.
	static void Update();

	static float* GetDeltaTime();

	// Создать окно
	static bqWindow* SummonWindow(bqWindowCallback*);

	// Получить количество реализаций графических систем
	static uint32_t GetGSNum();
	// Получить имя реализации графической стсиемы. Надо указать индекс.
	static bqString GetGSName(uint32_t);
	// Получить ID реализации графической системы по индексу
	static bqUID GetGSUID(uint32_t);
	// Получить графическую систему указав ID
	static bqGS* SummonGS(bqUID);
	// Получить графическую систему указав имя
	static bqGS* SummonGS(const char*);
	// Получить графическую систему указав имя и ID
	static bqGS* SummonGS(bqUID, const char*);

	// Сравнить UID
	static bool CompareUIDs(const bqUID&, const bqUID&);

	// Получить указатель на матрицу
	static bqMat4* GetMatrix(bqMatrixType);

	// Установить указатель на матрицу
	static void SetMatrix(bqMatrixType, bqMat4*);

	// Прочитать файл в буфер
	// если isText будут добавлены ' ' и 0 в конец
	// Функция выделит память для буфера. Использовать bqMemory::free для освобождения.
	static uint8_t* SummonFileBuffer(const char* path, uint32_t* szOut, bool isText);

	static uint32_t GetImageLoadersNum();
	static bqImageLoader* GetImageLoader(uint32_t);
	static bqImage* SummonImage(const char*);

	static bool FileExist(const char*);
	static bool FileExist(const bqString&);

	static uint64_t FileSize(const char*);
	static uint64_t FileSize(const bqString&);

	static bqPolygonMesh* SummonPolygonMesh();

	static uint32_t GetMeshLoadersNum();
	static bqMeshLoader* GetMeshLoader(uint32_t);
	static void SummonMesh(const char*, bqMeshLoaderCallback*);

	static bqString GetAppPath();
	static bqStringA GetPath(const bqString& v);

	static void InitDefaultFonts(bqGS* gs);
	static bqGUIFont* SummonFont();
	static bqGUIFont* GetDefaultFont(uint32_t index);

	static bqGUIStyle* GetGUIStyle(bqGUIStyleTheme);
};

#endif

