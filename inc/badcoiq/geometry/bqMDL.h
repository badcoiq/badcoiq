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
#ifndef __BQ_MDL_H__
#define __BQ_MDL_H__
#ifdef BQ_WITH_MESH

#include "badcoiq/geometry/bqAABB.h"
#include "badcoiq/geometry/bqMesh.h"
#include "badcoiq/geometry/bqSkeleton.h"
#include "badcoiq/archive/bqArchive.h"
#include "badcoiq/common/bqFileBuffer.h"
#include "badcoiq/gs/bqGS.h"

#include "bqmdlinfo.h"

// 
class bqMDL
{
	struct _mtl
	{
		bqMDLChunkHeaderMaterial m_header;
		bqMaterial m_material;

		bqTexture* m_diffuseMap = 0;
	};

	struct _mesh
	{
		bqMesh* m_mesh = 0;
		bqGPUMesh* m_GPUmesh = 0;
		bqMDLChunkHeaderMesh m_chunkHeaderMesh;

		_mtl m_mtl;
	};

	bqArray<_mesh> m_meshes;

	bqArray<bqStringA> m_strings;
	uint8_t* _decompress(const bqMDLFileHeader&, 
		bqFileBuffer& , 
		uint32_t*, bqCompressorType);
	uint8_t* m_compressedData = 0;

	bqSkeleton* m_skeleton = 0;
	bqArray<bqSkeletonAnimation*> m_loadedAnimations;

public:
	bqMDL();
	~bqMDL();
	BQ_PLACEMENT_ALLOCATOR(bqMDL);

	bool Load(const char* mdlFile, const char* textureDir, bqGS*, bool free_bqMesh);
	void Unload();
	
	void FreebqMesh();
	size_t GetMeshNum() { return m_meshes.m_size; }
	bqGPUMesh* GetGPUMesh(size_t i) { return m_meshes.m_data[i].m_GPUmesh; }
	bqSkeleton* GetSkeleton() { return m_skeleton; }
	bqShaderType GetShaderType(size_t i) { return m_meshes.m_data[i].m_mtl.m_material.m_shaderType; }
	bqMaterial* GetMaterial(size_t i) { return &m_meshes.m_data[i].m_mtl.m_material; }
	
	uint32_t GetAniNum() { return (uint32_t)m_loadedAnimations.m_size; }
	bqSkeletonAnimation* GetAnimation(uint32_t i) { return m_loadedAnimations.m_data[i]; }
};


#endif
#endif

