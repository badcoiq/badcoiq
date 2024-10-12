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
#ifndef __BQ_MESH_H__
/// \cond
#define __BQ_MESH_H__
/// \endcond

#ifdef BQ_WITH_MESH

#include "badcoiq/geometry/bqAABB.h"

/// Тип вершины
enum class bqMeshVertexType : uint32_t
{
	Null, // 0
	Triangle,
	// TriangleSkinned, // пока лишнее. всё работает норм благодоря m_stride
};

/// Тип индекса
enum class bqMeshIndexType : uint32_t
{
	u16, /// uint16_t
	u32  /// uint32_t
};

/// Дефолтная вершина
struct bqVertexTriangle
{
	bqVec3f Position;
	bqVec2f UV;
	bqVec3f Normal;
	bqVec3f Binormal;
	bqVec3f Tangent;
	bqVec4f Color;
};

/// Дефолтная вершина + данные для скининга
struct bqVertexTriangleSkinned
{
	bqVertexTriangle BaseData;
	bqVec4_t<uint8_t> BoneInds;
	bqVec4f Weights;
};

/// Информация о модели
struct bqMeshInfo
{
	bqMeshVertexType m_vertexType = bqMeshVertexType::Triangle;
	bqMeshIndexType m_indexType = bqMeshIndexType::u32;
	uint32_t m_vCount = 0;
	uint32_t m_iCount = 0;
	uint32_t m_stride = 0;
	bool m_skinned = false;
	bqAabb m_aabb;
};

/// 3Д моделька
class bqMesh
{
	uint8_t* m_vertices = nullptr;
	uint8_t* m_indices = nullptr;

	bqMeshInfo m_info;
public:
	bqMesh();
	~bqMesh();
	BQ_PLACEMENT_ALLOCATOR(bqMesh);

	const bqMeshInfo& GetInfo() const { return m_info; }
	bqMeshInfo& GetInfo(){ return m_info; }
	
	uint8_t* GetVBuffer() const { return m_vertices; }
	uint8_t* GetIBuffer() const { return m_indices; }

	void SetVBuffer(uint8_t* b) { m_vertices = b; }
	void SetIBuffer(uint8_t* b) { m_indices = b; }

	void Allocate(uint32_t triangles, bool skinned = false);
	void Allocate(uint32_t numV, uint32_t numI, bool skinned = false);
	void Free();

	void GenerateNormals(bool smooth);
	void GenerateTangents();

	void ToSkinned();
	void ApplySkeleton(bqSkeleton* skeleton);

	/// Получить указатель на вершину.
	/// Превеводить в другой тип нужно самостоятельно.
	/// Индексы предпологается брать из индексного буфера.
	///  например GetVertex(m_indices[i])
	bqVertexTriangle* GetVertex(uint32_t index);
};

#endif
#endif

