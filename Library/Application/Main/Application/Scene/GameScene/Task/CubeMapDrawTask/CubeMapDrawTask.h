/**
 * @file	CubeMapDrawTask.h
 * @brief	キューブマップテクスチャへの描画タスククラス定義
 * @author	morimoto
 */
#ifndef CUBEMAPDRAWTASK_H
#define CUBEMAPDRAWTASK_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "TaskManager\TaskBase\TaskBase.h"
#include "TaskManager\TaskManager.h"


class Object3DBase;


/**
 * 波テクスチャへの描画タスク
 */
class CubeMapDrawTask : public Lib::TaskBase<>
{
public:
	/**
	 * コンストラクタ
	 */
	CubeMapDrawTask();

	/**
	 * デストラクタ
	 */
	virtual ~CubeMapDrawTask();

	/**
	 * タスクの実行
	 */
	virtual void Run();

	/**
	 * 描画するオブジェクトをセット
	 * @param[in] _pObject オブジェクト
	 */
	void SetDrawObject(Object3DBase* _pObject);

private:
	Object3DBase* m_pObject;	//!< 描画を行うオブジェクト.

};


typedef Lib::TaskManager<CubeMapDrawTask> CubeMapDrawTaskManager;


#endif // !CUBEMAPDRAWTASK_H
