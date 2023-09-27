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
#include "badcoiq/geometry/bqPolygonMesh.h"
#include "badcoiq/containers/bqArray.h"

#include <map>
#include <string>
#include <vector>

// надо удалить вершины, и очистить список
void bqPolygonMeshPolygon::Clear()
{
	if (m_vertices.m_head)
	{
		auto curr = m_vertices.m_head;
		auto last = m_vertices.m_head->m_left;
		while (true)
		{
			auto next = curr->m_right;

			delete curr->m_data;

			if (curr == last)
				break;

			curr = next;
		}

		m_vertices.clear();
	}
}

bqPolygonMesh::bqPolygonMesh()
{
}

bqPolygonMesh::~bqPolygonMesh()
{
	Clear();
}

void bqPolygonMesh::Clear()
{
	for (auto o : m_controlPoints)
	{
		delete o;
	}

	for (auto o : m_edges)
	{
		delete o;
	}

	for (auto o : m_polygons)
	{
		delete o;
	}
	m_controlPoints.clear();
	m_polygons.clear();
	m_edges.clear();
	m_aabb.Reset();
}

// При добавлении полигона необходимо создавать новые управляющие точки
// Но может быть такое что управляющая точка уже создана, если два полигона
//  должны иметь общее ребро. Чтобы это всё учитывалось, надо указать weld = true
void bqPolygonMesh::AddPolygon(bqMeshPolygonCreator* pc, bool weld)
{
	BQ_ASSERT_STC(pc, "you must give bqMeshPolygonCreator");
	BQ_ASSERT_STC(pc->Size() > 2, "object bqMeshPolygonCreator must have at least 3 points");
	if(pc->Size() > 2)
	{
		// `соединять` надо только те точки что находятся на ребре который имеет только 1 полигон
		// если нет рёбер то убираю weld
		if (!m_edges.m_head)
			weld = false;

		auto newPolygon = pc->DropPolygon();
		if (newPolygon)
		{
			// добавить полигон в список
			m_polygons.push_back(newPolygon);

			for (auto v : newPolygon->m_vertices)
			{
				bqPolygonMeshControlPoint* cp = 0;

				// поиск управляющей точки
				if (weld)
				{
					
				}

				// если управляющая точка не найдена, то создаём
				if (!cp)
				{
					cp = new bqPolygonMeshControlPoint;

					// сохранить управляющую точку
					m_controlPoints.push_back(cp);
				}

				if (cp)
				{
					cp->m_vertices.push_back(v);
					
					m_aabb.Add(v->m_data.BaseData.Position);

					// сохранить указатель на управляющую точку
					v->m_controlPoint = cp;
				}
			}
		}
	}
}

void bqPolygonMesh::GenerateNormals(bool smooth)
{
	if (m_polygons.m_head)
	{
		for (auto p : m_polygons)
		{
			p->CalculateNormal();
		}
	
		if (smooth)
		{
			struct Container
			{
				bqVec3 normal;
				std::vector<bqPolygonMeshVertex*> verts;
			};

			std::map<std::string, Container> map;
			for (auto p : m_polygons)
			{
				bqStringA str;
				std::string stdstr;

				for(auto v : p->m_vertices)
				{
					str.clear();
					str.append(v->m_data.BaseData.Position.x);
					str.append(v->m_data.BaseData.Position.y);
					str.append(v->m_data.BaseData.Position.z);

					stdstr.clear();
					stdstr.append(str.c_str());

					map[stdstr].verts.push_back(v);
					map[stdstr].normal += p->GetFaceNormal();
				}

			}

			for (auto p : m_polygons)
			{
				for (auto& o : map)
				{
					for (auto& c : o.second.verts)
					{
						c->m_data.BaseData.Normal = o.second.normal;
						c->m_data.BaseData.Normal.Normalize();
					}
				}
			}
		}
	}
}

