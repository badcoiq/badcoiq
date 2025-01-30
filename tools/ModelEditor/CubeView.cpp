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
		m_cubeViewGPUMesh[i]=0;
	}
}

CubeView::~CubeView()
{
	for (size_t i = 0; i < meshID__size; ++i)
	{
		BQ_SAFEDESTROY(m_cubeViewMesh[i]);
		BQ_SAFEDESTROY(m_cubeViewGPUMesh[i]);
	}
}


bool CubeView::Init()
{
	{
		bqMat4 transform;
		bqPolygonMesh pm;
		pm.AddCube(1.f, transform);
		pm.GenerateNormals(false);
		m_cubeViewMesh[CubeView::meshID_top] = pm.CreateMesh();
		if (m_cubeViewMesh[CubeView::meshID_top])
		{
			m_cubeViewGPUMesh[CubeView::meshID_top] = g_app->m_gs->CreateMesh(m_cubeViewMesh[CubeView::meshID_top]);

		}
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
	g_app->m_gs->SetShader(material.m_shaderType, 0);
	g_app->m_gs->SetMaterial(&material);

	for (size_t i = 0; i < meshID__size; ++i)
	{
		if (m_cubeViewGPUMesh[i])
		{
			g_app->m_gs->SetMesh(m_cubeViewGPUMesh[i]);
			g_app->m_gs->Draw();
		}
	}
}

