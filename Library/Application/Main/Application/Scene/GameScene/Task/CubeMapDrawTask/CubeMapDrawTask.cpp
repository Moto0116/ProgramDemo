/**
 * @file	CubeMapDrawTask.cpp
 * @brief	波テクスチャへの描画タスククラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "CubeMapDrawTask.h"

#include "Main\Object3DBase\Object3DBase.h"


//----------------------------------------------------------------------
// Constructor	Destructor
//----------------------------------------------------------------------
CubeMapDrawTask::CubeMapDrawTask() : 
	m_pObject(nullptr)
{
}

CubeMapDrawTask::~CubeMapDrawTask()
{
}


//----------------------------------------------------------------------
// Public Functions
//----------------------------------------------------------------------
void CubeMapDrawTask::Run()
{
	m_pObject->CubeMapDraw();
}

void CubeMapDrawTask::SetObject(Object3DBase* _pObject)
{
	m_pObject = _pObject;
}

