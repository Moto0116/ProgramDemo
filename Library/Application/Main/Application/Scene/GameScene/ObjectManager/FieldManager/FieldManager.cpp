/**
 * @file	FieldManager.cpp
 * @brief	フィールド管理クラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "FieldManager.h"

#include "Ground\Ground.h"
#include "Mountain\Mountain.h"
#include "Sky\Sky.h"


//----------------------------------------------------------------------
// Constructor	Destructor
//----------------------------------------------------------------------
FieldManager::FieldManager()
{
	m_pObjects.push_back(new Ground());
	m_pObjects.push_back(new Mountain());
	m_pObjects.push_back(new Sky());
}

FieldManager::~FieldManager()
{
	for (auto itr = m_pObjects.begin(); itr != m_pObjects.end(); itr++)
	{
		delete (*itr);
	}
}


//----------------------------------------------------------------------
// Public Functions
//----------------------------------------------------------------------
bool FieldManager::Initialize()
{
	for (auto itr = m_pObjects.begin(); itr != m_pObjects.end(); itr++)
	{
		if (!(*itr)->Initialize())
		{
			Finalize();
			return false;
		}
	}

	return true;
}

void FieldManager::Finalize()
{
	for (auto itr = m_pObjects.begin(); itr != m_pObjects.end(); itr++)
	{
		(*itr)->Finalize();
	}
}

