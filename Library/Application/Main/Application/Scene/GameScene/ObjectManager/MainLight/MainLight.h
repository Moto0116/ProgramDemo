/**
 * @file	MainLight.h
 * @brief	メインライトクラス定義
 * @author	morimoto
 */
#ifndef MAINLIGHT_H
#define MAINLIGHT_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "DirectX11\Light\Light.h"
#include "TaskManager\TaskBase\TaskBase.h"
#include "ObjectManagerBase\ObjectBase\ObjectBase.h"


namespace Lib
{
	class UpdateTask;
	class DrawTask;
	class Vertex2D;
}

class MainCamera;


/**
 * メインライト
 * @todo 大きくなってるのでライトの描画と深度バッファ管理を分離する予定
 */
class MainLight : public Lib::ObjectBase
{
public:
	/**
	 * コンストラクタ
	 * @param[in] _pCamera カメラオブジェクト
	 */
	MainLight(MainCamera* _pCamera);

	/**
	 * デストラクタ
	 */
	virtual ~MainLight();

	/**
	 * 初期化処理
	 * @return 初期化に成功したらtrue 失敗したらfalse
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


private:
	enum
	{
		VERTEX_NUM = 4	//!< 頂点数.
	};

	/**
	 * 通常描画前処理のタスク
	 */
	class DrawBeginTask : public Lib::TaskBase<>
	{
	public:
		/**
		 * コンストラクタ
		 * @param[in] _pMainLight ライトオブジェクト
		 */
		DrawBeginTask(MainLight* _pMainLight);

		/**
		 * デストラクタ 
		 */
		virtual ~DrawBeginTask();

		/**
		 * タスクの実行
		 */
		virtual void Run();

	private:
		MainLight* m_pMainLight;	//!< 処理オブジェクト.

	};


	/**
	 * 深度値描画前処理のタスク
	 */
	class DepthDrawBeginTask : public Lib::TaskBase<>
	{
	public:
		/**
		 * コンストラクタ
		 * @param[in] _pMainLight ライトオブジェクト
		 */
		DepthDrawBeginTask(MainLight* _pMainLight);

		/**
		 * デストラクタ
		 */
		virtual ~DepthDrawBeginTask();

		/**
		 * タスクの実行
		 */
		virtual void Run();

	private:
		MainLight* m_pMainLight;	//!< 処理オブジェクト.

	};


	/**
	 * 深度値描画定数バッファ
	 */
	struct LIGHT_CONSTANT_BUFFER
	{
		D3DXVECTOR4 LightPos;	//!< ライト座標.
		D3DXVECTOR4 LightDir;	//!< ライトの向き.
		D3DXMATRIX	LightView;	//!< ライトのビュー行列.
		D3DXMATRIX	LightProj;	//!< ライトのプロジェクション行列.
		D3DXMATRIX	Matrix;		//!< ライトのワールド変換行列.
		D3DXVECTOR4	LightDot;	//!< ライトの逆ベクトルと上方ベクトルの内積.
	};

	/**
	 * ライト描画用の頂点構造体
	 */
	struct VERTEX
	{
		D3DXVECTOR3	Pos;	//!< 頂点座標.
		D3DXVECTOR2 UV;		//!< テクスチャ座標.
	};

	/**
	 * ライトの状態構造体
	 */
	struct LIGHT_STATE
	{
		LIGHT_STATE(D3DXVECTOR3 _pPos, float _time) :
			Pos(_pPos),
			Time(_time)
		{
		}

		D3DXVECTOR3	Pos;	//!< ライトの座標情報.
		float		Time;	//!< 太陽の位置を決める時間.
	};


	static const D3DXVECTOR3 m_DefaultLightPos;		//!< ライト座標.
	static const D3DXVECTOR3 m_DefaultLightDirPos;	//!< ライト注視座標.
	static const D3DXVECTOR2 m_DefaultSize;			//!< 描画するライトのサイズ.
	static const float m_NearPoint;					//!< 最近点.
	static const float m_FarPoint;					//!< 最遠点.
	static const float m_ViewAngle;					//!< 画角.
	static const float m_ClearColor[4];				//!< 初期化色.
	static const float m_DepthTextureWidth;			//!< 深度テクスチャ幅.
	static const float m_DepthTextureHeight;		//!< 深度テクスチャ高さ.
	static const int m_RenderTargetStage;			//!< レンダーターゲットステージ.


	//----------------------------------------------------------------------
	// 生成処理
	//----------------------------------------------------------------------

