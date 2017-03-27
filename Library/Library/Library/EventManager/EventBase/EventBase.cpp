/**
 * @file	EventBase.cpp
 * @brief	イベント基底クラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "EventBase.h"


namespace Lib
{
	//----------------------------------------------------------------------
	// Static Private Variables
	//----------------------------------------------------------------------
	unsigned int EventBase::m_EventBaseCount = 0;


	//----------------------------------------------------------------------
	// Constructor	Destructor
	//----------------------------------------------------------------------
	EventBase::EventBase()
	{
		m_EventBaseCount++;	// オーバーフローは考慮しない.
		m_EventID = m_EventBaseCount;
	}

	EventBase::~EventBase()
	{
	}
}

