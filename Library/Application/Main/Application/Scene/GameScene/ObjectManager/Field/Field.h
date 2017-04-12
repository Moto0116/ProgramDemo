/**
 * @file	Field.h
 * @brief	フィールドクラス定義
 * @author	morimoto
 */
#ifndef FIELD_H
#define FIELD_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "Main\Object3DBase\Object3DBase.h"


/**
 * フィールドクラス
 */
class Field : public Object3DBase
{
public:
	/**
	 * コンストラクタ
	 */
	Field();

	/**
	 * デストラクタ
	 */
	virtual ~Field();

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
	 * Z値をテクスチャに描画
	 */
	virtual void DepthDraw();

	/**
	 * マップへの描画
	 */
	virtual void MapDraw();

private:
	static D3DXVECTOR3 m_DefaultScale;	//!< デフォルトスケーリング値

	int m_GroundModelIndex;			//!< グラウンドのモデルインデックス
	int m_MountainModelIndex;		//!< 山のモデルインデックス
	int m_SkyModelIndex;			//!< 空のモデルインデックス
	int	m_ShadowVertexShaderIndex;	//!< 深度値描画の頂点シェーダーインデックス
	int	m_ShadowPixelShaderIndex;	//!< 深度値描画のピクセルシェーダーインデックス
	int	m_MapVertexShaderIndex;		//!< マップ描画の頂点シェーダーインデックス
	int	m_MapPixelShaderIndex;		//!< マップ描画のピクセルシェーダーインデックス


};


#endif // FIELD_H
