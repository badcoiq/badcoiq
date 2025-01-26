﻿/*
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

#pragma once
#ifndef _BQ_D3D11_SHADER_LINEMODEL_H__
#define _BQ_D3D11_SHADER_LINEMODEL_H__

class bqGSD3D11;
class bqD3D11ShaderLineModel : public bqGSD3D11ShaderBase
{
public:
	bqD3D11ShaderLineModel(bqGSD3D11* gs);
	virtual ~bqD3D11ShaderLineModel();

	bqGSD3D11* m_gs = 0;

	ID3D11Buffer* m_cbV = 0;
	ID3D11Buffer* m_cbP = 0;

	struct cbV
	{
		bqMat4 WVP;
	}m_cbDataV;
	struct cbP
	{
		bqColor BaseColor;
	}m_cbDataP;

	virtual void SetConstants(bqMaterial* material) final;

	bool Init();
};

//class bqD3D11ShaderStandartSkinned : public bqGSD3D11ShaderBase
//{
//public:
//	bqD3D11ShaderStandartSkinned(bqGSD3D11* gs);
//	virtual ~bqD3D11ShaderStandartSkinned();
//
//	bqGSD3D11* m_gs = 0;
//
//	ID3D11Buffer* m_cbV = 0;
//	ID3D11Buffer* m_cbP = 0;
//	ID3D11Buffer* m_cbSk = 0;
//
//	struct cbSk
//	{
//		bqMat4 M[255];
//	}m_cbDataSk;
//
//	struct cbV
//	{
//		bqMat4 WVP;
//		bqMat4 W;
//	}m_cbDataV;
//	struct cbP
//	{
//		bqVec4 SunPosition;
//		bqColor BaseColor;
//		bqColor AmbientColor;
//		float alphaDiscard = 0.5f;
//		float paddingf[3];
//	}m_cbDataP;
//
//	virtual void SetConstants(bqMaterial* material) final;
//
//	bool Init();
//};

#endif