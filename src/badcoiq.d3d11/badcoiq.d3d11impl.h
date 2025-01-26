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
#ifdef BQ_WITH_GS

#include <d3d11.h>

#include "badcoiq/geometry/bqMesh.h"
#include "badcoiq/gs/bqGS.h"
#include "badcoiq/gs/bqShader.h"
#include "badcoiq/math/bqMath.h"
#include "badcoiq/common/bqColor.h"

#include "shader/badcoiq.d3d11.shader.h"
#include "shader/badcoiq.d3d11.shader.Line3D.h"
#include "shader/badcoiq.d3d11.shader.Standart.h"
#include "shader/badcoiq.d3d11.shader.lineModel.h"
#include "shader/badcoiq.d3d11.shader.Sprite.h"
#include "shader/badcoiq.d3d11.shader.EndDraw.h"
#include "shader/badcoiq.d3d11.shader.GUIRectangle.h"
#include "shader/badcoiq.d3d11.shader.Occlusion.h"
#include "badcoiq.d3d11.mesh.h"
#include "badcoiq.d3d11.texture.h"
#include "badcoiq.d3d11.occlusionObject.h"

#define BQD3DSAFE_RELEASE(x) if(x){x->Release();x=0;}
#define BQSAFE_DESTROY2(x) if(x){bqDestroy(x);x=0;}

class bqGSD3D11 : public bqGS
{
	friend class bqD3D11ShaderLine3D;
	friend class bqD3D11ShaderStandart;
	friend class bqD3D11ShaderStandartSkinned;
	friend class bqD3D11ShaderLineModel;
	friend class bqD3D11ShaderEndDraw;
	friend class bqD3D11ShaderGUIRectangle;
#ifdef BQ_WITH_SPRITE
	friend class bqD3D11ShaderSprite;
#endif
	friend class bqD3D11ShaderOcclusion;

	bqGSD3D11ShaderBase* m_activeShader = 0;
	bqD3D11ShaderLine3D* m_shaderLine3D = 0;
	bqD3D11ShaderStandart* m_shaderStandart = 0;
	bqD3D11ShaderStandartSkinned* m_shaderStandartSk = 0;
	bqD3D11ShaderLineModel* m_shaderLineModel = 0;
	bqD3D11ShaderEndDraw* m_shaderEndDraw = 0;
	bqD3D11ShaderGUIRectangle* m_shaderGUIRectangle = 0;
#ifdef BQ_WITH_SPRITE
	bqD3D11ShaderSprite* m_shaderSprite = 0;
#endif
	bqD3D11ShaderOcclusion* m_shaderOcl = 0;

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


	bqGSD3D11Mesh* m_currMesh = 0;

	bqMaterial* m_currMaterial = 0;
	bqGSD3D11Texture* m_whiteTexture = 0;
	bool _createWindowBuffers(int32_t x, int32_t y);
	void _updateMainTarget();

	bqGSD3D11Texture* m_mainTargetRTT = 0;
#ifdef BQ_WITH_GUI
	bqGSD3D11Texture* m_GUIRTT = 0;
#endif

	bqPoint m_mainTargetSize;
	ID3D11RenderTargetView* m_currentTargetView = 0;
	ID3D11DepthStencilView* m_currentDepthStencilView = 0;

#ifdef BQ_WITH_GUI
	void _recalculateGUIMatrix();
#endif

public:
	bqGSD3D11();
	virtual ~bqGSD3D11();

	virtual bool Init(bqWindow*, const char* parameters) override;
	virtual void Shutdown() override;
	//virtual bool InitWindow(bqWindow*) override;
	//virtual void SetActiveWindow(bqWindow*) override;
	virtual bqString GetName() override;
	virtual bqString GetTextInfo() override;
	virtual bqUID GetUID() override;

