/**
 * @file	EventListener.cpp
 * @brief	イベント受信クラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "EventListener.h"


namespace Lib
{
	//----------------------------------------------------------------------
	// Static Private Variables
	//----------------------------------------------------------------------
	unsigned int EventListener::m_EventListenerCount = 0;


	//----------------------------------------------------------------------
	// Constructor	Destructor
	//----------------------------------------------------------------------
	EventListener::EventListener()
	{
		m_EventListenerCount++;	// オーバーフローは考慮しない.
		m_EventListenerID = m_EventListenerCount;
	}

	EventListener::~EventListener()
	{
	}


	//----------------------------------------------------------------------
	// Public Functions
	//----------------------------------------------------------------------
	void EventListener::RegistryFunc(std::function<void(EventBase*)>* _pFunc)
	{
		m_Func = *_pFunc;
	}

	void EventListener::EventMessage(EventBase* _pEvent)
	{
		m_Func(_pEvent);
	}
}
