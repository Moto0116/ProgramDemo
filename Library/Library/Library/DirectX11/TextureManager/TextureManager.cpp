/**
 * @file	TextureManager.cpp
 * @brief	テクスチャ管理クラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "TextureManager.h"

#include "..\GraphicsDevice\GraphicsDevice.h"
#include "Texture\Texture.h"


namespace Lib
{
	//----------------------------------------------------------------------
	// Static Private Variables
	//----------------------------------------------------------------------
	const int TextureManager::m_InvalidIndex = 0;


	//----------------------------------------------------------------------
	// Constructor	Destructor
	//----------------------------------------------------------------------
	TextureManager::TextureManager() : 
		m_pGraphicsDevice(NULL)
	{
		// 読み込みに失敗した際に参照する値としてNULLを追加
		m_pTextures.push_back(NULL);
	}

	TextureManager::~TextureManager()
	{
		Finalize();
	}


	//----------------------------------------------------------------------
	// Public Functions
	//----------------------------------------------------------------------
	bool TextureManager::Initialize(GraphicsDevice* _pGraphicsDevice)
	{
		m_pGraphicsDevice = _pGraphicsDevice;
		return true;
	}

	void TextureManager::Finalize()
	{
		for (auto itr = m_pTextures.begin(); itr != m_pTextures.end(); itr++)
		{
			SafeDelete(*itr);
		}
	}

	bool TextureManager::LoadTexture(LPCTSTR _pTexturePath, int* _pIndex)
	{
		Texture* pTexture = new Texture(m_pGraphicsDevice, _pTexturePath);

		if (pTexture->Get() == NULL)
		{
			delete pTexture;
			*_pIndex = m_InvalidIndex;
			return false;
		}

		*_pIndex = m_pTextures.size();
		m_pTextures.push_back(pTexture);

		return true;
	}

	void TextureManager::ReleaseTexture(int _index)
	{
		if (m_pTextures[_index] != NULL)
		{
			delete m_pTextures[_index];
			m_pTextures[_index] = NULL;
		}
	}

	ITexture* TextureManager::GetTexture(int _index)
	{
		return m_pTextures[_index];
	}

	void TextureManager::ClearBuffer()
	{
		m_pTextures.clear();
	}
}