bqVec3f bqPolygonMeshPolygon::GetFaceNormal()
{
	BQ_ASSERT_ST(m_vertices.m_head);

	// Был установлен(возвращён) правильный порядок. 1 2 3
	// больше не надо менять.

	bqVec3f _n;
	auto vertex_1 = m_vertices.m_head;
	auto vertex_2 = vertex_1->m_right;
	auto vertex_3 = vertex_2->m_right;
	while (true) {
		auto e1 = vertex_2->m_data->m_data.BaseData.Position - vertex_1->m_data->m_data.BaseData.Position;
		auto e2 = vertex_3->m_data->m_data.BaseData.Position - vertex_1->m_data->m_data.BaseData.Position;
		bqVec3f n;
		e1.Cross(e2, n);
		_n -= n;

		// ===============================
		vertex_2 = vertex_2->m_right;
		vertex_3 = vertex_3->m_right;

		if (vertex_2 == vertex_1)
			break;
	}
	_n.Normalize();
	return _n;
}

void bqPolygonMeshPolygon::CalculateNormal()
{
	BQ_ASSERT_ST(m_vertices.m_head);

	bqVec3f _n = GetFaceNormal();

	for (auto v : m_vertices)
	{
		v->m_data.BaseData.Normal = _n;
	}
}

uint32_t bqPolygonMeshPolygon::GetVerticesNumber()
{
	uint32_t numV = 0;
	for (auto v : m_vertices)
	{
		++numV;
	}
	return numV;
}


bqMesh* bqPolygonMesh::SummonMesh()
{
	bqMesh* m = 0;

	// хорошо бы сделать проверку и удалить невидимые полигоны
	DeleteBadPolygons();

	if (m_polygons.m_head)
	{
		// Надо найти количество вершин и индексов
		uint32_t numV = 0;
		uint32_t numI = 0;

		for (auto o : m_polygons)
		{
			uint32_t vn = o->GetVerticesNumber();
			BQ_ASSERT_ST(vn > 2);
			if (vn > 2)
			{
				numV += vn;

				// индексы описывают треугольники
				// поэтому : количество треугольников * 3
				// количество треугольников это например если 3 вершины, то 1 треугольник
				//                                            4          то 2 треугольника
				//                соответственно, для нахождения количества треугольников
				//                надо взять количество вершин и вычесть 2			                               
				numI += (vn - 2) * 3;
			}
		}

		// создание и настройка bqMesh
		m = new bqMesh;
		m->Allocate(numV, numI);
		m->GetInfo().m_aabb = m_aabb;

		bqVertexTriangle* vertex = (bqVertexTriangle*)m->GetVBuffer();
		uint32_t* ind32 = (uint32_t*)m->GetIBuffer();
		uint16_t* ind16 = (uint16_t*)m->GetIBuffer();

		uint32_t index = 0; // то самое значение которое пойдёт в ind16\ind32

		// заполнение буферов
		for (auto o : m_polygons)
		{
			auto vn = o->GetVerticesNumber();
			if (vn > 2)
			{
				// надо просто передать данные
				for (auto v : o->m_vertices)
				{
					*vertex = v->m_data.BaseData;
					++vertex;
				}

				// теперь индексы
				uint32_t baseIndex = index; // основная вершина. например полигон из 5ти вершин
				                            // 3 треугольника. основная вершина это первый индекс
				                            // 0 1 2
				                            // 0 2 3
				                            // 0 3 4
				for (uint32_t i = 0; i < vn - 2; ++i)
				{
					switch (m->GetInfo().m_indexType)
					{
					case bqMeshIndexType::u16:
						*ind16 = baseIndex; ++ind16;
						*ind16 = index + 2; ++ind16;
						*ind16 = index + 1; ++ind16;
						break;
					case bqMeshIndexType::u32:
						*ind32 = baseIndex; ++ind32;
						*ind32 = index + 2; ++ind32;
						*ind32 = index + 1; ++ind32;
						break;
					}

					++index;
				}

				// значение index надо увеличить по такой логике
				// ...например полигон из 5ти вершин...
				// ..3 треугольника...
				// 0 1 2
				// 0 2 3
				// 0 3 4
				// следующий полигон
				// ...например полигон из 5ти вершин...
				// ..3 треугольника...
				// 5 6 7
				// 5 7 8
				// 5 8 9
				index += 2;
			}
		}
	}

	return m;
}

