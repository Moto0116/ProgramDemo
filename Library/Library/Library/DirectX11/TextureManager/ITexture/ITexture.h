/**
 * @file	ITexture.h
 * @brief	テクスチャインターフェース定義
 * @author	morimoto
 */
#ifndef LIB_ITEXTURE_H
#define LIB_ITEXTURE_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include <Windows.h>

#include "..\..\..\Define\Define.h"


namespace Lib
{
	/**
	 * テクスチャインターフェース
	 */
	interface ITexture
	{
	public:
		/**
		 * コンストラクタ
		 */
		ITexture();

		/**
		 * デストラクタ
		 */
		virtual ~ITexture() = 0;

	private:
		DISALLOW_COPY_AND_ASSIGN(ITexture);

	};
}



#endif // LIB_ITEXTURE_H
