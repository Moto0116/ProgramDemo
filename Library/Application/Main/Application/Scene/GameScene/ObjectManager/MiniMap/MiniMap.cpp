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
#include "DirectX11\GraphicsDevice\Dx11GraphicsDevice.h"
#include "DirectX11\Camera\Dx11Camera.h"
#include "Main\Application\Scene\GameScene\Task\MapDrawTask\MapDrawTask.h"


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
	m_pMiniMapDrawStartUp = new MiniMapDrawStartUp(this);

	m_pDrawTask->SetName("MiniMap");
	m_pUpdateTask->SetName("MiniMap");
	m_pMiniMapDrawStartUp->SetName("MiniMap");

	SINGLETON_INSTANCE(Lib::Draw2DTaskManager)->AddTask(m_pDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->AddTask(m_pUpdateTask);
	SINGLETON_INSTANCE(MapDrawTaskManager)->AddStartUpTask(m_pMiniMapDrawStartUp);

	m_pCamera = new Lib::Dx11::Camera(
		m_TextureWidth,
		m_TextureHeight,
		m_NearPoint,
		m_FarPoint);

	if (!CreateTexture())			return false;
	if (!CreateConstantBuffer())	return false;
	if (!WriteConstantBuffer())		return false;
	if (!CreateVertex2D())			return false;

	if (!m_pVertex->WriteConstantBuffer(&m_Pos))
	{
		OutputErrorLog("定数バッファの書き込みに失敗しました");
		return false;
	}

	m_pVertex->SetInverse(false);

	return true;
}

void MiniMap::Finalize()
{
	ReleaseVertex2D();
	ReleaseConstantBuffer();
	ReleaseTexture();

	delete m_pCamera;

	SINGLETON_INSTANCE(MapDrawTaskManager)->RemoveStartUpTask(m_pMiniMapDrawStartUp);
	SINGLETON_INSTANCE(Lib::Draw2DTaskManager)->RemoveTask(m_pDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->RemoveTask(m_pUpdateTask);

	delete m_pMiniMapDrawStartUp;
}

void MiniMap::Draw()
{
	ID3D11DeviceContext* pDeviceContext = SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDeviceContext();

	pDeviceContext->PSSetShaderResources(0, 1, &m_pShaderResourceView);
	m_pVertex->ShaderSetup();
	m_pVertex->Draw();
}


//----------------------------------------------------------------------
// Private Functions
//----------------------------------------------------------------------
bool MiniMap::CreateConstantBuffer()
{
	Lib::Dx11::GraphicsDevice* pGraphicsDevice = SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice);

	// マップ描画の定数バッファ.
	D3D11_BUFFER_DESC ConstantBufferDesc;
	ConstantBufferDesc.ByteWidth = sizeof(MINIMAP_CONSTANT_BUFFER);
	ConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ConstantBufferDesc.MiscFlags = 0;
	ConstantBufferDesc.StructureByteStride = 0;

	if (FAILED(pGraphicsDevice->GetDevice()->CreateBuffer(&ConstantBufferDesc, nullptr, &m_pConstantBuffer)))
	{
		OutputErrorLog("定数バッファ生成に失敗しました");
		return false;
	}

	return true;
}

