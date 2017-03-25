/**
 * @file	Texture.h
 * @brief	テクスチャクラス定義
 * @author	morimoto
 */
#ifndef LIB_TEXTURE_H
#define LIB_TEXTURE_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include <D3DX11.h>
#include <D3DX10.h>

#include "..\..\..\Define\Define.h"
#include "..\..\TextureManager\ITexture\ITexture.h"


namespace Lib
{
	class GraphicsDevice;

	/**
	 * テクスチャクラス
	 */
	class Texture : public ITexture
	{
	public:
		/**
		 * コンストラクタ
		 * @param[in] _pGraphicsDevice グラフィックデバイス
		 * @param[in] _pTexturePath テクスチャのパス
		 */
		Texture(GraphicsDevice* _pGraphicsDevice, LPCTSTR _pTexturePath);

		/**
		 * デストラクタ
		 */
		virtual ~Texture();

		/**
		 * テクスチャの取得
		 * @return テクスチャ
		 */
		inline ID3D11ShaderResourceView* Get()
		{ 
			return m_pTexture; 
		}

	private:
		/**
		 * テクスチャの読み込み
		 * @param[in] _pTexturePath テクスチャのパス
		 */
		void Load(LPCTSTR _pTexturePath);

		/**
		 * テクスチャの解放
		 */
		void Release();


		GraphicsDevice*				m_pGraphicsDevice;
		ID3D11ShaderResourceView*	m_pTexture;

		DISALLOW_COPY_AND_ASSIGN(Texture);

	};
}


#endif // LIB_TEXTURE_H
