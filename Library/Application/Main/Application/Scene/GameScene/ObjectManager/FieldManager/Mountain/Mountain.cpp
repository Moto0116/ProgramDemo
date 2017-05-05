/**
 * @file	Mountain.cpp
 * @brief	山の管理クラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "Mountain.h"

#include "Debugger\Debugger.h"
#include "DirectX11\FbxFileManager\FbxFileManager.h"
#include "DirectX11\GraphicsDevice\GraphicsDevice.h"
#include "DirectX11\ShaderManager\ShaderManager.h"


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
	SINGLETON_INSTANCE(Lib::DrawTaskManager)->AddTask(m_pDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->AddTask(m_pUpdateTask);
	SINGLETON_INSTANCE(DepthDrawTaskManager)->AddTask(m_pDepthDrawTask);
	SINGLETON_INSTANCE(MapDrawTaskManager)->AddTask(m_pMapDrawTask);
	SINGLETON_INSTANCE(CubeMapDrawTaskManager)->AddTask(m_pCubeMapDrawTask);

	SINGLETON_INSTANCE(Lib::FbxFileManager)->LoadFbxModel(TEXT("Resource\\Model\\mountain.fbx"), &m_MountainModelIndex);

	SINGLETON_INSTANCE(Lib::ShaderManager)->LoadVertexShader(
		TEXT("Resource\\Effect\\MountainEffect.fx"),
		"VS",
		&m_VertexShaderIndex);

	SINGLETON_INSTANCE(Lib::ShaderManager)->LoadPixelShader(
		TEXT("Resource\\Effect\\MountainEffect.fx"),
		"PS",
		&m_PixelShaderIndex);


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

	if (!CreateTexture())
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

void Mountain::Finalize()
{
	ReleaseConstantBuffer();
	ReleaseDepthStencilState();
	ReleaseVertexLayout();
	ReleaseTexture();
	ReleaseCubeMapShader();
	ReleaseMapShader();
	ReleaseShadowShader();

	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleaseVertexShader(m_VertexShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleasePixelShader(m_PixelShaderIndex);

	SINGLETON_INSTANCE(Lib::FbxFileManager)->ReleaseFbxModel(m_MountainModelIndex);

	SINGLETON_INSTANCE(CubeMapDrawTaskManager)->RemoveTask(m_pCubeMapDrawTask);
	SINGLETON_INSTANCE(MapDrawTaskManager)->RemoveTask(m_pMapDrawTask);
	SINGLETON_INSTANCE(DepthDrawTaskManager)->RemoveTask(m_pDepthDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->RemoveTask(m_pUpdateTask);
	SINGLETON_INSTANCE(Lib::DrawTaskManager)->RemoveTask(m_pDrawTask);
}

void Mountain::Update()
{
}

void Mountain::Draw()
{
	ShaderSetup();
	TextureSetup();
	VertexLayoutSetup();
	DepthStencilStateSetup();
	ConstantBufferSetup();

	SINGLETON_INSTANCE(Lib::FbxFileManager)->GetFbxModel(m_MountainModelIndex)->Draw();
}

void Mountain::DepthDraw()
{
}

void Mountain::MapDraw()
{
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->VSSetShader(
		SINGLETON_INSTANCE(Lib::ShaderManager)->GetVertexShader(m_MapVertexShaderIndex),
		NULL,
		0);

	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->PSSetShader(
		SINGLETON_INSTANCE(Lib::ShaderManager)->GetPixelShader(m_MapPixelShaderIndex),
		NULL,
		0);

	VertexLayoutSetup();
	DepthStencilStateSetup();
	WriteConstantBuffer();
	ConstantBufferSetup();
	SINGLETON_INSTANCE(Lib::FbxFileManager)->GetFbxModel(m_MountainModelIndex)->Draw();
}

void Mountain::CubeMapDraw()
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

	VertexLayoutSetup();
	DepthStencilStateSetup();
	ConstantBufferSetup();
	SINGLETON_INSTANCE(Lib::FbxFileManager)->GetFbxModel(m_MountainModelIndex)->Draw();
}


//----------------------------------------------------------------------
// Private Functions
//----------------------------------------------------------------------
bool Mountain::CreateShadowShader()
{
	if (!SINGLETON_INSTANCE(Lib::ShaderManager)->LoadVertexShader(
		TEXT("Resource\\Effect\\DepthShadow.fx"), 
		"VS", 
		&m_ShadowVertexShaderIndex))
	{
		return false;
	}

	if(!SINGLETON_INSTANCE(Lib::ShaderManager)->LoadPixelShader(
		TEXT("Resource\\Effect\\DepthShadow.fx"), 
		"PS", 
		&m_ShadowPixelShaderIndex))
	{
		return false;
	}

	return true;
}

bool Mountain::CreateMapShader()
{
	if (!SINGLETON_INSTANCE(Lib::ShaderManager)->LoadVertexShader(
		TEXT("Resource\\Effect\\MiniMap.fx"), 
		"VS",
		&m_MapVertexShaderIndex))
	{
		return false;
	}

	if(!SINGLETON_INSTANCE(Lib::ShaderManager)->LoadPixelShader(
		TEXT("Resource\\Effect\\MiniMap.fx"), 
		"PS",
		&m_MapPixelShaderIndex))
	{
		return false;
	}

	return true;
}

bool Mountain::CreateCubeMapShader()
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

void Mountain::ReleaseShadowShader()
{
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleasePixelShader(m_ShadowPixelShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleaseVertexShader(m_ShadowVertexShaderIndex);
}

void Mountain::ReleaseMapShader()
{
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleasePixelShader(m_MapPixelShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleaseVertexShader(m_MapVertexShaderIndex);
}

void Mountain::ReleaseCubeMapShader()
{
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleasePixelShader(m_CubeMapPixelShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleaseGeometryShader(m_CubeMapGeometryShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleaseVertexShader(m_CubeMapVertexShaderIndex);
}


