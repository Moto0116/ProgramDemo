/**
 * @file	SceneBase.cpp
 * @brief	シーンの基底クラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "SceneBase.h"


namespace Lib
{
	//----------------------------------------------------------------------
	// Constructor	Destructor
	//----------------------------------------------------------------------
	SceneBase::SceneBase() : 
		m_State(INIT_STATE)
	{
	}

	SceneBase::~SceneBase()
	{
	}


	//----------------------------------------------------------------------
	// Public Functions
	//----------------------------------------------------------------------
	bool SceneBase::Initialize()
	{
		return true;
	}

	void SceneBase::Finalize()
	{
	}

	void SceneBase::Update()
	{
	}

}

