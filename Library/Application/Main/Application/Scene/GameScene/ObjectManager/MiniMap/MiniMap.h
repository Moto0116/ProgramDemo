﻿/**
 * @file	MiniMap.h
 * @brief	ミニマップクラス定義
 * @author	morimoto
 */
#ifndef MINIMAP_H
#define MINIMAP_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "Main\Object2DBase\Object2DBase.h"
#include "TaskManager\TaskBase\TaskBase.h"
#include "DirectX11\Camera\Camera.h"


/**
 * ミニマップクラス
 */
class MiniMap : public Object2DBase
{
public:
	/**
	 * コンストラクタ
	 */
	MiniMap();

	/**
	 * デストラクタ
	 */
	virtual ~MiniMap();

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
	 * ミニマップ描画前処理のタスク
	 */
	class BeginTask : public Lib::TaskBase<>
	{
	public:
		/**
		 * コンストラクタ
		 * @param[in] _pMiniMap ミニマップオブジェクト
		 */
		BeginTask(MiniMap* _pMiniMap);

		/**
		 * デストラクタ
		 */
		virtual ~BeginTask();

		/**
		 * タスクの実行
		 */
		virtual void Run();

	private:
		MiniMap* m_pMiniMap;	//!< 描画前処理を実行するオブジェクト

	};


	/**
	 * ミニマップ描画定数バッファ
	 */
	struct MINIMAP_CONSTANT_BUFFER
	{
		D3DXMATRIX View;	//!< ミニマップ描画ビュー行列
		D3DXMATRIX Proj;	//!< ミニマップ描画プロジェクション行列
	};


	static const float	m_NearPoint;		//!< 最近点
	static const float	m_FarPoint;			//!< 最遠点
	static const float	m_ViewAngle;		//!< 視野角
	static const float m_ClearColor[4];		//!< 初期化色
	static const float m_TextureWidth;		//!< マップテクスチャの幅
	static const float m_TextureHeight;		//!< マップテクスチャの高さ
	static const int m_RenderTargetStage;	//!< 描画ターゲットステージ
	static const D3DXVECTOR3 m_DefaultPos;	//!< 描画位置


	/**
	 * 定数バッファ初期化
	 * @return 成功したらtrue
	 */
	bool CreateConstantBuffer();

	/**
	 * テクスチャの生成
	 * @return 成功したらtrue
	 */
	bool CreateTexture();

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
	 */
	void WriteConstantBuffer();

	/**
	 * ミニマップ描画前処理
	 */
	void MiniMapBeginScene();


	BeginTask*					m_pBeginTask;			//!< 描画前タスクオブジェクト
	Lib::Camera*				m_pCamera;				//!< カメラオブジェクト
	ID3D11Texture2D*			m_pMapTexture;			//!< マップテクスチャ
	ID3D11RenderTargetView*		m_pRenderTarget;		//!< マップテクスチャレンダーターゲットビュー
	ID3D11ShaderResourceView*	m_pShaderResourceView;	//!< マップテクスチャシェーダーリソースビュー
	ID3D11Texture2D*			m_pDepthStencilTexture;	//!< 深度ステンシルテクスチャ
	ID3D11DepthStencilView*		m_pDepthStencilView;	//!< 深度ステンシルビュー
	D3D11_VIEWPORT				m_ViewPort;				//!< ビューポート
	ID3D11Buffer*				m_pConstantBuffer;		//!< マップ描画定数バッファ
	D3DXMATRIX					m_CameraView;			//!< マップ描画ビュー行列
	D3DXMATRIX					m_CameraProj;			//!< マップ描画プロジェクション行列


};


#endif // MINIMAP_H