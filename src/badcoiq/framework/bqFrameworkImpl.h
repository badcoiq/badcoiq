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
#ifndef __BQ_FRAMEWORKIMPL_H__
#define __BQ_FRAMEWORKIMPL_H__

#include "badcoiq/input/bqInput.h"
#include "badcoiq/containers/bqArray.h"
#include "badcoiq/containers/bqQueue.h"
#include "badcoiq/containers/bqThreadFIFO.h"
#include "badcoiq/containers/bqList.h"
#include "badcoiq/GUI/bqGUI.h"
#include "badcoiq/system/bqCursor.h"
#include "badcoiq/math/bqMath.h"
#include <vector>
#include <thread>
#include <deque>

class bqFrameworkCallbackDefault : public bqFrameworkCallback
{
public:
	bqFrameworkCallbackDefault() {}
	virtual ~bqFrameworkCallbackDefault() {}
	virtual void OnMessage() {}
};

class bqSoundSystemImpl;

class bqFrameworkImpl
{
public:
	bqFrameworkImpl() 
	{
		for (uint32_t i = 0; i < (uint32_t)bqMatrixType::_count; ++i)
		{
			m_matrixPtrs[i] = 0;
		}
	}
	~bqFrameworkImpl() {}
	BQ_PLACEMENT_ALLOCATOR(bqFrameworkImpl);

	float m_deltaTime = 0.f;
	bqFrameworkCallback* m_frameworkCallback = 0;

	bqInputData m_input;

	bqString m_appPath;
	bqStringA m_appPathA;

#ifdef BQ_WITH_GS
	std::vector<bqGS*> m_gss;
#endif

#ifdef BQ_WITH_IMAGE
	std::vector<bqImageLoader*> m_imageLoaders;

	// ??? разве не вручную нужно удалять созданные текстуры.
	// возможно я храню здесь те текстуры которые создаёт сам движок
	bqArray<bqTexture*> m_texturesForDestroy;
#endif

#ifdef BQ_WITH_MESH
	std::vector<bqMeshLoader*> m_meshLoaders;
#endif


	bqStringA m_fileExistString;
	bqStringA m_fileSizeString;

	bqMat4* m_matrixPtrs[(uint32_t)bqMatrixType::_count];
#ifdef BQ_WITH_MESH
	bqMat4 m_matrixSkinned[255];
#endif

#ifdef BQ_WITH_ARCHIVE
	// Архивы, сжатие
	bool _compress_fastlz(bqCompressionInfo* info);
	bool _decompress_fastlz(bqCompressionInfo* info);
	void _onDestroy_archive();
	std::vector<bqArchiveZipFile*> m_zipFiles;
#endif

	void OnDestroy();

#ifdef BQ_WITH_GUI
	bqArray<bqGUIFont*> m_defaultFonts;
	bqGUIStyle m_themeLight;
	bqGUIStyle m_themeDark;
	void _initGUIThemes();
	bqGUIState m_GUIState;
	bqList<bqGUIWindow*> m_GUIWindows;

	void _initGUITextDrawCallbacks();
	void _onDestroy_GUITextDrawCallbacks();
	bqGUIDrawTextCallback* m_defaultTextDrawCallback_button = 0;
	bqGUIDrawTextCallback* m_defaultTextDrawCallback_icons = 0;
	bqGUIDrawTextCallback* m_defaultTextDrawCallback_textEditor = 0;
	bqGUIDrawTextCallback* m_defaultTextDrawCallback_listbox = 0;
	bqGUIDrawTextCallback* m_defaultTextDrawCallback_slider = 0;
	bqGUIDrawTextCallback* m_defaultTextDrawCallback_staticText = 0;
	bqGUIDrawTextCallback* m_defaultTextDrawCallback_window = 0;
#endif

	bqCursor* m_defaultCursors[(uint32_t)bqCursorType::_count] = {0};
	bqCursor* m_activeCursor = 0;
	void _initDefaultCursors();

#ifdef BQ_WITH_SOUND
	bqSoundSystemImpl* m_soundSystem = 0;
#endif

#ifdef BQ_WITH_PHYSICS
	bqPhysics* m_physics = 0;
#endif
};


#endif
