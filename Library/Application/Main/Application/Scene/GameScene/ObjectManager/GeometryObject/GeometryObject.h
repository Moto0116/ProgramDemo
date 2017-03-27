﻿#ifndef GEOMETRYOBJECT_H
#define GEOMETRYOBJECT_H

#include "Main\Object3DBase\Object3DBase.h"
#include "Main\Application\Scene\GameScene\DepthDrawTask\DepthDrawTask.h"
#include "TaskManager\TaskBase\DrawTask\DrawTask.h"
#include "TaskManager\TaskBase\UpdateTask\UpdateTask.h"


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
	Lib::DrawTask*		m_pDrawTask;
	Lib::UpdateTask*	m_pUpdateTask;
	DepthDrawTask*		m_pDepthDrawTask;

	int					m_ModelIndex;
	int					m_ShadowVertexShaderIndex;
	int					m_ShadowPixelShaderIndex;

};


#endif // GEOMETRYOBJECT_H
