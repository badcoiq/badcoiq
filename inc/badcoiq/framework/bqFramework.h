﻿/*
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
/// \cond
#define __BQ_FRAMEWORK_H__
/// \endcond

#include "badcoiq/common/bqUID.h"
#include "badcoiq/containers/bqArray.h"

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

/// Указатель на матрицу хранится внутри фреймворка.
/// Установка и получения указателей происходит
/// с указанием типа.
enum class bqMatrixType : uint32_t
{
	/// Это матрица описывающая трансформацию объекта
	World,

	/// Эти матрицы вычисляются в классе камеры
	View,           /// так называемая видовая. Матрица вращения которая крутит мир вокруг зрителя.
	Projection,     /// проекционная. в ней настраиваются FOV. Может быть без перспективы вообще (ортогоналная проекция)
	ViewProjection, /// Для 3D линии. View * Projection
	ViewInvert,     /// Инвертированная View

	WorldViewProjection, /// Как ViewProjection только ещё учитывается World. Для 3D объектов.

	_count
};

/// Предполагаю что фреймворк должен посылать какие-то сообщения.
/// Я это ещё не реализовал
class bqFrameworkCallback
{
public:
	bqFrameworkCallback() {}
	virtual ~bqFrameworkCallback() {}
	BQ_PLACEMENT_ALLOCATOR(bqFrameworkCallback);

	virtual void OnMessage() = 0;
};

/// API для фреймворка.
class bqFramework
{
public:

	/// Когда нам нужно запустить ДВИЖЁК, вызываем Start
	/// Он выделит память для внутреннего класса
	static void Start(bqFrameworkCallback*);

	/// для завершения работы, он освободит ту память
	static void Stop();

	/// Например, он вычислит DeltaTime, обновит состояния ввода, GUI, окна.
	static void Update();

	/// Получить время затраченное между вызовами Update
	static float32_t* GetDeltaTime();

#ifdef BQ_WITH_WINDOW
	/// Создать окно
	static bqWindow* CreateSystemWindow(bqWindowCallback*);
#endif

#ifdef BQ_WITH_GS
	/// Получить количество реализаций графических систем
	static uint32_t GetGSNum();
	
	/// Получить имя реализации графической стсиемы. Надо указать индекс.
	static bqString GetGSName(uint32_t);
	
	/// Получить ID реализации графической системы по индексу
	static bqUID GetGSUID(uint32_t);
	
	/// Получить графическую систему указав ID
	static bqGS* CreateGS(bqUID);
	
	/// Получить графическую систему указав имя
	static bqGS* CreateGS(const char*);
	
	/// Получить графическую систему указав имя и ID
	static bqGS* CreateGS(bqUID, const char*);
#endif

	/// Сравнить UID
	static bool CompareUIDs(const bqUID&, const bqUID&);

	/// Получить указатель на матрицу
	static bqMat4* GetMatrix(bqMatrixType);

	/// Получить указатель на первую из 255 матриц для скелета
	static bqMat4* GetMatrixSkinned();


	static bqPolygonMesh* CreatePolygonMesh();


	static uint32_t GetMeshLoadersNum();


	static bqMeshLoader* GetMeshLoader(uint32_t);

	/// Загрузить 3D модель.
	/// Модельки приходят через коллбэк.
	/// В файле может быть множество 3D моделей.
	static void CreateMesh(const char*, bqMeshLoaderCallback*);

	/// Упрощённая функция загрузки 3D модели.
	/// Пока, вернётся первый мешбуфер который попадётся в файле.
	static bqMesh* CreateMesh(const char*);
	
	/// Упрощённая функция загрузки 3D модели.
	/// Мешбуферы будут добавлены в указанный массив.
	/// При вызове массив очищается
	static void CreateMesh(const char*, bqArray<bqMesh*>*);

	/// Установить указатель на матрицу
	static void SetMatrix(bqMatrixType, bqMat4*);

	/// Прочитать файл в буфер
	/// если isText будут добавлены ' ' и 0 в конец
	/// Функция выделит память для буфера. Использовать bqMemory::free для освобождения.
	static uint8_t* CreateFileBuffer(const char* path, uint32_t* szOut, bool isText);

#ifdef BQ_WITH_IMAGE
	static uint32_t GetImageLoadersNum();
	static bqImageLoader* GetImageLoader(uint32_t);
	static bqImage* CreateImage(const char*);
#endif

#ifdef BQ_WITH_GS
	// Метод сам загрузит bqImage, создаст текстуру и удалит bqImage
	static bqTexture* CreateTexture(bqGS*, const char*, bool genMipMaps = true, bool linearFilter = true);
	
	/// \brief Создать bqTextureCache
	static bqTextureCache* CreateTextureCache(bqGS*);
#endif

	/// \brief Узнать, существует ли файл
	static bool FileExist(const char*);
	static bool FileExist(const bqString&);

	/// \brief Получить размер файла
	static uint64_t FileSize(const char*);
	static uint64_t FileSize(const bqString&);

	/// \brief Получить путь к программе
	static bqString GetAppPath();
	static bqStringA GetAppPathA();

	/// \brief Получить путь к файлу относительно программы
	/// 
	/// Если файла нет, то путь урежится, и его можно
	/// будет использовать в поиске файла в архиве.
	/// Например, путь ../data/img/1.png
	/// Если файла нет то вернётся
	/// data/img/1.png
	/// 
	/// bqFramework::GetPath("../data/model_editor/gui.png").c_str()
	static bqStringA GetPath(const bqString& v);

#ifdef BQ_WITH_POPUP
	static bqPopup* CreatePopup();

	/// \brief Показать bqPopup на позиции курсора. 
	/// 
	/// Обычный способ показа - через поле bqPopup
	static void ShowPopupAtCursor(bqPopup*, bqWindow*);
#endif
	

#ifdef BQ_WITH_GUI
	static void InitDefaultFonts(bqGS* gs);
	static bqGUIFont* CreateGUIFont();
	static bqGUIFont* GetDefaultFont(bqGUIDefaultFont);
	static bqGUIStyle* GetGUIStyle(bqGUIStyleTheme);
	
	// пока лишнее
	static void Destroy(bqGUIWindow*);
	static void Destroy(bqGUIElement*);

	/// \brief Получить bqGUIState. Только для чтения.
	static const bqGUIState& GetGUIState();

#ifdef BQ_PLATFORM_WINDOWS
	static bqGUIIconTexture_GDI* CreateGUIIconTexture_GDI(bqImage*);
#endif
	static bqGUIMenu* CreateGUIMenu();

#endif
	static bqCursor* CreateCursor(const char* fn);
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

	static void ShowPopup(bqPopupData*);
	
	static void ShowCursor(bool);
	
	/// Вернётся ссылка на внутренний bqStringA.
	/// Необходимо иметь возможность быстрого преобразования
	/// Unicode в UTF-8.
	static const bqStringA& GetUTF8String(const bqString&);
	static const bqStringW& GetUTF16String(const bqString&);

	static void ClipCursor(bqVec4f*);
};

#endif

