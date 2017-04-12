/**
 * @file	MainLight.h
 * @brief	メインライトオブジェクト定義
 * @author	morimoto
 */
#ifndef MAINLIGHT_H
#define MAINLIGHT_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "DirectX11\Light\Light.h"
#include "ObjectManagerBase\ObjectBase\ObjectBase.h"
#include "TaskManager\TaskBase\DrawTask\DrawTask.h"
#include "TaskManager\TaskBase\UpdateTask\UpdateTask.h"


/**
 * メインライトクラス
 */
class MainLight : public Lib::ObjectBase
{
public:
	/**
	 * コンストラクタ
	 */
	MainLight();

	/**
	 * デストラクタ
	 */
	virtual ~MainLight();

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


private:
	/**
	 * バックバッファ描画前処理のタスク
	 */
	class BeginTask : public Lib::TaskBase<>
	{
	public:
		/**
		 * コンストラクタ
		 * @param[in] _pMainLight ライトオブジェクト
		 */
		BeginTask(MainLight* _pMainLight);

		/**
		 * デストラクタ
		 */
		virtual ~BeginTask();

		/**
		 * タスクの実行
		 */
		virtual void Run();


	private:
		MainLight* m_pMainLight;	//!< 描画前処理タスク

	};


	/**
	 * 深度値描画定数バッファ
	 */
	struct LIGHT_CONSTANT_BUFFER
	{
		D3DXVECTOR4 LightPos;
		D3DXVECTOR4 LightDir;
		D3DXMATRIX	LightView;
		D3DXMATRIX	LightProj;
		D3DXVECTOR4 FrameTime;
	};


	static const D3DXVECTOR3 m_DefaultLightPos;		//!< ライト座標
	static const D3DXVECTOR3 m_DefaultLightDirPos;	//!< ライト注視座標
	static const float m_NearPoint;					//!< 最近点
	static const float m_FarPoint;					//!< 最遠点
	static const float m_ViewAngle;					//!< 画角
	static const float m_ClearColor[4];				//!< 初期化色
	static const float m_DepthTextureWidth;			//!< 深度テクスチャ幅
	static const float m_DepthTextureHeight;		//!< 深度テクスチャ高さ
	static const int m_RenderTargetStage;			//!< レンダーターゲットステージ


	/**
	 * 定数バッファ初期化
	 * @return 初期化に成功したらtrue
	 */
	bool CreateConstantBuffer();

	/**
	 * 深度テクスチャ初期化
	 * @return 初期化に成功したらtrue
	 */
	bool CreateDepthTexture();

	/**
	 * 定数バッファの解放
	 */
	void ReleaseConstantBuffer();

	/**
	 * 深度テクスチャ解放
	 */
	void ReleaseDepthTexture();

	/**
	 * 定数バッファへの書き込み
	 */
	void WriteConstantBuffer();

	/**
	 * バックバッファ描画前処理
	 */
	void MainLightBeginScene();


	BeginTask*					m_pBeginTask;			//!< 描画前処理タスクオブジェクト
	Lib::DrawTask*				m_pDrawTask;			//!< 描画タスクオブジェクト
	Lib::UpdateTask*			m_pUpdateTask;			//!< 更新タスクオブジェクト
	Lib::Light*					m_pLight;				//!< ライトオブジェクト
	ID3D11Buffer*				m_pConstantBuffer;		//!< 定数バッファ
	D3DXVECTOR3					m_LightDir;				//!< ライトの向き
	D3DXMATRIX					m_LightView;			//!< ライトのビュー行列
	D3DXMATRIX					m_LightProj;			//!< ライトのプロジェクション行列
	ID3D11Texture2D*			m_pDepthTexture;		//!< 深度テクスチャ
	ID3D11RenderTargetView*		m_pRenderTarget;		//!< 深度テクスチャのレンダーターゲットビュー
	ID3D11ShaderResourceView*	m_pShaderResourceView;	//!< 深度テクスチャのシェーダーリソースビュー
	ID3D11Texture2D*			m_pDepthStencilTexture;	//!< 深度ステンシルテクスチャ
	ID3D11DepthStencilView*		m_pDepthStencilView;	//!< 深度ステンシルビュー
	D3D11_VIEWPORT				m_ViewPort;				//!< ビューポート

};


#endif // MAINLIGHT_H
