/**
 * @file	EventManager.cpp
 * @brief	イベント管理基底クラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "EventManager.h"

#include "EventListenerBase\EventListenerBase.h"


namespace Lib
{
	//----------------------------------------------------------------------
	// Constructor	Destructor
	//----------------------------------------------------------------------
	template <typename Listener, typename Event>
	EventManager<Listener, Event>::EventManager()
	{
	}

	template <typename Listener, typename Event>
	EventManager<Listener, Event>::~EventManager()
	{
	}


	//----------------------------------------------------------------------
	// Public Functions
	//----------------------------------------------------------------------
	template <typename Listener, typename Event>
	void EventManager<Listener, Event>::AddEventListener(Listener* _pEventListener)
	{
		m_pEventListener.push_back(_pEventListener);
	}

	template <typename Listener, typename Event>
	void EventManager<Listener, Event>::SendEventMessage(Event* _pEvent)
	{
		for (auto itr = m_pEventListener.begin(); itr != m_pEventListener.end(); itr++)
		{
			(*itr)->EventMessage(_pEvent);
		}
	}
}

