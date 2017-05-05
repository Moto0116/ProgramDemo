/**
 * @file	DepthDrawTask.cpp
 * @brief	深度バッファ書き込みタスククラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "DepthDrawTask.h"

#include "Main\Object3DBase\Object3DBase.h"


//----------------------------------------------------------------------
// Constructor	Destructor
//----------------------------------------------------------------------
DepthDrawTask::DepthDrawTask() : 
	m_pObject3D(NULL)
{
}

DepthDrawTask::~DepthDrawTask()
{
}

//----------------------------------------------------------------------
// Public Functions
//----------------------------------------------------------------------
void DepthDrawTask::Run()
{
	m_pObject3D->DepthDraw();
}

void DepthDrawTask::SetDrawObject(Object3DBase* _pObject3D)
{
	m_pObject3D = _pObject3D;
}

