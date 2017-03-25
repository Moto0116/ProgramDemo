#ifndef DEPTHDRAWTASK_H
#define DEPTHDRAWTASK_H

#include "TaskManager\TaskBase\TaskBase.h"
#include "TaskManager\TaskManager.h"

#include "Main\Object3DBase\Object3DBase.h"


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
	void SetDrawObject(Object3DBase* _pObject3D);

private:
	Object3DBase* m_pObject3D;	//!< 更新を行うオブジェクト

};

typedef Lib::TaskManager<DepthDrawTask> DepthDrawTaskManager;


#endif
