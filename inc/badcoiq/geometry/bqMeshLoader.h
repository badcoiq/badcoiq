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
#ifndef __BQ_MESHLOADER_H__
#define __BQ_MESHLOADER_H__

#include "badcoiq/geometry/bqMesh.h"
#include "badcoiq/geometry/bqSkeleton.h"
#include "badcoiq/gs/bqMaterial.h"

class bqMeshLoaderCallback
{
public:
	bqMeshLoaderCallback() {}
	virtual ~bqMeshLoaderCallback() {}
	BQ_PLACEMENT_ALLOCATOR(bqMeshLoaderCallback);

	virtual void OnMaterialLog(bqMaterial* m)
	{
		if (m)
		{
			bqLog::PrintInfo("Mesh Callback: Material:\n");
			if (m->m_name.size())
				bqLog::PrintInfo("\t\tname: %s\n", bqFramework::GetUTF8String(m->m_name).c_str());
			else
				bqLog::PrintInfo("\t\tname: without name :(\n");
		}
	}

	virtual void OnMeshLog(bqMesh* newMesh, bqString* name, bqString* materialName)
	{
		if (newMesh)
		{
			bqLog::PrintInfo("Mesh Callback: new mesh:\n");
			if (name->size())
				bqLog::PrintInfo("\tname: %s\n", bqFramework::GetUTF8String(*name).c_str());
			else
				bqLog::PrintInfo("\tname: without name :(\n");

			if (materialName->size())
				bqLog::PrintInfo("\tmaterialName: %s\n", bqFramework::GetUTF8String(*materialName).c_str());
			else
				bqLog::PrintInfo("\tmaterialName: without name :(\n");


			switch (newMesh->GetInfo().m_indexType)
			{
			case bqMeshIndexType::u16:
				bqLog::PrintInfo("\tindex type: u16\n");
				break;
			case bqMeshIndexType::u32:
				bqLog::PrintInfo("\tindex type: u32\n");
				break;
			}

			switch (newMesh->GetInfo().m_vertexType)
			{
			case bqMeshVertexType::Null:
				bqLog::PrintInfo("\tvertex type: Null\n");
				break;
			case bqMeshVertexType::Triangle:
				bqLog::PrintInfo("\tvertex type: Triangle\n");
				break;
			}
		
			bqLog::PrintInfo("\tv num: %u\n", newMesh->GetInfo().m_vCount);
			bqLog::PrintInfo("\ti num: %u\n", newMesh->GetInfo().m_iCount);
			bqLog::PrintInfo("\tstride: %u\n", newMesh->GetInfo().m_stride);
			
			bqLog::PrintInfo("\tskinned: %s\n", newMesh->GetInfo().m_skinned ? "true" : "false");
			bqLog::PrintInfo("\taabb: \n");
			bqLog::PrintInfo("\t\tminx: [%f] [%f] :maxx\n", newMesh->GetInfo().m_aabb.m_min.x, newMesh->GetInfo().m_aabb.m_max.x);
			bqLog::PrintInfo("\t\tminy: [%f] [%f] :maxy\n", newMesh->GetInfo().m_aabb.m_min.y, newMesh->GetInfo().m_aabb.m_max.y);
			bqLog::PrintInfo("\t\tminz: [%f] [%f] :maxz\n", newMesh->GetInfo().m_aabb.m_min.z, newMesh->GetInfo().m_aabb.m_max.z);
		}
	}

	virtual void OnMaterial(bqMaterial* m)
	{
		OnMaterialLog(m);
	}

	virtual void OnMesh(bqMesh* newMesh, bqString* n, bqString* mn)
	{
		if (newMesh)
		{
			OnMeshLog(newMesh, n, mn);

			// if don't need then destroy this mesh
			bqDestroy(newMesh);
		}
	}

	// Будет вызван по окончании чтения файла.
	// Неизвестно что будет прочитано первым, материал или модель.
	// После чтения, можно будет в данном методе найти нужный материал для нужной модели.
	virtual void Finale()
	{

	}

	virtual void OnSkeleton(bqSkeleton* s)
	{
		if (s)
		{
			// if don't need then destroy
			bqDestroy(s);
		}
	}
	virtual void OnAnimation(bqSkeletonAnimation* a)
	{
		if (a)
		{
			bqDestroy(a);
		}
	}

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

