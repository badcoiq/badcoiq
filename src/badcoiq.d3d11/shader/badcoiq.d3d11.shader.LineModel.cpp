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
#include "badcoiq/common/bqFileBuffer.h"

#include <d3dcompiler.h>


bqD3D11ShaderLineModel::bqD3D11ShaderLineModel(bqGSD3D11* gs)
	:
	m_gs(gs)
{}

bqD3D11ShaderLineModel::~bqD3D11ShaderLineModel()
{
	if (m_cbV)m_cbV->Release();
	if (m_cbP)m_cbP->Release();
}

bool bqD3D11ShaderLineModel::Init()
{
	uint32_t sz = 0;
	bqString appPath = bqFramework::GetAppPath();

	ID3D10Blob* VsBlob = nullptr;
	ID3D10Blob* PsBlob = nullptr;
	ID3D10Blob* ErrorBlob = nullptr;

	bqString textFilePath("../data/shaders/d3d11/badcoiq.d3d11.shader.LineModel.hlsl");
	bqString textFilePathBin = textFilePath + ".vs";

	{
		// чтение скомпилированного шейдера
		bqStringW shaderPathW;
		bqString shaderPathBin(appPath + textFilePathBin.c_str());
		shaderPathBin.to_utf16(shaderPathW);
		D3DReadFileToBlob(shaderPathW.c_str(), &VsBlob);
	}

	if (!VsBlob)
	{
		bqString shaderPath = std::move(appPath + textFilePath.c_str());
		bqStringA shaderPathA;
		shaderPath.to_utf8(shaderPathA);

		BQ_PTR_F(char, text, bqFramework::CreateFileBuffer(shaderPathA.c_str(), &sz, true));
		if (m_gs->CompileShader("vs_5_0", "VSMain", text.Ptr(), &VsBlob, &ErrorBlob))
		{
			bqStringW shaderPathW;
			bqString shaderPathBin(appPath + textFilePathBin.c_str());
			shaderPathBin.to_utf16(shaderPathW);
			D3DWriteBlobToFile(VsBlob, shaderPathW.c_str(), TRUE);
		}
	}

	if (VsBlob)
	{
		if (m_gs->CreateVertexShaderInputLayoutLineModel(VsBlob, &m_vLayout))
			m_gs->CreateVertexShader(VsBlob, &m_vShader);
	}

	BQD3DSAFE_RELEASE(ErrorBlob);
	BQD3DSAFE_RELEASE(VsBlob);

	textFilePath.assign("../data/shaders/d3d11/badcoiq.d3d11.shader.LineModel.hlsl");
	textFilePathBin = textFilePath + ".ps";

	{
		// чтение скомпилированного шейдера
		bqStringW shaderPathW;
		bqString shaderPathBin(appPath + textFilePathBin.c_str());
		shaderPathBin.to_utf16(shaderPathW);
		D3DReadFileToBlob(shaderPathW.c_str(), &PsBlob);
	}

	if (!PsBlob)
	{
		bqString shaderPath = std::move(appPath + textFilePath.c_str());
		bqStringA shaderPathA;
		shaderPath.to_utf8(shaderPathA);

		BQ_PTR_F(char, text, bqFramework::CreateFileBuffer(shaderPathA.c_str(), &sz, true));
		if (m_gs->CompileShader("ps_5_0", "PSMain", text.Ptr(), &PsBlob, &ErrorBlob))
		{
			bqStringW shaderPathW;
			bqString shaderPathBin(appPath + textFilePathBin.c_str());
			shaderPathBin.to_utf16(shaderPathW);
			D3DWriteBlobToFile(PsBlob, shaderPathW.c_str(), TRUE);
		}
	}

	if (PsBlob)
	{
		m_gs->CreatePixelShader(PsBlob, &m_pShader);
	}

	BQD3DSAFE_RELEASE(ErrorBlob);
	BQD3DSAFE_RELEASE(PsBlob);


	if (!m_gs->CreateConstantBuffer(sizeof(cbV), &m_cbV))
		return false;
	if (!m_gs->CreateConstantBuffer(sizeof(cbP), &m_cbP))
		return false;

	if(!m_vShader || !m_pShader)
		return false;

	return true;
}

