/**
 * @file	GraphicsDevice.h
 * @brief	グラフィックデバイスの管理クラス定義
 * @author	morimoto
 */
#ifndef LIB_GRAPHICSDEVICE_H
#define LIB_GRAPHICSDEVICE_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include <D3DX11.h>
#include <D3DX10.h>

#include "..\..\SingletonBase\SingletonBase.h"


namespace Lib
{
	/**
	 * グラフィックデバイスの管理クラス定義
	 */
	class GraphicsDevice : public SingletonBase<GraphicsDevice>
	{
	public:
		friend SingletonBase<GraphicsDevice>;

		/**
		 * 初期化処理
		 * @param[in] _hWnd GraphicsDeviceが対応するウィンドウのハンドル
		 * @return 初期化に成功したらtrue 失敗したらfalse
		 */
		bool Initialize(HWND _hWnd);

		/**
		 * 終了処理
		 */
		void Finalize();

		/**
		 * 描画前処理
		 */
		void BeginScene();

		/**
		 * 描画後処理
		 */
		void EndScene();

		/**
		 * 深度テストの設定
		 * @param[in] _isStencilTest 深度テストを使用するかのフラグ
		 */
		void SetDepthStencilTest(bool _isStencilTest);

		/**
		 * DirectXデバイスの取得
		 * @return DirectXデバイス
		 */
		inline ID3D11Device* GetDevice() 
		{
			return m_pDevice;
		}

		/**
		 * デバイスコンテキストの取得
		 * @return デバイスコンテキスト
		 */
		inline ID3D11DeviceContext* GetDeviceContext()
		{
			return m_pDeviceContext;
		}

		/**
		 * GraphicsDeviceに結びついているウィンドウのハンドルを取得する
		 * @return ウィンドウハンドル
		 */
		inline HWND GetMainWindowHandle()
		{
			return m_hWnd;
		}

		/**
		 * GraphicsDeviceに結びついているウィンドウのサイズを取得する
		 * @return ウィンドウサイズ
		 */
		inline const RECT* GetMainWindowRect()
		{
			return &m_WindowRect;
		}

		/**
		 * フルスクリーン設定を行う
		 * @param[in] _isFullScreenState フルスクリーンの設定
		 */
		inline void SetFullScreen(bool _isFullScreenState)
		{
			m_pDXGISwapChain->SetFullscreenState(_isFullScreenState, NULL);
		}


	private:
		/**
		 * コンストラクタ
		 */
		GraphicsDevice();

		/**
		 * デストラクタ
		 */
		virtual ~GraphicsDevice();

		/**
		 * デバイスの初期化関数
		 * @return 初期化に成功したらtrue 失敗したらfalse
		 */
		bool InitDevice();

		/** 
		 * ディスプレイ系の初期化関数
		 * @return 初期化に成功したらtrue 失敗したらfalse
		 */
		bool InitDisplay();

		/**
		 * デバイスの解放関数
		 */
		void ReleaseDevice();

		/**
		 * ディスプレイ系の解放関数
		 */
		void ReleaseDisplay();


		static float			m_ClearColor[4];
		ID3D11Device*			m_pDevice;
		ID3D11DeviceContext*	m_pDeviceContext;
		IDXGIDevice1*			m_pDXGI;
		IDXGIAdapter*			m_pAdapter;
		IDXGIFactory*			m_pDXGIFactory;
		DXGI_SWAP_CHAIN_DESC	m_DXGISwapChainDesc;
		IDXGISwapChain*			m_pDXGISwapChain;
		ID3D11Texture2D*		m_pBackBuffer;
		ID3D11RenderTargetView* m_pRenderTargetView;
		ID3D11Texture2D*		m_pDepthStencilBuffer;
		D3D11_TEXTURE2D_DESC	m_DepthDesc;
		ID3D11DepthStencilView* m_pDepthStencilView;
		D3D11_VIEWPORT			m_ViewPort;
		D3D11_RASTERIZER_DESC	m_RasterizerDesc;
		ID3D11RasterizerState*  m_pRasterizerState;
		HWND					m_hWnd;
		RECT					m_WindowRect;

	};
}


#endif // LIB_GRAPHICSDEVICE_H
