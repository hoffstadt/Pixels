#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Window.h"
#include <wrl.h>

namespace Pixels {

	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	class Color
	{

	public:

		Color()
			:
			dword()
		{}

		Color(const Color& col)
			:
			dword(col.dword)
		{}

		Color(unsigned char r, unsigned char g, unsigned char b)
			:
			dword((r << 16u) | (g << 8u) | b)
		{}

		Color& operator =(Color color)
		{
			dword = color.dword;
			return *this;
		}

	private:

		unsigned int dword;

	};

	class Graphics
	{

	public:

		Graphics(int width, int height);
		~Graphics();

		void beginFrame ();
		void endFrame   ();
		void putPixel(int x, int y, int r, int g, int b);

	private:

		ComPtr<IDXGISwapChain>           m_swapChain;
		ComPtr<ID3D11Device>             m_device;
		ComPtr<ID3D11DeviceContext>      m_immediateContext;
		ComPtr<ID3D11RenderTargetView>   m_renderTargetView;
		ComPtr<ID3D11Texture2D>          m_sysBufferTexture;
		ComPtr<ID3D11ShaderResourceView> m_sysBufferTextureView;
		ComPtr<ID3D11PixelShader>        m_pixelShader;
		ComPtr<ID3D11VertexShader>       m_vertexShader;
		ComPtr<ID3D11Buffer>             m_vertexBuffer;
		ComPtr<ID3D11InputLayout>        m_inputLayout;
		ComPtr<ID3D11SamplerState>       m_samplerState;
		D3D11_MAPPED_SUBRESOURCE         m_mappedSysBufferTexture;
		Color*                           m_sysBuffer = nullptr;
		Window*                          m_window;
		int                              m_width;
		int                              m_height;

	};

}