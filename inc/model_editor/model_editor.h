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
#ifndef __BQ_MODEL_EDITOR_H__
#define __BQ_MODEL_EDITOR_H__

class bqMEPlugin;
class bqME;

typedef BQ_API bqMEPlugin* (BQ_CDECL* bqMECreatePlugin_t)();

#define BQ_ME_SDK_VERSION 1

class bqMEPlugin
{
public:
	bqMEPlugin() {}
	virtual ~bqMEPlugin() {}

	virtual const wchar_t* GetName() { return L"Plugin name"; }
	virtual const wchar_t* GetDescription() { return L"Plugin description"; }
	virtual const wchar_t* GetAuthor() { return L"Plugin author"; }

	// debug plugins must works only in debug app
	virtual bool IsDebug() {
#ifdef BQ_DEBUG
		return true;
#else
		return false;
#endif
	}

	virtual void Init(bqME* sdk) = 0;

	// return BQ_ME_SDK_VERSION
	virtual int CheckVersion() 
	{
		return BQ_ME_SDK_VERSION;
	}

	virtual void OnCreateObject(unsigned int objectId) {}
	/*virtual void OnCursorMove(bqMESelectionFrust*, bool isCursorInGUI) {}
	virtual void OnLMBDown(bqMESelectionFrust*, bool isCursorInGUI) {}
	virtual void OnLMBUp(bqMESelectionFrust*, bool isCursorInGUI) {}
	virtual void OnCancel(bqMESelectionFrust*, bool isCursorInGUI) {}
	virtual void OnUpdate(bqMESelectionFrust*, bool isCursorInGUI) {}*/

	//virtual void OnImportExport(const wchar_t* fileName, unsigned int id) {}
	virtual void OnImport(uint32_t importerID) {}
	virtual void OnExport(uint32_t importerID) {}

	// when hold shift and first click on gizmo
	// will be called on each selected object
	//virtual void OnClickGizmo(KeyboardModifier, bqMEGizmoMode, bqMEEditMode, bqMESceneObject*) {}

	// return: new object
	//virtual bqMESceneObject* OnDuplicate(bqMESceneObject* object, bool isMirror, bqMEAxis axis) { return 0; }

	//virtual void OnReadObject(FILE*, u32 typeForPlugin) {}
};

#endif

