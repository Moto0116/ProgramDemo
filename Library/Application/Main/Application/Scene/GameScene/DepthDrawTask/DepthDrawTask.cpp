#include "DepthDrawTask.h"


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

