#include "Graphics.h"
#include "imgui.h"
#include "examples/imgui_impl_win32.h"
#include "examples/imgui_impl_dx11.h"
#include <assert.h>
#include <d3dcompiler.h>

namespace Pixels {

	// vertex format for the framebuffer fullscreen textured quad
	struct FSQVertex
	{
		float x, y, z;		// position
		float u, v;			// texcoords
	};

    Graphics::Graphics(int width, int height)
    {

		m_window = new Window("Pixels", width, height);
		m_width = width;
		m_height = height;

		// create device and swap chain/get render target view
		DXGI_SWAP_CHAIN_DESC sd = {};
		sd.BufferCount = 1;
		sd.BufferDesc.Width = m_width;
		sd.BufferDesc.Height = m_height;
		sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 1;
		sd.BufferDesc.RefreshRate.Denominator = 60;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = m_window->getHandle();
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;

		HRESULT	hr;

		// create device and front/back buffers
		hr = D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			0u,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&sd,
			&m_swapChain,
			&m_device,
			nullptr,
			&m_immediateContext);

		// get handle to backbuffer
		ComPtr<ID3D11Resource> pBackBuffer;
		hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

		// create a view on backbuffer that we can render to
		hr = m_device->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &m_renderTargetView);

		// set backbuffer as the render target using created view
		m_immediateContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), nullptr);

		// set viewport dimensions
		D3D11_VIEWPORT vp;
		vp.Width = float(m_width);
		vp.Height = float(m_height);
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		m_immediateContext->RSSetViewports(1, &vp);

		// create texture for cpu render target
		D3D11_TEXTURE2D_DESC sysTexDesc;
		sysTexDesc.Width = m_width;
		sysTexDesc.Height = m_height;
		sysTexDesc.MipLevels = 1;
		sysTexDesc.ArraySize = 1;
		sysTexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		sysTexDesc.SampleDesc.Count = 1;
		sysTexDesc.SampleDesc.Quality = 0;
		sysTexDesc.Usage = D3D11_USAGE_DYNAMIC;
		sysTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		sysTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		sysTexDesc.MiscFlags = 0;

		// create the texture
		hr = m_device->CreateTexture2D(&sysTexDesc, nullptr, &m_sysBufferTexture);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = sysTexDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		// create the resource view on the texture
		hr = m_device->CreateShaderResourceView(m_sysBufferTexture.Get(), &srvDesc, &m_sysBufferTextureView);


		// create pixel shader for framebuffer
		ComPtr<ID3DBlob> shaderCompileErrorsBlob;
		ComPtr<ID3DBlob> vblob;
		HRESULT hResult = D3DCompileFromFile(L"../../Pixels/shaders/FramebufferPS.hlsl", nullptr, nullptr, "FramebufferPS", "ps_5_0", 0, 0,
			vblob.GetAddressOf(), shaderCompileErrorsBlob.GetAddressOf());
		if (FAILED(hResult))
		{
			const char* errorString = NULL;
			if (hResult == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
				errorString = "Could not compile shader; file not found";
			else if (shaderCompileErrorsBlob)
				errorString = (const char*)shaderCompileErrorsBlob->GetBufferPointer();

			MessageBoxA(0, errorString, "Shader Compiler Error", MB_ICONERROR | MB_OK);
		}

		hResult = m_device.Get()->CreatePixelShader(vblob->GetBufferPointer(), vblob->GetBufferSize(), nullptr, m_pixelShader.GetAddressOf());
		assert(SUCCEEDED(hResult));


		// create vertex shader for framebuffer
		ComPtr<ID3DBlob> vshaderCompileErrorsBlob;
		ComPtr<ID3DBlob> pblob;
		hResult = D3DCompileFromFile(L"../../Pixels/shaders/FramebufferVS.hlsl", nullptr, nullptr, "FramebufferVS", "vs_5_0", 0, 0,
			pblob.GetAddressOf(), vshaderCompileErrorsBlob.GetAddressOf());
		if (FAILED(hResult))
		{
			const char* errorString = NULL;
			if (hResult == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
				errorString = "Could not compile shader; file not found";
			else if (vshaderCompileErrorsBlob)
				errorString = (const char*)vshaderCompileErrorsBlob->GetBufferPointer();

			MessageBoxA(0, errorString, "Shader Compiler Error", MB_ICONERROR | MB_OK);
		}

		hResult = m_device.Get()->CreateVertexShader(pblob->GetBufferPointer(), pblob->GetBufferSize(), nullptr, m_vertexShader.GetAddressOf());
		assert(SUCCEEDED(hResult));


		// create and fill vertex buffer with quad for rendering frame
		const FSQVertex vertices[] =
		{
			{ -1.0f,1.0f,0.5f,0.0f,0.0f },
			{ 1.0f,1.0f,0.5f,1.0f,0.0f },
			{ 1.0f,-1.0f,0.5f,1.0f,1.0f },
			{ -1.0f,1.0f,0.5f,0.0f,0.0f },
			{ 1.0f,-1.0f,0.5f,1.0f,1.0f },
			{ -1.0f,-1.0f,0.5f,0.0f,1.0f },
		};
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(FSQVertex) * 6;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0u;
		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = vertices;
		hr = m_device->CreateBuffer(&bd, &initData, &m_vertexBuffer);


		// create input layout for fullscreen quad
		const D3D11_INPUT_ELEMENT_DESC ied[] =
		{
			{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 }
		};

		// Ignore the intellisense error "namespace has no member"
		hResult = m_device.Get()->CreateInputLayout(ied, 2,
			pblob->GetBufferPointer(),
			pblob->GetBufferSize(),
			m_inputLayout.GetAddressOf());
		assert(SUCCEEDED(hResult));


		// Create sampler state for fullscreen textured quad
		D3D11_SAMPLER_DESC sampDesc = {};
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		hr = m_device->CreateSamplerState(&sampDesc, &m_samplerState);

		// allocate memory for sysbuffer (16-byte aligned for faster access)
		m_sysBuffer = reinterpret_cast<Color*>(
			_aligned_malloc(sizeof(Color) * m_width * m_height, 16u));

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		// setup platform/renderer backends for imgui
		ImGui_ImplWin32_Init(m_window->getHandle());
		ImGui_ImplDX11_Init(m_device.Get(), m_immediateContext.Get());
    }

    void Graphics::beginFrame()
    {
		// clear the sysbuffer
		memset(m_sysBuffer, 0u, sizeof(Color) * m_height * m_width);

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
    }

    void Graphics::endFrame()
    {

		HRESULT hr;

		// lock and map the adapter memory for copying over the sysbuffer
		hr = m_immediateContext->Map(m_sysBufferTexture.Get(), 0u,
			D3D11_MAP_WRITE_DISCARD, 0u, &m_mappedSysBufferTexture);

		// setup parameters for copy operation
		Color* pDst = reinterpret_cast<Color*>(m_mappedSysBufferTexture.pData);
		const size_t dstPitch = m_mappedSysBufferTexture.RowPitch / sizeof(Color);
		const size_t srcPitch = m_width;
		const size_t rowBytes = srcPitch * sizeof(Color);
		
		// perform the copy line-by-line
		for (size_t y = 0u; y < m_height; y++)
			memcpy(&pDst[y * dstPitch], &m_sysBuffer[y * srcPitch], rowBytes);

		// release the adapter memory
		m_immediateContext->Unmap(m_sysBufferTexture.Get(), 0u);

		// render offscreen scene texture to back buffer
		m_immediateContext->IASetInputLayout(m_inputLayout.Get());
		m_immediateContext->VSSetShader(m_vertexShader.Get(), nullptr, 0u);
		m_immediateContext->PSSetShader(m_pixelShader.Get(), nullptr, 0u);
		m_immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		const UINT stride = sizeof(FSQVertex);
		const UINT offset = 0u;
		m_immediateContext->IASetVertexBuffers(0u, 1u, m_vertexBuffer.GetAddressOf(), &stride, &offset);
		m_immediateContext->PSSetShaderResources(0u, 1u, m_sysBufferTextureView.GetAddressOf());
		m_immediateContext->PSSetSamplers(0u, 1u, m_samplerState.GetAddressOf());
		m_immediateContext->Draw(6u, 0u);

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// flip back/front buffers
		hr = m_swapChain->Present(1u, 0u);

    }

	void Graphics::putPixel(int x, int y, int r, int g, int b)
	{
		m_sysBuffer[m_width * y + x] = { unsigned char(r), unsigned char(g),unsigned char(b) };
	}

	Graphics::~Graphics()
	{
		// free sysbuffer memory (aligned free)
		if (m_sysBuffer)
		{
			_aligned_free(m_sysBuffer);
			m_sysBuffer = nullptr;
		}
		// clear the state of the device context before destruction
		if (m_immediateContext) 
			m_immediateContext->ClearState();

		// Cleanup
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		delete m_window;
	}

}