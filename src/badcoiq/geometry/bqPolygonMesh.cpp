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

void bqPolygonMeshPolygon::Clear()
{
	if (m_vertices.m_head)
	{
		auto curr = m_vertices.m_head;
		auto last = m_vertices.m_head->m_left;
		while (true)
		{
			auto next = curr->m_right;

			delete curr;

			if (curr == last)
				break;

			curr = next;
		}

		m_vertices.clear();
		m_controlPoints.clear();
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
				}

				if (cp)
				{
					cp->m_vertices.push_back(v);

					// надо сунуть управляющую вершину в полигон
					// не надо дублировать. если её нет то добавить
					if (!newPolygon->m_controlPoints.find(cp))
					{
						newPolygon->m_controlPoints.push_back(cp);
					}
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

bqMesh* bqPolygonMesh::SummonMesh()
{
	bqMesh* m = 0;

	if (m_polygons.m_head)
	{
		for (auto o : m_polygons)
		{
			o.
		}
	}

	return m;
}