bool MiniMap::CreateTexture()
{
	Lib::Dx11::GraphicsDevice* pGraphicsDevice = SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice);

	// マップテクスチャの生成.
	D3D11_TEXTURE2D_DESC MapTextureDesc;
	ZeroMemory(&MapTextureDesc, sizeof(MapTextureDesc));
	MapTextureDesc.Width = static_cast<UINT>(m_TextureWidth);
	MapTextureDesc.Height = static_cast<UINT>(m_TextureHeight);
	MapTextureDesc.MipLevels = 1;
	MapTextureDesc.ArraySize = 1;
	MapTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	MapTextureDesc.SampleDesc.Count = 1;
	MapTextureDesc.SampleDesc.Quality = 0;
	MapTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	MapTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	MapTextureDesc.CPUAccessFlags = 0;
	MapTextureDesc.MiscFlags = 0;

	if (FAILED(pGraphicsDevice->GetDevice()->CreateTexture2D(
		&MapTextureDesc,
		nullptr,
		&m_pMapTexture)))
	{
		OutputErrorLog("マップテクスチャ生成に失敗しました");
		return false;
	}

	if (FAILED(pGraphicsDevice->GetDevice()->CreateRenderTargetView(
		m_pMapTexture,
		nullptr,
		&m_pRenderTarget)))
	{
		OutputErrorLog("マップテクスチャのレンダーターゲットビューの設定に失敗しました");
		return false;
	}

	if (FAILED(pGraphicsDevice->GetDevice()->CreateShaderResourceView(
		m_pMapTexture,
		nullptr,
		&m_pShaderResourceView)))
	{
		OutputErrorLog("シェーダーリソースビューの生成に失敗しました");
		return false;
	}


	// 深度ステンシルテクスチャの生成.
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

	if (FAILED(FAILED(pGraphicsDevice->GetDevice()->CreateTexture2D(
		&DepthStencilDesc,
		nullptr,
		&m_pDepthStencilTexture))))
	{
		OutputErrorLog("深度ステンシルテクスチャ生成に失敗しました");
		return false;
	}

	if (FAILED(FAILED(pGraphicsDevice->GetDevice()->CreateDepthStencilView(
		m_pDepthStencilTexture,
		nullptr,
		&m_pDepthStencilView))))
	{
		OutputErrorLog("深度ステンシルテクスチャのデプスステンシルビューの生成に失敗しました");
		return false;
	}

	// マップテクスチャのビューポート設定.
	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;
	m_ViewPort.Width = static_cast<float>(m_TextureWidth);
	m_ViewPort.Height = static_cast<float>(m_TextureWidth);
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;

	pGraphicsDevice->SetRenderTarget(&m_pRenderTarget, m_RenderTargetStage);
	pGraphicsDevice->SetDepthStencil(&m_pDepthStencilView, m_RenderTargetStage);
	pGraphicsDevice->SetClearColor(m_ClearColor, m_RenderTargetStage);
	pGraphicsDevice->SetViewPort(&m_ViewPort, m_RenderTargetStage);

	return true;
}

void MiniMap::ReleaseConstantBuffer()
{
	SafeRelease(m_pConstantBuffer);
}

void MiniMap::ReleaseTexture()
{
	SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->SetViewPort(nullptr, m_RenderTargetStage);
	SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->SetClearColor(0xffffffff, m_RenderTargetStage);
	SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->SetDepthStencil(nullptr, m_RenderTargetStage);
	SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->SetRenderTarget(nullptr, m_RenderTargetStage);

	SafeRelease(m_pDepthStencilView);
	SafeRelease(m_pDepthStencilTexture);
	SafeRelease(m_pShaderResourceView);
	SafeRelease(m_pRenderTarget);
	SafeRelease(m_pMapTexture);

	SafeRelease(m_pShaderResourceView);
	SafeRelease(m_pRenderTarget);
	SafeRelease(m_pMapTexture);
}

bool MiniMap::WriteConstantBuffer()
{
	D3D11_MAPPED_SUBRESOURCE SubResourceData;
	if (SUCCEEDED(SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDeviceContext()->Map(
		m_pConstantBuffer, 
		0, 
		D3D11_MAP_WRITE_DISCARD,
		0, 
		&SubResourceData)))
	{
		m_pCamera->TransformView(&m_DefaultPos, &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 1, 0), m_ViewAngle);

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

		SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDeviceContext()->Unmap(m_pConstantBuffer, 0);

		return true;
	}

	return false;
}

void MiniMap::MiniMapBeginScene()
{
	Lib::Dx11::GraphicsDevice* pGraphicsDevice = SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice);
	ID3D11DeviceContext* pDeviceContext = SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDeviceContext();

	pGraphicsDevice->BeginScene(m_RenderTargetStage);

	// ミニマップ定数バッファの更新と設定.
	WriteConstantBuffer();
	pDeviceContext->VSSetConstantBuffers(4, 1, &m_pConstantBuffer);
	pDeviceContext->GSSetConstantBuffers(4, 1, &m_pConstantBuffer);
	pDeviceContext->HSSetConstantBuffers(4, 1, &m_pConstantBuffer);
	pDeviceContext->DSSetConstantBuffers(4, 1, &m_pConstantBuffer);
	pDeviceContext->PSSetConstantBuffers(4, 1, &m_pConstantBuffer);
}


//----------------------------------------------------------------------
// Inner Class Constructor Destructor
//----------------------------------------------------------------------
MiniMap::MiniMapDrawStartUp::MiniMapDrawStartUp(MiniMap* _pMiniMap) :
	m_pMiniMap(_pMiniMap)
{
}

MiniMap::MiniMapDrawStartUp::~MiniMapDrawStartUp()
{
}


//----------------------------------------------------------------------
// Inner Class Public Function
//----------------------------------------------------------------------
void MiniMap::MiniMapDrawStartUp::Run()
{
	m_pMiniMap->MiniMapBeginScene();
}

