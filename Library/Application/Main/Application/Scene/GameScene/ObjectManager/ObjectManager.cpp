#include "ObjectManager.h"

#include "FieldManager\Ground\Ground.h"
#include "MainCamera\MainCamera.h"
#include "MainLight\MainLight.h"
#include "House\House.h"


ObjectManager::ObjectManager()
{
	m_pObjects.push_back(new Ground());
	m_pObjects.push_back(new MainCamera());
	m_pObjects.push_back(new MainLight());
	m_pObjects.push_back(new House(D3DXVECTOR3(0, 0, 0)));
	m_pObjects.push_back(new House(D3DXVECTOR3(40, 0, 0)));
	m_pObjects.push_back(new House(D3DXVECTOR3(0, 0, 40)));
	m_pObjects.push_back(new House(D3DXVECTOR3(40, 0, 40)));
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

