#include "MainLight.h"

#include "Debugger\Debugger.h"
#include "DirectX11\GraphicsDevice\GraphicsDevice.h"
#include "Main\Application\Scene\GameScene\DepthDrawTask\DepthDrawTask.h"


const D3DXVECTOR3 MainLight::m_DefaultLightPos = D3DXVECTOR3(220, 190, 240);
const D3DXVECTOR3 MainLight::m_DefaultLightDirPos = D3DXVECTOR3(40, 0, 40);
const float MainLight::m_NearPoint = 1.f;
const float MainLight::m_FarPoint = 700.f;
const float MainLight::m_ViewAngle = 50.f;
const float MainLight::m_ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
const float MainLight::m_DepthTextureWidth = 3200.f;
const float MainLight::m_DepthTextureHeight = 1800.f;
const int MainLight::m_RenderTargetStage = 1;


MainLight::MainLight() : 
	m_pLight(NULL),
	m_pDepthTexture(NULL),
	m_pRenderTarget(NULL)
{
}

MainLight::~MainLight()
{
}

bool MainLight::Initialize()
{
	m_pDrawTask = new Lib::DrawTask();
	m_pUpdateTask = new Lib::UpdateTask();
	m_pBeginTask = new BeginTask(this);

	m_pDrawTask->SetDrawObject(this);
	m_pDrawTask->SetPriority(-1);
	m_pUpdateTask->SetUpdateObject(this);

	SINGLETON_INSTANCE(Lib::DrawTaskManager)->AddTask(m_pDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->AddTask(m_pUpdateTask);
	SINGLETON_INSTANCE(DepthDrawTaskManager)->AddBeginTask(m_pBeginTask);

	m_pLight = new Lib::Light();
	m_pLight->SetPos(&m_DefaultLightPos);
	D3DXVec3Normalize(&m_LightDir, &m_DefaultLightDirPos);
	m_pLight->SetDirectionPos(&m_LightDir);

	// ライト視点の行列を生成
	D3DXMatrixLookAtLH(&m_LightView, &m_DefaultLightPos, &m_DefaultLightDirPos, &D3DXVECTOR3(0, 1, 0));

	const RECT* pWindowRect = SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetMainWindowRect();
	float WindowWidth = static_cast<float>(pWindowRect->right - pWindowRect->left);
	float WindowHeight = static_cast<float>(pWindowRect->bottom - pWindowRect->top);
	float Aspect = WindowWidth / WindowHeight;
	D3DXMatrixPerspectiveFovLH(&m_LightProj, static_cast<float>(D3DXToRadian(m_ViewAngle)), Aspect, m_NearPoint, m_FarPoint);

	CreateConstantBuffer();
	WriteConstantBuffer();
	CreateDepthTexture();

	return true;
}

void MainLight::Finalize()
{
	ReleaseDepthTexture();
	ReleaseConstantBuffer();

	delete m_pLight;

	SINGLETON_INSTANCE(DepthDrawTaskManager)->RemoveBeginTask(m_pBeginTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->RemoveTask(m_pUpdateTask);
	SINGLETON_INSTANCE(Lib::DrawTaskManager)->RemoveTask(m_pDrawTask);

	delete m_pBeginTask;
	delete m_pUpdateTask;
	delete m_pDrawTask;
}

void MainLight::Update()
{
}

void MainLight::Draw()
{
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->PSSetShaderResources(2, 1, &m_pShaderResourceView);
}

bool MainLight::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC ConstantBufferDesc;
	ConstantBufferDesc.ByteWidth = sizeof(LIGHT_CONSTANT_BUFFER);
	ConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ConstantBufferDesc.MiscFlags = 0;
	ConstantBufferDesc.StructureByteStride = 0;

	if (FAILED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDevice()->CreateBuffer(&ConstantBufferDesc, NULL, &m_pConstantBuffer)))
	{
		OutputErrorLog("定数バッファ生成に失敗しました");
		return false;
	}

	return true;
}

