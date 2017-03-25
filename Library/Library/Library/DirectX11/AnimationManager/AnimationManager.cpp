/**
 * @file	AnimationManager.h
 * @brief	アニメーションの管理クラス定義
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "AnimationManager.h"

#include "Animation\Animation.h"


namespace Lib
{
	//----------------------------------------------------------------------
	// Static Private Variables
	//----------------------------------------------------------------------
	const int AnimationManager::m_InvalidIndex = 0;


	//----------------------------------------------------------------------
	// Constructor	Destructor
	//----------------------------------------------------------------------
	AnimationManager::AnimationManager()
	{
		// 読み込みに失敗した際に参照する値としてNULLを追加
		m_pAnimation.push_back(NULL);
	}

	AnimationManager::~AnimationManager()
	{
		Finalize();
	}


	//----------------------------------------------------------------------
	// Public Functions
	//----------------------------------------------------------------------
	bool AnimationManager::Initialize()
	{
		return true;
	}

	void AnimationManager::Finalize()
	{
		for (auto itr = m_pAnimation.begin(); itr != m_pAnimation.end(); itr++)
		{
			SafeDelete(*itr);
		}
	}

	bool AnimationManager::LoadAnimation(LPCTSTR _pAnimationPath, int* _pIndex)
	{
		Animation* pAnimation = new Animation(_pAnimationPath);

		if (!pAnimation->GetCurrentFrame())
		{
			delete pAnimation;
			*_pIndex = m_InvalidIndex;
			return false;
		}

		*_pIndex = m_pAnimation.size();
		m_pAnimation.push_back(pAnimation);

		return true;
	}

	void AnimationManager::ReleaseAnimation(int _index)
	{
		if (m_pAnimation[_index] != NULL)
		{
			delete m_pAnimation[_index];
			m_pAnimation[_index] = NULL;
		}
	}
}

