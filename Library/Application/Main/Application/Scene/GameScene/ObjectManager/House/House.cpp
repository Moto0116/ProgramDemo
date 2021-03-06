﻿/**
 * @file	House.cpp
 * @brief	ハウスオブジェクト実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "House.h"

#include "Debugger\Debugger.h"
#include "DirectX11\FbxFileManager\Dx11FbxFileManager.h"
#include "DirectX11\GraphicsDevice\Dx11GraphicsDevice.h"
#include "DirectX11\ShaderManager\Dx11ShaderManager.h"
#include "Smoke\Smoke.h"


//----------------------------------------------------------------------
// Private Static Variables
//----------------------------------------------------------------------
D3DXVECTOR3 House::m_DefaultScale = D3DXVECTOR3(50, 50, 50);
int	House::m_ModelIndex = Lib::Dx11::FbxFileManager::m_InvalidIndex;
int	House::m_ShadowVertexShaderIndex = Lib::Dx11::ShaderManager::m_InvalidIndex;
int	House::m_ShadowPixelShaderIndex = Lib::Dx11::ShaderManager::m_InvalidIndex;
int	House::m_MapVertexShaderIndex = Lib::Dx11::ShaderManager::m_InvalidIndex;
int	House::m_MapPixelShaderIndex = Lib::Dx11::ShaderManager::m_InvalidIndex;
int	House::m_CubeMapVertexShaderIndex = Lib::Dx11::ShaderManager::m_InvalidIndex;
int	House::m_CubeMapGeometryShaderIndex = Lib::Dx11::ShaderManager::m_InvalidIndex;
int	House::m_CubeMapPixelShaderIndex = Lib::Dx11::ShaderManager::m_InvalidIndex;
int	House::m_ReflectMapVertexShaderIndex = Lib::Dx11::ShaderManager::m_InvalidIndex;
int	House::m_ReflectMapPixelShaderIndex = Lib::Dx11::ShaderManager::m_InvalidIndex;


//----------------------------------------------------------------------
// Constructor	Destructor
//----------------------------------------------------------------------
House::House(MainCamera* _pCamera, D3DXVECTOR3 _Pos, float _rotate) : 
	m_pSmoke(nullptr)
{
	// 座標と角度を計算.
	D3DXVECTOR3 Pos = D3DXVECTOR3(4.6f, 25, 4.0f);
	D3DXVECTOR3 TempPos = Pos;

	Pos.x =
		TempPos.x * cos(static_cast<float>(D3DXToRadian(-_rotate))) -
		TempPos.z * sin(static_cast<float>(D3DXToRadian(-_rotate)));

	Pos.z =
		TempPos.x * sin(static_cast<float>(D3DXToRadian(-_rotate))) +
		TempPos.z * cos(static_cast<float>(D3DXToRadian(-_rotate)));

	Pos += _Pos;

	m_pSmoke = new Smoke(_pCamera, &Pos);

	m_Pos = _Pos;
	m_Scale = m_DefaultScale;
	m_Rotate.y = static_cast<float>(D3DXToRadian(_rotate));
}

House::~House()
{
	delete m_pSmoke;
}


//----------------------------------------------------------------------
// Public Functions
//----------------------------------------------------------------------
bool House::Initialize()
{
	if (!CreateTask())				return false;
	if (!CreateModel())				return false;
	if (!CreateShader())			return false;
	if (!CreateShadowShader())		return false;
	if (!CreateMapShader())			return false;
	if (!CreateCubeMapShader())		return false;
	if (!CreateReflectMapShader())	return false;
	if (!CreateTexture())			return false;
	if (!CreateVertexLayout())		return false;
	if (!CreateDepthStencilState())	return false;
	if (!CreateConstantBuffer())	return false;
	if (!WriteConstantBuffer())		return false;
	if (!m_pSmoke->Initialize())	return false;

	return true;
}

void House::Finalize()
{
	m_pSmoke->Finalize();
	ReleaseConstantBuffer();
	ReleaseDepthStencilState();
	ReleaseVertexLayout();
	ReleaseTexture();
	ReleaseReflectMapShader();
	ReleaseCubeMapShader();
	ReleaseMapShader();
	ReleaseShadowShader();
	ReleaseShader();
	ReleaseModel();
	ReleaseTask();
}

void House::Draw()
{
	ShaderSetup();
	TextureSetup();
	VertexLayoutSetup();
	DepthStencilStateSetup();
	ConstantBufferSetup();

	SINGLETON_INSTANCE(Lib::Dx11::FbxFileManager)->GetFbxModel(m_ModelIndex)->Draw();
}

void House::DepthDraw()
{
	ID3D11DeviceContext* pDeviceContext = SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDeviceContext();
	Lib::Dx11::ShaderManager* pShaderManager = SINGLETON_INSTANCE(Lib::Dx11::ShaderManager);
	Lib::Dx11::FbxFileManager* pFbxFileManager = SINGLETON_INSTANCE(Lib::Dx11::FbxFileManager);

	// シェーダーの設定.
	pDeviceContext->VSSetShader(pShaderManager->GetVertexShader(m_ShadowVertexShaderIndex), nullptr, 0);
	pDeviceContext->PSSetShader(pShaderManager->GetPixelShader(m_ShadowPixelShaderIndex), nullptr, 0);

	VertexLayoutSetup();
	DepthStencilStateSetup();
	ConstantBufferSetup();
	pFbxFileManager->GetFbxModel(m_ModelIndex)->Draw();
}

void House::MapDraw()
{
	ID3D11DeviceContext* pDeviceContext = SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDeviceContext();
	Lib::Dx11::ShaderManager* pShaderManager = SINGLETON_INSTANCE(Lib::Dx11::ShaderManager);
	Lib::Dx11::FbxFileManager* pFbxFileManager = SINGLETON_INSTANCE(Lib::Dx11::FbxFileManager);

	// シェーダーの設定.
	pDeviceContext->VSSetShader(pShaderManager->GetVertexShader(m_MapVertexShaderIndex), nullptr, 0);
	pDeviceContext->PSSetShader(pShaderManager->GetPixelShader(m_MapPixelShaderIndex), nullptr, 0);

	TextureSetup();
	VertexLayoutSetup();
	DepthStencilStateSetup();
	ConstantBufferSetup();
	pFbxFileManager->GetFbxModel(m_ModelIndex)->Draw();
}

void House::CubeMapDraw()
{
	ID3D11DeviceContext* pDeviceContext = SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDeviceContext();
	Lib::Dx11::ShaderManager* pShaderManager = SINGLETON_INSTANCE(Lib::Dx11::ShaderManager);
	Lib::Dx11::FbxFileManager* pFbxFileManager = SINGLETON_INSTANCE(Lib::Dx11::FbxFileManager);

	// シェーダーの設定.
	pDeviceContext->VSSetShader(pShaderManager->GetVertexShader(m_CubeMapVertexShaderIndex), nullptr, 0);
	pDeviceContext->GSSetShader(pShaderManager->GetGeometryShader(m_CubeMapGeometryShaderIndex), nullptr, 0);
	pDeviceContext->PSSetShader(pShaderManager->GetPixelShader(m_CubeMapPixelShaderIndex), nullptr, 0);

	VertexLayoutSetup();
	DepthStencilStateSetup();
	ConstantBufferSetup();
	pFbxFileManager->GetFbxModel(m_ModelIndex)->Draw();
}

void House::ReflectMapDraw()
{
	ID3D11DeviceContext* pDeviceContext = SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDeviceContext();
	Lib::Dx11::ShaderManager* pShaderManager = SINGLETON_INSTANCE(Lib::Dx11::ShaderManager);
	Lib::Dx11::FbxFileManager* pFbxFileManager = SINGLETON_INSTANCE(Lib::Dx11::FbxFileManager);

	// シェーダーの設定.
	pDeviceContext->VSSetShader(pShaderManager->GetVertexShader(m_ReflectMapVertexShaderIndex), nullptr, 0);
	pDeviceContext->GSSetShader(nullptr, nullptr, 0);
	pDeviceContext->PSSetShader(pShaderManager->GetPixelShader(m_ReflectMapPixelShaderIndex), nullptr, 0);

	VertexLayoutSetup();
	DepthStencilStateSetup();
	ConstantBufferSetup();
	pFbxFileManager->GetFbxModel(m_ModelIndex)->Draw();
}


//----------------------------------------------------------------------
// Private Functions
//----------------------------------------------------------------------
bool House::CreateTask()
{
	m_pUpdateTask->SetName("House");
	m_pDrawTask->SetName("House");
	m_pDepthDrawTask->SetName("House");
	m_pMapDrawTask->SetName("House");
	m_pCubeMapDrawTask->SetName("House");
	m_pReflectMapDrawTask->SetName("House");

	SINGLETON_INSTANCE(Lib::Draw3DTaskManager)->AddTask(m_pDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->AddTask(m_pUpdateTask);
	SINGLETON_INSTANCE(DepthDrawTaskManager)->AddTask(m_pDepthDrawTask);
	SINGLETON_INSTANCE(MapDrawTaskManager)->AddTask(m_pMapDrawTask);
	SINGLETON_INSTANCE(CubeMapDrawTaskManager)->AddTask(m_pCubeMapDrawTask);
	SINGLETON_INSTANCE(ReflectMapDrawTaskManager)->AddTask(m_pReflectMapDrawTask);

	return true;
}

bool House::CreateModel()
{
	if (m_ModelIndex == Lib::Dx11::FbxFileManager::m_InvalidIndex)
	{
		if (!SINGLETON_INSTANCE(Lib::Dx11::FbxFileManager)->LoadFbxModel(
			TEXT("Resource\\Model\\house_red.fbx"),
			&m_ModelIndex))
		{
			OutputErrorLog("モデル読み込みに失敗しました");
			return false;
		}
	}

	return true;
}

bool House::CreateShadowShader()
{
	if (m_ShadowVertexShaderIndex == Lib::Dx11::ShaderManager::m_InvalidIndex)
	{
		if (!SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadVertexShader(
			TEXT("Resource\\Effect\\DepthShadow.fx"),
			"VS",
			&m_ShadowVertexShaderIndex))
		{
			OutputErrorLog("深度シャドウ頂点シェーダーの生成に失敗しました");
			return false;
		}
	}

	if (m_ShadowPixelShaderIndex == Lib::Dx11::ShaderManager::m_InvalidIndex)
	{
		if (!SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadPixelShader(
			TEXT("Resource\\Effect\\DepthShadow.fx"),
			"PS",
			&m_ShadowPixelShaderIndex))
		{
			OutputErrorLog("深度シャドウピクセルシェーダーの生成に失敗しました");
			return false;
		}
	}

	return true;
}

bool House::CreateMapShader()
{
	if (m_MapVertexShaderIndex == Lib::Dx11::ShaderManager::m_InvalidIndex)
	{
		if (!SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadVertexShader(
			TEXT("Resource\\Effect\\MiniMap.fx"),
			"VS",
			&m_MapVertexShaderIndex))
		{
			OutputErrorLog("ミニマップ頂点シェーダーの生成に失敗しました");
			return false;
		}
	}

	if (m_MapPixelShaderIndex == Lib::Dx11::ShaderManager::m_InvalidIndex)
	{
		if (!SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadPixelShader(
			TEXT("Resource\\Effect\\MiniMap.fx"),
			"PS",
			&m_MapPixelShaderIndex))
		{
			OutputErrorLog("ミニマップピクセルシェーダーの生成に失敗しました");
			return false;
		}
	}

	return true;
}

bool House::CreateCubeMapShader()
{
	if (m_CubeMapVertexShaderIndex == Lib::Dx11::ShaderManager::m_InvalidIndex)
	{
		if (!SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadVertexShader(
			TEXT("Resource\\Effect\\CubeMap.fx"),
			"VS",
			&m_CubeMapVertexShaderIndex))
		{
			OutputErrorLog("キューブマップ頂点シェーダーの生成に失敗しました");
			return false;
		}
	}

	if (m_CubeMapGeometryShaderIndex == Lib::Dx11::ShaderManager::m_InvalidIndex)
	{
		if (!SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadGeometryShader(
			TEXT("Resource\\Effect\\CubeMap.fx"),
			"GS",
			&m_CubeMapGeometryShaderIndex))
		{
			OutputErrorLog("キューブマップジオメトリシェーダーの生成に失敗しました");
			return false;
		}
	}

	if (m_CubeMapPixelShaderIndex == Lib::Dx11::ShaderManager::m_InvalidIndex)
	{
		if (!SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadPixelShader(
			TEXT("Resource\\Effect\\CubeMap.fx"),
			"PS",
			&m_CubeMapPixelShaderIndex))
		{
			OutputErrorLog("キューブマップピクセルシェーダーの生成に失敗しました");
			return false;
		}
	}

	return true;
}

bool House::CreateReflectMapShader()
{
	if (m_ReflectMapVertexShaderIndex == Lib::Dx11::ShaderManager::m_InvalidIndex)
	{
		if (!SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadVertexShader(
			TEXT("Resource\\Effect\\ReflectMap.fx"),
			"VS",
			&m_ReflectMapVertexShaderIndex))
		{
			OutputErrorLog("反射マップ頂点シェーダーの生成に失敗しました");
			return false;
		}
	}

	if (m_ReflectMapPixelShaderIndex == Lib::Dx11::ShaderManager::m_InvalidIndex)
	{
		if (!SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadPixelShader(
			TEXT("Resource\\Effect\\ReflectMap.fx"),
			"PS",
			&m_ReflectMapPixelShaderIndex))
		{
			OutputErrorLog("反射マップピクセルシェーダーの生成に失敗しました");
			return false;
		}
	}

	return true;
}

void House::ReleaseTask()
{
	SINGLETON_INSTANCE(ReflectMapDrawTaskManager)->RemoveTask(m_pReflectMapDrawTask);
	SINGLETON_INSTANCE(CubeMapDrawTaskManager)->RemoveTask(m_pCubeMapDrawTask);
	SINGLETON_INSTANCE(MapDrawTaskManager)->RemoveTask(m_pMapDrawTask);
	SINGLETON_INSTANCE(DepthDrawTaskManager)->RemoveTask(m_pDepthDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->RemoveTask(m_pUpdateTask);
	SINGLETON_INSTANCE(Lib::Draw3DTaskManager)->RemoveTask(m_pDrawTask);
}

void House::ReleaseModel()
{
	SINGLETON_INSTANCE(Lib::Dx11::FbxFileManager)->ReleaseFbxModel(m_ModelIndex);
}

void House::ReleaseShadowShader()
{
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleasePixelShader(m_ShadowPixelShaderIndex);
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleaseVertexShader(m_ShadowVertexShaderIndex);
}

void House::ReleaseMapShader()
{
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleasePixelShader(m_MapPixelShaderIndex);
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleaseVertexShader(m_MapVertexShaderIndex);
}

void House::ReleaseCubeMapShader()
{
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleasePixelShader(m_CubeMapPixelShaderIndex);
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleaseGeometryShader(m_CubeMapGeometryShaderIndex);
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleaseVertexShader(m_CubeMapVertexShaderIndex);
}

void House::ReleaseReflectMapShader()
{
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleasePixelShader(m_ReflectMapPixelShaderIndex);
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleaseVertexShader(m_ReflectMapVertexShaderIndex);
}

