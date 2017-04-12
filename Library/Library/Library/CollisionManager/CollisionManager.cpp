/**
 * @file	CollisionManager.cpp
 * @brief	衝突判定管理クラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "CollisionManager.h"

#include "CollisionBase\CollisionQuad2D\CollisionQuad2D.h"


namespace Lib
{
	//----------------------------------------------------------------------
	// Constructor	Destructor
	//----------------------------------------------------------------------
	CollisionManager::CollisionManager()
	{
	}

	CollisionManager::~CollisionManager()
	{
	}


	//----------------------------------------------------------------------
	// Public Functions
	//----------------------------------------------------------------------
	void CollisionManager::Run()
	{
		for (auto itr = m_pQuadCollision.begin(); itr != m_pQuadCollision.end(); itr++)
		{
			for (auto itr2 = itr++; itr2 != m_pQuadCollision.end(); itr2++)
			{
				if ((*itr)->GetFilter() & (*itr2)->GetFilter())	///@todo 要チェック
				{
					CollisionCheck(*itr, *itr2);
				}
			}
		}
	}

	void CollisionManager::AddCollision(CollisionQuad2D* _pCollision)
	{
		m_pQuadCollision.push_back(_pCollision);
	}

	void CollisionManager::RemoveCollision(CollisionQuad2D* _pCollision)
	{
		for (auto itr = m_pQuadCollision.begin(); itr != m_pQuadCollision.end(); itr++)
		{
			if ((*itr)->GetID() == _pCollision->GetID())
			{
				m_pQuadCollision.erase(itr);
				break;
			}
		}
	}


	//----------------------------------------------------------------------
	// Private Functions
	//----------------------------------------------------------------------
	void CollisionManager::CollisionCheck(CollisionQuad2D* _pCollision1, CollisionQuad2D* _pCollision2)
	{
		D3DXVECTOR2 Pos1 = _pCollision1->GetPos();
		D3DXVECTOR2 Pos2 = _pCollision2->GetPos();

		D3DXVECTOR2 Size1 = _pCollision1->GetSize();
		D3DXVECTOR2 Size2 = _pCollision2->GetSize();

		if ((Pos1.x - Size1.x / 2) < (Pos2.x + Size2.x / 2) &&
			(Pos1.x + Size1.x / 2) > (Pos2.x - Size2.x / 2))
		{
			if ((Pos1.y - Size1.y / 2) < (Pos2.y + Size2.y / 2) &&
				(Pos1.y + Size1.y / 2) > (Pos2.y - Size2.y / 2))
			{
				_pCollision1->AddCollisionName(_pCollision2->GetName());
				_pCollision2->AddCollisionName(_pCollision1->GetName());
			}
		}
	}
}
