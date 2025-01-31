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
#include "badcoiq/gs/bqGS.h"
#include "badcoiq/scene/bqCamera.h"

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
		BQ_SAFEDESTROY(m_texture[i]);
	}
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
	{
		bqMat4 transform;
		transform.SetRotation(bqQuaternion(0, 0, 0));
		transform.SetTranslation(bqVec4(0.f, 1, 0., 0.));
		bqPolygonMesh pm;
		pm.AddQuad(bqVec4f(-1.f, 0.f, -1.f, 0.f), bqVec4f(1.f, 0.f, 1.f, 0.f), transform);
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
		pm.AddQuad(bqVec4f(-1.f, 0.f, -1.f, 0.f), bqVec4f(1.f, 0.f, 1.f, 0.f), transform);
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
		pm.AddQuad(bqVec4f(-1.f, 0.f, -1.f, 0.f), bqVec4f(1.f, 0.f, 1.f, 0.f), transform);
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
		pm.AddQuad(bqVec4f(-1.f, 0.f, -1.f, 0.f), bqVec4f(1.f, 0.f, 1.f, 0.f), transform);
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
		pm.AddQuad(bqVec4f(-1.f, 0.f, -1.f, 0.f), bqVec4f(1.f, 0.f, 1.f, 0.f), transform);
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
		pm.AddQuad(bqVec4f(-1.f, 0.f, -1.f, 0.f), bqVec4f(1.f, 0.f, 1.f, 0.f), transform);
		pm.GenerateNormals(false);
		m_cubeViewMesh[CubeView::meshID_front] = pm.CreateMesh();
		if (m_cubeViewMesh[CubeView::meshID_front])
			m_cubeViewGPUMesh[CubeView::meshID_front] = g_app->m_gs->CreateMesh(m_cubeViewMesh[CubeView::meshID_front]);
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

			g_app->m_gs->SetMesh(m_cubeViewGPUMesh[i]);
			g_app->m_gs->Draw();
		}
	}
}

