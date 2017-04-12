/**
 * @file	MiniMap.cpp
 * @brief	ミニマップクラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "MiniMap.h"

#include "Debugger\Debugger.h"
#include "DirectX11\GraphicsDevice\GraphicsDevice.h"
#include "Main\Application\Scene\GameScene\MapDrawTask\MapDrawTask.h"


//----------------------------------------------------------------------
// Static Private Variables
//----------------------------------------------------------------------
const float MiniMap::m_NearPoint = 1.f;
const float MiniMap::m_FarPoint = 900;
const float MiniMap::m_ViewAngle = 50.f;
const float MiniMap::m_ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
const float MiniMap::m_TextureWidth = 1600;
const float MiniMap::m_TextureHeight = 1600;
const int MiniMap::m_RenderTargetStage = 2;
const D3DXVECTOR3 MiniMap::m_DefaultPos = D3DXVECTOR3(0, 370, 10);


//----------------------------------------------------------------------
// Constructor	Destructor
//----------------------------------------------------------------------
MiniMap::MiniMap()
{
	m_Pos = D3DXVECTOR2(1350, 170);
	m_Size = D3DXVECTOR2(250, 250);
}

MiniMap::~MiniMap()
{
}


//----------------------------------------------------------------------
// Public Functions
//----------------------------------------------------------------------
bool MiniMap::Initialize()
{
	m_pBeginTask = new BeginTask(this);

	SINGLETON_INSTANCE(Lib::DrawTaskManager)->AddTask(m_pDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->AddTask(m_pUpdateTask);
	SINGLETON_INSTANCE(MapDrawTaskManager)->AddBeginTask(m_pBeginTask);

	m_pCamera = new Lib::Camera(
		m_TextureWidth,
		m_TextureHeight,
		m_NearPoint,
		m_FarPoint);

	CreateTexture();
	CreateConstantBuffer();
	WriteConstantBuffer();

	if (!CreateVertex2D())
	{
		return false;
	}

	m_pVertex->WriteConstantBuffer(&m_Pos);

	return true;
}

void MiniMap::Finalize()
{
	ReleaseVertex2D();

	ReleaseTexture();
	ReleaseConstantBuffer();

	delete m_pCamera;

	SINGLETON_INSTANCE(MapDrawTaskManager)->RemoveBeginTask(m_pBeginTask);
	SINGLETON_INSTANCE(Lib::DrawTaskManager)->RemoveTask(m_pDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->RemoveTask(m_pUpdateTask);

	delete m_pBeginTask;
}

void MiniMap::Update()
{
}

void MiniMap::Draw()
{
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->PSSetShaderResources(0, 1, &m_pShaderResourceView);
	m_pVertex->ShaderSetup();
	m_pVertex->Draw();
}


//----------------------------------------------------------------------
// Private Functions
//----------------------------------------------------------------------
bool MiniMap::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC ConstantBufferDesc;
	ConstantBufferDesc.ByteWidth = sizeof(MINIMAP_CONSTANT_BUFFER);
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

bool MiniMap::CreateTexture()
{
	D3D11_TEXTURE2D_DESC MapTextureDesc;
	ZeroMemory(&MapTextureDesc, sizeof(MapTextureDesc));
	MapTextureDesc.Width = static_cast<UINT>(m_TextureWidth);
	MapTextureDesc.Height = static_cast<UINT>(m_TextureHeight);
	MapTextureDesc.MipLevels = 1;
	MapTextureDesc.ArraySize = 1;
	MapTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	MapTextureDesc.SampleDesc.Count = 1;
	MapTextureDesc.SampleDesc.Quality = 0;
	MapTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	MapTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	MapTextureDesc.CPUAccessFlags = 0;
	MapTextureDesc.MiscFlags = 0;

	if (FAILED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDevice()->CreateTexture2D(
		&MapTextureDesc,
		NULL,
		&m_pMapTexture)))
	{
		OutputErrorLog("マップテクスチャ生成に失敗しました");
		return false;
	}

	if (FAILED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDevice()->CreateRenderTargetView(
		m_pMapTexture,
		NULL,
		&m_pRenderTarget)))
	{
		OutputErrorLog("マップテクスチャのレンダーターゲットビューの設定に失敗しました");
		return false;
	}

	if (FAILED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDevice()->CreateShaderResourceView(
		m_pMapTexture,
		NULL,
		&m_pShaderResourceView)))
	{
		OutputErrorLog("シェーダーリソースビューの生成に失敗しました");
		return false;
	}


	D3D11_TEXTURE2D_DESC DepthStencilDesc;
	DepthStencilDesc.Width = static_cast<UINT>(m_TextureWidth);
	DepthStencilDesc.Height = static_cast<UINT>(m_TextureHeight);
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
	m_ViewPort.Width = static_cast<float>(m_TextureWidth);
	m_ViewPort.Height = static_cast<float>(m_TextureWidth);
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;

	SINGLETON_INSTANCE(Lib::GraphicsDevice)->SetRenderTarget(&m_pRenderTarget, m_RenderTargetStage);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->SetDepthStencil(&m_pDepthStencilView, m_RenderTargetStage);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->SetClearColor(m_ClearColor, m_RenderTargetStage);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->SetViewPort(&m_ViewPort, m_RenderTargetStage);

	return true;
}

void MiniMap::ReleaseConstantBuffer()
{
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->SetViewPort(NULL, m_RenderTargetStage);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->SetClearColor(0xffffffff, m_RenderTargetStage);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->SetDepthStencil(NULL, m_RenderTargetStage);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->SetRenderTarget(NULL, m_RenderTargetStage);

	SafeRelease(m_pDepthStencilView);
	SafeRelease(m_pDepthStencilTexture);
	SafeRelease(m_pShaderResourceView);
	SafeRelease(m_pRenderTarget);
	SafeRelease(m_pMapTexture);
}

void MiniMap::ReleaseTexture()
{
	SafeRelease(m_pShaderResourceView);
	SafeRelease(m_pRenderTarget);
	SafeRelease(m_pMapTexture);
}

void MiniMap::WriteConstantBuffer()
{
	m_pCamera->TransformView(&m_DefaultPos, &D3DXVECTOR3(0, 0, 0), m_ViewAngle);

	D3D11_MAPPED_SUBRESOURCE SubResourceData;
	if (SUCCEEDED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResourceData)))
	{
		MINIMAP_CONSTANT_BUFFER ConstantBuffer;
		ConstantBuffer.Proj = m_pCamera->GetProjectionMatrix();
		ConstantBuffer.View = m_pCamera->GetViewMatrix();

		D3DXMatrixTranspose(&ConstantBuffer.View, &ConstantBuffer.View);
		D3DXMatrixTranspose(&ConstantBuffer.Proj, &ConstantBuffer.Proj);

		memcpy_s(
			SubResourceData.pData,
			SubResourceData.RowPitch,
			reinterpret_cast<void*>(&ConstantBuffer),
			sizeof(ConstantBuffer));

		SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->Unmap(m_pConstantBuffer, 0);
	}
}

void MiniMap::MiniMapBeginScene()
{
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->BeginScene(m_RenderTargetStage);

	WriteConstantBuffer();

	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->VSSetConstantBuffers(4, 1, &m_pConstantBuffer);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->GSSetConstantBuffers(4, 1, &m_pConstantBuffer);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->HSSetConstantBuffers(4, 1, &m_pConstantBuffer);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->DSSetConstantBuffers(4, 1, &m_pConstantBuffer);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->PSSetConstantBuffers(4, 1, &m_pConstantBuffer);
}


//----------------------------------------------------------------------
// Inner Class Constructor Destructor
//----------------------------------------------------------------------
MiniMap::BeginTask::BeginTask(MiniMap* _pMiniMap) :
	m_pMiniMap(_pMiniMap)
{
}

MiniMap::BeginTask::~BeginTask()
{
}


//----------------------------------------------------------------------
// Inner Class Public Function
//----------------------------------------------------------------------
void MiniMap::BeginTask::Run()
{
	m_pMiniMap->MiniMapBeginScene();
}

