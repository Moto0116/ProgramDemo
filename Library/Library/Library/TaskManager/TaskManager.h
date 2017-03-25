/**
 * @file	TaskManager.h
 * @brief	タスク管理の基底クラス定義
 * @author	morimoto
 */
#ifndef LIB_TASKMANAGER_H
#define LIB_TASKMANAGER_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include <list>

#include "..\SingletonBase\SingletonBase.h"


namespace Lib
{
	/**
	 * タスク管理の基底クラス
	 * @tparam Type 管理するタスクオブジェクト
	 */
	template <typename Type>
	class TaskManager : public SingletonBase<TaskManager<Type>>
	{
	public:
		friend SingletonBase<TaskManager<Type>>;
	
		/**
		 * タスクの実行
		 */
		inline void Run();
	
		/**
		 * タスクの追加
		 * @param[in] _pTask 追加するタスク
		 */
		inline void AddTask(Type* _pTask);

		/**
		 * タスクの削除
		 * @param[in] _pTask 削除するタスク
		 */
		inline void RemoveTask(Type* _pTask);
	
	
	private:
		/**
		 * コンストラクタ
		 */
		inline TaskManager();
	
		/**
		 * デストラクタ
		 */
		inline ~TaskManager();


		std::list<Type*> m_pTaskList;
	
	};
}


#include "TaskManager_private.h"


#endif // LIB_TASKMANAGER_H
