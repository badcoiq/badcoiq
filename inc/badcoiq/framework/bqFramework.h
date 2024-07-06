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
	BQ_PLACEMENT_ALLOCATOR(bqFrameworkCallback);

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

#ifdef BQ_WITH_WINDOW
	// Создать окно
	static bqWindow* SummonWindow(bqWindowCallback*);
#endif

#ifdef BQ_WITH_GS
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
#endif

	// Сравнить UID
	static bool CompareUIDs(const bqUID&, const bqUID&);

	// Получить указатель на матрицу
	static bqMat4* GetMatrix(bqMatrixType);

#ifdef BQ_WITH_MESH
	// Получить указатель на первую из 255 матриц для скелета
	static bqMat4* GetMatrixSkinned();
	static bqPolygonMesh* SummonPolygonMesh();
	static uint32_t GetMeshLoadersNum();
	static bqMeshLoader* GetMeshLoader(uint32_t);
	static void SummonMesh(const char*, bqMeshLoaderCallback*);
#endif

	// Установить указатель на матрицу
	static void SetMatrix(bqMatrixType, bqMat4*);

	// Прочитать файл в буфер
	// если isText будут добавлены ' ' и 0 в конец
	// Функция выделит память для буфера. Использовать bqMemory::free для освобождения.
	static uint8_t* SummonFileBuffer(const char* path, uint32_t* szOut, bool isText);

#ifdef BQ_WITH_IMAGE
	static uint32_t GetImageLoadersNum();
	static bqImageLoader* GetImageLoader(uint32_t);
	static bqImage* SummonImage(const char*);
#endif

#ifdef BQ_WITH_GS
	// Метод сам загрузит bqImage, создаст текстуру и удалит bqImage
	static bqTexture* SummonTexture(bqGS*, const char*, bool genMipMaps = true, bool linearFilter = true);
#endif

	static bool FileExist(const char*);
	static bool FileExist(const bqString&);

	static uint64_t FileSize(const char*);
	static uint64_t FileSize(const bqString&);

	static bqString GetAppPath();
	static bqStringA GetAppPathA();

	// Получить путь к файлу относительно программы
	// Если файла нет, то путь урежится, и его можно
	// будет использовать в поиске файла в архиве.
	// Например, путь ../data/img/1.png
	// Если файла нет то вернётся
	// data/img/1.png
	static bqStringA GetPath(const bqString& v);

#ifdef BQ_WITH_GUI
	static void InitDefaultFonts(bqGS* gs);
	static bqGUIFont* SummonFont();
	static bqGUIFont* GetDefaultFont(bqGUIDefaultFont);
	static bqGUIStyle* GetGUIStyle(bqGUIStyleTheme);
	static bqGUIWindow* SummonGUIWindow(bqWindow* window, const bqVec2f& position, const bqVec2f& size);
	static void UpdateGUI();
	static void DrawGUI(bqGS* gs);
	static void RebuildGUI();
	static void Destroy(bqGUIWindow*);
	static void Destroy(bqGUIElement*);
#endif

	static bqCursor* SummonCursor(const char* fn);
	static bqCursor* GetDefaultCursor(bqCursorType);
	static bqCursor* GetActiveCursor();
	static void SetActiveCursor(bqCursor*);

#ifdef BQ_WITH_SOUND
	static bqSoundSystem* GetSoundSystem();
#endif

#ifdef BQ_WITH_PHYSICS
	static bqPhysics* GetPhysicsSystem();
#endif

	static bqVec2i GetDesktopWindowSize();
};

#endif

