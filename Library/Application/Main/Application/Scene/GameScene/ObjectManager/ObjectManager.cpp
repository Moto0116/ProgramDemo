/**
 * @file	ObjectManager.cpp
 * @brief	オブジェクト管理クラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "ObjectManager.h"

#include "Field\Field.h"
#include "MainCamera\MainCamera.h"
#include "MainLight\MainLight.h"
#include "House\House.h"
#include "GeometryObject\GeometryObject.h"
#include "TessellationObject\TessellationObject.h"
#include "MiniMap\MiniMap.h"
#include "Rain\Rain.h"


//----------------------------------------------------------------------
// Constructor	Destructor
//----------------------------------------------------------------------
ObjectManager::ObjectManager()
{
	MainCamera* pCamera = new MainCamera();

	m_pObjects.push_back(new Field());
	m_pObjects.push_back(pCamera);
	m_pObjects.push_back(new MainLight());
	m_pObjects.push_back(new House(D3DXVECTOR3(20, 0, 25), 0));
	m_pObjects.push_back(new House(D3DXVECTOR3(40, 0, 25), 0));
	m_pObjects.push_back(new House(D3DXVECTOR3(60, 0, 25), 0));
	m_pObjects.push_back(new House(D3DXVECTOR3(20, 0, 75), 180));
	m_pObjects.push_back(new House(D3DXVECTOR3(40, 0, 75), 180));
	m_pObjects.push_back(new House(D3DXVECTOR3(60, 0, 75), 180));
	m_pObjects.push_back(new House(D3DXVECTOR3(100, 0, 80), -90));
	m_pObjects.push_back(new House(D3DXVECTOR3(100, 0, 60), -90));
	m_pObjects.push_back(new House(D3DXVECTOR3(100, 0, 40), -90));
	m_pObjects.push_back(new House(D3DXVECTOR3(100, 0, 20), -90));
	m_pObjects.push_back(new House(D3DXVECTOR3(-100, 0, 20), 90));
	m_pObjects.push_back(new House(D3DXVECTOR3(-100, 0, 40), 90));
	m_pObjects.push_back(new MiniMap());
	m_pObjects.push_back(new Rain(pCamera));
}

ObjectManager::~ObjectManager()
{
	for (auto itr = m_pObjects.begin(); itr != m_pObjects.end(); itr++)
	{
		delete (*itr);
	}
}

