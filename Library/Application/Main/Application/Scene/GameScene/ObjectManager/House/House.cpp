#include "House.h"

#include "Debugger\Debugger.h"
#include "DirectX11\FbxFileManager\FbxFileManager.h"
#include "DirectX11\GraphicsDevice\GraphicsDevice.h"
#include "DirectX11\ShaderManager\ShaderManager.h"


//----------------------------------------------------------------------
// Private Static Variables
//----------------------------------------------------------------------
D3DXVECTOR3 House::m_DefaultScale = D3DXVECTOR3(50, 50, 50);
int	House::m_ModelIndex = Lib::FbxFileManager::m_InvalidIndex;
int	House::m_ShadowVertexShaderIndex = Lib::ShaderManager::m_InvalidIndex;
int	House::m_ShadowPixelShaderIndex = Lib::ShaderManager::m_InvalidIndex;
int	House::m_MapVertexShaderIndex = Lib::ShaderManager::m_InvalidIndex;
int	House::m_MapPixelShaderIndex = Lib::ShaderManager::m_InvalidIndex;


House::House(D3DXVECTOR3 _Pos, float _rotate)
{
	m_Pos = _Pos;
	m_Scale = m_DefaultScale;
	m_Rotate.y = static_cast<float>(D3DXToRadian(_rotate));
}

House::~House()
{
}

bool House::Initialize()
{
	SINGLETON_INSTANCE(Lib::DrawTaskManager)->AddTask(m_pDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->AddTask(m_pUpdateTask);
	SINGLETON_INSTANCE(DepthDrawTaskManager)->AddTask(m_pDepthDrawTask);
	SINGLETON_INSTANCE(MapDrawTaskManager)->AddTask(m_pMapDrawTask);

	if (m_ModelIndex == Lib::FbxFileManager::m_InvalidIndex)
	{
		SINGLETON_INSTANCE(Lib::FbxFileManager)->LoadFbxModel(TEXT("Resource\\Model\\house_red.fbx"), &m_ModelIndex);
	}

	if (m_ShadowVertexShaderIndex == Lib::ShaderManager::m_InvalidIndex)
	{
		SINGLETON_INSTANCE(Lib::ShaderManager)->LoadVertexShader(TEXT("Resource\\Effect\\DepthShadow.fx"), "VS", &m_ShadowVertexShaderIndex);
	}

	if (m_ShadowPixelShaderIndex == Lib::ShaderManager::m_InvalidIndex)
	{
		SINGLETON_INSTANCE(Lib::ShaderManager)->LoadPixelShader(TEXT("Resource\\Effect\\DepthShadow.fx"), "PS", &m_ShadowPixelShaderIndex);
	}

	if (m_MapVertexShaderIndex == Lib::ShaderManager::m_InvalidIndex)
	{
		SINGLETON_INSTANCE(Lib::ShaderManager)->LoadVertexShader(TEXT("Resource\\Effect\\MiniMap.fx"), "VS", &m_MapVertexShaderIndex);
	}

	if (m_MapPixelShaderIndex == Lib::ShaderManager::m_InvalidIndex)
	{
		SINGLETON_INSTANCE(Lib::ShaderManager)->LoadPixelShader(TEXT("Resource\\Effect\\MiniMap.fx"), "PS", &m_MapPixelShaderIndex);
	}


	if (!CreateShader())
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

void House::Finalize()
{
	ReleaseConstantBuffer();
	ReleaseDepthStencilState();
	ReleaseVertexLayout();
	ReleaseShader();

	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleasePixelShader(m_MapPixelShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleaseVertexShader(m_MapVertexShaderIndex);

	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleasePixelShader(m_ShadowPixelShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleaseVertexShader(m_ShadowVertexShaderIndex);

	SINGLETON_INSTANCE(Lib::FbxFileManager)->ReleaseFbxModel(m_ModelIndex);

	SINGLETON_INSTANCE(MapDrawTaskManager)->RemoveTask(m_pMapDrawTask);
	SINGLETON_INSTANCE(DepthDrawTaskManager)->RemoveTask(m_pDepthDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->RemoveTask(m_pUpdateTask);
	SINGLETON_INSTANCE(Lib::DrawTaskManager)->RemoveTask(m_pDrawTask);
}

void House::Update()
{
}

void House::Draw()
{
	ShaderSetup();
	VertexLayoutSetup();
	DepthStencilStateSetup();
	ConstantBufferSetup();

	SINGLETON_INSTANCE(Lib::FbxFileManager)->GetFbxModel(m_ModelIndex)->Draw();
}

void House::DepthDraw()
{
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
	ConstantBufferSetup();
	SINGLETON_INSTANCE(Lib::FbxFileManager)->GetFbxModel(m_ModelIndex)->Draw();
}

void House::MapDraw()
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
	ConstantBufferSetup();
	SINGLETON_INSTANCE(Lib::FbxFileManager)->GetFbxModel(m_ModelIndex)->Draw();
}
