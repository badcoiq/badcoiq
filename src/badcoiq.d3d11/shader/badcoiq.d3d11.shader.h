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
#pragma once
#ifndef _BQ_D3D11SHADER_H_
#define _BQ_D3D11SHADER_H_

class bqGSD3D11ShaderBase
{
public:
	bqGSD3D11ShaderBase() :
		m_vShader(0),
		m_pShader(0),
		m_gShader(0),
		m_vLayout(0)
	{}

	virtual ~bqGSD3D11ShaderBase()
	{
		if (m_vLayout) m_vLayout->Release();
		if (m_gShader) m_gShader->Release();
		if (m_vShader) m_vShader->Release();
		if (m_pShader) m_pShader->Release();
	}

	ID3D11VertexShader* m_vShader = 0;
	ID3D11PixelShader* m_pShader = 0;
	ID3D11GeometryShader* m_gShader = 0;
	ID3D11InputLayout* m_vLayout = 0;

	bqShaderType m_type = bqShaderType::User;

	virtual void SetConstants(bqMaterial* material) = 0;
};

#endif
