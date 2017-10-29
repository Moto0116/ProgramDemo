/**
 * @file	ObjectManager.cpp
 * @brief	オブジェクト管理クラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "ObjectManager.h"

#include "FieldManager\FieldManager.h"
#include "MainCamera\MainCamera.h"
#include "MainLight\MainLight.h"
#include "House\House.h"
#include "MiniMap\MiniMap.h"
#include "Rain\Rain.h"
#include "Water\Water.h"
#include "test\test.h"


//----------------------------------------------------------------------
// Constructor	Destructor
//----------------------------------------------------------------------
ObjectManager::ObjectManager()
{
	m_pObjectManagers.push_back(new FieldManager());

	MainCamera* pCamera = new MainCamera();
	m_pObjects.push_back(pCamera);
	m_pObjects.push_back(new House(pCamera, D3DXVECTOR3(0, 0, 45), 0));
	m_pObjects.push_back(new House(pCamera, D3DXVECTOR3(20, 0, 45), 0));
	m_pObjects.push_back(new House(pCamera, D3DXVECTOR3(40, 0, 45), 0));
	m_pObjects.push_back(new House(pCamera, D3DXVECTOR3(0, 0, 95), 180));
	m_pObjects.push_back(new House(pCamera, D3DXVECTOR3(20, 0, 95), 180));
	m_pObjects.push_back(new House(pCamera, D3DXVECTOR3(40, 0, 95), 180));
	m_pObjects.push_back(new House(pCamera, D3DXVECTOR3(80, 0, 80), -90));
	m_pObjects.push_back(new House(pCamera, D3DXVECTOR3(80, 0, 60), -90));
	m_pObjects.push_back(new House(pCamera, D3DXVECTOR3(80, 0, 40), -90));
	m_pObjects.push_back(new House(pCamera, D3DXVECTOR3(80, 0, 20), -90));
	m_pObjects.push_back(new House(pCamera, D3DXVECTOR3(-100, 0, 20), 90));
	m_pObjects.push_back(new House(pCamera, D3DXVECTOR3(-100, 0, 40), 90));
	m_pObjects.push_back(new MiniMap());
	m_pObjects.push_back(new Water());
	m_pObjects.push_back(new Rain(pCamera));
	m_pObjects.push_back(new MainLight(pCamera));
	m_pObjects.push_back(new test());
}

ObjectManager::~ObjectManager()
{
	for (auto itr = m_pObjects.begin(); itr != m_pObjects.end(); itr++)
	{
		delete (*itr);
	}

	for (auto itr = m_pObjectManagers.begin(); itr != m_pObjectManagers.end(); itr++)
	{
		delete (*itr);
	}
}


//----------------------------------------------------------------------
// Public Functions
//----------------------------------------------------------------------
bool ObjectManager::Initialize()
{
	for (auto itr = m_pObjectManagers.begin(); itr != m_pObjectManagers.end(); itr++)
	{
		if (!(*itr)->Initialize())
		{
			Finalize();
			return false;
		}
	}

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

void ObjectManager::Finalize()
{
	for (auto itr = m_pObjects.begin(); itr != m_pObjects.end(); itr++)
	{
		(*itr)->Finalize();
	}

	for (auto itr = m_pObjectManagers.begin(); itr != m_pObjectManagers.end(); itr++)
	{
		(*itr)->Finalize();
	}
}

