/**
 * @file	Sky.h
 * @brief	空の管理オブジェクト定義
 * @author	morimoto
 */
#ifndef SKY_H
#define SKY_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "Main\Object3DBase\Object3DBase.h"


/**
 * 空の管理オブジェクト
 */
class Sky : public Object3DBase
{
public:
	/**
	 * コンストラクタ
	 */
	Sky();

	/**
	 * デストラクタ 
	 */
	virtual ~Sky();

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
	static D3DXVECTOR3 m_DefaultScale;	//!< デフォルトスケーリング値

	int m_SkyModelIndex;				//!< 空のモデルインデックス
	int m_SkyTextureIndex;				//!< 空のテクスチャインデックス
	int	m_ShadowVertexShaderIndex;		//!< 深度値描画の頂点シェーダーインデックス
	int	m_ShadowPixelShaderIndex;		//!< 深度値描画のピクセルシェーダーインデックス
	int	m_MapVertexShaderIndex;			//!< マップ描画の頂点シェーダーインデックス
	int	m_MapPixelShaderIndex;			//!< マップ描画のピクセルシェーダーインデックス
	int	m_CubeMapVertexShaderIndex;		//!< キューブマップ描画の頂点シェーダーインデックス
	int	m_CubeMapGeometryShaderIndex;	//!< キューブマップ描画のジオメトリシェーダーインデックス
	int	m_CubeMapPixelShaderIndex;		//!< キューブマップ描画のピクセルシェーダーインデックス

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
	 * キューブマップテクスチャの破棄
	 */
	void ReleaseCubeMapShader();

};



#endif // SKY_H
