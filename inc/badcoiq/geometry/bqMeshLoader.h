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
#ifndef __BQ_MESHLOADER_H__
#define __BQ_MESHLOADER_H__

#include "badcoiq/geometry/bqMesh.h"
#include "badcoiq/gs/bqMaterial.h"

class bqMeshLoaderCallback
{
public:
	bqMeshLoaderCallback() {}
	virtual ~bqMeshLoaderCallback() {}
	BQ_PLACEMENT_ALLOCATOR(bqMeshLoaderCallback);

	virtual void OnMaterial(bqMaterial* m)
	{
	}

	virtual void OnMesh(bqMesh* newMesh, bqString* name, bqString* materialName)
	{
		if (newMesh)
		{
			// if don't need then destroy this mesh
			bqDestroy(newMesh);
		}
	}

	// Будет вызван по окончании чтения файла.
	// Неизвестно что юудет прочитано первым, материал или модель.
	// После чтения, можно будет в данном методе найти нужный материал для нужной модели.
	virtual void Finale()
	{

	}

	//virtual void OnSkeleton(bqSkeleton* s)
	//{
	//	if (s)
	//	{
	//		// if don't need then destroy
	//		bqDestroy(s);
	//	}
	//}
	//virtual void OnAnimation(bqSkeletonAnimation* a)
	//{
	//	if (a)
	//	{
	//		bqDestroy(a);
	//	}
	//}
};

class bqMeshLoader
{
public:
	bqMeshLoader() {}
	virtual ~bqMeshLoader() {}

	virtual uint32_t GetSupportedFilesCount() = 0;
	virtual bqString GetSupportedFileExtension(uint32_t) = 0;
	virtual bqString GetSupportedFileName(uint32_t) = 0;

	virtual void Load(const char* path, bqMeshLoaderCallback*) = 0;
	virtual void Load(const char* path, bqMeshLoaderCallback*, uint8_t* buffer, uint32_t bufferSz) = 0;
};

#endif

