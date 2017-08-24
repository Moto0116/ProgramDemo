/**
 * @file	Groundcpp
 * @brief	地面の管理クラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "Ground.h"

#include "Debugger\Debugger.h"
#include "DirectX11\FbxFileManager\FbxFileManager.h"
#include "DirectX11\GraphicsDevice\GraphicsDevice.h"
#include "DirectX11\ShaderManager\ShaderManager.h"


//----------------------------------------------------------------------
// Private Static Variables
//----------------------------------------------------------------------
D3DXVECTOR3 Ground::m_DefaultScale = D3DXVECTOR3(3.5, 3.5, 3.5);


//----------------------------------------------------------------------
// Constructor	Destructor
//----------------------------------------------------------------------
Ground::Ground()
{
	m_Scale = m_DefaultScale;
}

Ground::~Ground()
{
}


//----------------------------------------------------------------------
// Public Functions
//----------------------------------------------------------------------
bool Ground::Initialize()
{
	if (!CreateTask())				return false;
	if (!CreateModel())				return false;
	if (!CreateDefaultShader())		return false;
	if (!CreateShadowShader())		return false;
	if (!CreateMapShader())			return false;
	if (!CreateTexture())			return false;
	if (!CreateVertexLayout())		return false;
	if (!CreateDepthStencilState())	return false;
	if (!CreateConstantBuffer())	return false;
	if (!WriteConstantBuffer())		return false;

	return true;
}

void Ground::Finalize()
{
	ReleaseConstantBuffer();
	ReleaseDepthStencilState();
	ReleaseVertexLayout();
	ReleaseTexture();
	ReleaseMapShader();
	ReleaseShadowShader();
	ReleaseDefaultShader();
	ReleaseModel();
	ReleaseTask();
}

void Ground::Update()
{
}

void Ground::Draw()
{
	Lib::Dx11::FbxFileManager* pFbxFileManager = SINGLETON_INSTANCE(Lib::Dx11::FbxFileManager);

	ShaderSetup();
	TextureSetup();
	VertexLayoutSetup();
	DepthStencilStateSetup();
	ConstantBufferSetup();

	pFbxFileManager->GetFbxModel(m_GroundModelIndex)->Draw();
}

void Ground::DepthDraw()
{
}

void Ground::MapDraw()
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
	pFbxFileManager->GetFbxModel(m_GroundModelIndex)->Draw();
}

bool Ground::CreateTask()
{
	SINGLETON_INSTANCE(Lib::DrawTaskManager)->AddTask(m_pDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->AddTask(m_pUpdateTask);
	SINGLETON_INSTANCE(DepthDrawTaskManager)->AddTask(m_pDepthDrawTask);
	SINGLETON_INSTANCE(MapDrawTaskManager)->AddTask(m_pMapDrawTask);

	return true;
}

bool Ground::CreateModel()
{
	if(!SINGLETON_INSTANCE(Lib::Dx11::FbxFileManager)->LoadFbxModel(
		TEXT("Resource\\Model\\map.fbx"),
		&m_GroundModelIndex))
	{
		OutputErrorLog("モデルの読み込みに失敗しました");
		return false;
	}

	return true;
}

bool Ground::CreateDefaultShader()
{
	if (!SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadVertexShader(
		TEXT("Resource\\Effect\\GroundEffect.fx"),
		"VS",
		&m_VertexShaderIndex))
	{
		OutputErrorLog("デフォルト頂点シェーダの読み込みに失敗しました");
		return false;
	}

	if (!SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadPixelShader(
		TEXT("Resource\\Effect\\GroundEffect.fx"),
		"PS",
		&m_PixelShaderIndex))
	{
		OutputErrorLog("デフォルトピクセルシェーダの読み込みに失敗しました");
		return false;
	}

	return true;
}

bool Ground::CreateShadowShader()
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

bool Ground::CreateMapShader()
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

void Ground::ReleaseTask()
{
	SINGLETON_INSTANCE(MapDrawTaskManager)->RemoveTask(m_pMapDrawTask);
	SINGLETON_INSTANCE(DepthDrawTaskManager)->RemoveTask(m_pDepthDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->RemoveTask(m_pUpdateTask);
	SINGLETON_INSTANCE(Lib::DrawTaskManager)->RemoveTask(m_pDrawTask);
}

void Ground::ReleaseModel()
{
	SINGLETON_INSTANCE(Lib::Dx11::FbxFileManager)->ReleaseFbxModel(m_GroundModelIndex);
}

void Ground::ReleaseDefaultShader()
{
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleaseVertexShader(m_VertexShaderIndex);
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleasePixelShader(m_PixelShaderIndex);
}

void Ground::ReleaseShadowShader()
{
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleasePixelShader(m_ShadowPixelShaderIndex);
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleaseVertexShader(m_ShadowVertexShaderIndex);
}

void Ground::ReleaseMapShader()
{
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleasePixelShader(m_MapPixelShaderIndex);
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleaseVertexShader(m_MapVertexShaderIndex);
}

