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

#include "badcoiq.h"

#include "badcoiq/geometry/bqMeshCreator.h"


bqMesh::bqMesh() {}
bqMesh::~bqMesh()
{
	Free();
}

void bqMesh_CalculateTangents(
	bqVec3f& normal, bqVec3f& tangent, bqVec3f& binormal,
	const bqVec3f& vt1, const bqVec3f& vt2, const bqVec3f& vt3, // vertices, in float
	const bqVec2f& tc1, const bqVec2f& tc2, const bqVec2f& tc3) // texture coords
{
	bqVec3f e1 = vt2 - vt1;
	bqVec3f e2 = vt3 - vt1;
	e1.Cross(e2, normal);
	normal.Normalize();

	float deltaX1 = tc1.x - tc2.x;
	float deltaX2 = tc3.x - tc1.x;
	binormal = (e1 * deltaX2) - (e2 * deltaX1);
	binormal.Normalize();

	float deltaY1 = tc1.y - tc2.y;
	float deltaY2 = tc3.y - tc1.y;
	tangent = (e1 * deltaY2) - (e2 * deltaY1);
	tangent.Normalize();

	bqVec3f txb;
	tangent.Cross(binormal, txb);
	if (txb.Dot(normal) < static_cast<bqReal>(0.0))
	{
		tangent *= static_cast<bqReal>(-1.0);
		binormal *= static_cast<bqReal>(-1.0);
	}
}

void bqMesh_GenerateTangents_u16(bqMesh* m)
{
	uint16_t* idx = (uint16_t*)m->GetIBuffer();
	bqVertexTriangle* v = (bqVertexTriangle*)m->GetVBuffer();

	bqVec3f localNormal;
	for (uint32_t i = 0; i < m->GetInfo().m_iCount; i += 3)
	{
		bqMesh_CalculateTangents(
			localNormal,
			v[idx[i + 0]].Tangent,
			v[idx[i + 0]].Binormal,
			v[idx[i + 0]].Position,
			v[idx[i + 1]].Position,
			v[idx[i + 2]].Position,
			v[idx[i + 0]].UV,
			v[idx[i + 1]].UV,
			v[idx[i + 2]].UV);

		bqMesh_CalculateTangents(
			localNormal,
			v[idx[i + 1]].Tangent,
			v[idx[i + 1]].Binormal,
			v[idx[i + 1]].Position,
			v[idx[i + 2]].Position,
			v[idx[i + 0]].Position,
			v[idx[i + 1]].UV,
			v[idx[i + 2]].UV,
			v[idx[i + 0]].UV);

		bqMesh_CalculateTangents(
			localNormal,
			v[idx[i + 2]].Tangent,
			v[idx[i + 2]].Binormal,
			v[idx[i + 2]].Position,
			v[idx[i + 0]].Position,
			v[idx[i + 1]].Position,
			v[idx[i + 2]].UV,
			v[idx[i + 0]].UV,
			v[idx[i + 1]].UV);
	}
}

void bqMesh_GenerateTangents_u32(bqMesh* m)
{
	uint32_t* idx = (uint32_t*)m->GetIBuffer();
	bqVertexTriangle* v = (bqVertexTriangle*)m->GetVBuffer();

	bqVec3f localNormal;
	for (uint32_t i = 0; i < m->GetInfo().m_iCount; i += 3)
	{
		bqMesh_CalculateTangents(
			localNormal,
			v[idx[i + 0]].Tangent,
			v[idx[i + 0]].Binormal,
			v[idx[i + 0]].Position,
			v[idx[i + 1]].Position,
			v[idx[i + 2]].Position,
			v[idx[i + 0]].UV,
			v[idx[i + 1]].UV,
			v[idx[i + 2]].UV);

		bqMesh_CalculateTangents(
			localNormal,
			v[idx[i + 1]].Tangent,
			v[idx[i + 1]].Binormal,
			v[idx[i + 1]].Position,
			v[idx[i + 2]].Position,
			v[idx[i + 0]].Position,
			v[idx[i + 1]].UV,
			v[idx[i + 2]].UV,
			v[idx[i + 0]].UV);

		bqMesh_CalculateTangents(
			localNormal,
			v[idx[i + 2]].Tangent,
			v[idx[i + 2]].Binormal,
			v[idx[i + 2]].Position,
			v[idx[i + 0]].Position,
			v[idx[i + 1]].Position,
			v[idx[i + 2]].UV,
			v[idx[i + 0]].UV,
			v[idx[i + 1]].UV);
	}
}

void bqMesh::GenerateTangents()
{
	if (m_info.m_vertexType != bqMeshVertexType::Triangle)
		return;

	switch (m_info.m_indexType)
	{
	case bqMeshIndexType::u16:
	default:
		bqMesh_GenerateTangents_u16(this);
		break;
	case bqMeshIndexType::u32:
		bqMesh_GenerateTangents_u32(this);
		break;
	}
}

void bqMesh::GenerateNormals(bool smooth)
{
}

void bqMesh::Allocate(uint32_t triangles)
{
	BQ_ASSERT_STC(triangles, "value must be > 0");
	Free();

	m_info.m_iCount = triangles * 3;
	m_info.m_vCount = triangles * 3;
	m_info.m_stride = sizeof(bqVertexTriangle);
	m_info.m_vertexType = bqMeshVertexType::Triangle;
	m_info.m_indexType = bqMeshIndexType::u16;

	if (m_info.m_vCount > 0xffff)
		m_info.m_indexType = bqMeshIndexType::u32;

	m_vertices = (uint8_t*)bqMemory::malloc(m_info.m_vCount * m_info.m_stride);

	if(m_info.m_indexType == bqMeshIndexType::u32)
		m_indices = (uint8_t*)bqMemory::calloc(m_info.m_iCount * sizeof(uint32_t));
	else
		m_indices = (uint8_t*)bqMemory::calloc(m_info.m_iCount * sizeof(uint16_t));
}

void bqMesh::Free()
{
	m_info = bqMeshInfo();

	if (m_vertices)
	{
		bqMemory::free(m_vertices);
		m_vertices = 0;
	}

	if (m_indices)
	{
		bqMemory::free(m_indices);
		m_indices = 0;
	}
}

