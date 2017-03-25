/**
 * @file	GraphicsDevice.cpp
 * @brief	グラフィックデバイスの管理クラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "GraphicsDevice.h"


namespace Lib
{
	//----------------------------------------------------------------------
	// Static Private Variables
	//----------------------------------------------------------------------
	float GraphicsDevice::m_ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };


	//----------------------------------------------------------------------
	// Constructor	Destructor
	//----------------------------------------------------------------------
	GraphicsDevice::GraphicsDevice() :
		m_pDevice(NULL),
		m_pDeviceContext(NULL),
		m_pDXGI(NULL),
		m_pAdapter(NULL),
		m_pDXGIFactory(NULL),
		m_pDXGISwapChain(NULL),
		m_pBackBuffer(NULL),
		m_pRenderTargetView(NULL),
		m_pDepthStencilBuffer(NULL),
		m_pDepthStencilView(NULL),
		m_pRasterizerState(NULL),
		m_hWnd(NULL)
	{
	}

	GraphicsDevice::~GraphicsDevice()
	{
		Finalize();
	}


	//----------------------------------------------------------------------
	// Public Functions
	//----------------------------------------------------------------------
	bool GraphicsDevice::Initialize(HWND _hWnd)
	{
		if (m_pDevice != NULL)
		{
			return false;
		}

		m_hWnd = _hWnd;
		GetWindowRect(m_hWnd, &m_WindowRect);

		if (!InitDevice())
		{
			return false;
		}

		if (!InitDisplay())
		{
			return false;
		}

		return true;
	}

	void GraphicsDevice::Finalize()
	{
		ReleaseDisplay();
		ReleaseDevice();
	}

	void GraphicsDevice::BeginScene()
	{
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, m_ClearColor);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
	}

	void GraphicsDevice::EndScene()
	{
		m_pDXGISwapChain->Present(1, 0);
	}

	void GraphicsDevice::SetDepthStencilTest(bool _isStencilTest)
	{
		if (_isStencilTest)
		{
			m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
		}
		else
		{
			m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);
		}
	}


	//----------------------------------------------------------------------
	// Private Functions
	//----------------------------------------------------------------------
	bool GraphicsDevice::InitDevice()
	{
		if (FAILED(D3D11CreateDevice(
			NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			0,
			NULL,
			0,
			D3D11_SDK_VERSION,
			&m_pDevice,
			NULL,
			&m_pDeviceContext)))
		{
			return false;
		}

		return true;
	}

	bool GraphicsDevice::InitDisplay()
	{
		if (FAILED(m_pDevice->QueryInterface(__uuidof(IDXGIDevice1), reinterpret_cast<void**>(&m_pDXGI))))
		{
			return false;
		}

		if (FAILED(m_pDXGI->GetAdapter(&m_pAdapter)))
		{
			m_pDXGI->Release();
			return false;
		}

		if (FAILED(m_pAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&m_pDXGIFactory))))
		{
			m_pAdapter->Release();
			m_pDXGI->Release();
			return false;
		}

		if (FAILED(m_pDXGIFactory->MakeWindowAssociation(m_hWnd, 0)))
		{
			m_pDXGIFactory->Release();
			m_pAdapter->Release();
			m_pDXGI->Release();
			return false;
		}


		m_DXGISwapChainDesc.BufferDesc.Width = m_WindowRect.right - m_WindowRect.left;
		m_DXGISwapChainDesc.BufferDesc.Height = m_WindowRect.bottom - m_WindowRect.top;
		m_DXGISwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		m_DXGISwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		m_DXGISwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		m_DXGISwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		m_DXGISwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		m_DXGISwapChainDesc.SampleDesc.Count = 1;
		m_DXGISwapChainDesc.SampleDesc.Quality = 0;
		m_DXGISwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		m_DXGISwapChainDesc.BufferCount = 1;
		m_DXGISwapChainDesc.OutputWindow = m_hWnd;
		m_DXGISwapChainDesc.Windowed = TRUE;
		m_DXGISwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		m_DXGISwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		if (FAILED(m_pDXGIFactory->CreateSwapChain(m_pDevice, &m_DXGISwapChainDesc, &m_pDXGISwapChain)))
		{
			m_pDXGIFactory->Release();
			m_pAdapter->Release();
			m_pDXGI->Release();
			return false;
		}

		if (FAILED(m_pDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pBackBuffer))))
		{
			m_pDXGISwapChain->Release();
			m_pDXGIFactory->Release();
			m_pAdapter->Release();
			m_pDXGI->Release();
			return false;
		}

		m_pDXGISwapChain->SetFullscreenState(false, NULL);


		if (FAILED(m_pDevice->CreateRenderTargetView(m_pBackBuffer, NULL, &m_pRenderTargetView)))
		{
			m_pBackBuffer->Release();
			m_pDXGISwapChain->Release();
			m_pDXGIFactory->Release();
			m_pAdapter->Release();
			m_pDXGI->Release();
			return false;
		}


		m_DepthDesc.Width = m_WindowRect.right - m_WindowRect.left;
		m_DepthDesc.Height = m_WindowRect.bottom - m_WindowRect.top;
		m_DepthDesc.MipLevels = 1;
		m_DepthDesc.ArraySize = 1;
		m_DepthDesc.Format = DXGI_FORMAT_D32_FLOAT;
		m_DepthDesc.SampleDesc.Count = 1;
		m_DepthDesc.SampleDesc.Quality = 0;
		m_DepthDesc.Usage = D3D11_USAGE_DEFAULT;
		m_DepthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		m_DepthDesc.CPUAccessFlags = 0;
		m_DepthDesc.MiscFlags = 0;

		if (FAILED(m_pDevice->CreateTexture2D(&m_DepthDesc, NULL, &m_pDepthStencilBuffer)))
		{
			m_pRenderTargetView->Release();
			m_pBackBuffer->Release();
			m_pDXGISwapChain->Release();
			m_pDXGIFactory->Release();
			m_pAdapter->Release();
			m_pDXGI->Release();
			return false;
		}

		if (FAILED(m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, NULL, &m_pDepthStencilView)))
		{
			m_pDepthStencilBuffer->Release();
			m_pRenderTargetView->Release();
			m_pBackBuffer->Release();
			m_pDXGISwapChain->Release();
			m_pDXGIFactory->Release();
			m_pAdapter->Release();
			m_pDXGI->Release();
			return false;
		}

		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);


		m_ViewPort.TopLeftX = 0;
		m_ViewPort.TopLeftY = 0;
		m_ViewPort.Width = static_cast<float>(m_WindowRect.right - m_WindowRect.left);
		m_ViewPort.Height = static_cast<float>(m_WindowRect.bottom - m_WindowRect.top);
		m_ViewPort.MinDepth = 0.0f;
		m_ViewPort.MaxDepth = 1.0f;
		m_pDeviceContext->RSSetViewports(1, &m_ViewPort);

		m_RasterizerDesc.FillMode = D3D11_FILL_SOLID;
		m_RasterizerDesc.CullMode = D3D11_CULL_BACK;
		m_RasterizerDesc.FrontCounterClockwise = FALSE;
		m_RasterizerDesc.DepthBias = 0;
		m_RasterizerDesc.DepthBiasClamp = 0.0f;
		m_RasterizerDesc.SlopeScaledDepthBias = 0.0f;
		m_RasterizerDesc.DepthClipEnable = FALSE;
		m_RasterizerDesc.ScissorEnable = FALSE;
		m_RasterizerDesc.MultisampleEnable = FALSE;
		m_RasterizerDesc.AntialiasedLineEnable = FALSE;

		if (FAILED(m_pDevice->CreateRasterizerState(&m_RasterizerDesc, &m_pRasterizerState)))
		{
			m_pDepthStencilView->Release();
			m_pDepthStencilBuffer->Release();
			m_pRenderTargetView->Release();
			m_pBackBuffer->Release();
			m_pDXGISwapChain->Release();
			m_pDXGIFactory->Release();
			m_pAdapter->Release();
			m_pDXGI->Release();
			return false;
		}

		m_pDeviceContext->RSSetState(m_pRasterizerState);

		return true;
	}

	void GraphicsDevice::ReleaseDevice()
	{
		if (m_pDeviceContext != NULL)
		{
			m_pDeviceContext->Release();
			m_pDeviceContext = NULL;
		}

		if (m_pDevice != NULL)
		{
			m_pDevice->Release();
			m_pDevice = NULL;
		}
	}

	void GraphicsDevice::ReleaseDisplay()
	{
		if (m_pRasterizerState != NULL)
		{
			m_pRasterizerState->Release();
			m_pRasterizerState = NULL;
		}

		if (m_pDepthStencilView != NULL)
		{
			m_pDepthStencilView->Release();
			m_pDepthStencilView = NULL;
		}

		if (m_pDepthStencilBuffer != NULL)
		{
			m_pDepthStencilBuffer->Release();
			m_pDepthStencilBuffer = NULL;
		}

		if (m_pRenderTargetView != NULL)
		{
			m_pRenderTargetView->Release();
			m_pRenderTargetView = NULL;
		}

		if (m_pBackBuffer != NULL)
		{
			m_pBackBuffer->Release();
			m_pBackBuffer = NULL;
		}

		if (m_pDXGISwapChain != NULL)
		{
			m_pDXGISwapChain->SetFullscreenState(false, NULL);	// フルスクリーンの場合はウィンドウモードに変更する

			m_pDXGISwapChain->Release();
			m_pDXGISwapChain = NULL;
		}

		if (m_pDXGIFactory != NULL)
		{
			m_pDXGIFactory->Release();
			m_pDXGIFactory = NULL;
		}

		if (m_pAdapter != NULL)
		{
			m_pAdapter->Release();
			m_pAdapter = NULL;
		}

		if (m_pDXGI != NULL)
		{
			m_pDXGI->Release();
			m_pDXGI = NULL;
		}
	}
}

