/**
 * @file	DepthDrawTask.h
 * @brief	深度バッファ書き込みタスククラス定義
 * @author	morimoto
 */
#ifndef DEPTHDRAWTASK_H
#define DEPTHDRAWTASK_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "TaskManager\TaskBase\TaskBase.h"
#include "TaskManager\TaskManager.h"


class Object3DBase;


/**
 * 深度バッファへの書き込みタスク
 */
class DepthDrawTask : public Lib::TaskBase<>
{
public:
	/**
	 * コンストラクタ
	 */
	DepthDrawTask();

	/**
	 * デストラクタ
	 */
	virtual ~DepthDrawTask();

	/**
	 * タスクの実行
	 */
	virtual void Run();

	/**
	 * 描画する3Dオブジェクトをセット
	 * @param[in] _pObject 3Dオブジェクト
	 */
	void SetObject(Object3DBase* _pObject3D);

private:
	Object3DBase* m_pObject3D;	//!< 描画を行うオブジェクト.


};


typedef Lib::TaskManager<DepthDrawTask> DepthDrawTaskManager;


#endif // !DEPTHDRAWTASK_H
