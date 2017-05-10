/**
 * @file	ReflectMapDrawTask.cpp
 * @brief	反射マップテクスチャへの描画タスククラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "ReflectMapDrawTask.h"

#include "Main\Object3DBase\Object3DBase.h"


//----------------------------------------------------------------------
// Constructor	Destructor
//----------------------------------------------------------------------
ReflectMapDrawTask::ReflectMapDrawTask() :
	m_pObject(NULL)
{
}

ReflectMapDrawTask::~ReflectMapDrawTask()
{
}


//----------------------------------------------------------------------
// Public Functions
//----------------------------------------------------------------------
void ReflectMapDrawTask::Run()
{
	m_pObject->ReflectMapDraw();
}

void ReflectMapDrawTask::SetDrawObject(Object3DBase* _pObject)
{
	m_pObject = _pObject;
}

