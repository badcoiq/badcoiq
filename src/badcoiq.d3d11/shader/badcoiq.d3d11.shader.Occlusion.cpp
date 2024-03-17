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

#include "badcoiq.h"

#include "../badcoiq.d3d11impl.h"
#include "badcoiq/gs/bqMaterial.h"


bqD3D11ShaderOcclusion::bqD3D11ShaderOcclusion(bqGSD3D11* gs)
	:
	m_gs(gs)
{}

bqD3D11ShaderOcclusion::~bqD3D11ShaderOcclusion()
{
	if (m_cbV)m_cbV->Release();
}

bool bqD3D11ShaderOcclusion::Init(){
	const char* text =
		"struct VSIn{\n"
		"   float3 position : POSITION;\n"
		"	float2 uv : TEXCOORD;\n"
		"   float3 normal : NORMAL;\n"
		"   float3 binormal : BINORMAL;\n"
		"   float3 tangent : TANGENT;\n"
		"   float4 color : COLOR;\n"
		"};\n"
		"cbuffer cbVertex{\n"
		"	double4x4 WVP;\n"
		"};\n"
		"struct VSOut{\n"
		"   float4 pos : SV_POSITION;\n"
		"};\n"
		"struct PSOut{\n"
		"    float4 color : SV_Target;\n"
		"};\n"
		"VSOut VSMain(VSIn input){\n"
		"   VSOut output;\n"
		"	output.pos   = mul(WVP, double4(input.position.x, input.position.y, input.position.z, 1.0));\n"
		"	return output;\n"
		"}\n"
		"PSOut PSMain(VSOut input){\n"
		"   PSOut output;\n"
		"   output.color = float4(1.f,0.f,0.f,1.0f);\n"
		"    return output;\n"
		"}\n";
	if (!m_gs->CreateShaders(
		"vs_5_0",
		"ps_5_0",
		text,
		text,
		"VSMain",
		"PSMain",
		bqMeshVertexType::Triangle,
		&this->m_vShader,
		&this->m_pShader,
		&this->m_vLayout))
		return false;

	if (!m_gs->CreateConstantBuffer(sizeof(cbV), &m_cbV))
		return false;

	return true;
}

void bqD3D11ShaderOcclusion::SetConstants(bqMaterial* material)
{
	BQ_ASSERT_ST(bqFramework::GetMatrix(bqMatrixType::WorldViewProjection));
	BQ_ASSERT_ST(bqFramework::GetMatrix(bqMatrixType::World));

	m_cbDataV.WVP = *bqFramework::GetMatrix(bqMatrixType::WorldViewProjection);

	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		D3D11_BUFFER_DESC d;
		m_gs->m_d3d11DevCon->Map(m_cbV, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		m_cbV->GetDesc(&d);
		memcpy(mappedResource.pData, &m_cbDataV, d.ByteWidth);
		m_gs->m_d3d11DevCon->Unmap(m_cbV, 0);
		m_gs->m_d3d11DevCon->VSSetConstantBuffers(0, 1, &m_cbV);
	}
}
