/**
 * @file	CubeMapDrawTask.cpp
 * @brief	波テクスチャへの描画タスク
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
CubeMapDrawTask::CubeMapDrawTask()
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

void CubeMapDrawTask::SetDrawObject(Object3DBase* _pObject)
{
	m_pObject = _pObject;
}

