#include "MainLight.h"

#include "Debugger\Debugger.h"
#include "DirectX11\GraphicsDevice\GraphicsDevice.h"

const D3DXVECTOR3 MainLight::m_DefaultLightPos = D3DXVECTOR3(120, 100, 140);
const D3DXVECTOR3 MainLight::m_DefaultLightDirPos = D3DXVECTOR3(40, 0, 40);
const float MainLight::m_NearPoint = 1.f;
const float MainLight::m_FarPoint = 700.f;
const float MainLight::m_ViewAngle = 50.f;
const float MainLight::m_ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };


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
	m_pVertex2D = new Lib::Vertex2D();
	m_pVertex2D->Initialize(SINGLETON_INSTANCE(Lib::GraphicsDevice));
	m_pVertex2D->CreateVertexBuffer(&D3DXVECTOR2(300, 300));

	m_pDrawTask = new Lib::DrawTask();
	m_pUpdateTask = new Lib::UpdateTask();

	m_pDrawTask->SetDrawObject(this);
	m_pDrawTask->SetPriority(-1);
	m_pUpdateTask->SetUpdateObject(this);

	SINGLETON_INSTANCE(Lib::DrawTaskManager)->AddTask(m_pDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->AddTask(m_pUpdateTask);

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

	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->RemoveTask(m_pUpdateTask);
	SINGLETON_INSTANCE(Lib::DrawTaskManager)->RemoveTask(m_pDrawTask);

	delete m_pUpdateTask;
	delete m_pDrawTask;
}

void MainLight::Update()
{
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->ClearRenderTargetView(m_pRenderTarget, m_ClearColor);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->OMSetRenderTargets(1, &m_pRenderTarget, m_pDepthStencilView);

	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->VSSetConstantBuffers(2, 1, &m_pConstantBuffer);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->PSSetConstantBuffers(2, 1, &m_pConstantBuffer);
}

void MainLight::Draw()
{
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->PSSetShaderResources(2, 1, &m_pShaderResourceView);

	m_pVertex2D->WriteConstantBuffer(&D3DXVECTOR2(200, 200));
	m_pVertex2D->SetTexture(m_pShaderResourceView);
	m_pVertex2D->Draw();
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
	const RECT* pWindowRect = SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetMainWindowRect();
	UINT WindowWidth = pWindowRect->right - pWindowRect->left;
	UINT WindowHeight = pWindowRect->bottom - pWindowRect->top;

	D3D11_TEXTURE2D_DESC DepthTextureDesc;
	ZeroMemory(&DepthTextureDesc, sizeof(DepthTextureDesc));
	DepthTextureDesc.Width = WindowWidth;
	DepthTextureDesc.Height = WindowHeight;
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
	DepthStencilDesc.Width = WindowWidth;
	DepthStencilDesc.Height = WindowHeight;
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

	return true;
}

void MainLight::ReleaseConstantBuffer()
{
	if (m_pConstantBuffer != NULL)
	{
		m_pConstantBuffer->Release();
		m_pConstantBuffer = NULL;
	}
}

void MainLight::ReleaseDepthTexture()
{
	if (m_pDepthStencilView != NULL)
	{
		m_pDepthStencilView->Release();
		m_pDepthStencilView = NULL;
	}

	if (m_pDepthStencilTexture != NULL)
	{
		m_pDepthStencilTexture->Release();
		m_pDepthStencilTexture = NULL;
	}

	if (m_pShaderResourceView != NULL)
	{
		m_pShaderResourceView->Release();
		m_pShaderResourceView = NULL;
	}

	if (m_pRenderTarget != NULL)
	{
		m_pRenderTarget->Release();
		m_pRenderTarget = NULL;
	}

	if (m_pDepthTexture != NULL)
	{
		m_pDepthTexture->Release();
		m_pDepthTexture = NULL;
	}
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

