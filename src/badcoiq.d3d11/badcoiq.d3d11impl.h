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
#ifndef _BQ_D3D11IMPL_H_
#define _BQ_D3D11IMPL_H_

#include <d3d11.h>

#include "badcoiq/geometry/bqMesh.h"
#include "badcoiq/gs/bqGS.h"
#include "badcoiq/gs/bqShader.h"
#include "badcoiq/math/bqMath.h"
#include "badcoiq/common/bqColor.h"

#include "shader/badcoiq.d3d11.shader.h"
#include "shader/badcoiq.d3d11.shader.Line3D.h"
#include "badcoiq.d3d11.mesh.h"

#define BQD3DSAFE_RELEASE(x) if(x){x->Release();x=0;}
#define BQSAFE_DESTROY2(x) if(x){bqDestroy(x);x=0;}

class bqGSD3D11 : public bqGS
{
	friend class bqD3D11ShaderLine3D;

	bqD3D11ShaderLine3D* m_shaderLine3D = 0;

	bqWindow* m_activeWindow = 0;
	bqPoint* m_windowCurrentSize = 0;

	ID3D11Device* m_d3d11Device = 0;
	ID3D11DeviceContext* m_d3d11DevCon = 0;
	IDXGISwapChain* m_SwapChain = 0;
	ID3D11RenderTargetView* m_windowTargetView = 0;
	ID3D11DepthStencilView* m_windowDepthStencilView = 0;
	ID3D11Texture2D* m_windowDepthStencilBuffer = 0;
	ID3D11DepthStencilState* m_depthStencilStateEnabled = 0;
	ID3D11DepthStencilState* m_depthStencilStateDisabled = 0;
	ID3D11RasterizerState* m_RasterizerSolid = 0;
	ID3D11RasterizerState* m_RasterizerSolidNoBackFaceCulling = 0;
	ID3D11RasterizerState* m_RasterizerWireframeNoBackFaceCulling = 0;
	ID3D11RasterizerState* m_RasterizerWireframe = 0;
	ID3D11BlendState* m_blendStateAlphaEnabled = 0;
	ID3D11BlendState* m_blendStateAlphaDisabled = 0;
	
	bqVec4f m_clearColor;
	
	// bool m_vsync = true; // можно же сделать по другому.
	UINT m_vsync = 1;       // m_SwapChain->Present(m_vsync, 0);

public:
	bqGSD3D11();
	virtual ~bqGSD3D11();

	virtual bool Init(bqWindow*, const char* parameters) final;
	virtual void Shutdown() final;
	virtual bool InitWindow(bqWindow*) final;
	virtual void SetActiveWindow(bqWindow*) final;
	virtual bqString GetName() final;
	virtual bqString GetTextInfo() final;
	virtual bqUID GetUID() final;

	virtual void SetClearColor(float r, float g, float b, float a) final;
	virtual void BeginDraw() final;
	virtual void ClearDepth() final;
	virtual void ClearColor() final;
	virtual void ClearAll() final;
	virtual void EndDraw() final;
	virtual void SwapBuffers() final;
	
	virtual void DrawLine3D(const bqVec4& p1, const bqVec4& p2, const bqColor& c) final;
	virtual void SetShader(bqShaderType, uint32_t userShaderIndex) final;

	virtual bqGPUMesh* SummonMesh(bqMesh* m) final;

	bool CreateShaders(
		const char* vertexTarget,
		const char* pixelTarget,
		const char* vertexShader,
		const char* pixelShader,
		const char* vertexEntryPoint,
		const char* pixelEntryPoint,
		bqMeshVertexType vertexType,
		ID3D11VertexShader** vs,
		ID3D11PixelShader** ps,
		ID3D11InputLayout** il);
	bool CreateConstantBuffer(uint32_t byteSize, ID3D11Buffer**);
	bool CreateGeometryShaders(const char* target,
		const char* shaderText,
		const char* entryPoint,
		ID3D11GeometryShader** gs);

	void SetActiveShader(bqGSD3D11ShaderBase* shader);
	bool CreateShaders();
};

#endif
