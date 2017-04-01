/**
 * @file   Object3DBase.cpp
 * @brief  Object3DBaseクラスの実装
 * @author morimoto
 */
#ifndef OBJECT2DBASE_H
#define OBJECT2DBASE_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "ObjectManagerBase\ObjectBase\ObjectBase.h"
#include "DirectX11\Vertex2D\Vertex2D.h"
#include "TaskManager\TaskBase\DrawTask\DrawTask.h"
#include "TaskManager\TaskBase\UpdateTask\UpdateTask.h"


/**
 * 2Dオブジェクトの基底クラス
 */
class Object2DBase : public Lib::ObjectBase
{
public:
	/**
	 * コンストラクタ
	 */
	Object2DBase();

	/**
	 * デストラクタ
	 */
	virtual ~Object2DBase();
	
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

protected:
	bool CreateVertex2D();
	void ReleaseVertex2D();

	Lib::DrawTask*		m_pDrawTask;
	Lib::UpdateTask*	m_pUpdateTask;

	Lib::Vertex2D*	m_pVertex;
	D3DXVECTOR2		m_Pos;
	D3DXVECTOR2		m_Rect;
	int				m_TextureIndex;

};

#endif // OBJECT2DBASE_H
