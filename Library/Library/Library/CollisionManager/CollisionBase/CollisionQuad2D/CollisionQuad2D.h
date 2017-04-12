/**
 * @file	2D四角形衝突判定オブジェクトクラス定義
 * @brief	CollisionQuad2D.h
 * @author	morimoto
 */
#ifndef COLLISIONQUAD2D_H
#define COLLISIONQUAD2D_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include <D3DX11.h>
#include <D3DX10.h>
#include <vector>
#include <string>

#include "..\CollisionBase.h"


namespace Lib
{
	/**
	 * 2D四角形衝突判定オブジェクトクラス
	 */
	class CollisionQuad2D : public CollisionBase
	{
	public:
		/**
		 * コンストラクタ
		 */
		CollisionQuad2D();

		/**
		 * デストラクタ
		 */
		virtual ~CollisionQuad2D();

		/**
		 * 衝突オブジェクトの名前を追加する
		 * @param[in] _pName 衝突したオブジェクトの名前
		 */
		void AddCollisionName(LPCTSTR _pName)
		{
			m_pCollisionNames.push_back(_pName);
		}

		/**
		 * 衝突オブジェクトの名前を取得する
		 * @return 衝突したオブジェクトの名前が格納されたコンテナ
		 */
		std::vector<LPCSTR>* GetCollisionNames()
		{
			return &m_pCollisionNames;
		}

		/**
		 * 衝突オブジェクトの名前を破棄する
		 */
		void ClearCollisionNames()
		{
			m_pCollisionNames.clear();
		}

		/**
		 * あたり判定座標の設定
		 * @param[in] _pPos 設定する座標
		 */
		inline void SetPos(D3DXVECTOR2* _pPos)
		{
			m_Pos = *_pPos;
		}

		/**
		 * あたり判定座標の取得
		 * @return あたりの座標
		 */
		inline D3DXVECTOR2 GetPos()
		{
			return m_Pos;
		}

		/**
		 * あたり判定サイズの設定
		 * @param[in] _pSize 設定するサイズ
		 */
		inline void SetSize(D3DXVECTOR2* _pSize)
		{
			m_Size = *_pSize;
		}

		/**
		 * あたり判定サイズの取得
		 * @return あたりのサイズ
		 */
		inline D3DXVECTOR2 GetSize()
		{
			return m_Size;
		}

		/**
		 * オブジェクトの名前を設定
		 * @param[in] _pName 設定する名前
		 */
		inline void SetName(LPCTSTR _pName)
		{
			m_Name = _pName;
		}

		/**
		 * オブジェクトの名前を取得
		 * @return オブジェクトの名前
		 */
		inline LPCTSTR GetName()
		{
			return m_Name.c_str();
		}


	private:
		D3DXVECTOR2		m_Pos;	//!< あたり判定の座標
		D3DXVECTOR2		m_Size;	//!< あたり判定のサイズ
		std::string		m_Name;	//!< あたり判定オブジェクトの名前

		std::vector<LPCSTR> m_pCollisionNames;	// 衝突したオブジェクトの名前を格納するコンテナ

	};
}


#endif // COLLISIONQUAD2D_H