bool MainLight::CreateDepthTexture()
{
	D3D11_TEXTURE2D_DESC DepthTextureDesc;
	ZeroMemory(&DepthTextureDesc, sizeof(DepthTextureDesc));
	DepthTextureDesc.Width = static_cast<UINT>(m_DepthTextureWidth);
	DepthTextureDesc.Height = static_cast<UINT>(m_DepthTextureHeight);
	DepthTextureDesc.MipLevels = 1;
	DepthTextureDesc.ArraySize = 1;
	DepthTextureDesc.Format = DXGI_FORMAT_R32_FLOAT;
	DepthTextureDesc.SampleDesc.Count = 1;
	DepthTextureDesc.SampleDesc.Quality = 0;
	DepthTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	DepthTextureDesc.CPUAccessFlags = 0;
	DepthTextureDesc.MiscFlags = 0;

	if (FAILED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDevice()->CreateTexture2D(
		&DepthTextureDesc,
		NULL,
		&m_pDepthTexture)))
	{
		OutputErrorLog("Z値テクスチャ生成に失敗しました");
		return false;
	}

	if (FAILED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDevice()->CreateRenderTargetView(
		m_pDepthTexture, 
		NULL,
		&m_pRenderTarget)))
	{
		OutputErrorLog("Z値テクスチャから描画ターゲットの生成に失敗しました");
		return false;
	}

	if (FAILED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDevice()->CreateShaderResourceView(
		m_pDepthTexture, 
		NULL, 
		&m_pShaderResourceView)))
	{
		OutputErrorLog("シェーダーリソースビューの生成に失敗しました");
		return false;
	}


	D3D11_TEXTURE2D_DESC DepthStencilDesc;
	DepthStencilDesc.Width = static_cast<UINT>(m_DepthTextureWidth);
	DepthStencilDesc.Height = static_cast<UINT>(m_DepthTextureHeight);
	DepthStencilDesc.MipLevels = 1;
	DepthStencilDesc.ArraySize = 1;
	DepthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	DepthStencilDesc.SampleDesc.Count = 1;
	DepthStencilDesc.SampleDesc.Quality = 0;
	DepthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DepthStencilDesc.CPUAccessFlags = 0;
	DepthStencilDesc.MiscFlags = 0;

	if (FAILED(FAILED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDevice()->CreateTexture2D(
		&DepthStencilDesc,
		NULL,
		&m_pDepthStencilTexture))))
	{
		OutputErrorLog("深度ステンシルテクスチャ生成に失敗しました");
		return false;
	}

	if (FAILED(FAILED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDevice()->CreateDepthStencilView(
		m_pDepthStencilTexture,
		NULL,
		&m_pDepthStencilView))))
	{
		OutputErrorLog("深度ステンシルテクスチャのデプスステンシルビューの生成に失敗しました");
		return false;
	}

	// ビューポート設定
	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;
	m_ViewPort.Width = m_DepthTextureWidth;
	m_ViewPort.Height = m_DepthTextureHeight;
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;


	SINGLETON_INSTANCE(Lib::GraphicsDevice)->SetRenderTarget(&m_pRenderTarget, m_RenderTargetStage);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->SetDepthStencil(&m_pDepthStencilView, m_RenderTargetStage);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->SetClearColor(m_ClearColor, m_RenderTargetStage);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->SetViewPort(&m_ViewPort, m_RenderTargetStage);

	return true;
}

void MainLight::ReleaseConstantBuffer()
{
	SafeRelease(m_pConstantBuffer);
}

void MainLight::ReleaseDepthTexture()
{
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->SetViewPort(NULL, m_RenderTargetStage);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->SetClearColor(0xffffffff, m_RenderTargetStage);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->SetDepthStencil(NULL, m_RenderTargetStage);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->SetRenderTarget(NULL, m_RenderTargetStage);

	SafeRelease(m_pDepthStencilView);
	SafeRelease(m_pDepthStencilTexture);
	SafeRelease(m_pShaderResourceView);
	SafeRelease(m_pRenderTarget);
	SafeRelease(m_pDepthTexture);
}

void MainLight::WriteConstantBuffer()
{
	D3D11_MAPPED_SUBRESOURCE SubResourceData;
	if (SUCCEEDED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResourceData)))
	{
		D3DXVECTOR3 LightPos = m_pLight->GetPos();
		D3DXVECTOR3 LightDir = m_pLight->GetDirectionPos();
		D3DXMATRIX LightView = m_LightView;
		D3DXMatrixTranspose(&LightView, &LightView);
		D3DXMATRIX LightProj = m_LightProj;
		D3DXMatrixTranspose(&LightProj, &LightProj);

		// 定数バッファは16バイト刻みじゃないといけない
		LIGHT_CONSTANT_BUFFER ConstantBuffer;
		ConstantBuffer.LightPos = D3DXVECTOR4(LightPos.x, LightPos.y, LightPos.z, 1.0);
		ConstantBuffer.LightDir = D3DXVECTOR4(LightDir.x, LightDir.y, LightDir.z, 1.0);
		ConstantBuffer.LightView = LightView;
		ConstantBuffer.LightProj = LightProj;

		memcpy_s(
			SubResourceData.pData,
			SubResourceData.RowPitch,
			reinterpret_cast<void*>(&ConstantBuffer),
			sizeof(ConstantBuffer));

		SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->Unmap(m_pConstantBuffer, 0);
	}
}

void MainLight::MainLightBeginScene()
{
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->BeginScene(m_RenderTargetStage);

	WriteConstantBuffer();

	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->VSSetConstantBuffers(2, 1, &m_pConstantBuffer);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->PSSetConstantBuffers(2, 1, &m_pConstantBuffer);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->GSSetConstantBuffers(2, 1, &m_pConstantBuffer);
}



//----------------------------------------------------------------------
// Inner Class Constructor Destructor
//----------------------------------------------------------------------
MainLight::BeginTask::BeginTask(MainLight* _pMainLight) :
	m_pMainLight(_pMainLight)
{
}

MainLight::BeginTask::~BeginTask()
{
}


//----------------------------------------------------------------------
// Inner Class Public Function
//----------------------------------------------------------------------
void MainLight::BeginTask::Run()
{
	m_pMainLight->MainLightBeginScene();
}

