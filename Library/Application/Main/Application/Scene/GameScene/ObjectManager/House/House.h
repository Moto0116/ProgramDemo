#ifndef HOUSE_H
#define HOUSE_H

#include "Main\Object3DBase\Object3DBase.h"
#include "Main\Application\Scene\GameScene\DepthDrawTask\DepthDrawTask.h"
#include "TaskManager\TaskBase\DrawTask\DrawTask.h"
#include "TaskManager\TaskBase\UpdateTask\UpdateTask.h"


class House : public Object3DBase
{
public:
	/**
	 * コンストラクタ
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

private:
	static D3DXVECTOR3 m_DefaultScale;
	static int	m_ModelIndex;
	static int	m_ShadowVertexShaderIndex;
	static int	m_ShadowPixelShaderIndex;
	static int	m_MapVertexShaderIndex;
	static int	m_MapPixelShaderIndex;

};


#endif // HOUSE_H
