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
#ifndef __BQ_BQMDLINFO_H__
#define __BQ_BQMDLINFO_H__

#include <stdint.h>

// файл чтобы можно было подключить к проекту с плагином для 3Ds Max

struct bqMDLMeshVertex
{
	float m_position[3];
	float m_uv[2];
	float m_normal[3];
	float m_binormal[3];
	float m_tangent[3];
	float m_color[4];
};

struct bqMDLMeshVertexSkinned
{
	bqMDLMeshVertex m_base;
	uint8_t m_boneInds[4] = {0,0,0,0};
	float m_weights[4] = {0.f, 0.f,0.f,0.f};
};

struct bqMDLFileHeader
{
	uint32_t m_bmld = 1818520930; // "bmdl"
	uint32_t m_version = 1;
	uint32_t m_chunkNum = 0;

	enum
	{
		compression_null,
		compression_fastlz,
	};
	// 0 - нет сжатия
	// 1 - сжато fastlz
	// Если есть сжатие то сжимается всё кроме bqMDLFileHeader
	uint32_t m_compression = compression_null;
	// если сжатие есть, размер после расжатия.
	uint32_t m_uncmpSz = 0;
	uint32_t m_cmpSz = 0;

	uint32_t m_reserved1 = 0;
	uint32_t m_reserved2 = 0;
	uint32_t m_reserved3 = 0;
	uint32_t m_reserved4 = 0;
};


struct bqMDLChunkHeader
{
	enum
	{
		ChunkType__null,
		ChunkType_Mesh,
		ChunkType_String,
		ChunkType_Skeleton,
	};

	uint32_t m_chunkType = ChunkType__null;
	
	// размер. bqMDLChunkHeader + header конкретного чанка 
	//    + размер данных этого чанка
	// Например для mesh chunk
	// m_chunkSz = sizeof(bqMDLChunkHeader) + sizeof(bqMDLChunkHeaderMesh) 
	// + meshHead.m_vertBufSz + meshHead.m_indBufSz;
	uint32_t m_chunkSz = 0;

	uint32_t m_reserved1 = 0;
	uint32_t m_reserved2 = 0;
};

// дальше идёт зависимое от типа чанка (m_chunkType)

struct bqMDLChunkHeaderMesh
{
	// все строки лежат отдельно.
	// здесь и далее указываются индексы строк
	uint32_t m_nameIndex = 0;
	
	float m_aabbMin[3] = { 0.f,0.f,0.f };
	float m_aabbMax[3] = { 0.f,0.f,0.f };

	enum
	{
		IndexType_16bit,
		IndexType_32bit,
	};
	uint32_t m_indexType = IndexType_16bit;
	
	enum
	{
		VertexType_Triangle,
		VertexType_TriangleSkinned,
	};
	uint32_t m_vertexType = VertexType_Triangle;
	
	uint32_t m_material = 0;
	uint32_t m_vertNum = 0;
	uint32_t m_indNum = 0;
	uint32_t m_vertBufSz = 0;
	uint32_t m_indBufSz = 0;
	uint32_t m_reserved1 = 0;
	uint32_t m_reserved2 = 0;
};

// m_strSz - размер строки
// после m_strSz идёт строка без завершающего нуля
struct bqMDLChunkHeaderString
{
	uint32_t m_strSz = 0;
};


// сразу после идут кости в соответствии с количеством (m_boneNum)
struct bqMDLChunkHeaderSkeleton
{
	// количество костей
	uint32_t m_boneNum = 0;
};
struct bqMDLBoneData
{
	uint32_t m_nameIndex = 0;
	// индекс родителя
	// -1 значит что нет родителя
	int32_t m_parent = -1;
	float m_position[3];
	float m_scale[3];
	float m_rotation[4]; // quaternion
};


#endif

