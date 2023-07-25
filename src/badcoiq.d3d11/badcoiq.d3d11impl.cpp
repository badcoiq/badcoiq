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

#include "badcoiq.d3d11impl.h"

#include "badcoiq/system/bqWindow.h"
#include "badcoiq/system/bqWindowWin32.h"

#include "badcoiq.d3d11.mesh.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// {0FE89B14-923C-4F2E-BA22-B18694D6F1ED}
bqDEFINE_UID(g_uid_d3d11, 0xfe89b14, 0x923c, 0x4f2e, 0xba22, 0xb1, 0x86, 0x94, 0xd6, 0xf1, 0xed);

bqGSD3D11::bqGSD3D11()
{
}

bqGSD3D11::~bqGSD3D11()
{
	Shutdown();
}

bool bqGSD3D11::Init(bqWindow* w, const char* parameters)
{
	BQ_ASSERT_ST(w);

	bqLog::PrintInfo("Init video driver - D3D11...\n");
	m_activeWindow = w;
	m_windowCurrentSize = w->GetCurrentSize();

	// создать девайс и контекст

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	auto hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_SINGLETHREADED,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&m_d3d11Device,
		&featureLevel,
		&m_d3d11DevCon);
	if (FAILED(hr))
	{
		bqLog::PrintError("Can't create Direct3D 11 Device : code %u\n", hr);
		return false;
	}

	// для создания SwapChain
	IDXGIDevice* dxgiDevice = nullptr;
	IDXGIAdapter* dxgiAdapter = nullptr;
	IDXGIFactory1* dxgiFactory = nullptr;
	hr = m_d3d11Device->QueryInterface(IID_IDXGIDevice, (void**)&dxgiDevice);
	if (FAILED(hr))
	{
		bqLog::PrintError("Can't QueryInterface : IID_IDXGIDevice, code %u\n", hr);
		return false;
	}

	hr = dxgiDevice->GetParent(IID_IDXGIAdapter, (void**)&dxgiAdapter);
	if (FAILED(hr))
	{
		bqLog::PrintError("Can't get DXGI adapter, code %u\n", hr);
		return false;
	}

	hr = dxgiAdapter->GetParent(IID_IDXGIFactory, (void**)&dxgiFactory);
	if (FAILED(hr))
	{
		bqLog::PrintError("Can't get DXGI factory, code %u\n", hr);
		return false;
	}


	DXGI_MODE_DESC	bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.Width = (UINT)m_windowCurrentSize->x;
	bufferDesc.Height = (UINT)m_windowCurrentSize->y;
	bufferDesc.RefreshRate.Numerator = 60;
	bufferDesc.RefreshRate.Denominator = 1;

	//DXGI_FORMAT_R16G16B16A16_FLOAT
	//DXGI_FORMAT_R10G10B10A2_UNORM
	//DXGI_FORMAT_R8G8B8A8_UNORM
	//DXGI_FORMAT_R8G8B8A8_UNORM_SRGB
	//DXGI_FORMAT_B8G8R8A8_UNORM
	//DXGI_FORMAT_B8G8R8A8_UNORM_SRGB
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	bqWindowWin32* w32 = (bqWindowWin32*)w->GetData()->m_implementation;
	DXGI_SWAP_CHAIN_DESC	swapChainDesc;
	memset(&swapChainDesc, 0, sizeof(swapChainDesc));
	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = w32->m_hWnd;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.Windowed = true/*m_params.m_fullScreen ? false : true*/;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	hr = dxgiFactory->CreateSwapChain(m_d3d11Device, &swapChainDesc, &m_SwapChain);
	if (FAILED(hr))
	{
		bqLog::PrintError("Can't create Swap Chain : code %u\n", hr);
		return false;
	}

	dxgiFactory->MakeWindowAssociation(w32->m_hWnd, DXGI_MWA_NO_ALT_ENTER);
	dxgiDevice->Release();
	dxgiAdapter->Release();
	dxgiFactory->Release();

	//createBackBuffer begin
	// создание того места куда будет рисоваться.
	// Пока это будет поверхность окна.
	// Потом после добавления Render Target Texture я изменю эту функцию.
	ID3D11Texture2D* BackBuffer = 0;
	if (FAILED(m_SwapChain->GetBuffer(
		0,
		IID_ID3D11Texture2D,
		(void**)&BackBuffer)))
	{
		bqLog::PrintError("Can't create Direct3D 11 back buffer\n");
		return false;
	}
	if (FAILED(this->m_d3d11Device->CreateRenderTargetView(
		BackBuffer, 0, &m_windowTargetView)))
	{
		bqLog::PrintError("Can't create Direct3D 11 render target\n");
		if (BackBuffer) BackBuffer->Release();
		return false;
	}
	if (BackBuffer) BackBuffer->Release();
	D3D11_TEXTURE2D_DESC	DSD;
	ZeroMemory(&DSD, sizeof(DSD));
	DSD.Width = (UINT)m_windowCurrentSize->x;
	DSD.Height = (UINT)m_windowCurrentSize->y;
	DSD.MipLevels = 1;
	DSD.ArraySize = 1;
	DSD.Format = DXGI_FORMAT_D32_FLOAT;
	DSD.SampleDesc.Count = 1;
	DSD.SampleDesc.Quality = 0;
	DSD.Usage = D3D11_USAGE_DEFAULT;
	DSD.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DSD.CPUAccessFlags = 0;
	DSD.MiscFlags = 0;
	if (FAILED(m_d3d11Device->CreateTexture2D(&DSD, 0, &m_windowDepthStencilBuffer)))
	{
		bqLog::PrintError("Can't create Direct3D 11 depth stencil buffer\n");
		return false;
	}
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	memset(&depthStencilViewDesc, 0, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	if (FAILED(m_d3d11Device->CreateDepthStencilView(m_windowDepthStencilBuffer, &depthStencilViewDesc, &m_windowDepthStencilView)))
	{
		bqLog::PrintError("Can't create Direct3D 11 depth stencil view\n");
		return false;
	}
	m_d3d11DevCon->OMSetRenderTargets(1, &m_windowTargetView, m_windowDepthStencilView);
	//createBackBuffer end

	D3D11_DEPTH_STENCIL_DESC	depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	if (FAILED(m_d3d11Device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilStateEnabled)))
	{
		bqLog::PrintError("Can't create Direct3D 11 depth stencil state\n");
		return false;
	}

	m_d3d11DevCon->OMSetDepthStencilState(this->m_depthStencilStateEnabled, 0);

	depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.StencilEnable = false;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthEnable = false;
	if (FAILED(m_d3d11Device->CreateDepthStencilState(&depthStencilDesc, &this->m_depthStencilStateDisabled)))
	{
		bqLog::PrintError("Can't create Direct3D 11 depth stencil state\n");
		return false;
	}

	D3D11_RASTERIZER_DESC	rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = true;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	if (FAILED(m_d3d11Device->CreateRasterizerState(&rasterDesc, &m_RasterizerSolid)))
	{
		bqLog::PrintError("Can not create rasterizer state\n");
		return false;
	}

	rasterDesc.CullMode = D3D11_CULL_NONE;
	m_d3d11Device->CreateRasterizerState(&rasterDesc, &m_RasterizerSolidNoBackFaceCulling);
	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	m_d3d11Device->CreateRasterizerState(&rasterDesc, &m_RasterizerWireframeNoBackFaceCulling);
	rasterDesc.CullMode = D3D11_CULL_BACK;
	m_d3d11Device->CreateRasterizerState(&rasterDesc, &m_RasterizerWireframe);

	m_d3d11DevCon->RSSetState(m_RasterizerSolid);

	D3D11_BLEND_DESC  bd;
	memset(&bd, 0, sizeof(bd));

	// This good for text
	bd.AlphaToCoverageEnable = 0;
	bd.RenderTarget[0].BlendEnable = TRUE;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;


	/*bd.AlphaToCoverageEnable = 0;
	bd.RenderTarget[0].BlendEnable = TRUE;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;*/

	if (FAILED(m_d3d11Device->CreateBlendState(&bd, &m_blendStateAlphaEnabled)))
	{
		bqLog::PrintError("Can't create Direct3D 11 blend state\n");
		return false;
	}
	bd.RenderTarget[0].BlendEnable = FALSE;
	if (FAILED(m_d3d11Device->CreateBlendState(&bd, &m_blendStateAlphaDisabled)))
	{
		bqLog::PrintError("Can't create Direct3D 11 blend state\n");
		return false;
	}

	float blendFactor[4];
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	m_d3d11DevCon->OMSetBlendState(m_blendStateAlphaEnabled, blendFactor, 0xffffffff);

	D3D11_RECT sr;
	sr.left = 0;
	sr.top = 0;
	sr.right = (LONG)m_windowCurrentSize->x;
	sr.bottom = (LONG)m_windowCurrentSize->y;
	m_d3d11DevCon->RSSetScissorRects(1, &sr);

	D3D11_VIEWPORT viewport;
	viewport.Width = (float)m_windowCurrentSize->x;
	viewport.Height = (float)m_windowCurrentSize->y;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	m_d3d11DevCon->RSSetViewports(1, &viewport);

	m_d3d11DevCon->OMSetDepthStencilState(m_depthStencilStateEnabled, 0);

	if (!CreateShaders())
	{
		bqLog::PrintError("Failed to create Direct3D 11 shaders\n");
		return false;
	}

	return true;
}

