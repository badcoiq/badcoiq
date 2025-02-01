/*
BSD 2-Clause License

Copyright (c) 2025, badcoiq
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

#include "ModelEditor.h"
#include "CubeView.h"
#include "badcoiq/geometry/bqMeshCreator.h"
#include "badcoiq/geometry/bqTriangle.h"
#include "badcoiq/gs/bqGS.h"
#include "badcoiq/scene/bqCamera.h"
#include "badcoiq/input/bqInputEx.h"

extern ModelEditor* g_app;

CubeView::CubeView()
{
	for (size_t i = 0; i < meshID__size; ++i)
	{
		m_cubeViewMesh[i]=0;
		m_cubeViewGPUMesh[i] = 0;
		m_texture[i]=0;
	}
}

CubeView::~CubeView()
{
	for (size_t i = 0; i < meshID__size; ++i)
	{
		BQ_SAFEDESTROY(m_cubeViewMesh[i]);
		BQ_SAFEDESTROY(m_cubeViewGPUMesh[i]);

		if (i < meshID_TB)
		{
			BQ_SAFEDESTROY(m_texture[i]);
			BQ_SAFEDESTROY(m_texture_i[i]);
		}
	}

	BQ_SAFEDESTROY(m_texture[meshID_TB]);
	BQ_SAFEDESTROY(m_texture_i[meshID_TB]);
}


bool CubeView::Init()
{
	m_texture[meshID_top] = bqFramework::CreateTexture(g_app->m_gs,
		bqFramework::GetPath("../data/model_editor/cube_view_top.png").c_str(),
		false, false);
	m_texture[meshID_bottom] = bqFramework::CreateTexture(g_app->m_gs,
		bqFramework::GetPath("../data/model_editor/cube_view_bottom.png").c_str(),
		false, false);
	m_texture[meshID_left] = bqFramework::CreateTexture(g_app->m_gs,
		bqFramework::GetPath("../data/model_editor/cube_view_left.png").c_str(),
		false, false);
	m_texture[meshID_right] = bqFramework::CreateTexture(g_app->m_gs,
		bqFramework::GetPath("../data/model_editor/cube_view_right.png").c_str(),
		false, false);
	m_texture[meshID_front] = bqFramework::CreateTexture(g_app->m_gs,
		bqFramework::GetPath("../data/model_editor/cube_view_front.png").c_str(),
		false, false);
	m_texture[meshID_back] = bqFramework::CreateTexture(g_app->m_gs,
		bqFramework::GetPath("../data/model_editor/cube_view_back.png").c_str(),
		false, false);
	m_texture[meshID_back] = bqFramework::CreateTexture(g_app->m_gs,
		bqFramework::GetPath("../data/model_editor/cube_view_back.png").c_str(),
		false, false);

	bqTexture* textureBG = bqFramework::CreateTexture(g_app->m_gs,
		bqFramework::GetPath("../data/model_editor/cube_view_bg.png").c_str(),
		false, false);
	bqTexture* textureBGi = bqFramework::CreateTexture(g_app->m_gs,
		bqFramework::GetPath("../data/model_editor/cube_view_bgi.png").c_str(),
		false, false);

	m_texture[meshID_TB] = textureBG;
	m_texture_i[meshID_TB] = textureBGi;

	m_texture_i[meshID_top] = bqFramework::CreateTexture(g_app->m_gs,
		bqFramework::GetPath("../data/model_editor/cube_view_topi.png").c_str(),
		false, false);
	m_texture_i[meshID_bottom] = bqFramework::CreateTexture(g_app->m_gs,
		bqFramework::GetPath("../data/model_editor/cube_view_bottomi.png").c_str(),
		false, false);
	m_texture_i[meshID_left] = bqFramework::CreateTexture(g_app->m_gs,
		bqFramework::GetPath("../data/model_editor/cube_view_lefti.png").c_str(),
		false, false);
	m_texture_i[meshID_right] = bqFramework::CreateTexture(g_app->m_gs,
		bqFramework::GetPath("../data/model_editor/cube_view_righti.png").c_str(),
		false, false);
	m_texture_i[meshID_front] = bqFramework::CreateTexture(g_app->m_gs,
		bqFramework::GetPath("../data/model_editor/cube_view_fronti.png").c_str(),
		false, false);
	m_texture_i[meshID_back] = bqFramework::CreateTexture(g_app->m_gs,
		bqFramework::GetPath("../data/model_editor/cube_view_backi.png").c_str(),
		false, false);

	float32_t Size1 = 0.8f;
	float32_t Size2 = 1.f - Size1;

	{
		bqMat4 transform;
		transform.SetRotation(bqQuaternion(0, 0, 0));
		transform.SetTranslation(bqVec4(0.f, 1, 0., 0.));
		bqPolygonMesh pm;
		pm.AddQuad(bqVec4f(-Size1, 0.f, -Size1, 0.f), bqVec4f(Size1, 0.f, Size1, 0.f), transform);
		pm.GenerateNormals(false);
		m_cubeViewMesh[CubeView::meshID_top] = pm.CreateMesh();
		if (m_cubeViewMesh[CubeView::meshID_top])
			m_cubeViewGPUMesh[CubeView::meshID_top] = g_app->m_gs->CreateMesh(m_cubeViewMesh[CubeView::meshID_top]);
	}

	{
		bqMat4 transform;
		transform.SetRotation(bqQuaternion(PI, 0, 0));
		transform.SetTranslation(bqVec4(0.f, -1, 0., 0.));
		bqPolygonMesh pm;
		pm.AddQuad(bqVec4f(-Size1, 0.f, -Size1, 0.f), bqVec4f(Size1, 0.f, Size1, 0.f), transform);
		pm.GenerateNormals(false);
		m_cubeViewMesh[CubeView::meshID_bottom] = pm.CreateMesh();
		if (m_cubeViewMesh[CubeView::meshID_bottom])
			m_cubeViewGPUMesh[CubeView::meshID_bottom] = g_app->m_gs->CreateMesh(m_cubeViewMesh[CubeView::meshID_bottom]);
	}

	{
		bqMat4 transform;
		transform.SetRotation(bqQuaternion(0, 0, -PIHalf));
		transform = transform * bqMat4(bqQuaternion(0, -PIHalf, 0));
		transform.SetTranslation(bqVec4(-1, 0, 0., 0.));
		bqPolygonMesh pm;
		pm.AddQuad(bqVec4f(-Size1, 0.f, -Size1, 0.f), bqVec4f(Size1, 0.f, Size1, 0.f), transform);
		pm.GenerateNormals(false);
		m_cubeViewMesh[CubeView::meshID_right] = pm.CreateMesh();
		if (m_cubeViewMesh[CubeView::meshID_right])
			m_cubeViewGPUMesh[CubeView::meshID_right] = g_app->m_gs->CreateMesh(m_cubeViewMesh[CubeView::meshID_right]);
	}

	{
		bqMat4 transform;
		transform.SetRotation(bqQuaternion(0, 0, PIHalf));
		transform = transform * bqMat4(bqQuaternion(0, PIHalf, 0));
		transform.SetTranslation(bqVec4(1, 0, 0., 0.));
		bqPolygonMesh pm;
		pm.AddQuad(bqVec4f(-Size1, 0.f, -Size1, 0.f), bqVec4f(Size1, 0.f, Size1, 0.f), transform);
		pm.GenerateNormals(false);
		m_cubeViewMesh[CubeView::meshID_left] = pm.CreateMesh();
		if (m_cubeViewMesh[CubeView::meshID_left])
			m_cubeViewGPUMesh[CubeView::meshID_left] = g_app->m_gs->CreateMesh(m_cubeViewMesh[CubeView::meshID_left]);
	}

	{
		bqMat4 transform;
		transform.SetRotation(bqQuaternion(PIHalf, 0, 0));
		transform.SetTranslation(bqVec4(0, 0, -1., 0.));
		bqPolygonMesh pm;
		pm.AddQuad(bqVec4f(-Size1, 0.f, -Size1, 0.f), bqVec4f(Size1, 0.f, Size1, 0.f), transform);
		pm.GenerateNormals(false);
		m_cubeViewMesh[CubeView::meshID_back] = pm.CreateMesh();
		if (m_cubeViewMesh[CubeView::meshID_back])
			m_cubeViewGPUMesh[CubeView::meshID_back] = g_app->m_gs->CreateMesh(m_cubeViewMesh[CubeView::meshID_back]);
	}

	{
		bqMat4 transform;
		transform.SetRotation(bqQuaternion(-PIHalf, 0, 0));
		transform = transform * bqMat4(bqQuaternion(0, PI, 0));
		transform.SetTranslation(bqVec4(0, 0, 1., 0.));
		bqPolygonMesh pm;
		pm.AddQuad(bqVec4f(-Size1, 0.f, -Size1, 0.f), bqVec4f(Size1, 0.f, Size1, 0.f), transform);
		pm.GenerateNormals(false);
		m_cubeViewMesh[CubeView::meshID_front] = pm.CreateMesh();
		if (m_cubeViewMesh[CubeView::meshID_front])
			m_cubeViewGPUMesh[CubeView::meshID_front] = g_app->m_gs->CreateMesh(m_cubeViewMesh[CubeView::meshID_front]);
	}

	{
		bqMat4 transform;
		transform.SetTranslation(bqVec4(0.f, 1, 0., 0.));
		bqPolygonMesh pm;
		bqMeshPolygonCreator pc;
		pc.SetPosition(bqVec3f(-Size1, 0.f, -1.f));
		pc.AddVertex();
		pc.SetPosition(bqVec3f(-Size1, 0.f, -1 + Size2));
		pc.AddVertex();
		pc.SetPosition(bqVec3f(Size1, 0.f, -1 + Size2));
		pc.AddVertex();
		pc.SetPosition(bqVec3f(Size1, 0.f, -1.f));
		pc.AddVertex();
		pc.Mul(transform);
		pm.AddPolygon(&pc, false);

		transform.SetTranslation(bqVec4(0.f, 2- Size2, -1., 0.));
		transform.SetRotation(bqQuaternion(PIHalf, 0, 0));
		pc.SetPosition(bqVec3f(-Size1, 0.f, -1.f));
		pc.AddVertex();
		pc.SetPosition(bqVec3f(-Size1, 0.f, -1 + Size2));
		pc.AddVertex();
		pc.SetPosition(bqVec3f(Size1, 0.f, -1 + Size2));
		pc.AddVertex();
		pc.SetPosition(bqVec3f(Size1, 0.f, -1.f));
		pc.AddVertex();
		pc.Mul(transform);
		pm.AddPolygon(&pc, false);

		pm.GenerateNormals(false);

		m_cubeViewMesh[CubeView::meshID_TB] = pm.CreateMesh();
		if (m_cubeViewMesh[CubeView::meshID_TB])
			m_cubeViewGPUMesh[CubeView::meshID_TB] = g_app->m_gs->CreateMesh(m_cubeViewMesh[CubeView::meshID_TB]);
	}

	return true;
}

void CubeView::Draw(bqCamera* camera)
{
	static bqMat4 WVP;
	static bqMat4 W;
	WVP = camera->m_projectionMatrix * camera->m_viewMatrix * W;
	bqFramework::SetMatrix(bqMatrixType::WorldViewProjection, &WVP);
	bqFramework::SetMatrix(bqMatrixType::World, &W);
	g_app->m_gs->EnableDepth();
	static bqMaterial material;
	material.m_shaderType = bqShaderType::Standart;
	material.m_sunPosition = camera->m_position;
	material.m_sunPosition.Normalize();
	g_app->m_gs->SetShader(material.m_shaderType, 0);
	g_app->m_gs->SetMaterial(&material);

	for (size_t i = 0; i < meshID__size; ++i)
	{
		if (m_cubeViewGPUMesh[i])
		{
			if(m_texture[i])
				material.m_maps[0].m_texture = m_texture[i];

			if (m_hoverMesh == i)
			{
				if (m_texture_i[i])
					material.m_maps[0].m_texture = m_texture_i[i];
			}

			g_app->m_gs->SetMesh(m_cubeViewGPUMesh[i]);
			g_app->m_gs->Draw();
		}
	}
}

uint32_t CubeView::IsMouseRayIntersect(bqCamera* c, const bqVec4f& rc)
{
	bqRay r;
	r.CreateFrom2DCoords(
		bqVec2f(g_app->m_inputData->m_mousePosition.x - rc.x, 
			g_app->m_inputData->m_mousePosition.y - rc.y),
		bqVec2f(rc.z - rc.x, rc.w - rc.y),
		c->m_viewProjectionInvertMatrix, g_app->m_gs->GetDepthRange());

	r.Update();

	uint32_t result = meshID__size;
	
	/// тут надо понять какое пересечение было ближ к камере
	bqReal minT = 99999;
	for (uint32_t i = 0; i < meshID__size; ++i)
	{
		bqReal T = 0;
		if (_IsMouseRayIntersect(r, i, T) != meshID__size)
		{
			if (T < minT)
			{
				minT = T;
				result = i;
			}
		}
	}

	/*if (result == meshID__size) result = _IsMouseRayIntersect(r, meshID_bottom, T);
	if (result == meshID__size) result = _IsMouseRayIntersect(r, meshID_left, T);
	if (result == meshID__size) result = _IsMouseRayIntersect(r, meshID_right, T);
	if (result == meshID__size) result = _IsMouseRayIntersect(r, meshID_front, T);
	if (result == meshID__size) result = _IsMouseRayIntersect(r, meshID_back, T);*/

	m_hoverMesh = result;

	return result;
}

