/**
 * @file	GraphicsDevice.cpp
 * @brief	グラフィックデバイスの管理クラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "GraphicsDevice.h"

#include "Debugger\Debugger.h"


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
			OutputErrorLog("グラフィックデバイスオブジェクトは既に初期化されています");
			return false;
		}

		m_hWnd = _hWnd;
		GetWindowRect(m_hWnd, &m_WindowRect);

		if (!CreateDevice())
		{
			return false;
		}

		if (!CreateDisplay())
		{
			ReleaseDevice();
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
	bool GraphicsDevice::CreateDevice()
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
			OutputErrorLog("DirectX11デバイスの生成に失敗しました");
			return false;
		}

		return true;
	}

	bool GraphicsDevice::CreateDisplay()
	{
		// グラフィックインターフェースの取得.
		if (FAILED(m_pDevice->QueryInterface(__uuidof(IDXGIDevice1), reinterpret_cast<void**>(&m_pDXGI))))
		{
			OutputErrorLog("グラフィックインターフェースの取得に失敗しました");
			return false;
		}

		// グラフィックインターフェースアダプタの取得.
		if (FAILED(m_pDXGI->GetAdapter(&m_pAdapter)))
		{
			OutputErrorLog("グラフィックインターフェースアダプタの取得に失敗しました");
			ReleaseDisplay();
			return false;
		}

		// グラフィックインターフェースファクトリの取得.
		if (FAILED(m_pAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&m_pDXGIFactory))))
		{
			OutputErrorLog("グラフィックインターフェースファクトリの取得に失敗しました");
			ReleaseDisplay();
			return false;
		}

		// DXGIにウィンドウメッセージキューの監視を設定.
		if (FAILED(m_pDXGIFactory->MakeWindowAssociation(m_hWnd, 0)))
		{
			OutputErrorLog("ウィンドウメッセージキュー監視設定に失敗しました");
			ReleaseDisplay();
			return false;
		}


		// スワップチェインの設定.
		DXGI_SWAP_CHAIN_DESC SwapChainDesc;
		SwapChainDesc.BufferDesc.Width = m_WindowRect.right - m_WindowRect.left;
		SwapChainDesc.BufferDesc.Height = m_WindowRect.bottom - m_WindowRect.top;
		SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		SwapChainDesc.SampleDesc.Count = 1;
		SwapChainDesc.SampleDesc.Quality = 0;
		SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		SwapChainDesc.BufferCount = 1;
		SwapChainDesc.OutputWindow = m_hWnd;
		SwapChainDesc.Windowed = TRUE;
		SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		// スワップチェインの生成.
		if (FAILED(m_pDXGIFactory->CreateSwapChain(m_pDevice, &SwapChainDesc, &m_pDXGISwapChain)))
		{
			OutputErrorLog("スワップチェインの作成に失敗しました");
			ReleaseDisplay();
			return false;
		}

		// バックバッファの取得.
		if (FAILED(m_pDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pBackBuffer))))
		{
			OutputErrorLog("バックバッファの取得に失敗しました");
			ReleaseDisplay();
			return false;
		}

		m_pDXGISwapChain->SetFullscreenState(false, NULL); // フルスクリーンモードを切る.


		// レンダーターゲットビューの作成に失敗しました.
		if (FAILED(m_pDevice->CreateRenderTargetView(m_pBackBuffer, NULL, &m_pRenderTargetView)))
		{
			OutputErrorLog("レンダーターゲットビューの作成に失敗しました");
			ReleaseDisplay();
			return false;
		}

		// 深度ステンシルバッファの設定.
		D3D11_TEXTURE2D_DESC DepthDesc;
		DepthDesc.Width = m_WindowRect.right - m_WindowRect.left;
		DepthDesc.Height = m_WindowRect.bottom - m_WindowRect.top;
		DepthDesc.MipLevels = 1;
		DepthDesc.ArraySize = 1;
		DepthDesc.Format = DXGI_FORMAT_D32_FLOAT;
		DepthDesc.SampleDesc.Count = 1;
		DepthDesc.SampleDesc.Quality = 0;
		DepthDesc.Usage = D3D11_USAGE_DEFAULT;
		DepthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		DepthDesc.CPUAccessFlags = 0;
		DepthDesc.MiscFlags = 0;

		// 深度ステンシルバッファの生成.
		if (FAILED(m_pDevice->CreateTexture2D(&DepthDesc, NULL, &m_pDepthStencilBuffer)))
		{
			OutputErrorLog("深度ステンシルバッファの作成に失敗しました");
			ReleaseDisplay();
			return false;
		}

		// 深度ステンシルビューの作成.
		if (FAILED(m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, NULL, &m_pDepthStencilView)))
		{
			OutputErrorLog("深度ステンシルビューの作成に失敗しました");
			ReleaseDisplay();
			return false;
		}

		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView); // 描画先に設定.


		// ビューポートの設定.
		m_ViewPort.TopLeftX = 0;
		m_ViewPort.TopLeftY = 0;
		m_ViewPort.Width = static_cast<float>(m_WindowRect.right - m_WindowRect.left);
		m_ViewPort.Height = static_cast<float>(m_WindowRect.bottom - m_WindowRect.top);
		m_ViewPort.MinDepth = 0.0f;
		m_ViewPort.MaxDepth = 1.0f;
		m_pDeviceContext->RSSetViewports(1, &m_ViewPort);


		// ラスタライザステートの設定.
		D3D11_RASTERIZER_DESC RasterizerDesc;
		RasterizerDesc.FillMode = D3D11_FILL_SOLID;
		RasterizerDesc.CullMode = D3D11_CULL_BACK;
		RasterizerDesc.FrontCounterClockwise = FALSE;
		RasterizerDesc.DepthBias = 0;
		RasterizerDesc.DepthBiasClamp = 0.0f;
		RasterizerDesc.SlopeScaledDepthBias = 0.0f;
		RasterizerDesc.DepthClipEnable = FALSE;
		RasterizerDesc.ScissorEnable = FALSE;
		RasterizerDesc.MultisampleEnable = FALSE;
		RasterizerDesc.AntialiasedLineEnable = FALSE;

		// ラスタライザステートの生成.
		if (FAILED(m_pDevice->CreateRasterizerState(&RasterizerDesc, &m_pRasterizerState)))
		{
			OutputErrorLog("ラスタライザステートの作成に失敗しました");
			ReleaseDisplay();
			return false;
		}

		m_pDeviceContext->RSSetState(m_pRasterizerState);	// ラスタライザステートをセット.

		return true;
	}

	void GraphicsDevice::ReleaseDevice()
	{
		SafeRelease(m_pDeviceContext);
		SafeRelease(m_pDevice);
	}

	void GraphicsDevice::ReleaseDisplay()
	{
		SafeRelease(m_pRasterizerState);
		SafeRelease(m_pDepthStencilView);
		SafeRelease(m_pDepthStencilBuffer);
		SafeRelease(m_pRenderTargetView);
		SafeRelease(m_pBackBuffer);
		SafeRelease(m_pDXGISwapChain);
		SafeRelease(m_pDXGIFactory);
		SafeRelease(m_pAdapter);
		SafeRelease(m_pDXGI);
	}
}

