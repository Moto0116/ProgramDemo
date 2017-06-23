﻿/**
 * @file	MadDrawTask.cpp
 * @brief	マップへの描画タスククラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "MapDrawTask.h"

#include "Main\Object3DBase\Object3DBase.h"


//----------------------------------------------------------------------
// Constructor	Destructor
//----------------------------------------------------------------------
MapDrawTask::MapDrawTask() :
	m_pObject3D(NULL)
{
}

MapDrawTask::~MapDrawTask()
{
}

//----------------------------------------------------------------------
// Public Functions
//----------------------------------------------------------------------
void MapDrawTask::Run()
{
	m_pObject3D->MapDraw();
}

void MapDrawTask::SetDrawObject(Object3DBase* _pObject3D)
{
	m_pObject3D = _pObject3D;
}
