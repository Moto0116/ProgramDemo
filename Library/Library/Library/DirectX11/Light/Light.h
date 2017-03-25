/**
 * @file   Light.h
 * @brief  Lightクラスの定義
 * @author morimoto
 */
#ifndef LIB_LIGHT_H
#define LIB_LiGHT_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include <D3DX11.h>
#include <D3DX10.h>


namespace Lib
{
	/**
	 * ライト情報の管理クラス
	 */
	class Light
	{
	public:
		/**
		 * コンストラクタ
		 */
		Light();

		/**
		 * デストラクタ
		 */
		~Light();

	
		/**
		 * ライト座標の設定
		 * @param[in] _pPos 設定する座標
		 */
		inline void SetPos(const D3DXVECTOR3* _pPos)
		{
			m_Pos = *_pPos;
		}

		/**
		 * ライトの向きの設定
		 * @param[in] 設定する向き
		 */
		inline void SetDirectionPos(const D3DXVECTOR3* _pDirectionPos)
		{
			m_DirectionPos = *_pDirectionPos;
		}

		/**
		 * ライト座標の取得
		 * @return ライト座標
		 */
		inline D3DXVECTOR3 GetPos()
		{
			return m_Pos;
		}
		
		/**
		 * ライトの向きの取得
		 * @return ライトの向き
		 */
		inline D3DXVECTOR3 GetDirectionPos()
		{
			return m_DirectionPos;
		}

	private:
		D3DXVECTOR3 m_Pos;
		D3DXVECTOR3 m_DirectionPos;

	};
}


#endif // LIB_LIGHT_H
