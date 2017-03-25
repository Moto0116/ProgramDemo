/**
 * @file	TaskManager_private.h
 * @brief	タスク管理の基底クラステンプレート実装
 * @author	morimoto
 */
#ifndef LIB_TASKMANAGER_PRIVATE_H
#define LIB_TASKMANAGER_PRIVATE_H


namespace Lib
{
	//----------------------------------------------------------------------
	// Constructor	Destructor
	//----------------------------------------------------------------------
	template <typename Type>
	inline TaskManager<Type>::TaskManager()
	{
	}

	template <typename Type>
	inline TaskManager<Type>::~TaskManager()
	{
	}


	//----------------------------------------------------------------------
	// Public Functions
	//----------------------------------------------------------------------
	template <typename Type>
	inline void TaskManager<Type>::Run()
	{
		for (auto itr = m_pTaskList.begin(); itr != m_pTaskList.end(); itr++)
		{
			(*itr)->Run();
		}
	}

	template <typename Type>
	inline void TaskManager<Type>::AddTask(Type* _pTask)
	{
		m_pTaskList.push_back(_pTask);
		m_pTaskList.sort(Type::TaskCmp());
	}

	template <typename Type>
	inline void TaskManager<Type>::RemoveTask(Type* _pTask)
	{
		for (auto itr = m_pTaskList.begin(); itr != m_pTaskList.end(); itr++)
		{
			if (_pTask->GetID() == (*itr)->GetID())
			{
				m_pTaskList.erase(itr);
				break;
			}
		}
	}
}



#endif // LIB_TASKMANAGER_PRIVATE_H
