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

bqMeshPolygonCreator::bqMeshPolygonCreator()
{
}

bqMeshPolygonCreator::~bqMeshPolygonCreator()
{
}

void bqMeshPolygonCreator::SetPosition(const bqVec3f& v)
{
	m_data.m_curr.m_baseData.BaseData.Position = v;
}

void bqMeshPolygonCreator::SetNormal(const bqVec3f& v)
{
	m_data.m_curr.m_baseData.BaseData.Normal = v;
}

void bqMeshPolygonCreator::SetBinormal(const bqVec3f& v)
{
	m_data.m_curr.m_baseData.BaseData.Binormal = v;
}

void bqMeshPolygonCreator::SetTangent(const bqVec3f& v)
{
	m_data.m_curr.m_baseData.BaseData.Tangent = v;
}

void bqMeshPolygonCreator::SetColor(const bqVec4f& v)
{
	m_data.m_curr.m_baseData.BaseData.Color = v;
}

void bqMeshPolygonCreator::SetUV(const bqVec2f& v)
{
	m_data.m_curr.m_baseData.BaseData.UV = v;
}

void bqMeshPolygonCreator::SetBoneInds(const bqVec4_t<uint8_t>& v)
{
	m_data.m_curr.m_baseData.BoneInds = v;
}

void bqMeshPolygonCreator::SetBoneWeights(const bqVec4f& v)
{
	m_data.m_curr.m_baseData.Weights = v;
}

void bqMeshPolygonCreator::SetVertex(const bqVertexTriangle& v)
{
	m_data.m_curr.m_baseData.BaseData = v;
}

void bqMeshPolygonCreator::AddVertex()
{
	m_data.m_array.push_back(m_data.m_curr);
}

void bqMeshPolygonCreator::Mul(const bqMat4& m)
{
	for (size_t i = 0; i < m_data.m_array.m_size; ++i)
	{
		auto p = m_data.m_array.m_data[i].m_baseData.BaseData.Position;
		bqMath::Mul(m, p, m_data.m_array.m_data[i].m_baseData.BaseData.Position);
		m_data.m_array.m_data[i].m_baseData.BaseData.Position.x += (float)m.m_data[3].x;
		m_data.m_array.m_data[i].m_baseData.BaseData.Position.y += (float)m.m_data[3].y;
		m_data.m_array.m_data[i].m_baseData.BaseData.Position.z += (float)m.m_data[3].z;

		// возможно здесь ещё нужно перемножать нормали
	}
}

uint32_t bqMeshPolygonCreator::Size()
{
	return m_data.m_array.size();
}


void bqMeshPolygonCreator::Clear()
{
	m_data.m_array.clear();
}