	/**
	 * タスクオブジェクト初期化
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	bool CreateTask();

	/**
	 * ライトオブジェクト初期化
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	bool CreateLight();

	/**
	 * 定数バッファ初期化
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	bool CreateConstantBuffer();

	/**
	 * 影描画用のZ値テクスチャ初期化
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	bool CreateDepthTexture();

	/**
	 * 描画オブジェクト初期化
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	bool CreateVertex();

	/**
	 * シェーダー初期化
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	bool CreateShader();

	/**
	 * 頂点入力レイアウトの初期化
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	bool CreateVertexLayout();

	/**
	 * ステートの初期化
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	bool CreateState();

	/**
	 * ライトのテクスチャ初期化
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	bool CreateLightTexture();


	//----------------------------------------------------------------------
	// 解放処理
	//----------------------------------------------------------------------

	/**
	 * タスクオブジェクトの解放
	 */
	void ReleaseTask();

	/**
	 * ライトオブジェクトの解放
	 */
	void ReleaseLight();

	/**
	 * 定数バッファの解放
	 */
	void ReleaseConstantBuffer();

	/**
	 * 深度テクスチャ解放
	 */
	void ReleaseDepthTexture();

	/**
	 * 描画オブジェクト解放
	 */
	void ReleaseVertex();

	/**
	 * シェーダー解放
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
	 * ライトテクスチャ解放
	 */
	void ReleaseLightTexture();


	//----------------------------------------------------------------------
	// その他処理
	//----------------------------------------------------------------------

	/** 
	 * 通常描画前処理
	 */
	void DrawBeginScene();

	/**
	 * 深度値描画前処理
	 */
	void MainLightBeginScene();

	/**
	 * 定数バッファへの書き込み
	 * @return 成功したらtrue 失敗したらfalse
	 */
	bool WriteConstantBuffer();



	//--------------------タスクオブジェクト--------------------
	DrawBeginTask*				m_pDrawBeginTask;		//!< 通常描画前処理タスクオブジェクト.
	DepthDrawBeginTask*			m_pDepthDrawBeginTask;	//!< 深度バッファ描画前処理タスクオブジェクト.
	Lib::DrawTask*				m_pDrawTask;			//!< 描画タスクオブジェクト.
	Lib::UpdateTask*			m_pUpdateTask;			//!< 更新タスクオブジェクト.


	//--------------------その他オブジェクト--------------------
	Lib::Light*					m_pLight;				//!< ライトオブジェクト.
	MainCamera*					m_pCamera;				//!< カメラオブジェクト.


	//--------------------ライト定数バッファ--------------------
	ID3D11Buffer*				m_pConstantBuffer;		//!< 定数バッファ.
	D3DXMATRIX					m_LightView;			//!< ライトのビュー行列.
	D3DXMATRIX					m_LightProj;			//!< ライトのプロジェクション行列.


	//--------------------レンダーターゲット関連--------------------
	ID3D11Texture2D*			m_pDepthTexture;		//!< 深度テクスチャ.
	ID3D11RenderTargetView*		m_pRenderTarget;		//!< 深度テクスチャのレンダーターゲットビュー.
	ID3D11ShaderResourceView*	m_pDepthStencilResource;//!< 深度テクスチャのシェーダーリソースビュー.
	ID3D11Texture2D*			m_pDepthStencilTexture;	//!< 深度ステンシルテクスチャ.
	ID3D11DepthStencilView*		m_pDepthStencilView;	//!< 深度ステンシルビュー.
	D3D11_VIEWPORT				m_ViewPort;				//!< ビューポート.


	//--------------------描画関連--------------------
	int							m_VertexShaderIndex;		//!< 頂点シェーダーインデックス.
	int							m_PixelShaderIndex;			//!< ピクセルシェーダーインデックス.
	ID3D11Buffer*				m_pVertexBuffer;			//!< 頂点バッファ.
	ID3D11InputLayout*			m_pVertexLayout;			//!< 頂点入力レイアウト.
	ID3D11DepthStencilState*	m_pDepthStencilState;		//!< 深度ステンシルステート.
	ID3D11BlendState*			m_pBlendState;				//!< ブレンドステート.
	VERTEX						m_pVertexData[VERTEX_NUM];	//!< 頂点データ.
	int							m_LightTextureIndex;		//!< ライトテクスチャインデックス.
	int							m_BloomTextureIndex;		//!< ブルームテクスチャインデックス.
	LIGHT_STATE					m_LightState;				//!< ライトの状態.

};


#endif // !MAINLIGHT_H
