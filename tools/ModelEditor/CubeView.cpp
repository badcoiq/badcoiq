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
#include "Viewport.h"
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
	m_texture[meshID_TL] = textureBG;
	m_texture_i[meshID_TL] = textureBGi;
	m_texture[meshID_TF] = textureBG;
	m_texture_i[meshID_TF] = textureBGi;
	m_texture[meshID_TR] = textureBG;
	m_texture_i[meshID_TR] = textureBGi;
	m_texture[meshID_BckR] = textureBG;
	m_texture_i[meshID_BckR] = textureBGi;
	m_texture[meshID_BckL] = textureBG;
	m_texture_i[meshID_BckL] = textureBGi;
	m_texture[meshID_FL] = textureBG;
	m_texture_i[meshID_FL] = textureBGi;
	m_texture[meshID_FR] = textureBG;
	m_texture_i[meshID_FR] = textureBGi;
	m_texture[meshID_BR] = textureBG;
	m_texture_i[meshID_BR] = textureBGi;
	m_texture[meshID_BB] = textureBG;
	m_texture_i[meshID_BB] = textureBGi;
	m_texture[meshID_BL] = textureBG;
	m_texture_i[meshID_BL] = textureBGi;
	m_texture[meshID_BF] = textureBG;
	m_texture_i[meshID_BF] = textureBGi;
	m_texture[meshID_TRB] = textureBG;
	m_texture_i[meshID_TRB] = textureBGi;
	m_texture[meshID_TLB] = textureBG;
	m_texture_i[meshID_TLB] = textureBGi;
	m_texture[meshID_TLF] = textureBG;
	m_texture_i[meshID_TLF] = textureBGi;
	m_texture[meshID_TRF] = textureBG;
	m_texture_i[meshID_TRF] = textureBGi;
	m_texture[meshID_BRF] = textureBG;
	m_texture_i[meshID_BRF] = textureBGi;
	m_texture[meshID_BRB] = textureBG;
	m_texture_i[meshID_BRB] = textureBGi;
	m_texture[meshID_BLB] = textureBG;
	m_texture_i[meshID_BLB] = textureBGi;
	m_texture[meshID_BLF] = textureBG;
	m_texture_i[meshID_BLF] = textureBGi;


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

	float32_t Size1 = 0.7f;
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
		
		transform.Identity();
		transform.SetRotation(bqQuaternion(0, PIf, 0));
		pm.ApplyMatrix(transform);

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

		transform.SetTranslation(bqVec4(0.f, 0, 0., 0.));
		transform.SetRotation(bqQuaternion(0, PIHalf, 0));
		pm.ApplyMatrix(transform);
		pm.GenerateNormals(false);
		m_cubeViewMesh[CubeView::meshID_TL] = pm.CreateMesh();
		if (m_cubeViewMesh[CubeView::meshID_TL])
			m_cubeViewGPUMesh[CubeView::meshID_TL] = g_app->m_gs->CreateMesh(m_cubeViewMesh[CubeView::meshID_TL]);

		transform.SetRotation(bqQuaternion(0, PIHalf, 0));
		pm.ApplyMatrix(transform);
		pm.GenerateNormals(false);
		m_cubeViewMesh[CubeView::meshID_TF] = pm.CreateMesh();
		if (m_cubeViewMesh[CubeView::meshID_TF])
			m_cubeViewGPUMesh[CubeView::meshID_TF] = g_app->m_gs->CreateMesh(m_cubeViewMesh[CubeView::meshID_TF]);

		transform.SetRotation(bqQuaternion(0, PIHalf, 0));
		pm.ApplyMatrix(transform);
		pm.GenerateNormals(false);
		m_cubeViewMesh[CubeView::meshID_TR] = pm.CreateMesh();
		if (m_cubeViewMesh[CubeView::meshID_TR])
			m_cubeViewGPUMesh[CubeView::meshID_TR] = g_app->m_gs->CreateMesh(m_cubeViewMesh[CubeView::meshID_TR]);

		transform.SetRotation(bqQuaternion(PIHalf, 0, 0));
		pm.ApplyMatrix(transform);
		pm.GenerateNormals(false);
		m_cubeViewMesh[CubeView::meshID_BckR] = pm.CreateMesh();
		if (m_cubeViewMesh[CubeView::meshID_BckR])
			m_cubeViewGPUMesh[CubeView::meshID_BckR] = g_app->m_gs->CreateMesh(m_cubeViewMesh[CubeView::meshID_BckR]);

		transform.SetRotation(bqQuaternion(0, PIHalf, 0));
		pm.ApplyMatrix(transform);
		pm.GenerateNormals(false);
		m_cubeViewMesh[CubeView::meshID_BckL] = pm.CreateMesh();
		if (m_cubeViewMesh[CubeView::meshID_BckL])
			m_cubeViewGPUMesh[CubeView::meshID_BckL] = g_app->m_gs->CreateMesh(m_cubeViewMesh[CubeView::meshID_BckL]);

		transform.SetRotation(bqQuaternion(0, PIHalf, 0));
		pm.ApplyMatrix(transform);
		pm.GenerateNormals(false);
		m_cubeViewMesh[CubeView::meshID_FL] = pm.CreateMesh();
		if (m_cubeViewMesh[CubeView::meshID_FL])
			m_cubeViewGPUMesh[CubeView::meshID_FL] = g_app->m_gs->CreateMesh(m_cubeViewMesh[CubeView::meshID_FL]);

		transform.SetRotation(bqQuaternion(0, PIHalf, 0));
		pm.ApplyMatrix(transform);
		pm.GenerateNormals(false);
		m_cubeViewMesh[CubeView::meshID_FR] = pm.CreateMesh();
		if (m_cubeViewMesh[CubeView::meshID_FR])
			m_cubeViewGPUMesh[CubeView::meshID_FR] = g_app->m_gs->CreateMesh(m_cubeViewMesh[CubeView::meshID_FR]);

		transform.SetRotation(bqQuaternion(0, PIHalf, 0));
		pm.ApplyMatrix(transform);
		transform.SetRotation(bqQuaternion(PIHalf, 0, 0));
		pm.ApplyMatrix(transform);
		pm.GenerateNormals(false);
		m_cubeViewMesh[CubeView::meshID_BR] = pm.CreateMesh();
		if (m_cubeViewMesh[CubeView::meshID_BR])
			m_cubeViewGPUMesh[CubeView::meshID_BR] = g_app->m_gs->CreateMesh(m_cubeViewMesh[CubeView::meshID_BR]);

		transform.SetRotation(bqQuaternion(0, PIHalf, 0));
		pm.ApplyMatrix(transform);
		pm.GenerateNormals(false);
		m_cubeViewMesh[CubeView::meshID_BB] = pm.CreateMesh();
		if (m_cubeViewMesh[CubeView::meshID_BB])
			m_cubeViewGPUMesh[CubeView::meshID_BB] = g_app->m_gs->CreateMesh(m_cubeViewMesh[CubeView::meshID_BB]);

		transform.SetRotation(bqQuaternion(0, PIHalf, 0));
		pm.ApplyMatrix(transform);
		pm.GenerateNormals(false);
		m_cubeViewMesh[CubeView::meshID_BL] = pm.CreateMesh();
		if (m_cubeViewMesh[CubeView::meshID_BL])
			m_cubeViewGPUMesh[CubeView::meshID_BL] = g_app->m_gs->CreateMesh(m_cubeViewMesh[CubeView::meshID_BL]);

		transform.SetRotation(bqQuaternion(0, PIHalf, 0));
		pm.ApplyMatrix(transform);
		pm.GenerateNormals(false);
		m_cubeViewMesh[CubeView::meshID_BF] = pm.CreateMesh();
		if (m_cubeViewMesh[CubeView::meshID_BF])
			m_cubeViewGPUMesh[CubeView::meshID_BF] = g_app->m_gs->CreateMesh(m_cubeViewMesh[CubeView::meshID_BF]);
	}

	{
		bqMat4 transform;
		//transform.SetTranslation(bqVec4(0.f, 1, 0., 0.));
		bqPolygonMesh pm;
		bqMeshPolygonCreator pc;
		pc.SetPosition(bqVec3f(-1.f, 1.f, -1.f));
		pc.AddVertex();
		pc.SetPosition(bqVec3f(-1.f, 1.f, -1 + Size2));
		pc.AddVertex();
		pc.SetPosition(bqVec3f(-1.f + Size2, 1.f, -1 + Size2));
		pc.AddVertex();
		pc.SetPosition(bqVec3f(-1.f + Size2, 1.f, -1.f));
		pc.AddVertex();
		pc.Mul(transform);
		pm.AddPolygon(&pc, false);

		pc.SetPosition(bqVec3f(-1.f, 1.f - Size2, -1.f));
		pc.AddVertex();
		pc.SetPosition(bqVec3f(-1.f, 1.f - Size2, -1 + Size2));
		pc.AddVertex();
		pc.SetPosition(bqVec3f(-1.f, 1.f, -1 + Size2));
		pc.AddVertex();
		pc.SetPosition(bqVec3f(-1.f, 1.f, -1.f));
		pc.AddVertex();
		pc.Mul(transform);
		pm.AddPolygon(&pc, false);

		pc.SetPosition(bqVec3f(-1.f, 1.f - Size2, -1.f));
		pc.AddVertex();
		pc.SetPosition(bqVec3f(-1.f, 1.f, -1));
		pc.AddVertex();
		pc.SetPosition(bqVec3f(-1.f + Size2, 1.f, -1));
		pc.AddVertex();
		pc.SetPosition(bqVec3f(-1.f + Size2, 1.f - Size2, -1.f));
		pc.AddVertex();
		pc.Mul(transform);
		pm.AddPolygon(&pc, false);

		pm.GenerateNormals(false);

		m_cubeViewMesh[CubeView::meshID_TRB] = pm.CreateMesh();
		if (m_cubeViewMesh[CubeView::meshID_TRB])
			m_cubeViewGPUMesh[CubeView::meshID_TRB] = g_app->m_gs->CreateMesh(m_cubeViewMesh[CubeView::meshID_TRB]);
	
		transform.SetRotation(bqQuaternion(0, PIHalf, 0));
		pm.ApplyMatrix(transform);
		pm.GenerateNormals(false);
		m_cubeViewMesh[CubeView::meshID_TLB] = pm.CreateMesh();
		if (m_cubeViewMesh[CubeView::meshID_TLB])
			m_cubeViewGPUMesh[CubeView::meshID_TLB] = g_app->m_gs->CreateMesh(m_cubeViewMesh[CubeView::meshID_TLB]);

		transform.SetRotation(bqQuaternion(0, PIHalf, 0));
		pm.ApplyMatrix(transform);
		pm.GenerateNormals(false);
		m_cubeViewMesh[CubeView::meshID_TLF] = pm.CreateMesh();
		if (m_cubeViewMesh[CubeView::meshID_TLF])
			m_cubeViewGPUMesh[CubeView::meshID_TLF] = g_app->m_gs->CreateMesh(m_cubeViewMesh[CubeView::meshID_TLF]);

		transform.SetRotation(bqQuaternion(0, PIHalf, 0));
		pm.ApplyMatrix(transform);
		pm.GenerateNormals(false);
		m_cubeViewMesh[CubeView::meshID_TRF] = pm.CreateMesh();
		if (m_cubeViewMesh[CubeView::meshID_TRF])
			m_cubeViewGPUMesh[CubeView::meshID_TRF] = g_app->m_gs->CreateMesh(m_cubeViewMesh[CubeView::meshID_TRF]);

		transform.SetRotation(bqQuaternion(-PIHalf, 0, 0));
		pm.ApplyMatrix(transform);
		pm.GenerateNormals(false);
		m_cubeViewMesh[CubeView::meshID_BRF] = pm.CreateMesh();
		if (m_cubeViewMesh[CubeView::meshID_BRF])
			m_cubeViewGPUMesh[CubeView::meshID_BRF] = g_app->m_gs->CreateMesh(m_cubeViewMesh[CubeView::meshID_BRF]);

		transform.SetRotation(bqQuaternion(0, PIHalf, 0));
		pm.ApplyMatrix(transform);
		pm.GenerateNormals(false);
		m_cubeViewMesh[CubeView::meshID_BRB] = pm.CreateMesh();
		if (m_cubeViewMesh[CubeView::meshID_BRB])
			m_cubeViewGPUMesh[CubeView::meshID_BRB] = g_app->m_gs->CreateMesh(m_cubeViewMesh[CubeView::meshID_BRB]);

		transform.SetRotation(bqQuaternion(0, PIHalf, 0));
		pm.ApplyMatrix(transform);
		pm.GenerateNormals(false);
		m_cubeViewMesh[CubeView::meshID_BLB] = pm.CreateMesh();
		if (m_cubeViewMesh[CubeView::meshID_BLB])
			m_cubeViewGPUMesh[CubeView::meshID_BLB] = g_app->m_gs->CreateMesh(m_cubeViewMesh[CubeView::meshID_BLB]);

		transform.SetRotation(bqQuaternion(0, PIHalf, 0));
		pm.ApplyMatrix(transform);
		pm.GenerateNormals(false);
		m_cubeViewMesh[CubeView::meshID_BLF] = pm.CreateMesh();
		if (m_cubeViewMesh[CubeView::meshID_BLF])
			m_cubeViewGPUMesh[CubeView::meshID_BLF] = g_app->m_gs->CreateMesh(m_cubeViewMesh[CubeView::meshID_BLF]);
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

	m_hoverMesh = result;

	return result;
}

uint32_t CubeView::_IsMouseRayIntersect(const bqRay& r, uint32_t meshID, bqReal& T)
{
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
		}
	}
	return meshID__size;
}

void CubeView::Update(ViewportView* view)
{
	if (m_isClickedLMB)
	{
		if (bqInput::IsLMBRelease())
		{
			m_isClickedLMB = false;
		
			if (m_hoverMesh != meshID__size)
			{
				if (m_hoverMesh == m_LMBHitMesh)
				{
					view->CubeViewOnClick(m_LMBHitMesh);
				}
			}
		
			m_LMBHitMesh = meshID__size;
		}
		else
		{

		}

	}
	else
	{
		m_LMBHitMesh = meshID__size;

		if (m_hoverMesh != meshID__size)
		{
			if (bqInput::IsLMBHit())
			{
				m_LMBHitMesh = m_hoverMesh;

				m_isClickedLMB = true;
			}
		}
	}
}
