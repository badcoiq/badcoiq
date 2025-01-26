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
#ifndef __BQ_POLYGONMESH_H__
/// \cond
#define __BQ_POLYGONMESH_H__
/// \endcond

#include "badcoiq/geometry/bqMesh.h"

#include "badcoiq/containers/bqList.h"

// Вершина хранит данные (напр. координаты)
// Полигон состоит из вершин
class bqPolygonMeshVertex
{
public:
	bqPolygonMeshVertex() {}
	~bqPolygonMeshVertex() {}
	BQ_PLACEMENT_ALLOCATOR(bqPolygonMeshVertex);

	bqVertexTriangleSkinned m_data;
	
	bqPolygonMeshControlPoint* m_controlPoint = 0;

	// может понадобиться знать какому полигону принадлежит вершина
	bqPolygonMeshPolygon* m_polygon = 0;
};


// Управляющая точка. Хранит указатели на вершины
class bqPolygonMeshControlPoint
{
public:
	bqPolygonMeshControlPoint() {}
	~bqPolygonMeshControlPoint() {}
	BQ_PLACEMENT_ALLOCATOR(bqPolygonMeshControlPoint);

	bqList<bqPolygonMeshVertex*> m_vertices;
};

// Хранит список вершин
class bqPolygonMeshPolygon
{
public:
	bqPolygonMeshPolygon() {}
	~bqPolygonMeshPolygon() { Clear(); }
	BQ_PLACEMENT_ALLOCATOR(bqPolygonMeshPolygon);

	// Удалить все вершины
	void Clear();

	bqVec3f GetFaceNormal();
	void CalculateNormal();
	bool IsVisible();
	uint32_t GetVerticesNumber();

	// Вершины из которых состоит полигон
	bqList<bqPolygonMeshVertex*> m_vertices;
};

// Возможно для ребра достаточно хранить указатели на управляющие точки
class bqPolygonMeshEdge
{
public:
	bqPolygonMeshEdge() {}
	~bqPolygonMeshEdge() {}
	BQ_PLACEMENT_ALLOCATOR(bqPolygonMeshEdge);

	bqPolygonMeshControlPoint* m_a = 0;
	bqPolygonMeshControlPoint* m_b = 0;
};

// Хранит все полигоны, управляющие точки и рёбра
// Когда уничтожается, так-же должен уничтожить все полигоны, УТ и рёбра.
class bqPolygonMesh
{
public:
	bqPolygonMesh();
	~bqPolygonMesh();
	BQ_PLACEMENT_ALLOCATOR(bqPolygonMesh);

	void AddPolygon(bqMeshPolygonCreator*, bool weld);

	// Удалить невидимые полигоны и те в которых менее 3х вершин
	void DeleteBadPolygons();

	// Удалить управляющие точки, которые больше не имеют вершин для управления
	//  возможно в будущем появятся другие причины для удаления
	void DeleteBadControlPoints();

	bqMesh* CreateMesh(bool skinned = false);
	void GenerateNormals(bool smooth = true);

	// Удалить полигон. bqListNode должен принадлежать списку m_polygons.
	void DeletePolygon(bqListNode<bqPolygonMeshPolygon*>*);

	void AddCube(float size, const bqMat4& m);
	void AddBox(const bqAabb&, const bqMat4& m);
	void AddSphere(float radius, uint32_t segments, const bqMat4& m);
	void AddCylinder(float radius, float height, uint32_t segments, bool topSide, bool bottomSide, const bqMat4& m);

	void GenerateUVPlanar(float scale);

	void Clear();

	bqList<bqPolygonMeshControlPoint*> m_controlPoints;
	bqList<bqPolygonMeshPolygon*> m_polygons;
	bqList<bqPolygonMeshEdge*> m_edges;

	bqAabb m_aabb;
};
#endif