void bqGSD3D11::Shutdown()
{
	BQSAFE_DESTROY2(m_shaderLine3D);

	BQD3DSAFE_RELEASE(m_blendStateAlphaDisabled);
	BQD3DSAFE_RELEASE(m_blendStateAlphaEnabled);
	BQD3DSAFE_RELEASE(m_RasterizerWireframe);
	BQD3DSAFE_RELEASE(m_RasterizerWireframeNoBackFaceCulling);
	BQD3DSAFE_RELEASE(m_RasterizerSolidNoBackFaceCulling);
	BQD3DSAFE_RELEASE(m_RasterizerSolid);
	BQD3DSAFE_RELEASE(m_depthStencilStateDisabled);
	BQD3DSAFE_RELEASE(m_depthStencilStateEnabled);
	BQD3DSAFE_RELEASE(m_windowDepthStencilBuffer);
	BQD3DSAFE_RELEASE(m_windowDepthStencilView);
	BQD3DSAFE_RELEASE(m_windowTargetView);
	BQD3DSAFE_RELEASE(m_SwapChain);
	BQD3DSAFE_RELEASE(m_d3d11DevCon);
	BQD3DSAFE_RELEASE(m_d3d11Device);
}

// не реализовано
bool bqGSD3D11::InitWindow(bqWindow* w)
{
	BQ_ASSERT_ST(w);
	return true;
}

