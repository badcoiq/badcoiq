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
	Clear();
}

void bqMeshPolygonCreator::SetPosition(const bqVec3f& v)
{
	m_vertexData.BaseData.Position = v;
}

void bqMeshPolygonCreator::SetNormal(const bqVec3f& v)
{
	m_vertexData.BaseData.Normal = v;
}

void bqMeshPolygonCreator::SetBinormal(const bqVec3f& v)
{
	m_vertexData.BaseData.Binormal = v;
}

void bqMeshPolygonCreator::SetTangent(const bqVec3f& v)
{
	m_vertexData.BaseData.Tangent = v;
}

void bqMeshPolygonCreator::SetColor(const bqVec4f& v)
{
	m_vertexData.BaseData.Color = v;
}

void bqMeshPolygonCreator::SetUV(const bqVec2f& v)
{
	m_vertexData.BaseData.UV = v;
}

void bqMeshPolygonCreator::SetBoneInds(const bqVec4_t<uint8_t>& v)
{
	m_vertexData.BoneInds = v;
}

void bqMeshPolygonCreator::SetBoneWeights(const bqVec4f& v)
{
	m_vertexData.Weights = v;
}

void bqMeshPolygonCreator::SetVertex(const bqVertexTriangle& v)
{
	m_vertexData.BaseData = v;
}

void bqMeshPolygonCreator::AddVertex()
{
	_createPolygon();

	bqPolygonMeshVertex* newVertex = new bqPolygonMeshVertex;
	newVertex->m_data = m_vertexData;

	// сохраняем указатель на полигон
	newVertex->m_polygon = m_polygon;

	m_polygon->m_vertices.push_back(newVertex);
	++m_size;
}

uint32_t bqMeshPolygonCreator::Size()
{
	return m_size;
}

void bqMeshPolygonCreator::_createPolygon()
{
	if (!m_polygon)
		m_polygon = new bqPolygonMeshPolygon;
}

void bqMeshPolygonCreator::Clear()
{
	if (m_polygon)
		delete m_polygon;

	DropPolygon();
}

bqPolygonMeshPolygon* bqMeshPolygonCreator::DropPolygon()
{
	bqPolygonMeshPolygon* p = m_polygon;

	m_polygon = 0;
	m_size = 0;

	return p;
}
