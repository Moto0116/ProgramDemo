/**
 * @file	Vertex2D.h
 * @brief	2Dポリゴンの描画クラス定義
 * @author	morimoto
 */
#ifndef LIB_VERTEX2D_H
#define LIB_VERTEX2D_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include <D3DX11.h>
#include <D3DX10.h>

#include "..\GraphicsDevice\GraphicsDevice.h"
#include "..\AnimationManager\IAnimation\IAnimation.h"
#include "..\TextureManager\ITexture\ITexture.h"


namespace Lib
{
	/**
	 * 2Dポリゴンの描画クラス
	 */
	class Vertex2D
	{
	public:
		/**
		 * コンストラクタ
		 */
		Vertex2D();

		/**
		 * デストラクタ
		 */
		~Vertex2D();

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
		 * @param[in] _pRect 矩形情報
		 * @param[in] _pMinUV テクスチャのMin座標
		 * @param[in] _pMaxUV テクスチャのMax座標
		 * @param[in] _pColor 頂点色
		 * @return 初期化に成功したらtrue 失敗したらfalse
		 */
		bool CreateVertexBuffer(
			const D3DXVECTOR2* _pRect, 
			const D3DXVECTOR2* _pMinUV = &D3DXVECTOR2(0.f, 0.f),
			const D3DXVECTOR2* _pMaxUV = &D3DXVECTOR2(1.0f, 1.0f),
			const D3DXCOLOR* _pColor = &D3DXCOLOR(0xffffffff));

		/**
		 * 頂点バッファの解放
		 */
		void ReleaseVertexBuffer();

		/**
		 * 頂点バッファにデータを書き込む
		 * @return 初期化に成功したらtrue 失敗したらfalse
		 */
		bool WriteVertexBuffer();

		/**
		 * 頂点座標をセットする
		 * @param[in] _pDrawPos 描画する2Dポリゴンの座標
		 */
		void SetVertex(const D3DXVECTOR2* _pRect);

		/**
		 * テクスチャ座標をセットする
		 * @param[in] _pScale 描画する2Dポリゴンの拡縮率
		 */
		void SetUV(const D3DXVECTOR2* _pMinUV, const D3DXVECTOR2* _pMaxUV);

		/**
		 * カラー情報をセットする
		 * @param[in] _angle 描画する2Dポリゴンの傾き
		 */
		void SetColor(const D3DXCOLOR* _pColor);

		/**
		 * 定数バッファにデータを書き込む
		 * @return 書き込みに成功したらtrue 失敗したらfalse
		 */
		bool WriteConstantBuffer(
			const D3DXVECTOR2* _pDrawPos,
			const D3DXVECTOR2* _pScale = &D3DXVECTOR2(1.f, 1.f),
			float _angle = 0.f);

		/**
		 * 2Dポリゴンの描画
		 */
		void Draw();

		/**
		 * 描画するテクスチャをセットする
		 * @param[in] _pTexture テクスチャオブジェクト
		 */
		void SetTexture(ITexture* _pTexture);

		/**
		 * 描画するテクスチャをセットする
		 * @param[in] _pTexture テクスチャオブジェクト
		 */
		void SetTexture(ID3D11ShaderResourceView* _pTexture);///@todo テスト用

		/**
		 * 描画時のアニメーションをセットする
		 * @param[in] _pAnimation アニメーションオブジェクト
		 */
		inline void SetAnimation(IAnimation* _pAnimation)
		{
			m_pAnimation = _pAnimation;
		}


	private:
		enum 
		{ 
			VERTEX_NUM = 4 //!< 頂点数
		};

		/**
		 * 2D矩形描画用の頂点構造体
		 */
		struct VERTEX
		{
			D3DXVECTOR3		Pos;	//!< 頂点座標
			D3DXVECTOR2		UV;		//!< テクスチャ座標
			D3DXCOLOR		Color;	//!< 頂点カラー値
		};

		/**
		 * シェーダーで使用する定数バッファ
		 */
		struct CONSTANT_BUFFER
		{
			D3DXMATRIX  MatWorld;	//!< ワールド変換行列
			D3DXVECTOR4 WindowSize;	//!< ウィンドウの縦横サイズ
		};


		/**
		 * 頂点シェーダーの初期化
		 * @return 初期化に成功したらtrue 失敗したらfalse
		 */
		bool InitVertexShader();

		/**
		 * 頂点入力レイアウトの初期化
		 * @return 初期化に成功したらtrue 失敗したらfalse
		 */
		bool InitVertexLayout();

		/**
		 * ピクセルシェーダーの初期化
		 * @return 初期化に成功したらtrue 失敗したらfalse
		 */
		bool InitPixelShader();

		/**
		 * 描画ステートの初期化
		 * @return 初期化に成功したらtrue 失敗したらfalse
		 */
		bool InitState();
		
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
		 * ステートの解放
		 */
		void ReleaseState();


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
		ITexture*					m_pTexture;
		IAnimation*					m_pAnimation;
		VERTEX						m_pVertexData[VERTEX_NUM];
		float						m_WindowWidth;
		float						m_WindowHeight;

		ID3D11ShaderResourceView*	m_pTextureResource;

	};
}


#endif // LIB_VERTEX2D_H
