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
#ifndef __BQ_MESHCREATOR_H__
#define __BQ_MESHCREATOR_H__

#ifdef BQ_WITH_MESH

#include "badcoiq/geometry/bqPolygonMesh.h"

/// Класс для ручного создания 3Д моделей
/// Пример использования:
/// 
/// bqMeshPolygonCreator pc;
/// pc.SetPosition(bqVec3f((float)box.m_min.x, (float)box.m_max.y, (float)box.m_max.z));
/// pc.SetUV(bqVec2f(0.f, 0.f));
/// pc.AddVertex();
/// pc.SetPosition(bqVec3f((float)box.m_max.x, (float)box.m_max.y, (float)box.m_max.z));
/// pc.SetUV(bqVec2f(1.f, 0.f));
/// pc.AddVertex();
/// pc.SetPosition(bqVec3f((float)box.m_max.x, (float)box.m_max.y, (float)box.m_min.z));
/// pc.SetUV(bqVec2f(1.f, 1.f));
/// pc.AddVertex();
/// pc.SetPosition(bqVec3f((float)box.m_min.x, (float)box.m_max.y, (float)box.m_min.z));
/// pc.SetUV(bqVec2f(0.f, 1.f));
/// pc.AddVertex();
/// pc.Mul(m);      // опционально
/// 
/// bqPolygonMesh pm;
/// pm.AddPolygon(&pc, true);
/// pc.Clear();
class bqMeshPolygonCreator
{
	bqVertexTriangleSkinned m_vertexData;
	bqPolygonMeshPolygon* m_polygon = 0;
	void _createPolygon();
	uint32_t m_size = 0;
public:
	bqMeshPolygonCreator();
	~bqMeshPolygonCreator();

	// set vertex data using this
	void SetPosition(const bqVec3f&);
	void SetNormal(const bqVec3f&);
	void SetBinormal(const bqVec3f&);
	void SetTangent(const bqVec3f&);
	void SetColor(const bqVec4f&);
	void SetUV(const bqVec2f&);
	void SetBoneInds(const bqVec4_t<uint8_t>&);
	void SetBoneWeights(const bqVec4f&);
	// or this
	void SetVertex(const bqVertexTriangle&);
	// then call this

	void AddVertex();
	uint32_t Size();

	void Mul(const bqMat4& m);

	void Clear();
	bqPolygonMeshPolygon* DropPolygon();
};

#endif
#endif

