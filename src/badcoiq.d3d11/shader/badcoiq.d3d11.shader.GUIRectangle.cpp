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
#ifdef BQ_WITH_GS

#include "../badcoiq.d3d11impl.h"
#include "badcoiq/gs/bqMaterial.h"

bqD3D11ShaderGUIRectangle::bqD3D11ShaderGUIRectangle(bqGSD3D11* gs)
	:
	m_gs(gs)
{}

bqD3D11ShaderGUIRectangle::~bqD3D11ShaderGUIRectangle()
{
	if (m_cbFrame)m_cbFrame->Release();
	if (m_cbElement)m_cbElement->Release();
}

bool bqD3D11ShaderGUIRectangle::Init() {
	const char* text =
		"Texture2D tex2d_1;\n"
		"SamplerState tex2D_sampler_1;\n"
		"struct VSIn{\n"
		"   float3 position : POSITION;\n"
		"   float2 uv : TEXCOORD;\n"
		"};\n"
		"cbuffer cbFrame{\n"
		"	float4x4 ProjMtx;\n"
		"};\n"
		"cbuffer cbElement{\n"
		"	float4 Corners;\n"
		"	float4 Color1;\n"
		"	float4 Color2;\n"
		"	float4 UVs;\n"
		"};\n"
		"struct VSOut{\n"
		"   float4 pos : SV_POSITION;\n"
		"   float2 uv : TEXCOORD0;\n"
		"	float4 color : COLOR0;\n"
		"};\n"
		"struct PSOut{\n"
		"    float4 color : SV_Target;\n"
		"};\n"
		"VSOut VSMain(VSIn input){\n"
		"   VSOut output;\n"
		"	output.pos.xyz   = input.position.xyz;\n"
		"	output.pos.w   = 1.f;\n"
		"	output.uv   = input.uv;\n"
		"	return output;\n"
		"}\n"
		"PSOut PSMain(VSOut input){\n"
		"   PSOut output;\n"
		"   output.color = saturate(tex2d_1.Sample(tex2D_sampler_1, input.uv) * input.color);\n"
		"   return output;\n"
		"}\n"
		"[maxvertexcount(4)]\n"
		"void GSMain(point VSOut input[1], inout TriangleStream<VSOut> TriStream ){\n"
		"	VSOut Out;\n"

		"   float4 v1 = mul(ProjMtx, float4(Corners.x, Corners.y, 0.f, 1.f));\n"
		"   float4 v2 = mul(ProjMtx, float4(Corners.z, Corners.w, 0.f, 1.f));\n"

		"	Out.pos   = float4(v2.x, v1.y, 0.f, 1.f);\n"
		"	Out.uv = float2(UVs.z,UVs.y);\n"
		"	Out.color = Color1;\n"
		"	TriStream.Append(Out);\n"

		"	Out.pos   = float4(v2.x, v2.y, 0.f, 1.f);\n"
		"	Out.uv = float2(UVs.z,UVs.w);\n"
		"	Out.color = Color2;\n"
		"	TriStream.Append(Out);\n"

		"	Out.pos   = float4(v1.x, v1.y, 0.f, 1.f);\n"
		"	Out.uv = float2(UVs.x,UVs.y);\n"
		"	Out.color = Color1;\n"
		"	TriStream.Append(Out);\n"

		"	Out.pos   = float4(v1.x, v2.y, 0.f, 1.f);\n"
		"	Out.uv = float2(UVs.x,UVs.w);\n"
		"	Out.color = Color2;\n"
		"	TriStream.Append(Out);\n"

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

	if (!m_gs->CreateConstantBuffer(sizeof(cbFrame), &m_cbFrame))
		return false;
	if (!m_gs->CreateConstantBuffer(sizeof(cbElement), &m_cbElement))
		return false;

	return true;
}

void bqD3D11ShaderGUIRectangle::SetConstants(bqMaterial* material) {}

void bqD3D11ShaderGUIRectangle::SetOnFrame()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	D3D11_BUFFER_DESC d;
	m_gs->m_d3d11DevCon->Map(m_cbFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	m_cbFrame->GetDesc(&d);
	memcpy(mappedResource.pData, &m_cbDataFrame, d.ByteWidth);
	m_gs->m_d3d11DevCon->Unmap(m_cbFrame, 0);
	m_gs->m_d3d11DevCon->GSSetConstantBuffers(0, 1, &m_cbFrame);
}

void bqD3D11ShaderGUIRectangle::SetOnElement(bqGSD3D11Texture* texture)
{
	m_gs->m_d3d11DevCon->PSSetShaderResources(0, 1, &texture->m_textureResView);
	m_gs->m_d3d11DevCon->PSSetSamplers(0, 1, &texture->m_samplerState);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	D3D11_BUFFER_DESC d;
	m_gs->m_d3d11DevCon->Map(m_cbElement, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	m_cbElement->GetDesc(&d);
	memcpy(mappedResource.pData, &m_cbDataElement, d.ByteWidth);
	m_gs->m_d3d11DevCon->Unmap(m_cbElement, 0);
	m_gs->m_d3d11DevCon->GSSetConstantBuffers(1, 1, &m_cbElement);
}
#endif