bool bqPolygonMeshPolygon::IsVisible()
{
	auto vertex_1 = m_vertices.m_head;
	auto vertex_2 = vertex_1->m_right;
	auto vertex_3 = vertex_2->m_right;
	while (true)
	{
		auto a = vertex_2->m_data->m_data.BaseData.Position - vertex_1->m_data->m_data.BaseData.Position;
		auto b = vertex_3->m_data->m_data.BaseData.Position - vertex_1->m_data->m_data.BaseData.Position;

		bqVec3f n;
		a.Cross(b, n);

		float area = 0.5f * sqrt(n.Dot());

		if (area > 0.000001)
			return true;


		vertex_2 = vertex_2->m_right;
		vertex_3 = vertex_3->m_right;
		if (vertex_3 == vertex_1)
			break;
	}
	return false;
}

void bqPolygonMesh::DeleteBadPolygons()
{
	if (m_polygons.m_head)
	{
		// Удалять лучше имея bqListNode, так как надо будет изъять эту ноду из списка,
		// её ещё надо найти. bqListNode позволит избежать поиск.
		std::vector<bqListNode<bqPolygonMeshPolygon*>*> forDelete;

		// проход по списку, нодами
		auto curr = m_polygons.m_head;
		auto last = curr->m_left;
		while(true)
		{
			if (!curr->m_data->IsVisible() || (curr->m_data->GetVerticesNumber() < 3))
				forDelete.push_back(curr);

			if (curr == last)
				break;

			curr = curr->m_right;
		}

		for (auto o : forDelete)
		{
			// там ошибка, надо исправить и раздокументировать
		//	DeletePolygon(o);
		}
	}
}

void bqPolygonMesh::DeletePolygon(bqListNode<bqPolygonMeshPolygon*>* P)
{
	BQ_ASSERT_ST(P);

	// просто и быстро удаление из списка
	P->m_left->m_right = P->m_right;
	P->m_right->m_left = P->m_left;

	// Если этот полигон был первым, то надо поменять m_polygons.m_head
	if (P == m_polygons.m_head)
		m_polygons.m_head = m_polygons.m_head->m_right;

	// Если всё ещё оно, то это последний полигон
	if (P == m_polygons.m_head)
	{
		m_polygons.m_head = 0;
		// m_polygons.clear(); // не надо

		// скорее всего в данном случае, проще удалить всё оставшееся
		// так как полигонов больше нет.
		// DELETEALL();
		// RETURN;
	}

	// так как удаляются вершины, надо удалить указатели на них которые находятся
	// в управляющих точках. 
	for (auto v : P->m_data->m_vertices)
	{
		v->m_controlPoint->m_vertices.erase_first(v);
	}
	//  Если, после удаления указателя, больше не остаётся
	//  указателей, то такая управляющая точка больше не нужна.
	DeleteBadControlPoints();

	// так-же надо будет позаботится о рёбрах. но это будет потом, так как они пока не нужны

	// вершины удалятся в деструкторе полигона
	delete P->m_data; // delete bqPolygonMeshPolygon
	delete P; // delete bqListNode
}

void bqPolygonMesh::DeleteBadControlPoints()
{
	std::vector<bqListNode<bqPolygonMeshControlPoint*>*> forDelete;

	auto curr = m_controlPoints.m_head;
	if (curr)
	{
		auto last = curr->m_left;
		while (true)
		{
			if (!curr->m_data->m_vertices.m_head)
				forDelete.push_back(curr);

			if (curr == last)
				break;
			curr = curr->m_right;
		}
	}

	for (auto cp : forDelete)
	{
		delete cp->m_data;
		m_controlPoints.erase_by_node(cp);
	}
}

void bqPolygonMesh::AddCube(float size, const bqMat4& m)
{
	BQ_ASSERT_ST(size != 0.f);

	float halfSize = size * 0.5f;

	bqAabb aabb;
	aabb.m_min.Set(-halfSize);
	aabb.m_max.Set(halfSize);

	AddBox(aabb, m);
}


/*
* x - max
* n - min

  nxx+______________+ xxx
	 |\             \
	 | \            .\
	 |  \           . \
	 |   \  nxn     .  \
	 |    \+____________\+ xxn
	 |     |        .    |
  nnx+.....|........+xnx |
	  \    |         .   |
	   \   |          .  |
		\  |           . |
		 \ |            .|
	  nnn \+_____________+ xnn
*/

