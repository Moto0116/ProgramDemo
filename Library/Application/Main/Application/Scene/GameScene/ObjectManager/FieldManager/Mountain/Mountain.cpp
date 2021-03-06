﻿/**
 * @file	Mountain.cpp
 * @brief	山の管理クラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "Mountain.h"

#include "Debugger\Debugger.h"
#include "DirectX11\FbxFileManager\Dx11FbxFileManager.h"
#include "DirectX11\GraphicsDevice\Dx11GraphicsDevice.h"
#include "DirectX11\ShaderManager\Dx11ShaderManager.h"


//----------------------------------------------------------------------
// Private Static Variables
//----------------------------------------------------------------------
D3DXVECTOR3 Mountain::m_DefaultScale = D3DXVECTOR3(3.5, 3.5, 3.5);


//----------------------------------------------------------------------
// Constructor	Destructor
//----------------------------------------------------------------------
Mountain::Mountain()
{
	m_Scale = m_DefaultScale;
}

Mountain::~Mountain()
{
}


//----------------------------------------------------------------------
// Public Functions
//----------------------------------------------------------------------
bool Mountain::Initialize()
{
	if (!CreateTask())				return false;
	if (!CreateModel())				return false;
	if (!CreateDefaultShader())		return false;
	if (!CreateShadowShader())		return false;
	if (!CreateMapShader())			return false;
	if (!CreateCubeMapShader())		return false;
	if (!CreateTexture())			return false;
	if (!CreateVertexLayout())		return false;
	if (!CreateDepthStencilState())	return false;
	if (!CreateConstantBuffer())	return false;
	if (!WriteConstantBuffer())		return false;

	return true;
}

void Mountain::Finalize()
{
	ReleaseConstantBuffer();
	ReleaseDepthStencilState();
	ReleaseVertexLayout();
	ReleaseTexture();
	ReleaseCubeMapShader();
	ReleaseMapShader();
	ReleaseShadowShader();
	ReleaseDefaultShader();
	ReleaseModel();
	ReleaseTask();
}

void Mountain::Draw()
{
	Lib::Dx11::FbxFileManager* pFbxFileManager = SINGLETON_INSTANCE(Lib::Dx11::FbxFileManager);

	ShaderSetup();
	TextureSetup();
	VertexLayoutSetup();
	DepthStencilStateSetup();
	ConstantBufferSetup();

	pFbxFileManager->GetFbxModel(m_MountainModelIndex)->Draw();
}

void Mountain::MapDraw()
{
	ID3D11DeviceContext* pDeviceContext = SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDeviceContext();
	Lib::Dx11::ShaderManager* pShaderManager = SINGLETON_INSTANCE(Lib::Dx11::ShaderManager);
	Lib::Dx11::FbxFileManager* pFbxFileManager = SINGLETON_INSTANCE(Lib::Dx11::FbxFileManager);

	pDeviceContext->VSSetShader(pShaderManager->GetVertexShader(m_MapVertexShaderIndex), nullptr, 0);
	pDeviceContext->PSSetShader(pShaderManager->GetPixelShader(m_MapPixelShaderIndex), nullptr, 0);

	VertexLayoutSetup();
	DepthStencilStateSetup();
	WriteConstantBuffer();
	ConstantBufferSetup();
	pFbxFileManager->GetFbxModel(m_MountainModelIndex)->Draw();
}

void Mountain::CubeMapDraw()
{
	ID3D11DeviceContext* pDeviceContext = SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDeviceContext();
	Lib::Dx11::ShaderManager* pShaderManager = SINGLETON_INSTANCE(Lib::Dx11::ShaderManager);
	Lib::Dx11::FbxFileManager* pFbxFileManager = SINGLETON_INSTANCE(Lib::Dx11::FbxFileManager);

	pDeviceContext->VSSetShader(pShaderManager->GetVertexShader(m_CubeMapVertexShaderIndex), nullptr, 0);
	pDeviceContext->GSSetShader(pShaderManager->GetGeometryShader(m_CubeMapGeometryShaderIndex), nullptr, 0);
	pDeviceContext->PSSetShader(pShaderManager->GetPixelShader(m_CubeMapPixelShaderIndex), nullptr, 0);

	VertexLayoutSetup();
	DepthStencilStateSetup();
	ConstantBufferSetup();
	pFbxFileManager->GetFbxModel(m_MountainModelIndex)->Draw();
}


//----------------------------------------------------------------------
// Private Functions
//----------------------------------------------------------------------
bool Mountain::CreateTask()
{
	m_pDrawTask->SetName("Mountain");
	m_pUpdateTask->SetName("Mountain");
	m_pDepthDrawTask->SetName("Mountain");
	m_pMapDrawTask->SetName("Mountain");
	m_pCubeMapDrawTask->SetName("Mountain");

	SINGLETON_INSTANCE(Lib::Draw3DTaskManager)->AddTask(m_pDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->AddTask(m_pUpdateTask);
	SINGLETON_INSTANCE(DepthDrawTaskManager)->AddTask(m_pDepthDrawTask);
	SINGLETON_INSTANCE(MapDrawTaskManager)->AddTask(m_pMapDrawTask);
	SINGLETON_INSTANCE(CubeMapDrawTaskManager)->AddTask(m_pCubeMapDrawTask);

	return true;
}

bool Mountain::CreateModel()
{
	if (!SINGLETON_INSTANCE(Lib::Dx11::FbxFileManager)->LoadFbxModel(
		TEXT("Resource\\Model\\mountain.fbx"),
		&m_MountainModelIndex))
	{
		OutputErrorLog("モデルの読み込みに失敗しました");
		return false;
	}

	return true;
}

bool Mountain::CreateDefaultShader()
{
	if (!SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadVertexShader(
		TEXT("Resource\\Effect\\MountainEffect.fx"),
		"VS",
		&m_VertexShaderIndex))
	{
		OutputErrorLog("頂点シェーダの読み込みに失敗しました");
		return false;
	}

	if (!SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadPixelShader(
		TEXT("Resource\\Effect\\MountainEffect.fx"),
		"PS",
		&m_PixelShaderIndex))
	{
		OutputErrorLog("ピクセルシェーダの読み込みに失敗しました");
		return false;
	}

	return true;
}

bool Mountain::CreateShadowShader()
{
	if (!SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadVertexShader(
		TEXT("Resource\\Effect\\DepthShadow.fx"), 
		"VS", 
		&m_ShadowVertexShaderIndex))
	{
		OutputErrorLog("深度シャドウ頂点シェーダの読み込みに失敗しました");
		return false;
	}

	if(!SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadPixelShader(
		TEXT("Resource\\Effect\\DepthShadow.fx"), 
		"PS", 
		&m_ShadowPixelShaderIndex))
	{
		OutputErrorLog("深度シャドウピクセルシェーダの読み込みに失敗しました");
		return false;
	}

	return true;
}

bool Mountain::CreateMapShader()
{
	if (!SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadVertexShader(
		TEXT("Resource\\Effect\\MiniMap.fx"), 
		"VS",
		&m_MapVertexShaderIndex))
	{
		OutputErrorLog("ミニマップ頂点シェーダの読み込みに失敗しました");
		return false;
	}

	if(!SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadPixelShader(
		TEXT("Resource\\Effect\\MiniMap.fx"), 
		"PS",
		&m_MapPixelShaderIndex))
	{
		OutputErrorLog("ミニマップピクセルシェーダの読み込みに失敗しました");
		return false;
	}

	return true;
}

bool Mountain::CreateCubeMapShader()
{
	if (!SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadVertexShader(
		TEXT("Resource\\Effect\\CubeMap.fx"),
		"VS",
		&m_CubeMapVertexShaderIndex))
	{
		OutputErrorLog("キューブマップ頂点シェーダの読み込みに失敗しました");
		return false;
	}

	if (!SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadGeometryShader(
		TEXT("Resource\\Effect\\CubeMap.fx"),
		"GS",
		&m_CubeMapGeometryShaderIndex))
	{
		OutputErrorLog("キューブマップジオメトリシェーダの読み込みに失敗しました");
		return false;
	}

	if (!SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadPixelShader(
		TEXT("Resource\\Effect\\CubeMap.fx"),
		"PS",
		&m_CubeMapPixelShaderIndex))
	{
		OutputErrorLog("キューブマップピクセルシェーダの読み込みに失敗しました");
		return false;
	}

	return true;
}

void Mountain::ReleaseTask()
{
	SINGLETON_INSTANCE(MapDrawTaskManager)->RemoveTask(m_pMapDrawTask);
	SINGLETON_INSTANCE(DepthDrawTaskManager)->RemoveTask(m_pDepthDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->RemoveTask(m_pUpdateTask);
	SINGLETON_INSTANCE(Lib::Draw3DTaskManager)->RemoveTask(m_pDrawTask);
}

void Mountain::ReleaseModel()
{
	SINGLETON_INSTANCE(Lib::Dx11::FbxFileManager)->ReleaseFbxModel(m_MountainModelIndex);
}

void Mountain::ReleaseDefaultShader()
{
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleaseVertexShader(m_VertexShaderIndex);
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleasePixelShader(m_PixelShaderIndex);
}

void Mountain::ReleaseShadowShader()
{
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleasePixelShader(m_ShadowPixelShaderIndex);
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleaseVertexShader(m_ShadowVertexShaderIndex);
}

void Mountain::ReleaseMapShader()
{
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleasePixelShader(m_MapPixelShaderIndex);
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleaseVertexShader(m_MapVertexShaderIndex);
}

void Mountain::ReleaseCubeMapShader()
{
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleasePixelShader(m_CubeMapPixelShaderIndex);
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleaseGeometryShader(m_CubeMapGeometryShaderIndex);
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleaseVertexShader(m_CubeMapVertexShaderIndex);
}

