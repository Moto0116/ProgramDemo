#include "ObjectManager.h"

#include "FieldManager\Ground\Ground.h"
#include "MainCamera\MainCamera.h"
#include "MainLight\MainLight.h"
#include "House\House.h"
#include "GeometryObject\GeometryObject.h"
#include "TessellationObject\TessellationObject.h"
#include "MiniMap\MiniMap.h"


ObjectManager::ObjectManager()
{
	m_pObjects.push_back(new Ground());
	m_pObjects.push_back(new MainCamera());
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
	m_pObjects.push_back(new GeometryObject());
	m_pObjects.push_back(new TessellationObject());
	m_pObjects.push_back(new MiniMap());
}

ObjectManager::~ObjectManager()
{
	for (auto itr = m_pObjects.begin(); itr != m_pObjects.end(); itr++)
	{
		delete (*itr);
	}
}

bool ObjectManager::Initialize()
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

void ObjectManager::Finalize()
{
	for (auto itr = m_pObjects.begin(); itr != m_pObjects.end(); itr++)
	{
		(*itr)->Finalize();
	}
}

