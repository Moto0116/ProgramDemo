/**
 * @file	House.h
 * @brief	ハウスオブジェクト定義
 * @author	morimoto
 */
#ifndef HOUSE_H
#define HOUSE_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "Main\Object3DBase\Object3DBase.h"
#include "Main\Application\Scene\GameScene\Task\DepthDrawTask\DepthDrawTask.h"
#include "TaskManager\TaskBase\DrawTask\DrawTask.h"
#include "TaskManager\TaskBase\UpdateTask\UpdateTask.h"


/**
 * ハウスクラス
 */
class House : public Object3DBase
{
public:
	/**
	 * コンストラクタ
	 * @param[in] _pos 描画座標
	 * @param[in] _rotate Y軸回転
	 */
	House(D3DXVECTOR3 _pos, float _rotate);

	/**
	 * デストラクタ
	 */
	virtual ~House();

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

	/**
	 * キューブマップへの描画
	 */
	virtual void CubeMapDraw();


private:
	static D3DXVECTOR3 m_DefaultScale;			//!< デフォルトスケーリング値
	static int	m_ModelIndex;					//!< モデルのインデックス
	static int	m_ShadowVertexShaderIndex;		//!< 深度値描画の頂点シェーダーインデックス
	static int	m_ShadowPixelShaderIndex;		//!< 深度値描画のピクセルシェーダーインデックス
	static int	m_MapVertexShaderIndex;			//!< マップ描画の頂点シェーダーインデックス
	static int	m_MapPixelShaderIndex;			//!< マップ描画のピクセルシェーダーインデックス
	static int	m_CubeMapVertexShaderIndex;		//!< キューブマップ描画の頂点シェーダーインデックス
	static int	m_CubeMapGeometryShaderIndex;	//!< キューブマップ描画のジオメトリシェーダーインデックス
	static int	m_CubeMapPixelShaderIndex;		//!< キューブマップ描画のピクセルシェーダーインデックス

	/**
	 * シャドウマップシェーダの生成
	 * @return 成功したらtrue 失敗しましたfalse
	 */
	bool CreateShadowShader();

	/**
	 * マップシェーダの生成
	 * @return 成功したらtrue 失敗しましたfalse
	 */
	bool CreateMapShader();

	/**
	 * キューブマップシェーダの生成
	 * @return 成功したらtrue 失敗しましたfalse
	 */
	bool CreateCubeMapShader();

	/**
	 * シャドウマップシェーダの破棄
	 */
	void ReleaseShadowShader();

	/**
	 * マップシェーダの破棄
	 */
	void ReleaseMapShader();

	/**
	 * キューブマップシェーダの破棄
	 */
	void ReleaseCubeMapShader();

};


#endif // HOUSE_H
