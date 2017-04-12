/**
 * @file	Field.cpp
 * @brief	フィールドクラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "Field.h"

#include "Debugger\Debugger.h"
#include "DirectX11\FbxFileManager\FbxFileManager.h"
#include "DirectX11\GraphicsDevice\GraphicsDevice.h"
#include "DirectX11\ShaderManager\ShaderManager.h"


//----------------------------------------------------------------------
// Private Static Variables
//----------------------------------------------------------------------
D3DXVECTOR3 Field::m_DefaultScale = D3DXVECTOR3(3.5, 3.5, 3.5);


//----------------------------------------------------------------------
// Constructor	Destructor
//----------------------------------------------------------------------
Field::Field()
{
	m_Scale = m_DefaultScale;
}

Field::~Field()
{
}


//----------------------------------------------------------------------
// Public Functions
//----------------------------------------------------------------------
bool Field::Initialize()
{
	SINGLETON_INSTANCE(Lib::DrawTaskManager)->AddTask(m_pDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->AddTask(m_pUpdateTask);
	SINGLETON_INSTANCE(DepthDrawTaskManager)->AddTask(m_pDepthDrawTask);
	SINGLETON_INSTANCE(MapDrawTaskManager)->AddTask(m_pMapDrawTask);

	SINGLETON_INSTANCE(Lib::FbxFileManager)->LoadFbxModel(TEXT("Resource\\Model\\map.fbx"), &m_GroundModelIndex);
	SINGLETON_INSTANCE(Lib::FbxFileManager)->LoadFbxModel(TEXT("Resource\\Model\\mountain.fbx"), &m_MountainModelIndex);
	SINGLETON_INSTANCE(Lib::FbxFileManager)->LoadFbxModel(TEXT("Resource\\Model\\sky.fbx"), &m_SkyModelIndex);

	SINGLETON_INSTANCE(Lib::ShaderManager)->LoadVertexShader(TEXT("Resource\\Effect\\DepthShadow.fx"), "VS", &m_ShadowVertexShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->LoadPixelShader(TEXT("Resource\\Effect\\DepthShadow.fx"), "PS", &m_ShadowPixelShaderIndex);

	SINGLETON_INSTANCE(Lib::ShaderManager)->LoadVertexShader(TEXT("Resource\\Effect\\MiniMap.fx"), "VS", &m_MapVertexShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->LoadPixelShader(TEXT("Resource\\Effect\\MiniMap.fx"), "PS", &m_MapPixelShaderIndex);


	if (!CreateShader())
	{
		return false;
	}

	if(!CreateVertexLayout())
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

void Field::Finalize()
{
	ReleaseConstantBuffer();
	ReleaseDepthStencilState();
	ReleaseVertexLayout();
	ReleaseShader();

	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleasePixelShader(m_MapPixelShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleaseVertexShader(m_MapVertexShaderIndex);

	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleasePixelShader(m_ShadowPixelShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleaseVertexShader(m_ShadowVertexShaderIndex);

	SINGLETON_INSTANCE(Lib::FbxFileManager)->ReleaseFbxModel(m_SkyModelIndex);
	SINGLETON_INSTANCE(Lib::FbxFileManager)->ReleaseFbxModel(m_MountainModelIndex);
	SINGLETON_INSTANCE(Lib::FbxFileManager)->ReleaseFbxModel(m_GroundModelIndex);

	SINGLETON_INSTANCE(MapDrawTaskManager)->RemoveTask(m_pMapDrawTask);
	SINGLETON_INSTANCE(DepthDrawTaskManager)->RemoveTask(m_pDepthDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->RemoveTask(m_pUpdateTask);
	SINGLETON_INSTANCE(Lib::DrawTaskManager)->RemoveTask(m_pDrawTask);
}

void Field::Update()
{
}

void Field::Draw()
{
	ShaderSetup();
	VertexLayoutSetup();
	DepthStencilStateSetup();
	ConstantBufferSetup();

	SINGLETON_INSTANCE(Lib::FbxFileManager)->GetFbxModel(m_MountainModelIndex)->Draw();
	SINGLETON_INSTANCE(Lib::FbxFileManager)->GetFbxModel(m_GroundModelIndex)->Draw();
	SINGLETON_INSTANCE(Lib::FbxFileManager)->GetFbxModel(m_SkyModelIndex)->Draw();
}

void Field::DepthDraw()
{
	// 深度値描画シェーダーの設定
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->VSSetShader(
		SINGLETON_INSTANCE(Lib::ShaderManager)->GetVertexShader(m_ShadowVertexShaderIndex),
		NULL,
		0);

	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->PSSetShader(
		SINGLETON_INSTANCE(Lib::ShaderManager)->GetPixelShader(m_ShadowPixelShaderIndex),
		NULL,
		0);

	VertexLayoutSetup();
	DepthStencilStateSetup();
	WriteConstantBuffer();
	ConstantBufferSetup();

	SINGLETON_INSTANCE(Lib::FbxFileManager)->GetFbxModel(m_MountainModelIndex)->Draw();
	SINGLETON_INSTANCE(Lib::FbxFileManager)->GetFbxModel(m_GroundModelIndex)->Draw();
}

void Field::MapDraw()
{
	// マップ描画シェーダーの設定
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
	SINGLETON_INSTANCE(Lib::FbxFileManager)->GetFbxModel(m_GroundModelIndex)->Draw();
}
