#ifndef GEOMETRYOBJECT_H
#define GEOMETRYOBJECT_H

#include "Main\Object3DBase\Object3DBase.h"


class GeometryObject : public Object3DBase
{
public:
	/**
	 * コンストラクタ
	 */
	GeometryObject();
	
	/**
	 * デストラクタ
	 */
	virtual ~GeometryObject();

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

private:
	int	m_ModelIndex;
	int	m_ShadowVertexShaderIndex;
	int	m_ShadowPixelShaderIndex;
	int	m_GeometryVertexShaderIndex;
	int	m_GeometryPixelShaderIndex;
	int	m_GeometryShaderIndex;

};


#endif // GEOMETRYOBJECT_H
