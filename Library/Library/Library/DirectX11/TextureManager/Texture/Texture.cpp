/**
 * @file	Texture.cpp
 * @brief	テクスチャクラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "Texture.h"

#include "..\..\..\Debugger\Debugger.h"
#include "..\..\GraphicsDevice\GraphicsDevice.h"


namespace Lib
{
	//----------------------------------------------------------------------
	// Constructor	Destructor
	//----------------------------------------------------------------------
	Texture::Texture(GraphicsDevice* _pGraphicsDevice, LPCTSTR _pTexturePath) :
		m_pGraphicsDevice(_pGraphicsDevice),
		m_pTexture(NULL)
	{
		Load(_pTexturePath);
	}

	Texture::~Texture()
	{
		Release();
	}


	//----------------------------------------------------------------------
	// Private Functions
	//----------------------------------------------------------------------
	void Texture::Load(LPCTSTR _pTexturePath)
	{
		MyAssert(m_pGraphicsDevice != NULL, "GraphicsDeviceがありません");


		D3DX11_IMAGE_LOAD_INFO LoadInfo;
		ZeroMemory(&LoadInfo, sizeof(D3DX11_IMAGE_LOAD_INFO));
		LoadInfo.Width = D3DX11_DEFAULT;
		LoadInfo.Height = D3DX11_DEFAULT;
		LoadInfo.Depth = D3DX11_DEFAULT;
		LoadInfo.FirstMipLevel = D3DX11_DEFAULT;
		LoadInfo.MipLevels = 1;
		LoadInfo.Usage = D3D11_USAGE_DEFAULT;
		LoadInfo.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		LoadInfo.CpuAccessFlags = 0;
		LoadInfo.MiscFlags = 0;
		LoadInfo.Format = DXGI_FORMAT_FROM_FILE;
		LoadInfo.Filter = D3DX11_FILTER_POINT;
		LoadInfo.MipFilter = D3DX11_FILTER_POINT;
		LoadInfo.pSrcInfo = NULL;

		if (FAILED(D3DX11CreateShaderResourceViewFromFile(
			m_pGraphicsDevice->GetDevice(),
			_pTexturePath,
			&LoadInfo,
			NULL,
			&m_pTexture,
			NULL)))
		{
			OutputErrorLog("テクスチャの読み込みに失敗しました");
		}
	}

	void Texture::Release()
	{
		if (m_pTexture != NULL)
		{
			m_pTexture->Release();
			m_pTexture = NULL;
		}
	}
}
