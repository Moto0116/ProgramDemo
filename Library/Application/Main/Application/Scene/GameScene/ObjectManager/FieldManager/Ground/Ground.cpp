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
	SINGLETON_INSTANCE(Lib::DrawTaskManager)->AddTask(m_pDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->AddTask(m_pUpdateTask);
	SINGLETON_INSTANCE(DepthDrawTaskManager)->AddTask(m_pDepthDrawTask);
	SINGLETON_INSTANCE(MapDrawTaskManager)->AddTask(m_pMapDrawTask);

	SINGLETON_INSTANCE(Lib::FbxFileManager)->LoadFbxModel(TEXT("Resource\\Model\\map.fbx"), &m_GroundModelIndex);

	SINGLETON_INSTANCE(Lib::ShaderManager)->LoadVertexShader(TEXT("Resource\\Effect\\DepthShadow.fx"), "VS", &m_ShadowVertexShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->LoadPixelShader(TEXT("Resource\\Effect\\DepthShadow.fx"), "PS", &m_ShadowPixelShaderIndex);

	SINGLETON_INSTANCE(Lib::ShaderManager)->LoadVertexShader(TEXT("Resource\\Effect\\MiniMap.fx"), "VS", &m_MapVertexShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->LoadPixelShader(TEXT("Resource\\Effect\\MiniMap.fx"), "PS", &m_MapPixelShaderIndex);


	SINGLETON_INSTANCE(Lib::ShaderManager)->LoadVertexShader(
		TEXT("Resource\\Effect\\GroundEffect.fx"),
		"VS",
		&m_VertexShaderIndex);

	SINGLETON_INSTANCE(Lib::ShaderManager)->LoadPixelShader(
		TEXT("Resource\\Effect\\GroundEffect.fx"),
		"PS",
		&m_PixelShaderIndex);

	
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

void Ground::Finalize()
{
	ReleaseConstantBuffer();
	ReleaseDepthStencilState();
	ReleaseVertexLayout();
	ReleaseTexture();

	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleaseVertexShader(m_VertexShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleasePixelShader(m_PixelShaderIndex);

	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleasePixelShader(m_MapPixelShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleaseVertexShader(m_MapVertexShaderIndex);

	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleasePixelShader(m_ShadowPixelShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleaseVertexShader(m_ShadowVertexShaderIndex);

	SINGLETON_INSTANCE(Lib::FbxFileManager)->ReleaseFbxModel(m_GroundModelIndex);

	SINGLETON_INSTANCE(MapDrawTaskManager)->RemoveTask(m_pMapDrawTask);
	SINGLETON_INSTANCE(DepthDrawTaskManager)->RemoveTask(m_pDepthDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->RemoveTask(m_pUpdateTask);
	SINGLETON_INSTANCE(Lib::DrawTaskManager)->RemoveTask(m_pDrawTask);
}

void Ground::Update()
{
}

void Ground::Draw()
{
	ShaderSetup();
	TextureSetup();
	VertexLayoutSetup();
	DepthStencilStateSetup();
	ConstantBufferSetup();

	SINGLETON_INSTANCE(Lib::FbxFileManager)->GetFbxModel(m_GroundModelIndex)->Draw();
}

void Ground::DepthDraw()
{
}

void Ground::MapDraw()
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
	SINGLETON_INSTANCE(Lib::FbxFileManager)->GetFbxModel(m_GroundModelIndex)->Draw();
}