void bqPolygonMesh::AddBox(const bqAabb& box, const bqMat4& m)
{
	bqMeshPolygonCreator pc;
	// maybe UV is not correct...
	// top
	pc.SetPosition(bqVec3f((float)box.m_min.x, (float)box.m_max.y, (float)box.m_max.z));
	pc.SetUV(bqVec2f(0.f, 0.f));
	pc.AddVertex();
	pc.SetPosition(bqVec3f((float)box.m_max.x, (float)box.m_max.y, (float)box.m_max.z));
	pc.SetUV(bqVec2f(1.f, 0.f));
	pc.AddVertex();
	pc.SetPosition(bqVec3f((float)box.m_max.x, (float)box.m_max.y, (float)box.m_min.z));
	pc.SetUV(bqVec2f(1.f, 1.f));
	pc.AddVertex();
	pc.SetPosition(bqVec3f((float)box.m_min.x, (float)box.m_max.y, (float)box.m_min.z));
	pc.SetUV(bqVec2f(0.f, 1.f));
	pc.AddVertex();
	pc.Mul(m);
	AddPolygon(&pc, true);

	// bottom
	pc.Clear();
	pc.SetPosition(bqVec3f((float)box.m_min.x, (float)box.m_min.y, (float)box.m_min.z));
	pc.SetUV(bqVec2f(0.f, 0.f));
	pc.AddVertex();
	pc.SetPosition(bqVec3f((float)box.m_max.x, (float)box.m_min.y, (float)box.m_min.z));
	pc.SetUV(bqVec2f(1.f, 0.f));
	pc.AddVertex();
	pc.SetPosition(bqVec3f((float)box.m_max.x, (float)box.m_min.y, (float)box.m_max.z));
	pc.SetUV(bqVec2f(1.f, 1.f));
	pc.AddVertex();
	pc.SetPosition(bqVec3f((float)box.m_min.x, (float)box.m_min.y, (float)box.m_max.z));
	pc.SetUV(bqVec2f(0.f, 1.f));
	pc.AddVertex();
	pc.Mul(m);
	AddPolygon(&pc, true);

	// left
	pc.Clear();
	pc.SetPosition(bqVec3f((float)box.m_min.x, (float)box.m_max.y, (float)box.m_max.z));
	pc.SetUV(bqVec2f(0.f, 0.f));
	pc.AddVertex();
	pc.SetPosition(bqVec3f((float)box.m_min.x, (float)box.m_max.y, (float)box.m_min.z));
	pc.SetUV(bqVec2f(1.f, 0.f));
	pc.AddVertex();
	pc.SetPosition(bqVec3f((float)box.m_min.x, (float)box.m_min.y, (float)box.m_min.z));
	pc.SetUV(bqVec2f(1.f, 1.f));
	pc.AddVertex();
	pc.SetPosition(bqVec3f((float)box.m_min.x, (float)box.m_min.y, (float)box.m_max.z));
	pc.SetUV(bqVec2f(0.f, 1.f));
	pc.AddVertex();
	pc.Mul(m);
	AddPolygon(&pc, true);

	// right
	pc.Clear();
	pc.SetPosition(bqVec3f((float)box.m_max.x, (float)box.m_max.y, (float)box.m_max.z));
	pc.SetUV(bqVec2f(0.f, 0.f));
	pc.AddVertex();
	pc.SetPosition(bqVec3f((float)box.m_max.x, (float)box.m_min.y, (float)box.m_max.z));
	pc.SetUV(bqVec2f(1.f, 0.f));
	pc.AddVertex();
	pc.SetPosition(bqVec3f((float)box.m_max.x, (float)box.m_min.y, (float)box.m_min.z));
	pc.SetUV(bqVec2f(1.f, 1.f));
	pc.AddVertex();
	pc.SetPosition(bqVec3f((float)box.m_max.x, (float)box.m_max.y, (float)box.m_min.z));
	pc.SetUV(bqVec2f(0.f, 1.f));
	pc.AddVertex();
	pc.Mul(m);
	AddPolygon(&pc, true);

	// back
	pc.Clear();
	pc.SetPosition(bqVec3f((float)box.m_min.x, (float)box.m_max.y, (float)box.m_min.z));
	pc.SetUV(bqVec2f(0.f, 0.f));
	pc.AddVertex();
	pc.SetPosition(bqVec3f((float)box.m_max.x, (float)box.m_max.y, (float)box.m_min.z));
	pc.SetUV(bqVec2f(1.f, 0.f));
	pc.AddVertex();
	pc.SetPosition(bqVec3f((float)box.m_max.x, (float)box.m_min.y, (float)box.m_min.z));
	pc.SetUV(bqVec2f(1.f, 1.f));
	pc.AddVertex();
	pc.SetPosition(bqVec3f((float)box.m_min.x, (float)box.m_min.y, (float)box.m_min.z));
	pc.SetUV(bqVec2f(0.f, 1.f));
	pc.AddVertex();
	pc.Mul(m);
	AddPolygon(&pc, true);

	// front
	pc.Clear();
	pc.SetPosition(bqVec3f((float)box.m_min.x, (float)box.m_max.y, (float)box.m_max.z));
	pc.SetUV(bqVec2f(0.f, 0.f));
	pc.AddVertex();
	pc.SetPosition(bqVec3f((float)box.m_min.x, (float)box.m_min.y, (float)box.m_max.z));
	pc.SetUV(bqVec2f(1.f, 0.f));
	pc.AddVertex();
	pc.SetPosition(bqVec3f((float)box.m_max.x, (float)box.m_min.y, (float)box.m_max.z));
	pc.SetUV(bqVec2f(1.f, 1.f));
	pc.AddVertex();
	pc.SetPosition(bqVec3f((float)box.m_max.x, (float)box.m_max.y, (float)box.m_max.z));
	pc.SetUV(bqVec2f(0.f, 1.f));
	pc.AddVertex();
	pc.Mul(m);
	AddPolygon(&pc, true);
}

