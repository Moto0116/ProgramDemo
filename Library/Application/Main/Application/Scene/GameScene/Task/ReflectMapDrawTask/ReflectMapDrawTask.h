/**
 * @file	ReflectMapDrawTask.h
 * @brief	反射マップテクスチャへの描画タスククラス定義
 * @author	morimoto
 */
#ifndef REFLECTMAPDRAWTASK_H
#define REFLECTMAPDRAWTASK_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "TaskManager\TaskBase\TaskBase.h"
#include "TaskManager\TaskManager.h"


class Object3DBase;


/**
 * 反射マップテクスチャへの描画タスク
 */
class ReflectMapDrawTask : public Lib::TaskBase<>
{
public:
	/**
	 * コンストラクタ
	 */
	ReflectMapDrawTask();

	/**
	 * デストラクタ
	 */
	virtual ~ReflectMapDrawTask();

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


typedef Lib::TaskManager<ReflectMapDrawTask> ReflectMapDrawTaskManager;



#endif // !REFLECTMAPDRAWTASK_H
