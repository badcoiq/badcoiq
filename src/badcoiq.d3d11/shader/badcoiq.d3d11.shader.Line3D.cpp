﻿/*
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

#include "badcoiq.h"
#ifdef BQ_WITH_GS

#include "../badcoiq.d3d11impl.h"


bqD3D11ShaderLine3D::bqD3D11ShaderLine3D(bqGSD3D11* gs)
	:
	m_gs(gs)
{}

bqD3D11ShaderLine3D::~bqD3D11ShaderLine3D() 
{
	if (m_cb)m_cb->Release();
}

bool bqD3D11ShaderLine3D::Init(){
	uint32_t sz = 0;
	bqString appPath = bqFramework::GetAppPath();
	bqString shaderPath = std::move(appPath + "../data/shaders/d3d11/badcoiq.d3d11.shader.Line3D.hlsl");
	bqStringA shaderPathA;
	shaderPath.to_utf8(shaderPathA);

	//char* text = (char*)bqFramework::CreateFileBuffer(shaderPathA.c_str(), &sz, true);
	//bqPtr pText(text, bqPtr::Free());
	BQ_PTR_F(char, text, bqFramework::CreateFileBuffer(shaderPathA.c_str(), &sz, true));

	if (!m_gs->CreateShaders(
		"vs_5_0",
		"ps_5_0",
		text.Ptr(),
		text.Ptr(),
		"VSMain",
		"PSMain",
		bqMeshVertexType::Null,
		&this->m_vShader,
		&this->m_pShader,
		&this->m_vLayout))
		return false;

	if (!m_gs->CreateConstantBuffer(sizeof(cb), &m_cb))
		return false;

	return true;
}

void bqD3D11ShaderLine3D::SetData(const bqVec3& p1, const bqVec3& p2, const bqColor& color, const bqMat4& projMat)
{
	m_cbData.P1.x = p1.x;
	m_cbData.P1.y = p1.y;
	m_cbData.P1.z = p1.z;
	m_cbData.P2.x = p2.x;
	m_cbData.P2.y = p2.y;
	m_cbData.P2.z = p2.z;
	m_cbData.P1.w = 1.f;
	m_cbData.P2.w = 1.f;
	m_cbData.Color = color;
	m_cbData.VP = projMat; // g_d3d11->m_matrixViewProjection;
}

void bqD3D11ShaderLine3D::SetConstants(bqMaterial* material){

	m_gs->m_d3d11DevCon->VSSetConstantBuffers(0, 1, &m_cb);
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	m_gs->m_d3d11DevCon->Map(m_cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	D3D11_BUFFER_DESC d;
	m_cb->GetDesc(&d);
	memcpy(mappedResource.pData, &m_cbData, d.ByteWidth);
	m_gs->m_d3d11DevCon->Unmap(m_cb, 0);
}
#endif
