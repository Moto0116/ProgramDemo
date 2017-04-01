#ifndef GROUND_H
#define GROUND_H

#include "Main\Object3DBase\Object3DBase.h"


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
	static D3DXVECTOR3 m_DefaultScale;

	int m_GroundModelIndex;
	int m_MountainModelIndex;
	int m_SkyModelIndex;
	int	m_ShadowVertexShaderIndex;
	int	m_ShadowPixelShaderIndex;
	int	m_MapVertexShaderIndex;
	int	m_MapPixelShaderIndex;


};


#endif