void bqGSD3D11::SetActiveWindow(bqWindow* w)
{
	BQ_ASSERT_ST(w);
	m_activeWindow = w;
}

bqString bqGSD3D11::GetName()
{
	return bqString(U"D3D11");
}

bqString bqGSD3D11::GetTextInfo()
{
	bqString info;
	info += U"GS D3D11\r\n";


	return info;
}

bqUID bqGSD3D11::GetUID()
{
	return g_uid_d3d11;
}

void bqGSD3D11::SetClearColor(float r, float g, float b, float a)
{
	m_clearColor.Set(r, g, b, a);
}

void bqGSD3D11::BeginDraw()
{
	// пока пусто
}

void bqGSD3D11::ClearDepth()
{
	m_d3d11DevCon->ClearDepthStencilView(m_windowDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void bqGSD3D11::ClearColor()
{
	m_d3d11DevCon->ClearRenderTargetView(m_windowTargetView, m_clearColor.Data());
}

void bqGSD3D11::ClearAll()
{
	m_d3d11DevCon->ClearDepthStencilView(m_windowDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	m_d3d11DevCon->ClearRenderTargetView(m_windowTargetView, m_clearColor.Data());
}

void bqGSD3D11::EndDraw()
{
	// пока пусто
}

void bqGSD3D11::SwapBuffers()
{
	m_SwapChain->Present(m_vsync, 0);
}

bool bqGSD3D11::CreateShaders(
	const char* vertexTarget,
	const char* pixelTarget,
	const char* vertexShader,
	const char* pixelShader,
	const char* vertexEntryPoint,
	const char* pixelEntryPoint,
	bqMeshVertexType vertexType,
	ID3D11VertexShader** vs,
	ID3D11PixelShader** ps,
	ID3D11InputLayout** il)
{
	ID3D10Blob* m_VsBlob = nullptr;
	ID3D10Blob* m_PsBlob = nullptr;
	ID3D10Blob* m_errorBlob = nullptr;

	HRESULT hr = D3DCompile(
		vertexShader,
		strlen(vertexShader),
		0, 0, 0,
		vertexEntryPoint,
		vertexTarget,
		0,
		0,
		&m_VsBlob,
		&m_errorBlob
	);

	if (FAILED(hr))
	{
		char* message = (char*)m_errorBlob->GetBufferPointer();
		bqLog::PrintError("Vertex shader compile error: %s\n", message);
		return false;
	}

	hr = D3DCompile(
		pixelShader,
		strlen(pixelShader),
		0, 0, 0,
		pixelEntryPoint,
		pixelTarget,
		0,
		0,
		&m_PsBlob,
		&m_errorBlob
	);

	if (FAILED(hr))
	{
		char* message = (char*)m_errorBlob->GetBufferPointer();
		bqLog::PrintError("Pixel shader compile error: %s\n", message);
		return false;
	}

	hr = m_d3d11Device->CreateVertexShader(
		m_VsBlob->GetBufferPointer(),
		m_VsBlob->GetBufferSize(),
		0,
		vs);
	if (FAILED(hr))
	{
		bqLog::PrintError("Can't create vertex shader. Error code [%u]\n", hr);
		return false;
	}

	hr = m_d3d11Device->CreatePixelShader(
		m_PsBlob->GetBufferPointer(),
		m_PsBlob->GetBufferSize(),
		0,
		ps);
	if (FAILED(hr))
	{
		bqLog::PrintError("Can't create pixel shader. Error code [%u]\n", hr);
		return false;
	}

	if (vertexType != bqMeshVertexType::Null)
	{
		D3D11_INPUT_ELEMENT_DESC vertexLayout[8];
		uint32_t vertexLayoutSize = 0;
		/*
		LPCSTR SemanticName;
		UINT SemanticIndex;
		DXGI_FORMAT Format;
		UINT InputSlot;
		UINT AlignedByteOffset;
		D3D11_INPUT_CLASSIFICATION InputSlotClass;
		UINT InstanceDataStepRate;
		*/

		int ind = 0;
		switch (vertexType)
		{
		case bqMeshVertexType::Triangle:
			ind = 0;
			vertexLayout[ind].SemanticName = "POSITION";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 0;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "TEXCOORD";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 12;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "NORMAL";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 20;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "BINORMAL";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 32;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "TANGENT";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 44;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "COLOR";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 56;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "BONES";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R8G8B8A8_UINT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 72;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "WEIGHTS";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 76;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			break;
		}
		vertexLayoutSize = ind + 1;

		hr = m_d3d11Device->CreateInputLayout(
			vertexLayout,
			vertexLayoutSize,
			m_VsBlob->GetBufferPointer(),
			m_VsBlob->GetBufferSize(),
			il);
		if (FAILED(hr))
		{
			bqLog::PrintError("Can't create input layout. Error code [%u]\n", hr);
			return false;
		}
	}


	if (m_VsBlob)    m_VsBlob->Release();
	if (m_PsBlob)    m_PsBlob->Release();
	if (m_errorBlob) m_errorBlob->Release();

	return true;
}

bool bqGSD3D11::CreateConstantBuffer(uint32_t byteSize, ID3D11Buffer** cb)
{
	D3D11_BUFFER_DESC mbd;
	memset(&mbd, 0, sizeof(mbd));
	mbd.Usage = D3D11_USAGE_DYNAMIC;
	mbd.ByteWidth = byteSize;
	mbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	mbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	mbd.MiscFlags = 0;
	mbd.StructureByteStride = 0;

	HRESULT hr = m_d3d11Device->CreateBuffer(&mbd, 0, cb);
	if (FAILED(hr))
	{
		bqLog::PrintError("Can't create constant buffer. Error code [%u]\n", hr);
		return false;
	}
	return true;
}

bool bqGSD3D11::CreateGeometryShaders(const char* target,
	const char* shaderText,
	const char* entryPoint,
	ID3D11GeometryShader** gs)
{
	ID3D10Blob* m_GsBlob = nullptr;
	ID3D10Blob* m_errorBlob = nullptr;
	HRESULT hr = D3DCompile(
		shaderText,
		strlen(shaderText),
		0, 0, 0,
		entryPoint,
		target,
		0,
		0,
		&m_GsBlob,
		&m_errorBlob
	);
	if (FAILED(hr))
	{
		char* message = (char*)m_errorBlob->GetBufferPointer();
		bqLog::PrintError("Geometry shader compile error: %s\n", message);
		return false;
	}

	hr = m_d3d11Device->CreateGeometryShader(
		m_GsBlob->GetBufferPointer(),
		m_GsBlob->GetBufferSize(),
		0,
		gs);
	if (FAILED(hr))
	{
		bqLog::PrintError("Can't create geometry shader. Error code [%u]\n", hr);
		return false;
	}

	return true;
}

void bqGSD3D11::SetActiveShader(bqGSD3D11ShaderBase* shader)
{
	m_d3d11DevCon->IASetInputLayout(shader->m_vLayout);
	m_d3d11DevCon->VSSetShader(shader->m_vShader, 0, 0);
	m_d3d11DevCon->GSSetShader(shader->m_gShader, 0, 0);
	m_d3d11DevCon->PSSetShader(shader->m_pShader, 0, 0);
}

bool bqGSD3D11::CreateShaders()
{
	m_shaderLine3D = bqCreate<bqD3D11ShaderLine3D>(this);
	if (!m_shaderLine3D->Init())
		return false;

	return true;
}

void bqGSD3D11::DrawLine3D(const bqVec4& p1, const bqVec4& p2, const bqColor& c)
{
	m_shaderLine3D->SetData(p1, p2, c, *bqFramework::GetMatrix(bqMatrixType::ViewProjection));
	m_shaderLine3D->SetConstants(0); // возможно лишнее. Не не лишнее. Там работа с константным буфером.
	m_d3d11DevCon->Draw(2, 0);
}

void bqGSD3D11::SetShader(bqShaderType t, uint32_t userShaderIndex)
{
	switch (t)
	{
	case bqShaderType::Line3D:
		SetActiveShader(m_shaderLine3D);
		m_d3d11DevCon->IASetInputLayout(NULL);
		m_d3d11DevCon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
		break;
	case bqShaderType::User:
		break;
	default:
		break;
	}
}

// Надо создать два буфера. Вершинный и индексный.
// Так-же перед созданием надо дать DirectX'у знать что за буферы создаём.
bqGPUMesh* bqGSD3D11::SummonMesh(bqMesh* m)
{
	BQ_ASSERT_ST(m);
	BQ_ASSERT_ST(m->GetVBuffer());
	BQ_ASSERT_ST(m->GetIBuffer());
	BQ_ASSERT_ST(m->GetInfo().m_iCount);
	BQ_ASSERT_ST(m->GetInfo().m_vCount);

	bqGSD3D11Mesh* newMesh = new bqGSD3D11Mesh;
	newMesh->m_meshInfo = m->GetInfo();

	D3D11_BUFFER_DESC vbd;
	D3D11_BUFFER_DESC ibd;
	memset(&vbd, 0, sizeof(vbd));
	memset(&ibd, 0, sizeof(ibd));

	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;

	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vData;
	D3D11_SUBRESOURCE_DATA iData;
	memset(&vData, 0, sizeof(vData));
	memset(&iData, 0, sizeof(iData));

	vbd.ByteWidth = newMesh->m_meshInfo.m_stride * newMesh->m_meshInfo.m_vCount;
	vData.pSysMem = &m->GetVBuffer()[0];

	auto hr = m_d3d11Device->CreateBuffer(&vbd, &vData, &newMesh->m_vBuffer);
	if (FAILED(hr))
	{
		bqLog::PrintError("Can't create Direct3D 11 vertex buffer [%u]\n", hr);
	}
	else
	{
		uint32_t index_sizeof = sizeof(uint16_t);
		newMesh->m_indexType = DXGI_FORMAT_R16_UINT;
		if (newMesh->m_meshInfo.m_indexType == bqMeshIndexType::u32)
		{
			newMesh->m_indexType = DXGI_FORMAT_R32_UINT;
			index_sizeof = sizeof(uint32_t);
		}
		ibd.ByteWidth = index_sizeof * newMesh->m_meshInfo.m_iCount;
		iData.pSysMem = &m->GetIBuffer()[0];

		hr = m_d3d11Device->CreateBuffer(&ibd, &iData, &newMesh->m_iBuffer);
		if (FAILED(hr))
		{
			bqLog::PrintError("Can't create Direct3D 11 index buffer [%u]\n", hr);
		}
		else
		{
			return newMesh;
		}
	}
	if (newMesh)
		delete newMesh; // в данном месте работает только в случае ошибки
	return 0;
}
