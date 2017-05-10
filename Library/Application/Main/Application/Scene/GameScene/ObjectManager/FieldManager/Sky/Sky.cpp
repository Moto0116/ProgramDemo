/**
 * @file	Sky.cpp
 * @brief	空の管理オブジェクト実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "Sky.h"

#include "Debugger\Debugger.h"
#include "DirectX11\FbxFileManager\FbxFileManager.h"
#include "DirectX11\GraphicsDevice\GraphicsDevice.h"
#include "DirectX11\ShaderManager\ShaderManager.h"
#include "DirectX11\TextureManager\TextureManager.h"
#include "DirectX11\TextureManager\ITexture\ITexture.h"


//----------------------------------------------------------------------
// Private Static Variables
//----------------------------------------------------------------------
D3DXVECTOR3 Sky::m_DefaultScale = D3DXVECTOR3(5.5, 5.5, 5.5);


//----------------------------------------------------------------------
// Constructor	Destructor
//----------------------------------------------------------------------
Sky::Sky()
{
	m_Scale = m_DefaultScale;
}

Sky::~Sky()
{
}


//----------------------------------------------------------------------
// Public Functions
//----------------------------------------------------------------------
bool Sky::Initialize()
{
	SINGLETON_INSTANCE(Lib::DrawTaskManager)->AddTask(m_pDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->AddTask(m_pUpdateTask);
	SINGLETON_INSTANCE(DepthDrawTaskManager)->AddTask(m_pDepthDrawTask);
	SINGLETON_INSTANCE(MapDrawTaskManager)->AddTask(m_pMapDrawTask);
	SINGLETON_INSTANCE(CubeMapDrawTaskManager)->AddTask(m_pCubeMapDrawTask);

	SINGLETON_INSTANCE(Lib::FbxFileManager)->LoadFbxModel(
		TEXT("Resource\\Model\\sky.fbx"),
		&m_SkyModelIndex);

	SINGLETON_INSTANCE(Lib::ShaderManager)->LoadVertexShader(
		TEXT("Resource\\Effect\\SkyEffect.fx"),
		"VS",
		&m_VertexShaderIndex);

	SINGLETON_INSTANCE(Lib::ShaderManager)->LoadPixelShader(
		TEXT("Resource\\Effect\\SkyEffect.fx"), 
		"PS",
		&m_PixelShaderIndex);

	SINGLETON_INSTANCE(Lib::TextureManager)->LoadTexture(
		"Resource\\Texture\\SkyCLUT.png", 
		&m_SkyCLUTIndex);


	if (!CreateShadowShader())
	{
		return false;
	}

	if (!CreateMapShader())
	{
		return false;
	}

	if (!CreateCubeMapShader())
	{
		return false;
	}

	if (!CreateVertexLayout())
	{
		return false;
	}

	if (!CreateDepthStencilState())
	{
		return false;
	}

	if (!CreateConstantBuffer())
	{
		return false;
	}

	if (!WriteConstantBuffer())
	{
		return false;
	}

	return true;
}

void Sky::Finalize()
{
	ReleaseConstantBuffer();
	ReleaseDepthStencilState();
	ReleaseVertexLayout();
	ReleaseCubeMapShader();
	ReleaseMapShader();
	ReleaseShadowShader();

	SINGLETON_INSTANCE(Lib::TextureManager)->ReleaseTexture(m_SkyCLUTIndex);

	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleaseVertexShader(m_VertexShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleasePixelShader(m_PixelShaderIndex);

	SINGLETON_INSTANCE(Lib::FbxFileManager)->ReleaseFbxModel(m_SkyModelIndex);

	SINGLETON_INSTANCE(MapDrawTaskManager)->RemoveTask(m_pMapDrawTask);
	SINGLETON_INSTANCE(DepthDrawTaskManager)->RemoveTask(m_pDepthDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->RemoveTask(m_pUpdateTask);
	SINGLETON_INSTANCE(Lib::DrawTaskManager)->RemoveTask(m_pDrawTask);
}

void Sky::Update()
{
}

void Sky::Draw()
{
	ShaderSetup();

	ID3D11DeviceContext* pContext = SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext();
	ID3D11ShaderResourceView* pResource =
		SINGLETON_INSTANCE(Lib::TextureManager)->GetTexture(m_SkyCLUTIndex)->Get();
	pContext->PSSetShaderResources(4, 1, &pResource);

	VertexLayoutSetup();
	DepthStencilStateSetup();
	ConstantBufferSetup();

	SINGLETON_INSTANCE(Lib::FbxFileManager)->GetFbxModel(m_SkyModelIndex)->Draw();
}

void Sky::DepthDraw()
{
}

void Sky::MapDraw()
{
}

void Sky::CubeMapDraw()
{
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->VSSetShader(
		SINGLETON_INSTANCE(Lib::ShaderManager)->GetVertexShader(m_CubeMapVertexShaderIndex),
		NULL,
		0);

	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->GSSetShader(
		SINGLETON_INSTANCE(Lib::ShaderManager)->GetGeometryShader(m_CubeMapGeometryShaderIndex),
		NULL,
		0);

	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->PSSetShader(
		SINGLETON_INSTANCE(Lib::ShaderManager)->GetPixelShader(m_CubeMapPixelShaderIndex),
		NULL,
		0);

	TextureSetup();
	VertexLayoutSetup();
	DepthStencilStateSetup();
	ConstantBufferSetup();
	SINGLETON_INSTANCE(Lib::FbxFileManager)->GetFbxModel(m_SkyModelIndex)->Draw();
}


//----------------------------------------------------------------------
// Private Functions
//----------------------------------------------------------------------
bool Sky::CreateShadowShader()
{
	if (FAILED(SINGLETON_INSTANCE(Lib::ShaderManager)->LoadVertexShader(
		TEXT("Resource\\Effect\\DepthShadow.fx"),
		"VS",
		&m_ShadowVertexShaderIndex)))
	{
		return false;
	}

	if (FAILED(SINGLETON_INSTANCE(Lib::ShaderManager)->LoadPixelShader(
		TEXT("Resource\\Effect\\DepthShadow.fx"),
		"PS",
		&m_ShadowPixelShaderIndex)))
	{
		return false;
	}

	return true;
}

bool Sky::CreateMapShader()
{
	if (FAILED(SINGLETON_INSTANCE(Lib::ShaderManager)->LoadVertexShader(
		TEXT("Resource\\Effect\\MiniMap.fx"),
		"VS",
		&m_MapVertexShaderIndex)))
	{
		return false;
	}
	
	if (FAILED(SINGLETON_INSTANCE(Lib::ShaderManager)->LoadPixelShader(
		TEXT("Resource\\Effect\\MiniMap.fx"),
		"PS",
		&m_MapPixelShaderIndex)))
	{
		return false;
	}

	return true;
}

bool Sky::CreateCubeMapShader()
{
	if (!SINGLETON_INSTANCE(Lib::ShaderManager)->LoadVertexShader(
		TEXT("Resource\\Effect\\CubeMap.fx"),
		"VS",
		&m_CubeMapVertexShaderIndex))
	{
		return false;
	}

	if (!SINGLETON_INSTANCE(Lib::ShaderManager)->LoadGeometryShader(
		TEXT("Resource\\Effect\\CubeMap.fx"),
		"GS",
		&m_CubeMapGeometryShaderIndex))
	{
		return false;
	}

	if (!SINGLETON_INSTANCE(Lib::ShaderManager)->LoadPixelShader(
		TEXT("Resource\\Effect\\CubeMap.fx"),
		"PS",
		&m_CubeMapPixelShaderIndex))
	{
		return false;
	}

	return true;
}

void Sky::ReleaseShadowShader()
{
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleasePixelShader(m_ShadowPixelShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleaseVertexShader(m_ShadowVertexShaderIndex);
}

void Sky::ReleaseMapShader()
{
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleasePixelShader(m_MapPixelShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleaseVertexShader(m_MapVertexShaderIndex);
}

void Sky::ReleaseCubeMapShader()
{
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleasePixelShader(m_CubeMapPixelShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleaseGeometryShader(m_CubeMapGeometryShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleaseVertexShader(m_CubeMapVertexShaderIndex);
}