uint32_t CubeView::_IsMouseRayIntersect(const bqRay& r, uint32_t meshID, bqReal& T)
{
	T = 999999;

	bqTriangle tr;
	{
		uint32_t triNum = m_cubeViewMesh[meshID]->GetInfo().m_iCount / 3;
		bqVertexTriangle* verts = (bqVertexTriangle*)m_cubeViewMesh[meshID]->GetVBuffer();
		uint16_t* inds = (uint16_t*)m_cubeViewMesh[meshID]->GetIBuffer();
		for (uint32_t i = 0, ind = 0; i < triNum; ++i)
		{

			bqReal  U, V, W;
			tr.v1 = verts[inds[ind]].Position;
			tr.v2 = verts[inds[ind + 1]].Position;
			tr.v3 = verts[inds[ind + 2]].Position;
			tr.Update();
			T = U = V = W = 0;
			if (tr.RayIntersect_MT(r, true, T, U, V, W, true))
				return meshID;
		
			ind += 3;
			/*tr.v1 = verts[0].Position;
			tr.v2 = verts[2].Position;
			tr.v3 = verts[3].Position;
			tr.Update();
			T = U = V = W = 0;
			if (tr.RayIntersect_MT(r, true, T, U, V, W, true))
				return meshID;*/
		}
	}
	return meshID__size;
}