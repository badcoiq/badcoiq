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

///
/// Тут просто иинициализация 3Д модели сетки
///


#include "ModelEditor.h"
#include "Grid.h"

#include "badcoiq/geometry/bqMesh.h"
#include "badcoiq/gs/bqGS.h"

extern ModelEditor* g_app;

void ModelEditor::_initGrid()
{

	auto _get_mesh = [&](int32_t linesNum)->bqMesh*
		{
			bqMesh* mesh = new bqMesh();
			mesh->GetInfo().m_stride = sizeof(bqVertexLine);
			mesh->GetInfo().m_vertexType = bqMeshVertexType::Line;
			mesh->GetInfo().m_indexType = bqMeshIndexType::u16;
			mesh->GetInfo().m_vCount = linesNum * 2;
			mesh->GetInfo().m_iCount = linesNum * 2;
			auto vb = (uint8_t*)bqMemory::malloc(
				mesh->GetInfo().m_vCount 
				* mesh->GetInfo().m_stride);
			auto ib = (uint8_t*)bqMemory::malloc(
				mesh->GetInfo().m_iCount
				* sizeof(uint16_t));
			mesh->SetVBuffer(vb);
			mesh->SetIBuffer(ib);
			return mesh;
		};

	bqColor colorBase(180, 180, 180, 255);
	bqVec4f colorRed(1.f, 0.f, 0.f, 1.f);
	bqVec4f colorGreen(0.f, 1.f, 0.f, 1.f);

	auto _build = [&](bqMesh* mesh, float32_t minimum, int32_t linesNum, float32_t step,
		const bqColor& half1Color, const bqColor& half2Color, const bqColor& specColor1,
		const bqColor& specColor2, int32_t specColorIndex,
		uint32_t ct
		)
		{
			auto vertex = (bqVertexLine*)mesh->GetVBuffer();
			float32_t pos = minimum;

			const int32_t halfNum = linesNum / 2;

			for (int32_t i = 0; i < halfNum; ++i)
			{
				bqColor color = half1Color;

				if (i == specColorIndex)
					color = specColor1;

				switch (ct)
				{
				case ViewportView::type_left:
				case ViewportView::type_right:
					vertex->Position.Set(0.f, minimum, pos);
					vertex->Color = color.GetVec4();
					vertex++;
					vertex->Position.Set(0.f, std::abs(minimum), pos);
					vertex->Color = color.GetVec4();
					vertex++;
					break;
				case ViewportView::type_perspective:
				case ViewportView::type_top:
				case ViewportView::type_bottom:
					vertex->Position.Set(pos, 0.f, minimum);
					vertex->Color = color.GetVec4();
					vertex++;
					vertex->Position.Set(pos, 0.f, std::abs(minimum));
					vertex->Color = color.GetVec4();
					vertex++;
					break;
				case ViewportView::type_front:
				case ViewportView::type_back:
					vertex->Position.Set(pos, minimum, 0.f);
					vertex->Color = color.GetVec4();
					vertex++;
					vertex->Position.Set(pos, std::abs(minimum), 0.f);
					vertex->Color = color.GetVec4();
					vertex++;
					break;
				default:
					break;
				}

				pos += step;
			}

			pos = minimum;
			for (int32_t i = 0; i < halfNum; ++i)
			{
				bqColor color = half2Color;

				if (i == specColorIndex)
					color = specColor2;

				switch (ct)
				{
				case ViewportView::type_left:
				case ViewportView::type_right:
					vertex->Position.Set(0.f, pos, -minimum);
					vertex->Color = color.GetVec4();
					vertex++;
					vertex->Position.Set(0.f, pos, minimum);
					vertex->Color = color.GetVec4();
					vertex++;
					break;
				case ViewportView::type_perspective:
				case ViewportView::type_top:
				case ViewportView::type_bottom:
					vertex->Position.Set(-minimum, 0.f, pos);
					vertex->Color = color.GetVec4();
					vertex++;
					vertex->Position.Set(minimum, 0.f, pos);
					vertex->Color = color.GetVec4();
					vertex++;
					break;
				case ViewportView::type_front:
				case ViewportView::type_back:
					vertex->Position.Set(-minimum, pos, 0.f);
					vertex->Color = color.GetVec4();
					vertex++;
					vertex->Position.Set(minimum, pos, 0.f);
					vertex->Color = color.GetVec4();
					vertex++;
					break;
				default:
					break;
				}

				pos += step;
			}

			uint16_t* index = (uint16_t*)mesh->GetIBuffer();
			for (uint32_t i = 0; i < mesh->GetInfo().m_iCount; ++i)
			{
				*index = (uint16_t)i;
				index++;
			}
		};

	{
		const int32_t linesNum = 22;
		auto mesh = _get_mesh(linesNum);

		_build(mesh, -5.f, linesNum, 1.f, colorBase, colorBase, bq::ColorLime, bq::ColorRed, 5, ViewportView::type_perspective);
		m_gridModel_perspective1 = m_gs->CreateMesh(mesh);

		colorBase = bqColor(150, 150, 150, 255);
		_build(mesh, -5.f, linesNum, 1.f, colorBase, colorBase, colorBase, colorBase, 5, ViewportView::type_perspective);
		m_gridModel_perspective2 = m_gs->CreateMesh(mesh);
		bqDestroy(mesh);
	}
	{
		const int32_t linesNum = 1002;
		auto mesh = _get_mesh(linesNum);

		_build(mesh, -251.f, linesNum, 1.f, colorBase, colorBase, bq::ColorLime, bq::ColorRed, 251, ViewportView::type_top);
		m_gridModel_top1 = m_gs->CreateMesh(mesh);

		colorBase = bqColor(150, 150, 150, 255);
		_build(mesh, -251.f, linesNum, 1.f, colorBase, colorBase, colorBase, colorBase, 251, ViewportView::type_top);
		m_gridModel_top2 = m_gs->CreateMesh(mesh);
		bqDestroy(mesh);
	}
	{
		const int32_t linesNum = 1002;
		auto mesh = _get_mesh(linesNum);

		_build(mesh, -251.f, linesNum, 1.f, colorBase, colorBase, bq::ColorBlue, bq::ColorRed, 251, ViewportView::type_front);
		m_gridModel_front1 = m_gs->CreateMesh(mesh);

		colorBase = bqColor(150, 150, 150, 255);
		_build(mesh, -251.f, linesNum, 1.f, colorBase, colorBase, colorBase, colorBase, 251, ViewportView::type_front);
		m_gridModel_front2 = m_gs->CreateMesh(mesh);
		bqDestroy(mesh);
	}
	{
		const int32_t linesNum = 1002;
		auto mesh = _get_mesh(linesNum);

		_build(mesh, -251.f, linesNum, 1.f, colorBase, colorBase, bq::ColorBlue, bq::ColorLime, 251, ViewportView::type_left);
		m_gridModel_left1 = m_gs->CreateMesh(mesh);

		colorBase = bqColor(150, 150, 150, 255);
		_build(mesh, -251.f, linesNum, 1.f, colorBase, colorBase, colorBase, colorBase, 251, ViewportView::type_left);
		m_gridModel_left2 = m_gs->CreateMesh(mesh);

		bqDestroy(mesh);
	}
}


