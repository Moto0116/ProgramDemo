/**
 * @file	DrawTask.cpp
 * @brief	描画のタスククラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "DrawTask.h"

#include <Windows.h>

#include "..\..\..\ObjectBase\ObjectBase.h"


namespace Lib
{
	//----------------------------------------------------------------------
	// Constructor	Destructor
	//----------------------------------------------------------------------
	DrawTask::DrawTask() :
		m_pObject(NULL)
	{
	}

	DrawTask::~DrawTask()
	{
	}


	//----------------------------------------------------------------------
	// Public Functions
	//----------------------------------------------------------------------
	void DrawTask::Run()
	{
		m_pObject->Draw();
	}

	void DrawTask::SetDrawObject(ObjectBase* _pObject)
	{
		m_pObject = _pObject;
	}
}

