/**
 * @file	EventManager.inl
 * @brief	イベント管理基底クラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "EventManager.h"

#include "EventListener\EventListener.h"


namespace Lib
{
	//----------------------------------------------------------------------
	// Constructor	Destructor
	//----------------------------------------------------------------------
	template <typename Event>
	EventManager<Event>::EventManager()
	{
	}

	template <typename Event>
	EventManager<Event>::~EventManager()
	{
	}


	//----------------------------------------------------------------------
	// Public Functions
	//----------------------------------------------------------------------
	template <typename Event>
	void EventManager<Event>::AddEventListener(EventListener* _pEventListener)
	{
		m_pEventListener.push_back(_pEventListener);
	}

	template <typename Event>
	void EventManager<Event>::RemoveEventListener(EventListener* _pEventListener)
	{
		for (auto itr = m_pEventListener.begin(); itr != m_pEventListener.end(); itr++)
		{
			if ((*itr)->GetEventListenerID() == _pEventListener->GetEventListenerID())
			{
				m_pEventListener.erase(itr);
				break;
			}
		}
	}

	template <typename Event>
	void EventManager<Event>::SendEventMessage(Event* _pEvent)
	{
		for (auto itr = m_pEventListener.begin(); itr != m_pEventListener.end(); itr++)
		{
			(*itr)->EventMessage(_pEvent);
		}
	}
}