void bqPolygonMesh::AddSphere(float radius, uint32_t segments, const bqMat4& m)
{
	if (radius == 0.f) radius = 1.f;
	if (radius < 0.f) radius = 1.f;

	if (segments < 3)
		segments = 3;
	if (segments > 30)
		segments = 30;

	// 1. Generate points in 2D.
	//    Generate from 90 to 270 degrees
	//    2 points must be in 90 and in 270 degrees
	bqArray<bqVec3> points;
	float angle = 90.f;
	float angleStep = 180.f / (segments - 1);
	for (uint32_t i = 0; i < segments; ++i)
	{
		if (i == segments - 1)
			angle = 270.f;

		auto sn = ::sin(bqMath::DegToRad(angle)) * radius;
		auto cs = ::cos(bqMath::DegToRad(angle)) * radius;
		//	printf("A[%f]: %f %f\n", angle, sn, cs);
		points.push_back(bqVec3((bqReal)cs, (bqReal)sn, 0.0));

		angle += angleStep;
	}

	// 2. Create polygons. Take points, rotate them, use them to build new polygon.
	/*
	*                    +0
	*                  / |
	*                /   |
	*              /     |
	*            /      /
	*          1+  -  - +1rotated
	*           |       |
	*           |       |
	*           |       |
	*           |       |
	*          2+  -  - +2rotated
	*            \      \
	*              \     |
	*                \   |
	*                  \ |
	*                   3+
	*   Top and bottom polygons are triangles
	*/

	angle = 0.f;
	angleStep = 360.f / (segments);
	float nextAngle = angleStep;
	bqVec2f UV;
	float UVstep = 1.f / (segments - 1);
	for (uint32_t i1 = 0; i1 < segments; ++i1)
	{
		bqMat4 M, Mn;
		M.SetRotation(bqQuaternion(0.f, bqMath::DegToRad(angle), 0.f));
		Mn.SetRotation(bqQuaternion(0.f, bqMath::DegToRad(nextAngle), 0.f));

		for (uint32_t i2 = 0; i2 < segments - 1; ++i2)
		{
			// top triangle
			if (i2 == 0)
			{
				bqVec3 v0 = points[i2];
				bqVec3 v1 = points[i2 + 1];
				bqVec3 v2 = v1;

				bqMath::Mul(M, points[i2], v0);
				bqMath::Mul(M, points[i2 + 1], v1);
				bqMath::Mul(Mn, points[i2 + 1], v2);

				bqMeshPolygonCreator pc;
				pc.SetPosition(bqVec3f((float)v1.x, (float)v1.y, (float)v1.z));
				pc.SetUV(bqVec2f(UV.x, UV.y + UVstep));
				pc.AddVertex();
				pc.SetPosition(bqVec3f((float)v0.x, (float)v0.y, (float)v0.z));
				pc.SetUV(bqVec2f(UV.x + (UVstep * 0.5f), UV.y));
				pc.AddVertex();
				pc.SetPosition(bqVec3f((float)v2.x, (float)v2.y, (float)v2.z));
				pc.SetUV(bqVec2f(UV.x + UVstep, UV.y + UVstep));
				pc.AddVertex();
				pc.Mul(m);
				AddPolygon(&pc, true);
			}
			else if (i2 == segments - 2) // bottom
			{
				bqVec3 v0 = points[i2];
				bqVec3 v1 = points[i2 + 1];
				bqVec3 v2 = points[i2];

				bqMath::Mul(M, points[i2], v0);
				bqMath::Mul(M, points[i2 + 1], v1);
				bqMath::Mul(Mn, points[i2], v2);

				bqMeshPolygonCreator pc;
				pc.SetPosition(bqVec3f((float)v1.x, (float)v1.y, (float)v1.z));
				pc.SetUV(bqVec2f(UV.x + (UVstep * 0.5f), UV.y + UVstep));
				pc.AddVertex();
				pc.SetPosition(bqVec3f((float)v0.x, (float)v0.y, (float)v0.z));
				pc.SetUV(bqVec2f(UV.x, UV.y));
				pc.AddVertex();
				pc.SetPosition(bqVec3f((float)v2.x, (float)v2.y, (float)v2.z));
				pc.SetUV(bqVec2f(UV.x + UVstep, UV.y));
				pc.AddVertex();
				pc.Mul(m);
				AddPolygon(&pc, true);
			}
			else
			{
				bqVec3 v0 = points[i2];
				bqVec3 v1 = points[i2 + 1];
				bqVec3 v2 = v0;
				bqVec3 v3 = v1;

				bqMath::Mul(M, points[i2], v0);
				bqMath::Mul(M, points[i2 + 1], v1);
				bqMath::Mul(Mn, points[i2], v2);
				bqMath::Mul(Mn, points[i2 + 1], v3);

				bqMeshPolygonCreator pc;
				pc.SetPosition(bqVec3f((float)v1.x, (float)v1.y, (float)v1.z));
				pc.SetUV(bqVec2f(UV.x, UV.y + UVstep));
				pc.AddVertex();
				pc.SetPosition(bqVec3f((float)v0.x, (float)v0.y, (float)v0.z));
				pc.SetUV(bqVec2f(UV.x, UV.y));
				pc.AddVertex();
				pc.SetPosition(bqVec3f((float)v2.x, (float)v2.y, (float)v2.z));
				pc.SetUV(bqVec2f(UV.x + UVstep, UV.y));
				pc.AddVertex();
				pc.SetPosition(bqVec3f((float)v3.x, (float)v3.y, (float)v3.z));
				pc.SetUV(bqVec2f(UV.x + UVstep, UV.y + UVstep));
				pc.AddVertex();
				pc.Mul(m);
				AddPolygon(&pc, true);
			}

			UV.y += UVstep;
		}

		UV.x += UVstep;

		angle += angleStep;
		nextAngle += angleStep;
	}
}

