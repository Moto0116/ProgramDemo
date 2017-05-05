/**
 * @file	Water.h
 * @brief	水の管理クラス定義
 * @author	morimoto
 */
#ifndef WATER_H
#define WATER_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include <D3DX11.h>
#include <D3DX10.h>

#include "ObjectManagerBase\ObjectBase\ObjectBase.h"
#include "TaskManager\TaskBase\DrawTask\DrawTask.h"
#include "TaskManager\TaskBase\UpdateTask\UpdateTask.h"
#include "DirectX11\Camera\Camera.h"


/**
 * 水の管理クラス
 */
class Water : public Lib::ObjectBase
{
public:
	/**
	 * コンストラクタ
	 */
	Water();

	/**
	 * デストラクタ
	 */
	virtual ~Water();

	/**
	 * 初期化処理
	 * @return 初期化に成功したか
	 */
	virtual bool Initialize();

	/**
	 * 終了処理
	 */
	virtual void Finalize();

	/**
	 * オブジェクトの更新
	 */
	virtual void Update();

	/**
	 * オブジェクトの描画
	 */
	virtual void Draw();

	/**
	 * 波マップの描画
	 */
	void WaveDraw();


private:
	/**
	 * キューブマップ描画前処理のタスク
	 */
	class BeginTask : public Lib::TaskBase<>
	{
	public:
		/**
		 * コンストラクタ
		 * @param[in] _pWater 水オブジェクト
		 */
		BeginTask(Water* _pWater);

		/**
		 * デストラクタ
		 */
		virtual ~BeginTask();

		/**
		 * タスクの実行
		 */
		virtual void Run();


	private:
		Water* m_pWater;	//!< 描画前処理タスク

	};

	enum
	{
		VERTEX_NUM = 4,	//!< 頂点数
	};


	/**
	 * 水オブジェクト描画の定数バッファ
	 */
	struct CONSTANT_BUFFER
	{
		D3DXMATRIX World; //!< ワールド変換行列.
	};

	/**
	 * 水のキューブマップ作成に関する定数バッファ
	 */
	struct WATER_CONSTANT_BUFFER
	{
		D3DXMATRIX View[6];		//!< ビュー変換行列.
		D3DXMATRIX Proj;		//!< プロジェクション変換行列.
	};

	/**
	 * 水描画用の頂点構造体
	 */
	struct VERTEX
	{
		D3DXVECTOR3	Pos;	//!< 頂点座標.
		D3DXVECTOR2 UV;		//!< テクスチャ座標.
	};


	static const D3DXVECTOR3 m_DefaultPos;	//!< 描画する波の位置.
	static const D3DXVECTOR2 m_DefaultSize;	//!< 描画する波のサイズ.
	static const float m_ClearColor[4];		//!< 初期化色.
	static const float m_TextureWidth;		//!< 波テクスチャの幅.
	static const float m_TextureHeight;		//!< 波テクスチャの高さ.
	static const int m_RenderTargetStage;	//!< レンダーターゲットステージ.


	/**
	 * 頂点バッファの生成
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	bool CreateVertexBuffer();

	/**
	 * シェーダーの初期化
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	bool CreateShader();

	/**
	 * 頂点入力レイアウトの初期化
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	bool CreateVertexLayout();

	/**
	 * 描画ステートの初期化
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	bool CreateState();

	/**
	 * 定数バッファ初期化
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	bool CreateConstantBuffer();
	
	/**
	 * テクスチャの生成
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	bool CreateTexture();

	/**
	 * 頂点バッファの解放
	 */
	void ReleaseVertexBuffer();

	/**
	 * シェーダーの解放
	 */
	void ReleaseShader();

	/**
	 * 頂点入力レイアウトの解放
	 */
	void ReleaseVertexLayout();

	/**
	 * ステートの解放
	 */
	void ReleaseState();

	/**
	 * 定数バッファの解放
	 */
	void ReleaseConstantBuffer();

	/**
	 * テクスチャの解放
	 */
	void ReleaseTexture();

	/**
	 * 定数バッファへの書き込み
	 * @return 成功したらtrue 失敗したらfalse
	 */
	bool WriteConstantBuffer();

	/**
	 * キューブマップ作成に関する定数バッファへの書き込み
	 * @return 成功したらtrue 失敗したらfalse
	 */
	bool WriteCubeMapConstantBuffer();

	/**
	 * キューブマップ描画前処理
	 */
	void CubeMapBeginScene();


	Lib::DrawTask*				m_pDrawTask;				//!< 描画タスクオブジェクト.
	Lib::UpdateTask*			m_pUpdateTask;				//!< 更新タスクオブジェクト.
	BeginTask*					m_pBeginTask;				//!< キューブマップ描画タスクオブジェクト.
	int							m_VertexShaderIndex;		//!< 頂点シェーダーインデックス.
	int							m_PixelShaderIndex;			//!< ピクセルシェーダーインデックス.
	ID3D11Buffer*				m_pVertexBuffer;			//!< 頂点バッファ.
	ID3D11InputLayout*			m_pVertexLayout;			//!< 頂点入力レイアウト.
	ID3D11DepthStencilState*	m_pDepthStencilState;		//!< 深度ステンシルステート.
	ID3D11BlendState*			m_pBlendState;				//!< ブレンドステート.
	VERTEX						m_pVertexData[VERTEX_NUM];	//!< 頂点データ.

	ID3D11Buffer*				m_pConstantBuffer;			//!< 水描画の定数バッファ.
	ID3D11Texture2D*			m_pCubeTexture;				//!< 波情報が入ったテクスチャ.
	ID3D11ShaderResourceView*	m_pCubeTextureResource;		//!< 波テクスチャのリソースビュー.
	ID3D11RenderTargetView*		m_pCubeTextureRenderTarget;	//!< 波テクスチャレンダーターゲットビュー.
	ID3D11Buffer*				m_pCubeMapConstantBuffer;	//!< キューブマップ作成の定数バッファ.
	int							m_PuddleTextureIndex;		//!< 水たまりテクスチャインデックス.
	int							m_SkyCLUTIndex;				//!< 空のカラーテーブル.

	ID3D11Texture2D*			m_pDepthStencilTexture;		//!< 深度ステンシルテクスチャ.
	ID3D11DepthStencilView*		m_pDepthStencilView;		//!< 深度ステンシルビュー.
	D3D11_VIEWPORT				m_ViewPort;					//!< ビューポート.

	Lib::Camera*				m_pCamera;					//!< カメラオブジェクト.
	D3DXMATRIX					m_ViewMat[6];				//!< ビュー変換行列.
	D3DXMATRIX					m_ProjMat;					//!< プロジェクション変換行列.
	WATER_CONSTANT_BUFFER		m_pWaterConstantBuffer;		//!< 定数バッファデータ.
	

};



#endif // WATER_H
