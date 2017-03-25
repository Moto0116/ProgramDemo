/**
 * @file	Font.h
 * @brief	フォントクラス定義
 * @author	morimoto
 */
#ifndef LIB_FONT_H
#define LIB_FONT_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include <D3DX11.h>
#include <D3DX10.h>

#include "..\GraphicsDevice\GraphicsDevice.h"


namespace Lib
{
	/**
	 * フォントクラス
	 */
	class Font
	{
	public:
		/**
		 * コンストラクタ
		 */
		Font();

		/**
		 * デストラクタ
		 */
		~Font();

		/**
		 * 初期化処理
		 * @param[in] _pGraphicsDevice グラフィックデバイス
		 * @return 初期化に成功したらtrue 失敗したらfalse
		 */
		bool Initialize(GraphicsDevice* _pGraphicsDevice);

		/**
		 * 終了処理
		 */
		void Finalize();

		/**
		 * 頂点バッファの生成
		 */
		bool CreateVertexBuffer(const D3DXVECTOR2* _pRect, const D3DXCOLOR* _pColor = &D3DXCOLOR(0xffffffff));

		/**
		 * 頂点バッファの解放
		 */
		void ReleaseVertexBuffer();

		/**
		 * 2Dポリゴンの描画
		 */
		void Draw(const D3DXVECTOR2* _pDrawPos, LPCTSTR _pStr);

	private:
		enum
		{
			VERTEX_NUM = 4 //!< 頂点数
		};

		/**
		 * フォント表示のための頂点構造体
		 */
		struct FONT_VERTEX
		{
			D3DXVECTOR3 Pos;	//!< 頂点座標
			D3DXVECTOR2	Texel;	//!< テクスチャ座標
			D3DXCOLOR	Color;	//!< 頂点カラー
		};

		/**
		 * シェーダーに渡す定数バッファ構造体
		 */
		struct CONSTANT_BUFFER
		{
			D3DXMATRIX	MatWorld;	//!< ワールド変換行列
			D3DXVECTOR4 TexelOffset;//!< テクスチャ座標のオフセット
			D3DXVECTOR4 PosOffset;	//!< 頂点座標のオフセット
			D3DXVECTOR4 WindowSize;	//!< ウィンドウの縦横サイズ
		};

		/**
		 * 頂点シェーダーの初期化
		 * @return 初期化に成功したらtrue
		 */
		bool InitVertexShader();

		/**
		 * 頂点入力レイアウトの初期化
		 * @return 初期化に成功したらtrue 失敗したらfalse
		 */
		bool InitVertexLayout();

		/**
		 * ピクセルシェーダーの初期化
		 * @return 初期化に成功したらtrue
		 */
		bool InitPixelShader();

		/**
		 * 描画ステートの初期化
		 * @return 初期化に成功したらtrue 失敗したらfalse
		 */
		bool InitState();

		/**
		 * リソースビューの初期化
		 * @return 初期化に成功したらtrue
		 */
		bool InitResourceView();

		/**
		 * 頂点シェーダーの解放
		 */
		void ReleaseVertexShader();

		/**
		 * 頂点入力レイアウトの解放
		 */
		void ReleaseVertexLayout();

		/**
		 * ピクセルシェーダーの解放
		 */
		void ReleasePixelShader();

		/**
		 * 描画ステートの解放
		 */
		void ReleaseState();

		/**
		 * リソースビューの解放
		 */
		void ReleaseResourceView();


		static const float			m_FontTu;
		static const int			m_SpaceAsciiCode;
		GraphicsDevice*				m_pGraphicsDevice;
		ID3D11VertexShader*			m_pVertexShader;
		ID3DBlob*					m_pVertexCompiledShader;
		ID3D11InputLayout*			m_pVertexLayout;
		ID3D11PixelShader*			m_pPixelShader;
		ID3DBlob*					m_pPixelCompiledShader;
		ID3D11BlendState*			m_pBlendState;
		ID3D11SamplerState*			m_pSamplerState;
		ID3D11Buffer*				m_pVertexBuffer;
		ID3D11Buffer*				m_pConstantBuffer;
		ID3D11ShaderResourceView*	m_pShaderResource;
		D3DXVECTOR2					m_FontSize;
		float						m_WindowWidth;
		float						m_WindowHeight;

	};
}


#endif
