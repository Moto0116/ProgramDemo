/**
 * @file	Water.cpp
 * @brief	水の管理クラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "Water.h"

#include "Debugger\Debugger.h"
#include "DirectX11\GraphicsDevice\GraphicsDevice.h"
#include "DirectX11\ShaderManager\ShaderManager.h"
#include "DirectX11\SoundManager\SoundManager.h"
#include "DirectX11\SoundManager\ISound\ISound.h"
#include "DirectX11\TextureManager\TextureManager.h"
#include "DirectX11\TextureManager\ITexture\ITexture.h"
#include "Main\Application\Scene\GameScene\Task\CubeMapDrawTask\CubeMapDrawTask.h"


//----------------------------------------------------------------------
// Static Private Variables
//----------------------------------------------------------------------
const D3DXVECTOR3 Water::m_DefaultPos = D3DXVECTOR3(0, 0.1f, 0);
const D3DXVECTOR2 Water::m_DefaultSize = D3DXVECTOR2(30, 30);
const float Water::m_ClearColor[4] = { 0, 0, 0, 0 };
const float Water::m_TextureWidth = 800;
const float Water::m_TextureHeight = 800;
const int Water::m_RenderTargetStage = 3;


//----------------------------------------------------------------------
// Constructor	Destructor
//----------------------------------------------------------------------
Water::Water()
{
}

Water::~Water()
{
}


//----------------------------------------------------------------------
// Public Functions
//----------------------------------------------------------------------
bool Water::Initialize()
{
	// タスク生成処理
	m_pDrawTask = new Lib::DrawTask();
	m_pUpdateTask = new Lib::UpdateTask();
	m_pBeginTask = new Water::BeginTask(this);

	// タスクにオブジェクト設定
	m_pDrawTask->SetDrawObject(this);
	m_pUpdateTask->SetUpdateObject(this);

	SINGLETON_INSTANCE(Lib::DrawTaskManager)->AddTask(m_pDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->AddTask(m_pUpdateTask);
	SINGLETON_INSTANCE(CubeMapDrawTaskManager)->AddBeginTask(m_pBeginTask);


	if (!CreateVertexBuffer())
	{
		return false;
	}

	if (!CreateShader())
	{
		return false;
	}

	if (!CreateVertexLayout())
	{
		return false;
	}

	if (!CreateState())
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

	if (!CreateTexture())
	{
		return false;
	}

	return true;
}

void Water::Finalize()
{
	ReleaseTexture();
	ReleaseConstantBuffer();
	ReleaseState();
	ReleaseVertexLayout();
	ReleaseShader();
	ReleaseVertexBuffer();

	SINGLETON_INSTANCE(CubeMapDrawTaskManager)->RemoveBeginTask(m_pBeginTask);
	SINGLETON_INSTANCE(Lib::DrawTaskManager)->RemoveTask(m_pDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->RemoveTask(m_pUpdateTask);

	delete m_pBeginTask;
	delete m_pUpdateTask;
	delete m_pDrawTask;
}

void Water::Update()
{
}

void Water::Draw()
{
	ID3D11DeviceContext* pContext = SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext();

	pContext->VSSetShader(SINGLETON_INSTANCE(Lib::ShaderManager)->GetVertexShader(m_VertexShaderIndex), NULL, 0);
	pContext->PSSetShader(SINGLETON_INSTANCE(Lib::ShaderManager)->GetPixelShader(m_PixelShaderIndex), NULL, 0);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	pContext->IASetInputLayout(m_pVertexLayout);
	pContext->OMSetDepthStencilState(m_pDepthStencilState, 0);
	pContext->OMSetBlendState(m_pBlendState, NULL, 0xffffffff);

	UINT Stride = sizeof(VERTEX);
	UINT Offset = 0;
	pContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &Stride, &Offset);

	ID3D11ShaderResourceView* pPuddleResource =
		SINGLETON_INSTANCE(Lib::TextureManager)->GetTexture(m_PuddleTextureIndex)->Get();	

	ID3D11ShaderResourceView* pSkyResource =
		SINGLETON_INSTANCE(Lib::TextureManager)->GetTexture(m_SkyCLUTIndex)->Get();

	pContext->PSSetShaderResources(0, 1, &m_pCubeTextureResource);
	pContext->PSSetShaderResources(1, 1, &pPuddleResource);
	pContext->PSSetShaderResources(2, 1, &pSkyResource);

	pContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	pContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	pContext->Draw(VERTEX_NUM, 0);
}

void Water::WaveDraw()
{
}


//----------------------------------------------------------------------
// Private Functions
//----------------------------------------------------------------------
bool Water::CreateVertexBuffer()
{
	VERTEX VertexData[VERTEX_NUM] =
	{
		VERTEX{ D3DXVECTOR3(-m_DefaultSize.x, 0.1f, -m_DefaultSize.y), D3DXVECTOR2(0, 0) },
		VERTEX{ D3DXVECTOR3(-m_DefaultSize.x, 0.1f, m_DefaultSize.y), D3DXVECTOR2(1, 0) },
		VERTEX{ D3DXVECTOR3(m_DefaultSize.x, 0.1f, -m_DefaultSize.y), D3DXVECTOR2(0, 1) },
		VERTEX{ D3DXVECTOR3(m_DefaultSize.x, 0.1f, m_DefaultSize.y), D3DXVECTOR2(1, 1) }
	};

	for (int i = 0; i < VERTEX_NUM; i++)
	{
		m_pVertexData[i].Pos = VertexData[i].Pos;
		m_pVertexData[i].UV = VertexData[i].UV;
	}

	// 頂点バッファの設定.
	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	BufferDesc.ByteWidth = sizeof(VERTEX) * VERTEX_NUM;
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	BufferDesc.MiscFlags = 0;
	BufferDesc.StructureByteStride = 0;

	// 頂点バッファに格納するデータの設定.
	D3D11_SUBRESOURCE_DATA ResourceData;
	ZeroMemory(&ResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	ResourceData.pSysMem = m_pVertexData;

	if (FAILED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDevice()->CreateBuffer(
		&BufferDesc,
		&ResourceData,
		&m_pVertexBuffer)))
	{
		OutputErrorLog("頂点バッファの生成に失敗しました");
		return false;
	}

	return true;
}

bool Water::CreateShader()
{
	if (!SINGLETON_INSTANCE(Lib::ShaderManager)->LoadVertexShader(
		TEXT("Resource\\Effect\\Water.fx"),
		"VS",
		&m_VertexShaderIndex))
	{
		OutputErrorLog("頂点シェーダーの読み込みに失敗しました");
		return false;
	}

	if (!SINGLETON_INSTANCE(Lib::ShaderManager)->LoadPixelShader(
		TEXT("Resource\\Effect\\Water.fx"),
		"PS",
		&m_PixelShaderIndex))
	{
		OutputErrorLog("ピクセルシェーダーの読み込みに失敗しました");
		return false;
	}

	return true;
}

bool Water::CreateVertexLayout()
{
	D3D11_INPUT_ELEMENT_DESC InputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	if (FAILED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDevice()->CreateInputLayout(
		InputElementDesc,
		sizeof(InputElementDesc) / sizeof(InputElementDesc[0]),
		SINGLETON_INSTANCE(Lib::ShaderManager)->GetCompiledVertexShader(m_VertexShaderIndex)->GetBufferPointer(),
		SINGLETON_INSTANCE(Lib::ShaderManager)->GetCompiledVertexShader(m_VertexShaderIndex)->GetBufferSize(),
		&m_pVertexLayout)))
	{
		OutputErrorLog("入力レイアウトの生成に失敗しました");
		return false;
	}

	return true;
}

bool Water::CreateState()
{
	D3D11_BLEND_DESC BlendDesc;
	ZeroMemory(&BlendDesc, sizeof(D3D11_BLEND_DESC));
	BlendDesc.AlphaToCoverageEnable = false;
	BlendDesc.IndependentBlendEnable = false;
	BlendDesc.RenderTarget[0].BlendEnable = true;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	if (FAILED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDevice()->CreateBlendState(
		&BlendDesc,
		&m_pBlendState)))
	{
		OutputErrorLog("ブレンドステートの生成に失敗しました");
		ReleaseState();
		return false;
	}

	D3D11_DEPTH_STENCIL_DESC DepthStencilDesc;
	ZeroMemory(&DepthStencilDesc, sizeof(DepthStencilDesc));
	DepthStencilDesc.DepthEnable = TRUE;
	DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	DepthStencilDesc.StencilEnable = FALSE;
	if (FAILED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDevice()->CreateDepthStencilState(
		&DepthStencilDesc,
		&m_pDepthStencilState)))
	{
		OutputErrorLog("深度ステンシルステートの生成に失敗しました");
		return false;
	}

	return true;
}

bool Water::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC ConstantBufferDesc;
	ConstantBufferDesc.ByteWidth = sizeof(CONSTANT_BUFFER);
	ConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ConstantBufferDesc.MiscFlags = 0;
	ConstantBufferDesc.StructureByteStride = 0;

	if (FAILED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDevice()->CreateBuffer(
		&ConstantBufferDesc,
		NULL,
		&m_pConstantBuffer)))
	{
		OutputErrorLog("水の描画に関する定数バッファ生成に失敗しました");
		return false;
	}

	D3D11_BUFFER_DESC CubeMapConstantBufferDesc;
	CubeMapConstantBufferDesc.ByteWidth = sizeof(WATER_CONSTANT_BUFFER);
	CubeMapConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	CubeMapConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	CubeMapConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	CubeMapConstantBufferDesc.MiscFlags = 0;
	CubeMapConstantBufferDesc.StructureByteStride = 0;

	if (FAILED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDevice()->CreateBuffer(
		&CubeMapConstantBufferDesc,
		NULL, 
		&m_pCubeMapConstantBuffer)))
	{
		OutputErrorLog("キューブマップ作成に関する定数バッファ生成に失敗しました");
		return false;
	}

	return true;
}

bool Water::CreateTexture()
{
	// 水たまりテクスチャの読み込み
	SINGLETON_INSTANCE(Lib::TextureManager)->LoadTexture(TEXT("Resource\\Texture\\Water.png"), &m_PuddleTextureIndex);

	// 空のカラーテーブル読み込み
	SINGLETON_INSTANCE(Lib::TextureManager)->LoadTexture(TEXT("Resource\\Texture\\MainLightCLUT.png"), &m_SkyCLUTIndex);


	// キューブマップテクスチャ
	D3D11_TEXTURE2D_DESC CubeTextureDesc;
	ZeroMemory(&CubeTextureDesc, sizeof(CubeTextureDesc));
	CubeTextureDesc.Width = static_cast<UINT>(m_TextureWidth);
	CubeTextureDesc.Height = static_cast<UINT>(m_TextureHeight);
	CubeTextureDesc.MipLevels = 1;
	CubeTextureDesc.ArraySize = 6;	// キューブマップなので6方向分用意
	CubeTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	CubeTextureDesc.SampleDesc.Count = 1;
	CubeTextureDesc.SampleDesc.Quality = 0;
	CubeTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	CubeTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	CubeTextureDesc.CPUAccessFlags = 0;
	CubeTextureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE; // キューブマップ指定

	if (FAILED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDevice()->CreateTexture2D(
		&CubeTextureDesc,
		NULL,
		&m_pCubeTexture)))
	{
		OutputErrorLog("マップテクスチャ生成に失敗しました");
		return false;
	}

	D3D11_RENDER_TARGET_VIEW_DESC CubeRenderTargetDesc;
	ZeroMemory(&CubeRenderTargetDesc, sizeof(CubeRenderTargetDesc));
	CubeRenderTargetDesc.Format = CubeTextureDesc.Format;
	CubeRenderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	CubeRenderTargetDesc.Texture2DArray.FirstArraySlice = 0;
	CubeRenderTargetDesc.Texture2DArray.MipSlice = 0;
	CubeRenderTargetDesc.Texture2DArray.ArraySize = 6;	// キューブマップなので6つ

	if (FAILED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDevice()->CreateRenderTargetView(
		m_pCubeTexture,
		&CubeRenderTargetDesc,
		&m_pCubeTextureRenderTarget)))
	{
		OutputErrorLog("マップテクスチャのレンダーターゲットビューの設定に失敗しました");
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC CubeResourceDesc;
	ZeroMemory(&CubeResourceDesc, sizeof(CubeResourceDesc));
	CubeResourceDesc.Format = CubeTextureDesc.Format;
	CubeResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;	// キューブテクスチャ指定
	CubeResourceDesc.TextureCube.MipLevels = CubeTextureDesc.MipLevels;
	CubeResourceDesc.TextureCube.MostDetailedMip = 0;

	if (FAILED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDevice()->CreateShaderResourceView(
		m_pCubeTexture,
		&CubeResourceDesc,
		&m_pCubeTextureResource)))
	{
		OutputErrorLog("シェーダーリソースビューの生成に失敗しました");
		return false;
	}


	// 深度ステンシルテクスチャの生成
	D3D11_TEXTURE2D_DESC DepthStencilTextureDesc;
	DepthStencilTextureDesc.Width = static_cast<UINT>(m_TextureWidth);
	DepthStencilTextureDesc.Height = static_cast<UINT>(m_TextureHeight);
	DepthStencilTextureDesc.MipLevels = 1;
	DepthStencilTextureDesc.ArraySize = 6;
	DepthStencilTextureDesc.Format = DXGI_FORMAT_D32_FLOAT;
	DepthStencilTextureDesc.SampleDesc.Count = 1;
	DepthStencilTextureDesc.SampleDesc.Quality = 0;
	DepthStencilTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DepthStencilTextureDesc.CPUAccessFlags = 0;
	DepthStencilTextureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	if (FAILED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDevice()->CreateTexture2D(
		&DepthStencilTextureDesc,
		NULL,
		&m_pDepthStencilTexture)))
	{
		OutputErrorLog("深度ステンシルテクスチャの生成に失敗しました");
		return false;
	}

	// 深度ステンシルビューの生成
	if (FAILED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDevice()->CreateDepthStencilView(
		m_pDepthStencilTexture,
		NULL,
		&m_pDepthStencilView)))
	{
		OutputErrorLog("深度ステンシルビューの生成に失敗しました");
		return false;
	}

	// ビューポート設定
	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;
	m_ViewPort.Width = m_TextureWidth;
	m_ViewPort.Height = m_TextureHeight;
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;

	// グラフィックデバイスに描画先として深度テクスチャを設定
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->SetRenderTarget(&m_pCubeTextureRenderTarget, m_RenderTargetStage);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->SetDepthStencil(&m_pDepthStencilView, m_RenderTargetStage);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->SetClearColor(m_ClearColor, m_RenderTargetStage);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->SetViewPort(&m_ViewPort, m_RenderTargetStage);


	D3DXVECTOR3 EyePos = D3DXVECTOR3(0, 0, 0);
	D3DXVECTOR3 LookPos = D3DXVECTOR3(0, 0, 0);
	D3DXVECTOR3 UpVec = D3DXVECTOR3(0, 1, 0);
	float ViewAngle = 90;

	m_pCamera = new Lib::Camera(m_TextureWidth, m_TextureHeight, 1, 700);

	// 6つのView行列を生成する
	LookPos = D3DXVECTOR3(1, 0, 0);
	UpVec = D3DXVECTOR3(0, 1, 0);
	m_pCamera->TransformView(&EyePos, &LookPos, &UpVec, ViewAngle);
	m_ViewMat[0] = m_pCamera->GetViewMatrix();
	
	LookPos = D3DXVECTOR3(-1, 0, 0);
	UpVec = D3DXVECTOR3(0, 1, 0);
	m_pCamera->TransformView(&EyePos, &LookPos, &UpVec, ViewAngle);
	m_ViewMat[1] = m_pCamera->GetViewMatrix();

	LookPos = D3DXVECTOR3(0, 1, 0);
	UpVec = D3DXVECTOR3(0, 0, -1);
	m_pCamera->TransformView(&EyePos, &LookPos, &UpVec, ViewAngle);
	m_ViewMat[2] = m_pCamera->GetViewMatrix();

	LookPos = D3DXVECTOR3(0, -1, 0);
	UpVec = D3DXVECTOR3(0, 0, 1);
	m_pCamera->TransformView(&EyePos, &LookPos, &UpVec, ViewAngle);
	m_ViewMat[3] = m_pCamera->GetViewMatrix();

	LookPos = D3DXVECTOR3(0, 0, 1);
	UpVec = D3DXVECTOR3(0, 1, 0);
	m_pCamera->TransformView(&EyePos, &LookPos, &UpVec, ViewAngle);
	m_ViewMat[4] = m_pCamera->GetViewMatrix();

	LookPos = D3DXVECTOR3(0, 0, -1);
	UpVec = D3DXVECTOR3(0, 1, 0);
	m_pCamera->TransformView(&EyePos, &LookPos, &UpVec, ViewAngle);
	m_ViewMat[5] = m_pCamera->GetViewMatrix();

	m_ProjMat = m_pCamera->GetProjectionMatrix();

	if (!WriteCubeMapConstantBuffer())
	{
		return false;
	}

	return true;
}

void Water::ReleaseVertexBuffer()
{
	SafeRelease(m_pVertexBuffer);
}

void Water::ReleaseShader()
{
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleasePixelShader(m_PixelShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleaseVertexShader(m_VertexShaderIndex);
}

void Water::ReleaseVertexLayout()
{
	SafeRelease(m_pVertexLayout);
}

void Water::ReleaseState()
{
	SafeRelease(m_pDepthStencilState);
	SafeRelease(m_pBlendState);
}

void Water::ReleaseConstantBuffer()
{
	SafeRelease(m_pCubeMapConstantBuffer);
	SafeRelease(m_pConstantBuffer);
}

void Water::ReleaseTexture()
{
	delete m_pCamera;
	SafeRelease(m_pDepthStencilView);
	SafeRelease(m_pDepthStencilTexture);
	SafeRelease(m_pCubeTextureResource);
	SafeRelease(m_pCubeTextureRenderTarget);
	SafeRelease(m_pCubeTexture);

	SINGLETON_INSTANCE(Lib::TextureManager)->ReleaseTexture(m_SkyCLUTIndex);
	SINGLETON_INSTANCE(Lib::TextureManager)->ReleaseTexture(m_PuddleTextureIndex);
}

bool Water::WriteConstantBuffer()
{
	D3D11_MAPPED_SUBRESOURCE SubResourceData;
	if (SUCCEEDED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->Map(
		m_pConstantBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD, 
		0, 
		&SubResourceData)))
	{
		D3DXMATRIX MatWorld;
		D3DXMatrixIdentity(&MatWorld);
		D3DXMatrixTranspose(&MatWorld, &MatWorld);

		CONSTANT_BUFFER ConstantBuffer;
		ConstantBuffer.World = MatWorld;

		memcpy_s(
			SubResourceData.pData,
			SubResourceData.RowPitch,
			reinterpret_cast<void*>(&ConstantBuffer),
			sizeof(ConstantBuffer));

		SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->Unmap(m_pConstantBuffer, 0);

		return true;
	}

	return false;
}

bool Water::WriteCubeMapConstantBuffer()
{
	D3D11_MAPPED_SUBRESOURCE SubResourceData;
	if (SUCCEEDED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->Map(
		m_pCubeMapConstantBuffer,
		0, 
		D3D11_MAP_WRITE_DISCARD, 
		0, 
		&SubResourceData)))
	{
		for (int i = 0; i < 6; i++)
		{
			m_pWaterConstantBuffer.View[i] = m_ViewMat[i];
			D3DXMatrixTranspose(&m_pWaterConstantBuffer.View[i], &m_pWaterConstantBuffer.View[i]);
		}

		m_pWaterConstantBuffer.Proj = m_ProjMat;
		D3DXMatrixTranspose(&m_pWaterConstantBuffer.Proj, &m_pWaterConstantBuffer.Proj);

		memcpy_s(
			SubResourceData.pData,
			SubResourceData.RowPitch,
			reinterpret_cast<void*>(&m_pWaterConstantBuffer),
			sizeof(m_pWaterConstantBuffer));

		SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->Unmap(m_pCubeMapConstantBuffer, 0);

		return true;
	}

	return false;
}

void Water::CubeMapBeginScene()
{
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->BeginScene(m_RenderTargetStage);
	
	WriteCubeMapConstantBuffer();

	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->VSSetConstantBuffers(
		5,
		1,
		&m_pCubeMapConstantBuffer);

	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->GSSetConstantBuffers(
		5,
		1,
		&m_pCubeMapConstantBuffer);

	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->PSSetConstantBuffers(
		5,
		1,
		&m_pCubeMapConstantBuffer);
}



//----------------------------------------------------------------------
// Inner Class Constructor Destructor
//----------------------------------------------------------------------
Water::BeginTask::BeginTask(Water* _pWater) :
	m_pWater(_pWater)
{
}

Water::BeginTask::~BeginTask()
{
}


//----------------------------------------------------------------------
// Inner Class Public Function
//----------------------------------------------------------------------
void Water::BeginTask::Run()
{
	m_pWater->CubeMapBeginScene();
}