void bqPolygonMesh::GenerateUVPlanar(float scale)
{
	if (!m_polygons.m_head)
		return;

	bqVec3f n;
	auto cp = m_polygons.m_head;
	auto lp = cp->m_left;
	while (true)
	{
		n += cp->m_data->GetFaceNormal();

		if (cp == lp)
			break;
		cp = cp->m_right;
	}
	n.Normalize();
	if (n.x == 0.f) n.x = 0.0001f;
	if (n.y == 0.f) n.y = 0.0001f;
	if (n.z == 0.f) n.z = 0.0001f;

	bqMat4 V;
	bqMat4 P;
	bqMath::LookAtRH(V, n, bqVec4(), bqVec4(0.f, 1.f, 0.f, 0.f));
	bqMath::OrthoRH(P, 100.f, 100.f, -1000.f, 1000.f);

	bqMat4 W;
	W.m_data[0].x = scale;
	W.m_data[1].y = scale;
	W.m_data[2].z = scale;

	bqMat4 VP = P * V * W;

	{
		auto cp = m_polygons.m_head;
		auto lp = cp->m_left;
		while (true)
		{
			auto cv = cp->m_data->m_vertices.m_head;
			auto lv = cv->m_left;
			while (true)
			{
				bqVec4 point = cv->m_data->m_data.BaseData.Position;// -center3d;
				point.w = 1.f;
				bqMath::Mul(VP, bqVec4(point), point);

				cv->m_data->m_data.BaseData.UV.x = (float)(point.x / point.w);
				cv->m_data->m_data.BaseData.UV.y = (float)(-point.y / point.w);

				if (cv == lv)
					break;
				cv = cv->m_right;
			}
			if (cp == lp)
				break;
			cp = cp->m_right;
		}
	}
}

