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
	m_pObjects.push_back(new House());
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