	virtual void SetClearColor(float r, float g, float b, float a) override;
	virtual void BeginDraw() override;
	virtual void ClearDepth() override;
	virtual void ClearColor() override;
	virtual void ClearAll() override;
	virtual void EndDraw() override;
	virtual void SwapBuffers() override;

	virtual void DrawLine3D(const bqVec3& p1, const bqVec3& p2, const bqColor& c) override;
	virtual void SetShader(bqShaderType, uint32_t userShaderIndex) override;


	virtual bqGPUMesh* CreateMesh(bqMesh* m) override;
	virtual void SetMesh(bqGPUMesh* m) override;
	virtual void SetMaterial(bqMaterial* m) override;
	virtual void Draw() override;
	virtual void SetRasterizerState(bqGSRasterizerState) override;

	virtual bqTexture* CreateTexture(bqImage*, const bqTextureInfo&) override;
	virtual bqTexture* CreateRTT(const bqPoint& size, const bqTextureInfo&) override;
	virtual void SetRenderTarget(bqTexture*) override;
	virtual void SetRenderTargetDefault() override;

	virtual void UseVSync(bool) override;
	virtual void EnableDepth()override;
	virtual void DisableDepth() override;
	virtual void EnableBlend() override;
	virtual void DisableBlend() override;
	virtual void EnableBackFaceCulling() override;
	virtual void DisableBackFaceCulling() override;

	virtual bqVec2f GetDepthRange() override;
	virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
	virtual void SetScissorRect(const bqRect&) override;
	virtual void SetScissorRect(const bqVec4f&) override;

	virtual void OnWindowSize() override;

	virtual void SetMainTargetSize(const bqPoint&) override;

#ifdef BQ_WITH_GUI
	virtual void BeginGUI(bool clear) override;
	virtual void EndGUI() override;
	virtual void DrawGUIRectangle(const bqVec4f& rect, const bqColor& color1, const bqColor& color2,
		bqTexture* t, bqVec4f* UVs) override;
	virtual void DrawGUIText(const char32_t* text, uint32_t textSz, const bqVec2f& position,
		bqGUIDrawTextCallback*) override;
#ifdef BQ_WITH_SPRITE
	virtual void DrawText3D(const bqVec3& pos, const char32_t* text, size_t textLen,
		bqGUIFont* font, const bqColor& color, float sizeMultipler, size_t textSizeInPixels) override;
#endif
#endif

#ifdef BQ_WITH_SPRITE
	virtual void DrawSprite(bqSprite*) override;
	void _drawSprite(const bqColor& color, const bqVec4& corners, const bqVec4f& UVs, float alphaDiscard, bqGSD3D11Texture* t);
#endif

	virtual bqGPUOcclusionObject* SummonOcclusionObject() override;
	virtual void OcclusionBegin() override;
	virtual void OcclusionEnd() override;
	virtual void OcclusionDraw(bqGPUOcclusionObject*) override;
	virtual void OcclusionResult(bqGPUOcclusionObject*) override;

	bool CompileShader(const char* target,
		const char* entryPoint,
		const char* text,
		ID3D10Blob** shaderData,
		ID3D10Blob** error);

	bool CreateVertexShader(ID3D10Blob* shaderBlob, ID3D11VertexShader** vs);
	bool CreateVertexShaderInputLayout(ID3D10Blob* shaderBlob, ID3D11InputLayout**);
	bool CreateVertexShaderInputLayoutLineModel(ID3D10Blob* shaderBlob, ID3D11InputLayout**);
	bool CreatePixelShader(ID3D10Blob* shaderBlob, ID3D11PixelShader** ps);

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
	HRESULT	CreateSamplerState(D3D11_FILTER filter,
		D3D11_TEXTURE_ADDRESS_MODE addressMode,
		uint32_t anisotropic_level,
		ID3D11SamplerState** samplerState,
		D3D11_COMPARISON_FUNC cmpFunc);

	void SetActiveShader(bqGSD3D11ShaderBase* shader);
	bool CreateShaders();
};

#endif
#endif