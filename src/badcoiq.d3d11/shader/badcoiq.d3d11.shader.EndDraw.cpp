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


bqD3D11ShaderEndDraw::bqD3D11ShaderEndDraw(bqGSD3D11* gs)
	:
	m_gs(gs)
{}

bqD3D11ShaderEndDraw::~bqD3D11ShaderEndDraw()
{
	if (m_cbP)m_cbP->Release();
}

bool bqD3D11ShaderEndDraw::Init() {
	const char* text =
		"Texture2D tex2d_1;\n"
		"SamplerState tex2D_sampler_1;\n"
		"Texture2D tex2d_2;\n" // GUI
		"SamplerState tex2D_sampler_2;\n" // GUI
		"struct VSIn{\n"
		"   float3 position : POSITION;\n"
		"   float2 uv : TEXCOORD;\n"
		"};\n"
		"cbuffer cbPixel{\n"
		"	float4 something;\n"
		"};\n"
		"struct VSOut{\n"
		"   float4 pos : SV_POSITION;\n"
		"   float2 uv : TEXCOORD;\n"
		"};\n"
		"struct PSOut{\n"
		"    float4 color : SV_Target;\n"
		"};\n"
		"VSOut VSMain(VSIn input){\n"
		"   VSOut output;\n"
		"	output.pos.xyz   = input.position;\n"
		"	output.pos.w   = 0.f;\n"
		"	output.uv   = input.uv;\n"
		"	return output;\n"
		"}\n"
		"PSOut PSMain(VSOut input){\n"
		"   PSOut output;\n"
		"   float4 GUIColor = tex2d_2.Sample(tex2D_sampler_2, input.uv);\n"
		"   output.color = lerp(tex2d_1.Sample(tex2D_sampler_1, input.uv),GUIColor,GUIColor.w);\n"
		"   return output;\n"
		"}\n"
		"[maxvertexcount(4)]\n"
		"void GSMain(point VSOut input[1], inout TriangleStream<VSOut> TriStream ){\n"
		"	VSOut Out;\n"
		"	const float4 sizes[4] = {\n"
		"		float4(-1.f, -1.f, 0.f, 1.f),\n"
		"		float4(-1.f, 1.f, 0.f, 1.f),\n"
		"		float4(1.f, -1.f, 0.f, 1.f),\n"
		"		float4(1.f, 1.f, 0.f, 1.f)\n"
		"	};\n"
		"	const float2 uvs[4] = {\n"
		"		float2(0.f, 1.f),\n"
		"		float2(0.f, 0.f),\n"
		"		float2(1.f, 1.f),\n"
		"		float2(1.f, 0.f)\n"
		"	};\n"
		"	for (uint i = 0; i<4; ++i)\n"
		"	{\n"
		"		Out.pos = sizes[i];\n"
		"		Out.uv = uvs[i];\n"
		"		TriStream.Append(Out);\n"
		"	}\n"

		"	TriStream.RestartStrip();\n"
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
	if (!m_gs->CreateGeometryShaders("gs_5_0", text, "GSMain", &m_gShader))
		return false;

	if (!m_gs->CreateConstantBuffer(sizeof(cbP), &m_cbP))
		return false;

	return true;
}

void bqD3D11ShaderEndDraw::SetConstants(bqMaterial* material)
{
	m_gs->m_d3d11DevCon->PSSetShaderResources(0, 1, &m_gs->m_mainTargetRTT->m_textureResView);
	m_gs->m_d3d11DevCon->PSSetSamplers(0, 1, &m_gs->m_mainTargetRTT->m_samplerState);
	m_gs->m_d3d11DevCon->PSSetShaderResources(1, 1, &m_gs->m_GUIRTT->m_textureResView);
	m_gs->m_d3d11DevCon->PSSetSamplers(1, 1, &m_gs->m_GUIRTT->m_samplerState);
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		D3D11_BUFFER_DESC d;
		m_gs->m_d3d11DevCon->Map(m_cbP, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		m_cbP->GetDesc(&d);
		memcpy(mappedResource.pData, &m_cbDataP, d.ByteWidth);
		m_gs->m_d3d11DevCon->Unmap(m_cbP, 0);
		m_gs->m_d3d11DevCon->PSSetConstantBuffers(0, 1, &m_cbP);
	}
}
