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
	if (!CreateTask())					return false;
	if (!CreateModel())					return false;
	if (!CreateTexture())				return false;
	if (!CreateDefaultShader())			return false;
	if (!CreateShadowShader())			return false;
	if (!CreateMapShader())				return false;
	if (!CreateCubeMapShader())			return false;
	if (!CreateVertexLayout())			return false;
	if (!CreateDepthStencilState())		return false;
	if (!CreateConstantBuffer())		return false;
	if (!WriteConstantBuffer())			return false;

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
	ReleaseDefaultShader();
	ReleaseTexture();
	ReleaseModel();
	ReleaseTask();
}

void Sky::Update()
{
}

void Sky::Draw()
{
	ID3D11DeviceContext* pDeviceContext = SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDeviceContext();
	Lib::Dx11::TextureManager* pTextureManager = SINGLETON_INSTANCE(Lib::Dx11::TextureManager);

	ShaderSetup();

	ID3D11ShaderResourceView* pResource = pTextureManager->GetTexture(m_SkyCLUTIndex)->Get();
	pDeviceContext->PSSetShaderResources(4, 1, &pResource);

	VertexLayoutSetup();
	DepthStencilStateSetup();
	ConstantBufferSetup();

	SINGLETON_INSTANCE(Lib::Dx11::FbxFileManager)->GetFbxModel(m_SkyModelIndex)->Draw();
}

void Sky::DepthDraw()
{
}

void Sky::MapDraw()
{
}

void Sky::CubeMapDraw()
{
	ID3D11DeviceContext* pDeviceContext = SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDeviceContext();
	Lib::Dx11::ShaderManager* pShaderManager = SINGLETON_INSTANCE(Lib::Dx11::ShaderManager);
	Lib::Dx11::FbxFileManager* pFbxFileManager = SINGLETON_INSTANCE(Lib::Dx11::FbxFileManager);

	pDeviceContext->VSSetShader(pShaderManager->GetVertexShader(m_CubeMapVertexShaderIndex), nullptr, 0);
	pDeviceContext->GSSetShader(pShaderManager->GetGeometryShader(m_CubeMapGeometryShaderIndex), nullptr, 0);
	pDeviceContext->PSSetShader(pShaderManager->GetPixelShader(m_CubeMapPixelShaderIndex), nullptr, 0);

	TextureSetup();
	VertexLayoutSetup();
	DepthStencilStateSetup();
	ConstantBufferSetup();
	pFbxFileManager->GetFbxModel(m_SkyModelIndex)->Draw();
}


