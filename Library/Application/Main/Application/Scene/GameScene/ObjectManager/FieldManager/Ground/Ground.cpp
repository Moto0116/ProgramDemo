#include "Ground.h"

#include "Debugger\Debugger.h"
#include "DirectX11\FbxFileManager\FbxFileManager.h"
#include "DirectX11\GraphicsDevice\GraphicsDevice.h"
#include "DirectX11\ShaderManager\ShaderManager.h"


//----------------------------------------------------------------------
// Constructor	Destructor
//----------------------------------------------------------------------
Ground::Ground()
{
}

Ground::~Ground()
{
}

//----------------------------------------------------------------------
// Public Functions
//----------------------------------------------------------------------
bool Ground::Initialize()
{
	m_pDrawTask = new Lib::DrawTask();
	m_pUpdateTask = new Lib::UpdateTask();
	m_pDepthDrawTask = new DepthDrawTask();

	m_pDrawTask->SetDrawObject(this);
	m_pUpdateTask->SetUpdateObject(this);
	m_pDepthDrawTask->SetDrawObject(this);

	SINGLETON_INSTANCE(Lib::DrawTaskManager)->AddTask(m_pDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->AddTask(m_pUpdateTask);
	SINGLETON_INSTANCE(DepthDrawTaskManager)->AddTask(m_pDepthDrawTask);

	SINGLETON_INSTANCE(Lib::FbxFileManager)->LoadFbxModel(TEXT("Resource\\Model\\map.fbx"), &m_GroundModelIndex);
	SINGLETON_INSTANCE(Lib::FbxFileManager)->LoadFbxModel(TEXT("Resource\\Model\\mountain.fbx"), &m_MountainModelIndex);

	SINGLETON_INSTANCE(Lib::ShaderManager)->LoadVertexShader(TEXT("Resource\\Effect\\DepthShadow.fx"), "VS", &m_ShadowVertexShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->LoadPixelShader(TEXT("Resource\\Effect\\DepthShadow.fx"), "PS", &m_ShadowPixelShaderIndex);

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

void Ground::Finalize()
{
	ReleaseShader();
	ReleaseConstantBuffer();
	ReleaseDepthStencilState();
	ReleaseVertexLayout();

	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleasePixelShader(m_ShadowPixelShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleaseVertexShader(m_ShadowVertexShaderIndex);

	SINGLETON_INSTANCE(Lib::FbxFileManager)->ReleaseFbxModel(m_MountainModelIndex);
	SINGLETON_INSTANCE(Lib::FbxFileManager)->ReleaseFbxModel(m_GroundModelIndex);

	SINGLETON_INSTANCE(DepthDrawTaskManager)->RemoveTask(m_pDepthDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->RemoveTask(m_pUpdateTask);
	SINGLETON_INSTANCE(Lib::DrawTaskManager)->RemoveTask(m_pDrawTask);

	delete m_pDepthDrawTask;
	delete m_pUpdateTask;
	delete m_pDrawTask;
}

void Ground::Update()
{
}

void Ground::Draw()
{
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->SetDepthStencilTest(true);

	ShaderSetup();
	VertexLayoutSetup();
	DepthStencilStateSetup();
	ConstantBufferSetup();

	SINGLETON_INSTANCE(Lib::FbxFileManager)->GetFbxModel(m_MountainModelIndex)->Draw();
	SINGLETON_INSTANCE(Lib::FbxFileManager)->GetFbxModel(m_GroundModelIndex)->Draw();
}

void Ground::DepthDraw()
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
	WriteConstantBuffer();
	ConstantBufferSetup();
	SINGLETON_INSTANCE(Lib::FbxFileManager)->GetFbxModel(m_MountainModelIndex)->Draw();
	SINGLETON_INSTANCE(Lib::FbxFileManager)->GetFbxModel(m_GroundModelIndex)->Draw();
}

