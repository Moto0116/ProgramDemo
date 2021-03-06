﻿/**
 * @file	Ground.
 * @brief	地面の管理クラス定義
 * @author	morimoto
 */
#ifndef GROUND_H
#define GROUND_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "Main\Object3DBase\Object3DBase.h"


/**
 * 地面の管理クラス
 */
class Ground : public Object3DBase
{
public:
	/**
	 * コンストラクタ
	 */
	Ground();

	/**
	 * デストラクタ
	 */
	virtual ~Ground();

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
	 * オブジェクトの描画
	 */
	virtual void Draw();

	/**
	 * マップへの描画
	 */
	virtual void MapDraw();

private:
	static D3DXVECTOR3 m_DefaultScale;	//!< デフォルトスケーリング値.


	//----------------------------------------------------------------------
	// 生成処理
	//----------------------------------------------------------------------

	/**
	 * タスクの生成
	 * @return 成功したらtrue 失敗したらfalse
	 */
	bool CreateTask();

	/**
	 * モデルの生成
	 * @return 成功したらtrue 失敗したらfalse
	 */
	bool CreateModel();

	/**
	 * 通常描画時のシェーダー生成
	 * @return 成功したらtrue 失敗したらfalse
	 */
	bool CreateDefaultShader();

	/**
	 * シャドウマップシェーダの生成
	 * @return 成功したらtrue 失敗したらfalse
	 */
	bool CreateShadowShader();

	/**
	 * マップシェーダの生成
	 * @return 成功したらtrue 失敗したらfalse
	 */
	bool CreateMapShader();


	//----------------------------------------------------------------------
	// 解放処理
	//----------------------------------------------------------------------

	/**
	 * タスクの解放
	 */
	void ReleaseTask();

	/**
	 * モデルの解放
	 */
	void ReleaseModel();

	/**
	 * 通常描画時シェーダの解放
	 */
	void ReleaseDefaultShader();

	/**
	 * シャドウマップシェーダの破棄
	 */
	void ReleaseShadowShader();

	/**
	 * マップシェーダの破棄
	 */
	void ReleaseMapShader();



	//--------------------描画関連--------------------
	int m_GroundModelIndex;			//!< グラウンドのモデルインデックス.
	int	m_ShadowVertexShaderIndex;	//!< 深度値描画の頂点シェーダーインデックス.
	int	m_ShadowPixelShaderIndex;	//!< 深度値描画のピクセルシェーダーインデックス.
	int	m_MapVertexShaderIndex;		//!< マップ描画の頂点シェーダーインデックス.
	int	m_MapPixelShaderIndex;		//!< マップ描画のピクセルシェーダーインデックス.

};


#endif // !GROUND_H