void bqPolygonMesh::AddCylinder(
	float radius, 
	float height, 
	uint32_t segments, 
	bool topSide, 
	bool bottomSide, 
	const bqMat4& m)
{
	if (segments < 3)
		segments = 3;
	if (segments > 100)
		segments = 100;

	// сгенерирую точки по окружности, в соответствии с сегментами.
	// если 3 сегмента, то и точек 3, ну и так далее по возростанию.
	bqArray<bqVec3f> points;
	float angle = 0.f;
	float angleStep = 360.f / segments;

	for (uint32_t i = 0; i < segments; ++i)
	{
		auto sn = ::sin(bqMath::DegToRad(angle)) * radius;
		auto cs = ::cos(bqMath::DegToRad(angle)) * radius;

		points.push_back(bqVec3(cs, 0.0, sn));

		angle += angleStep;
	}

	// создам боковые стороны.
	// проход циклом по количеству сегментов
	// надо взять 2 точки из points, и использовать их
	// делая квадрат
	for (uint32_t i = 0; i < segments; ++i)
	{
		bqVec3f p1, p2, p3, p4;

		p1 = points[i];
		if (i)
			p2 = points[i - 1];
		else
			p2 = points[segments-1];

		p3 = p2;
		p4 = p1;

		p3.y += height;
		p4.y += height;

		bqMeshPolygonCreator pc;
		pc.SetPosition(p1);
		//pc.SetUV(bqVec2f(UV.x, UV.y + UVstep));
		pc.AddVertex();
		pc.SetPosition(p2);
		//pc.SetUV(bqVec2f(UV.x + (UVstep * 0.5f), UV.y));
		pc.AddVertex();
		pc.SetPosition(p3);
		//pc.SetUV(bqVec2f(UV.x + UVstep, UV.y + UVstep));
		pc.AddVertex();
		pc.SetPosition(p4);
		//pc.SetUV(bqVec2f(UV.x + UVstep, UV.y + UVstep));
		pc.AddVertex();
		pc.Mul(m);
		AddPolygon(&pc, true);
	}

	if (bottomSide)
	{
		bqMeshPolygonCreator pc;
		for (uint32_t i = 0; i < segments; ++i)
		{
			pc.SetPosition(points[i]);
			//pc.SetUV(bqVec2f(UV.x, UV.y + UVstep));
			pc.AddVertex();
		}
		pc.Mul(m);
		AddPolygon(&pc, true);
	}

	if (topSide)
	{
		bqMeshPolygonCreator pc;
		for (uint32_t i = 0; i < segments; ++i)
		{
			for (uint32_t i = segments - 1; i >= 0; --i)
			{
				pc.SetPosition(points[i] + bqVec3f(0.f, height, 0.f));
				//pc.SetUV(bqVec2f(UV.x, UV.y + UVstep));
				pc.AddVertex();

				if (i == 0)
					break;
			}
		}
		pc.Mul(m);
		AddPolygon(&pc, true);
	}
}