void bqD3D11ShaderLineModel::SetConstants(bqMaterial* material)
{
	BQ_ASSERT_ST(bqFramework::GetMatrix(bqMatrixType::WorldViewProjection));

	m_cbDataV.WVP = *bqFramework::GetMatrix(bqMatrixType::WorldViewProjection);

	/*bqGSD3D11Texture* _t = m_gs->m_whiteTexture;
	if (material->m_maps[0].m_texture)
		_t = (bqGSD3D11Texture*)material->m_maps[0].m_texture;

	m_gs->m_d3d11DevCon->PSSetShaderResources(0, 1, &_t->m_textureResView);
	m_gs->m_d3d11DevCon->PSSetSamplers(0, 1, &_t->m_samplerState);*/

	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		D3D11_BUFFER_DESC d;
		m_gs->m_d3d11DevCon->Map(m_cbV, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		m_cbV->GetDesc(&d);
		memcpy(mappedResource.pData, &m_cbDataV, d.ByteWidth);
		m_gs->m_d3d11DevCon->Unmap(m_cbV, 0);
		m_gs->m_d3d11DevCon->VSSetConstantBuffers(0, 1, &m_cbV);
	}

	m_cbDataP.BaseColor = material->m_colorDiffuse;

	// это надо удалить
	m_cbDataP.BaseColor.m_data[0] = 1.f;
	m_cbDataP.BaseColor.m_data[1] = 1.f;
	m_cbDataP.BaseColor.m_data[2] = 1.f;
	m_cbDataP.BaseColor.m_data[3] = 1.f;

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


//bqD3D11ShaderStandartSkinned::bqD3D11ShaderStandartSkinned(bqGSD3D11* gs)
//	:
//	m_gs(gs)
//{}
//
//bqD3D11ShaderStandartSkinned::~bqD3D11ShaderStandartSkinned()
//{
//	if (m_cbV)m_cbV->Release();
//	if (m_cbP)m_cbP->Release();
//	if (m_cbSk)m_cbSk->Release();
//}
//
//bool bqD3D11ShaderStandartSkinned::Init()
//{
//	uint32_t sz = 0;
//	bqString appPath = bqFramework::GetAppPath();
//
//	ID3D10Blob* VsBlob = nullptr;
//	ID3D10Blob* PsBlob = nullptr;
//	ID3D10Blob* ErrorBlob = nullptr;
//
//	bqString textFilePath("../data/shaders/d3d11/badcoiq.d3d11.shader.Standart.hlsl");
//	bqString textFilePathBin = textFilePath + ".sk.vs";
//
//	{
//		// чтение скомпилированного шейдера
//		bqStringW shaderPathW;
//		bqString shaderPathBin(appPath + textFilePathBin.c_str());
//		shaderPathBin.to_utf16(shaderPathW);
//		D3DReadFileToBlob(shaderPathW.c_str(), &VsBlob);
//	}
//
//	if (!VsBlob)
//	{
//		bqString shaderPath = std::move(appPath + textFilePath.c_str());
//		bqStringA shaderPathA;
//		shaderPath.to_utf8(shaderPathA);
//
//		BQ_PTR_F(char, text, bqFramework::CreateFileBuffer(shaderPathA.c_str(), &sz, true));
//		if (m_gs->CompileShader("vs_5_0", "VSMainSk", text.Ptr(), &VsBlob, &ErrorBlob))
//		{
//			bqStringW shaderPathW;
//			bqString shaderPathBin(appPath + textFilePathBin.c_str());
//			shaderPathBin.to_utf16(shaderPathW);
//			D3DWriteBlobToFile(VsBlob, shaderPathW.c_str(), TRUE);
//		}
//	}
//
//	if (VsBlob)
//	{
//		if (m_gs->CreateVertexShaderInputLayout(VsBlob, &m_vLayout))
//			m_gs->CreateVertexShader(VsBlob, &m_vShader);
//	}
//
//	BQD3DSAFE_RELEASE(ErrorBlob);
//	BQD3DSAFE_RELEASE(VsBlob);
//
//	textFilePath.assign("../data/shaders/d3d11/badcoiq.d3d11.shader.Standart.hlsl");
//	textFilePathBin = textFilePath + ".ps";
//
//	{
//		// чтение скомпилированного шейдера
//		bqStringW shaderPathW;
//		bqString shaderPathBin(appPath + textFilePathBin.c_str());
//		shaderPathBin.to_utf16(shaderPathW);
//		D3DReadFileToBlob(shaderPathW.c_str(), &PsBlob);
//	}
//
//	if (!PsBlob)
//	{
//		bqString shaderPath = std::move(appPath + textFilePath.c_str());
//		bqStringA shaderPathA;
//		shaderPath.to_utf8(shaderPathA);
//
//		BQ_PTR_F(char, text, bqFramework::CreateFileBuffer(shaderPathA.c_str(), &sz, true));
//		if (m_gs->CompileShader("ps_5_0", "PSMain", text.Ptr(), &PsBlob, &ErrorBlob))
//		{
//			bqStringW shaderPathW;
//			bqString shaderPathBin(appPath + textFilePathBin.c_str());
//			shaderPathBin.to_utf16(shaderPathW);
//			D3DWriteBlobToFile(PsBlob, shaderPathW.c_str(), TRUE);
//		}
//	}
//
//	if (PsBlob)
//	{
//		m_gs->CreatePixelShader(PsBlob, &m_pShader);
//	}
//
//	BQD3DSAFE_RELEASE(ErrorBlob);
//	BQD3DSAFE_RELEASE(PsBlob);
//
//
//	if (!m_gs->CreateConstantBuffer(sizeof(cbV), &m_cbV))
//		return false;
//	if (!m_gs->CreateConstantBuffer(sizeof(cbSk), &m_cbSk))
//		return false;
//	if (!m_gs->CreateConstantBuffer(sizeof(cbP), &m_cbP))
//		return false;
//
//	if (!m_vShader || !m_pShader)
//		return false;
//
//	return true;
//}
//
//void bqD3D11ShaderStandartSkinned::SetConstants(bqMaterial* material)
//{
//	BQ_ASSERT_ST(bqFramework::GetMatrix(bqMatrixType::WorldViewProjection));
//	BQ_ASSERT_ST(bqFramework::GetMatrix(bqMatrixType::World));
//
//	m_cbDataV.W = *bqFramework::GetMatrix(bqMatrixType::World);
//	m_cbDataV.WVP = *bqFramework::GetMatrix(bqMatrixType::WorldViewProjection);
//
//	auto MS = bqFramework::GetMatrixSkinned();
//	memcpy(&m_cbDataSk.M[0], MS, sizeof(bqMat4) * 255);
//
//	bqGSD3D11Texture* _t = m_gs->m_whiteTexture;
//	if (material->m_maps[0].m_texture)
//		_t = (bqGSD3D11Texture*)material->m_maps[0].m_texture;
//
//	m_gs->m_d3d11DevCon->PSSetShaderResources(0, 1, &_t->m_textureResView);
//	m_gs->m_d3d11DevCon->PSSetSamplers(0, 1, &_t->m_samplerState);
//
//	{
//		D3D11_MAPPED_SUBRESOURCE mappedResource;
//		D3D11_BUFFER_DESC d;
//		m_gs->m_d3d11DevCon->Map(m_cbV, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
//		m_cbV->GetDesc(&d);
//		memcpy(mappedResource.pData, &m_cbDataV, d.ByteWidth);
//		m_gs->m_d3d11DevCon->Unmap(m_cbV, 0);
//		m_gs->m_d3d11DevCon->VSSetConstantBuffers(0, 1, &m_cbV);
//	}
//
//	{
//		D3D11_MAPPED_SUBRESOURCE mappedResource;
//		D3D11_BUFFER_DESC d;
//		m_gs->m_d3d11DevCon->Map(m_cbSk, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
//		m_cbSk->GetDesc(&d);
//		memcpy(mappedResource.pData, &m_cbDataSk, d.ByteWidth);
//		m_gs->m_d3d11DevCon->Unmap(m_cbSk, 0);
//		m_gs->m_d3d11DevCon->VSSetConstantBuffers(1, 1, &m_cbSk);
//	}
//
//
//	m_cbDataP.alphaDiscard = material->m_alphaDiscard;
//
//	m_cbDataP.BaseColor = material->m_colorDiffuse;
//	m_cbDataP.AmbientColor = material->m_colorAmbient;
//	m_cbDataP.SunPosition.x = material->m_sunPosition.x;
//	m_cbDataP.SunPosition.y = material->m_sunPosition.y;
//	m_cbDataP.SunPosition.z = material->m_sunPosition.z;
//
//	{
//		D3D11_MAPPED_SUBRESOURCE mappedResource;
//		D3D11_BUFFER_DESC d;
//		m_gs->m_d3d11DevCon->Map(m_cbP, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
//		m_cbP->GetDesc(&d);
//		memcpy(mappedResource.pData, &m_cbDataP, d.ByteWidth);
//		m_gs->m_d3d11DevCon->Unmap(m_cbP, 0);
//		m_gs->m_d3d11DevCon->PSSetConstantBuffers(0, 1, &m_cbP);
//	}
//}
#endif
