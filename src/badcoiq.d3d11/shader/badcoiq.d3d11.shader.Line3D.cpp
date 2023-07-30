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
	const char* text =
		"struct VSIn{\n"
		"	uint vertexID : SV_VertexID;\n"
		"};\n"
		"cbuffer cbVertex{\n"
		"	double4x4 VP;\n"
		"	double4 P1;\n"
		"	double4 P2;\n"
		"	float4 Color;\n"
		"};\n"
		"struct VSOut{\n"
		"   float4 pos : SV_POSITION;\n"
		"   float4 color : COLOR0;\n"
		"};\n"
		
		"struct PSOut{\n"
		"    float4 color : SV_Target;\n"
		"};\n"

		"VSOut VSMain(VSIn input){\n"
			"double4 vertices[2] = {\n"
			"double4( P1.xyz, 1.0),\n"
			"double4( P2.xyz, 1.0)\n"
			"};\n"
		"   VSOut output;\n"
		"	output.pos  = mul(VP, vertices[input.vertexID]);\n"
		"	output.color = Color;\n"
		"	return output;\n"
		"}\n"
		"PSOut PSMain(VSOut input){\n"
		"    PSOut output;\n"
		"    output.color = input.color;\n"
		"    return output;\n"
		"}\n";
	if (!m_gs->CreateShaders(
		"vs_5_0",
		"ps_5_0",
		text,
		text,
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

void bqD3D11ShaderLine3D::SetData(const bqVec4& p1, const bqVec4& p2, const bqColor& color, const bqMat4& projMat)
{
	m_cbData.P1 = p1;
	m_cbData.P2 = p2;
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