//----------------------------------------------------------------------
// Private Functions
//----------------------------------------------------------------------
bool Sky::CreateTask()
{
	SINGLETON_INSTANCE(Lib::DrawTaskManager)->AddTask(m_pDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->AddTask(m_pUpdateTask);
	SINGLETON_INSTANCE(DepthDrawTaskManager)->AddTask(m_pDepthDrawTask);
	SINGLETON_INSTANCE(MapDrawTaskManager)->AddTask(m_pMapDrawTask);
	SINGLETON_INSTANCE(CubeMapDrawTaskManager)->AddTask(m_pCubeMapDrawTask);

	return true;
}

bool Sky::CreateModel()
{
	if (!SINGLETON_INSTANCE(Lib::Dx11::FbxFileManager)->LoadFbxModel(
		TEXT("Resource\\Model\\sky.fbx"),
		&m_SkyModelIndex))
	{
		OutputErrorLog("空モデルの読み込みに失敗しました");
		return false;
	}

	return true;
}

bool Sky::CreateTexture()
{
	if (!SINGLETON_INSTANCE(Lib::Dx11::TextureManager)->LoadTexture(
		"Resource\\Texture\\SkyCLUT.png",
		&m_SkyCLUTIndex))
	{
		OutputErrorLog("空のカラーテーブル読み込みに失敗しました");
		return false;
	}

	return true;
}

bool Sky::CreateDefaultShader()
{
	if (!SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadVertexShader(
		TEXT("Resource\\Effect\\SkyEffect.fx"),
		"VS",
		&m_VertexShaderIndex))
	{
		OutputErrorLog("頂点シェーダの読み込みに失敗しました");
		return false;
	}

	if (!SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadPixelShader(
		TEXT("Resource\\Effect\\SkyEffect.fx"),
		"PS",
		&m_PixelShaderIndex))
	{
		OutputErrorLog("ピクセルシェーダの読み込みに失敗しました");
		return false;
	}

	return true;
}

bool Sky::CreateShadowShader()
{
	if (FAILED(SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadVertexShader(
		TEXT("Resource\\Effect\\DepthShadow.fx"),
		"VS",
		&m_ShadowVertexShaderIndex)))
	{
		OutputErrorLog("影の頂点シェーダの読み込みに失敗しました");
		return false;
	}

	if (FAILED(SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadPixelShader(
		TEXT("Resource\\Effect\\DepthShadow.fx"),
		"PS",
		&m_ShadowPixelShaderIndex)))
	{
		OutputErrorLog("影のピクセルシェーダの読み込みに失敗しました");
		return false;
	}

	return true;
}

bool Sky::CreateMapShader()
{
	if (FAILED(SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadVertexShader(
		TEXT("Resource\\Effect\\MiniMap.fx"),
		"VS",
		&m_MapVertexShaderIndex)))
	{
		OutputErrorLog("マップの頂点シェーダの読み込みに失敗しました");
		return false;
	}
	
	if (FAILED(SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadPixelShader(
		TEXT("Resource\\Effect\\MiniMap.fx"),
		"PS",
		&m_MapPixelShaderIndex)))
	{
		OutputErrorLog("マップのピクセルシェーダの読み込みに失敗しました");
		return false;
	}

	return true;
}

bool Sky::CreateCubeMapShader()
{
	if (!SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadVertexShader(
		TEXT("Resource\\Effect\\CubeMap.fx"),
		"VS",
		&m_CubeMapVertexShaderIndex))
	{
		OutputErrorLog("キューブマップの頂点シェーダの読み込みに失敗しました");
		return false;
	}

	if (!SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadGeometryShader(
		TEXT("Resource\\Effect\\CubeMap.fx"),
		"GS",
		&m_CubeMapGeometryShaderIndex))
	{
		OutputErrorLog("キューブマップのジオメトリシェーダの読み込みに失敗しました");
		return false;
	}

	if (!SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadPixelShader(
		TEXT("Resource\\Effect\\CubeMap.fx"),
		"PS",
		&m_CubeMapPixelShaderIndex))
	{
		OutputErrorLog("キューブマップのピクセルシェーダの読み込みに失敗しました");
		return false;
	}

	return true;
}

void Sky::ReleaseTask()
{
	SINGLETON_INSTANCE(MapDrawTaskManager)->RemoveTask(m_pMapDrawTask);
	SINGLETON_INSTANCE(DepthDrawTaskManager)->RemoveTask(m_pDepthDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->RemoveTask(m_pUpdateTask);
	SINGLETON_INSTANCE(Lib::DrawTaskManager)->RemoveTask(m_pDrawTask);
}

void Sky::ReleaseModel()
{
	SINGLETON_INSTANCE(Lib::Dx11::FbxFileManager)->ReleaseFbxModel(m_SkyModelIndex);
}

void Sky::ReleaseTexture()
{
	SINGLETON_INSTANCE(Lib::Dx11::TextureManager)->ReleaseTexture(m_SkyCLUTIndex);
}

void Sky::ReleaseDefaultShader()
{
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleaseVertexShader(m_VertexShaderIndex);
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleasePixelShader(m_PixelShaderIndex);
}

void Sky::ReleaseShadowShader()
{
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleasePixelShader(m_ShadowPixelShaderIndex);
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleaseVertexShader(m_ShadowVertexShaderIndex);
}

void Sky::ReleaseMapShader()
{
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleasePixelShader(m_MapPixelShaderIndex);
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleaseVertexShader(m_MapVertexShaderIndex);
}

void Sky::ReleaseCubeMapShader()
{
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleasePixelShader(m_CubeMapPixelShaderIndex);
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleaseGeometryShader(m_CubeMapGeometryShaderIndex);
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleaseVertexShader(m_CubeMapVertexShaderIndex);
}


