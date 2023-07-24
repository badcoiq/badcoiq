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

			for (auto p : m_polygons)
			{
				std::map<std::string, Container> map;
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

	bqVec3f _n;
	auto vertex_1 = m_vertices.m_head;
	auto vertex_3 = vertex_1->m_right;
	auto vertex_2 = vertex_3->m_right;
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
				for (uint32_t i = 0; i < vn; ++i)
				{
					switch (m->GetInfo().m_indexType)
					{
					case bqMeshIndexType::u16:
						*ind16 = baseIndex; ++ind16;
						*ind16 = index + 1; ++ind16;
						*ind16 = index + 2; ++ind16;
						break;
					case bqMeshIndexType::u32:
						*ind32 = baseIndex; ++ind32;
						*ind32 = index + 1; ++ind32;
						*ind32 = index + 2; ++ind32;
						break;
					}

					++index;
				}
			}
		}
	}

	return m;
}

bool bqPolygonMeshPolygon::IsVisible()
{
	auto vertex_1 = m_vertices.m_head;
	auto vertex_3 = vertex_1->m_right;
	auto vertex_2 = vertex_3->m_right;
	while (true)
	{
		auto a = vertex_2->m_data->m_data.BaseData.Position - vertex_1->m_data->m_data.BaseData.Position;
		auto b = vertex_3->m_data->m_data.BaseData.Position - vertex_1->m_data->m_data.BaseData.Position;

		bqVec3f n;
		a.Cross(b, n);

		float area = 0.5f * sqrt(n.Dot());

		if (area > 0.001)
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
			DeletePolygon(o);
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
