/**
 * @file	EventListenerBase.cpp
 * @brief	イベント受信基底クラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "EventListenerBase.h"


namespace Lib
{
	//----------------------------------------------------------------------
	// Static Private Variables
	//----------------------------------------------------------------------
	unsigned int EventListenerBase::m_EventListenerBaseCount = 0;


	//----------------------------------------------------------------------
	// Constructor	Destructor
	//----------------------------------------------------------------------
	EventListenerBase::EventListenerBase()
	{
		m_EventListenerBaseCount++;	// オーバーフローは考慮しない
		m_EventListenerID = m_EventListenerBaseCount;
	}

	EventListenerBase::~EventListenerBase()
	{
	}


	//----------------------------------------------------------------------
	// Public Functions
	//----------------------------------------------------------------------
	void EventListenerBase::EventMessage(EventBase* _pEvent)
	{
	}
}
