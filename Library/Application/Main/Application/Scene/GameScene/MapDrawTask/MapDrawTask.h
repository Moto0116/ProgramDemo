/**
 * @file	マップへの描画タスククラス定義
 * @brief	MadDrawTask.h
 * @author	morimoto
 */
#ifndef MAPDRAWTASK_H
#define MAPDRAWTASK_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "TaskManager\TaskBase\TaskBase.h"
#include "TaskManager\TaskManager.h"


class Object3DBase;


/**
 * マップへの描画タスク
 */
class MapDrawTask : public Lib::TaskBase<>
{
public:
	/**
	 * コンストラクタ
	 */
	MapDrawTask();

	/**
	 * デストラクタ
	 */
	virtual ~MapDrawTask();

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
	Object3DBase* m_pObject3D;	//!< 描画を行うオブジェクト


};


typedef Lib::TaskManager<MapDrawTask> MapDrawTaskManager;


#endif // MAPDRAWTASK_H
