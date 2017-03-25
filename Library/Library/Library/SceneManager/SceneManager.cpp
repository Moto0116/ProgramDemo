/**
 * @file	SceneManager.cpp
 * @brief	シーン管理クラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "SceneManager.h"

#include <Windows.h>

#include "..\Define\Define.h"
#include "SceneBase\SceneBase.h"


namespace Lib
{
	//----------------------------------------------------------------------
	// Static Private Variables
	//----------------------------------------------------------------------
	const int SceneManager::m_EmptySceneID = -1;


	//----------------------------------------------------------------------
	// Constructor	Destructor
	//----------------------------------------------------------------------
	SceneManager::SceneManager() :
		m_pCurrentScene(NULL)
	{
	}

	SceneManager::~SceneManager()
	{
		for (auto itr = m_pSceneData.begin(); itr != m_pSceneData.end(); itr++)
		{
			SafeDelete(*itr);
		}
	}


	//----------------------------------------------------------------------
	// Public Functions
	//----------------------------------------------------------------------
	bool SceneManager::Initialize()
	{
		SceneBase* pEmptyScene = new SceneBase();
		m_pSceneData.push_back(new SCENE_DATA(pEmptyScene, m_EmptySceneID));

		m_pCurrentScene = pEmptyScene;	// 空シーンを現在のシーンとして登録しておく

		return true;
	}

	void SceneManager::Finalize()
	{
		for (auto itr = m_pSceneData.begin(); itr != m_pSceneData.end(); itr++)
		{
			if ((*itr)->SceneID == m_EmptySceneID)	// 空シーンを探して削除する
			{
				delete (*itr)->pScene;
				delete (*itr);
				(*itr) = NULL;

				break;
			}
		}
	}

	void SceneManager::Update()
	{
		switch (m_pCurrentScene->GetState())
		{
		case SceneBase::INIT_STATE:
			m_pCurrentScene->Initialize();

			break;
		case SceneBase::UPDATE_STATE:
			m_pCurrentScene->Update();

			break;
		case SceneBase::FINAL_STATE:
			m_pCurrentScene->Finalize();

			break;
		}
	}

	bool SceneManager::AddScene(SceneBase* _pScene, int _sceneId)
	{
		for (auto itr = m_pSceneData.begin(); itr != m_pSceneData.end(); itr++)
		{
			if ((*itr)->SceneID == _sceneId)
			{
				return false;	// SceneIDがユニークではない
			}
		}

		m_pSceneData.push_back(new SCENE_DATA(_pScene, _sceneId));

		return true;
	}

	void SceneManager::DeleteScene(int _sceneId)
	{
		for (auto itr = m_pSceneData.begin(); itr != m_pSceneData.end(); itr++)
		{
			if ((*itr)->SceneID == _sceneId)
			{
				SafeDelete(*itr);
				m_pSceneData.erase(itr);

				break;
			}
		}
	}

	void SceneManager::SetEntryScene(int _sceneId)
	{
		for (auto itr = m_pSceneData.begin(); itr != m_pSceneData.end(); itr++)
		{
			if ((*itr)->SceneID == _sceneId)
			{
				m_pCurrentScene = (*itr)->pScene;
			}
		}
	}
}

