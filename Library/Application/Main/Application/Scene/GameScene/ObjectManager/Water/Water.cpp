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
#include "TaskManager\TaskBase\DrawTask\DrawTask.h"
#include "TaskManager\TaskBase\UpdateTask\UpdateTask.h"
#include "DirectX11\GraphicsDevice\GraphicsDevice.h"
#include "DirectX11\ShaderManager\ShaderManager.h"
#include "DirectX11\TextureManager\TextureManager.h"
#include "DirectX11\TextureManager\ITexture\ITexture.h"
#include "DirectX11\TextureManager\Texture\Texture.h"
#include "DirectX11\Camera\Camera.h"
#include "DirectX11\Font\Font.h"
#include "Main\Application\Scene\GameScene\Task\CubeMapDrawTask\CubeMapDrawTask.h"
#include "Main\Application\Scene\GameScene\Task\ReflectMapDrawTask\ReflectMapDrawTask.h"


//----------------------------------------------------------------------
// Static Private Variables
//----------------------------------------------------------------------
const D3DXVECTOR3 Water::m_DefaultPos = D3DXVECTOR3(0, 0.1f, 0);
const D3DXVECTOR2 Water::m_DefaultSize = D3DXVECTOR2(160, 160);
const D3DXVECTOR2 Water::m_DefaultFontPos = D3DXVECTOR2(25, 50);
const D3DXVECTOR2 Water::m_DefaultFontSize = D3DXVECTOR2(16, 32);
const D3DXCOLOR Water::m_DefaultFontColor = 0xffffffff;
const float Water::m_ClearColor[4] = { 0, 0, 0, 0 };
const float Water::m_WaterClearColor[4] = { 0.1f, 0.1f, 0, 0};
const float Water::m_CubeTextureWidth = 800;
const float Water::m_CubeTextureHeight = 800;
const float Water::m_WaveTextureWidth = 800;
const float Water::m_WaveTextureHeight = 800;
const float Water::m_ReflectTextureWidth = 3200;
const float Water::m_ReflectTextureHeight = 1800;
const int Water::m_CubeRenderTargetStage = 3;
const int Water::m_WaveRenderTargetStage = 4;
const int Water::m_BumpRenderTargetStage = 5;
const int Water::m_ReflectRenderTargetStage = 6;


//----------------------------------------------------------------------
// Constructor	Destructor
//----------------------------------------------------------------------
Water::Water() : 
	m_pCamera(nullptr),
	m_pFont(nullptr),
	m_CubeVertexShaderIndex(Lib::ShaderManager::m_InvalidIndex),
	m_CubePixelShaderIndex(Lib::ShaderManager::m_InvalidIndex),
	m_ReflectVertexShaderIndex(Lib::ShaderManager::m_InvalidIndex),
	m_ReflectPixelShaderIndex(Lib::ShaderManager::m_InvalidIndex),
	m_WaveVertexShaderIndex(Lib::ShaderManager::m_InvalidIndex),
	m_WavePixelShaderIndex(Lib::ShaderManager::m_InvalidIndex),
	m_BumpPixelShaderIndex(Lib::ShaderManager::m_InvalidIndex),
	m_WaveRenderIndex(0),
	m_AddWavePos(D3DXVECTOR2(0, 0)),
	m_AddWaveHeight(0),
	m_RandDevice(),
	m_MersenneTwister(m_RandDevice()),
	m_WaveAddTime(750),
	m_WaveAddCount(0),
	m_IsCubeMapDraw(true)
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
	// タスク生成処理.
	m_pDrawTask = new Lib::DrawTask();
	m_pUpdateTask = new Lib::UpdateTask();
	m_pCubeDrawBeginTask = new CubeDrawBeginTask(this);
	m_pReflectDrawBeginTask = new ReflectDrawBeginTask(this);

	// タスクにオブジェクト設定.
	m_pDrawTask->SetDrawObject(this);
	m_pUpdateTask->SetUpdateObject(this);

	SINGLETON_INSTANCE(Lib::DrawTaskManager)->AddTask(m_pDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->AddTask(m_pUpdateTask);
	SINGLETON_INSTANCE(CubeMapDrawTaskManager)->AddBeginTask(m_pCubeDrawBeginTask);
	SINGLETON_INSTANCE(ReflectMapDrawTaskManager)->AddBeginTask(m_pReflectDrawBeginTask);


	if (!CreateVertexBuffer())		return false;
	if (!CreateShader())			return false;
	if (!CreateVertexLayout())		return false;
	if (!CreateState())				return false;
	if (!CreateConstantBuffer())	return false;
	if (!WriteConstantBuffer())		return false;
	if (!CreateTexture())			return false;
	if (!CreateFontObject())		return false;

	return true;
}

void Water::Finalize()
{
	ReleaseFontObject();
	ReleaseTexture();
	ReleaseConstantBuffer();
	ReleaseState();
	ReleaseVertexLayout();
	ReleaseShader();
	ReleaseVertexBuffer();

	SINGLETON_INSTANCE(ReflectMapDrawTaskManager)->RemoveBeginTask(m_pReflectDrawBeginTask);
	SINGLETON_INSTANCE(CubeMapDrawTaskManager)->RemoveBeginTask(m_pCubeDrawBeginTask);
	SINGLETON_INSTANCE(Lib::DrawTaskManager)->RemoveTask(m_pDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->RemoveTask(m_pUpdateTask);

	delete m_pReflectDrawBeginTask;
	delete m_pCubeDrawBeginTask;
	delete m_pUpdateTask;
	delete m_pDrawTask;
}

void Water::Update()
{
	m_pKeyState = SINGLETON_INSTANCE(Lib::InputDeviceManager)->GetKeyState();

	if (m_pKeyState[DIK_T] == Lib::KeyDevice::KEYSTATE::KEY_PUSH)
	{
		m_IsCubeMapDraw = !m_IsCubeMapDraw;
	}

	if (m_IsCubeMapDraw)
	{
	}
	else
	{
		///@todo 波の追加処理

		//m_WaveAddTime++;

		//if (m_WaveAddTime >= 800 && m_WaveAddCount <= 6)
		//{
		//	m_WaveAddTime = 0;
		//	m_WaveAddCount++;

		//	m_AddWavePos = D3DXVECTOR2(
		//		m_MersenneTwister() % static_cast<int>(m_WaveTextureWidth)* (1 / m_WaveTextureWidth),
		//		m_MersenneTwister() % static_cast<int>(m_WaveTextureHeight)* (1 / m_WaveTextureHeight)
		//		);

		//	m_AddWaveHeight = (m_MersenneTwister() % 30 + 20) * 0.001f;

		//	WriteConstantBuffer();
		//}
	}
}

void Water::Draw()
{
	Lib::GraphicsDevice* pGraphicdDevice = SINGLETON_INSTANCE(Lib::GraphicsDevice);
	ID3D11DeviceContext* pDeviceContext = SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext();
	Lib::TextureManager* pTextureManager = SINGLETON_INSTANCE(Lib::TextureManager);
	Lib::ShaderManager* pShaderManager = SINGLETON_INSTANCE(Lib::ShaderManager);

	if (m_IsCubeMapDraw)
	{
		pDeviceContext->VSSetShader(pShaderManager->GetVertexShader(m_CubeVertexShaderIndex), nullptr, 0);
		pDeviceContext->PSSetShader(pShaderManager->GetPixelShader(m_CubePixelShaderIndex), nullptr, 0);
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		pDeviceContext->IASetInputLayout(m_pVertexLayout);
		pDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 0);
		pDeviceContext->OMSetBlendState(m_pBlendState, nullptr, 0xffffffff);

		UINT Stride = sizeof(VERTEX);
		UINT Offset = 0;
		pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &Stride, &Offset);

		ID3D11ShaderResourceView* pPuddleResource = pTextureManager->GetTexture(m_PuddleTextureIndex)->Get();
		ID3D11ShaderResourceView* pSkyResource = pTextureManager->GetTexture(m_SkyCLUTIndex)->Get();

		// テクスチャリソースの設定.
		pDeviceContext->PSSetShaderResources(0, 1, &m_pCubeTextureResource);
		pDeviceContext->PSSetShaderResources(1, 1, &pPuddleResource);
		pDeviceContext->PSSetShaderResources(2, 1, &pSkyResource);
		pDeviceContext->PSSetShaderResources(3, 1, &m_pWaveShaderResourceView[m_WaveRenderIndex ^ 1]);
		pDeviceContext->PSSetShaderResources(4, 1, &m_pBumpShaderResourceView);

		// 定数バッファの設定.
		pDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
		pDeviceContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);

		pDeviceContext->Draw(VERTEX_NUM, 0);

		m_pFont->Draw(&m_DefaultFontPos, "Water : CubeMap");
		m_pFont->Draw(&D3DXVECTOR2(m_DefaultFontPos.x + 320, m_DefaultFontPos.y), "T key");
	}
	else
	{
		WaveDraw(); // 波マップの描画.
		BumpDraw();	// 法線マップの描画.

		pGraphicdDevice->SetScene(Lib::GraphicsDevice::BACKBUFFER_TARGET);	// 描画先を設定.

		pDeviceContext->VSSetShader(pShaderManager->GetVertexShader(m_ReflectVertexShaderIndex), nullptr, 0);
		pDeviceContext->PSSetShader(pShaderManager->GetPixelShader(m_ReflectPixelShaderIndex), nullptr, 0);
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		pDeviceContext->IASetInputLayout(m_pVertexLayout);
		pDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 0);
		pDeviceContext->OMSetBlendState(m_pBlendState, nullptr, 0xffffffff);

		UINT Stride = sizeof(VERTEX);
		UINT Offset = 0;
		pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &Stride, &Offset);

		ID3D11ShaderResourceView* pPuddleResource = pTextureManager->GetTexture(m_PuddleTextureIndex)->Get();
		ID3D11ShaderResourceView* pSkyResource = pTextureManager->GetTexture(m_SkyCLUTIndex)->Get();
		ID3D11ShaderResourceView* pColorResource = pTextureManager->GetTexture(m_WaterColorIndex)->Get();

		// テクスチャリソースの設定.
		pDeviceContext->PSSetShaderResources(0, 1, &m_pReflectShaderResourceView);
		pDeviceContext->PSSetShaderResources(1, 1, &pPuddleResource);
		pDeviceContext->PSSetShaderResources(2, 1, &pSkyResource);
		pDeviceContext->PSSetShaderResources(3, 1, &m_pWaveShaderResourceView[m_WaveRenderIndex ^ 1]);
		pDeviceContext->PSSetShaderResources(4, 1, &m_pBumpShaderResourceView);
		pDeviceContext->PSSetShaderResources(5, 1, &pColorResource);

		// 定数バッファの設定.
		pDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
		pDeviceContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);

		pDeviceContext->Draw(VERTEX_NUM, 0);

		m_pFont->Draw(&m_DefaultFontPos, "Water : ReflectMap");
		m_pFont->Draw(&D3DXVECTOR2(m_DefaultFontPos.x + 320, m_DefaultFontPos.y), "T key");
	}
}

void Water::WaveDraw()
{
	Lib::GraphicsDevice* pGraphicdDevice = SINGLETON_INSTANCE(Lib::GraphicsDevice);
	ID3D11DeviceContext* pDeviceContext = SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext();
	Lib::ShaderManager* pShaderManager = SINGLETON_INSTANCE(Lib::ShaderManager);

	// 描画先を波マップに変更.
	pGraphicdDevice->SetRenderTarget(&m_pWaveRenderTarget[m_WaveRenderIndex], m_WaveRenderTargetStage);
	pGraphicdDevice->SetDepthStencil(&m_pWaveDepthStencilView[m_WaveRenderIndex], m_WaveRenderTargetStage);
	pGraphicdDevice->SetClearColor(m_WaterClearColor, m_WaveRenderTargetStage);
	pGraphicdDevice->SetViewPort(&m_ViewPort, m_WaveRenderTargetStage);
	pGraphicdDevice->BeginScene(m_WaveRenderTargetStage);

	// 描画準備.
	pDeviceContext->VSSetShader(pShaderManager->GetVertexShader(m_WaveVertexShaderIndex), nullptr, 0);
	pDeviceContext->PSSetShader(pShaderManager->GetPixelShader(m_WavePixelShaderIndex), nullptr, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	pDeviceContext->IASetInputLayout(m_pWaveVertexLayout);
	pDeviceContext->OMSetDepthStencilState(nullptr, 0);
	pDeviceContext->OMSetBlendState(m_pBlendState, nullptr, 0xffffffff);

	UINT Stride = sizeof(MAP_VERTEX);
	UINT Offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pWaveVertexBuffer, &Stride, &Offset);
	pDeviceContext->PSSetShaderResources(0, 1, &m_pWaveShaderResourceView[m_WaveRenderIndex ^ 1]);
	pDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	pDeviceContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	pDeviceContext->Draw(VERTEX_NUM, 0);

	m_WaveRenderIndex ^= 1; // 描画先のテクスチャを入れ替える.
}

void Water::BumpDraw()
{
	Lib::GraphicsDevice* pGraphicdDevice = SINGLETON_INSTANCE(Lib::GraphicsDevice);
	ID3D11DeviceContext* pContext = SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext();
	Lib::ShaderManager*	pShaderManager = SINGLETON_INSTANCE(Lib::ShaderManager);

	// 描画先を法線マップに変更.
	pGraphicdDevice->SetRenderTarget(&m_pBumpRenderTarget, m_BumpRenderTargetStage);
	pGraphicdDevice->SetDepthStencil(&m_pWaveDepthStencilView[m_WaveRenderIndex], m_BumpRenderTargetStage);
	pGraphicdDevice->SetClearColor(m_ClearColor, m_BumpRenderTargetStage);
	pGraphicdDevice->SetViewPort(&m_ViewPort, m_BumpRenderTargetStage);
	pGraphicdDevice->BeginScene(m_BumpRenderTargetStage);

	// 描画準備.
	pContext->VSSetShader(pShaderManager->GetVertexShader(m_WaveVertexShaderIndex), nullptr, 0);
	pContext->PSSetShader(pShaderManager->GetPixelShader(m_BumpPixelShaderIndex), nullptr, 0);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	pContext->IASetInputLayout(m_pWaveVertexLayout);
	pContext->OMSetDepthStencilState(m_pDepthStencilState, 0);
	pContext->OMSetBlendState(m_pBlendState, nullptr, 0xffffffff);

	UINT Stride = sizeof(MAP_VERTEX);
	UINT Offset = 0;
	pContext->IASetVertexBuffers(0, 1, &m_pWaveVertexBuffer, &Stride, &Offset);
	pContext->PSSetShaderResources(0, 1, &m_pWaveShaderResourceView[m_WaveRenderIndex]);
	pContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	pContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	pContext->VSSetConstantBuffers(5, 1, &m_pReflectMapConstantBuffer);
	pContext->PSSetConstantBuffers(5, 1, &m_pReflectMapConstantBuffer);

	pContext->Draw(VERTEX_NUM, 0);
}


//----------------------------------------------------------------------
// Private Functions
//----------------------------------------------------------------------
bool Water::CreateVertexBuffer()
{
	Lib::GraphicsDevice* pGraphicdDevice = SINGLETON_INSTANCE(Lib::GraphicsDevice);

	D3DXVECTOR3 Normal = D3DXVECTOR3(0, 1, 0);
	D3DXVECTOR3 Tangent = D3DXVECTOR3(1, 0, 0);
	D3DXVECTOR3 Binormal = D3DXVECTOR3(0, 0, 1);

	VERTEX VertexData[VERTEX_NUM] =
	{
		VERTEX{ D3DXVECTOR3(-m_DefaultSize.x, 0.1f,  m_DefaultSize.y), D3DXVECTOR2(0, 0), Normal, Tangent, Binormal },
		VERTEX{ D3DXVECTOR3( m_DefaultSize.x, 0.1f,  m_DefaultSize.y), D3DXVECTOR2(1, 0), Normal, Tangent, Binormal },
		VERTEX{ D3DXVECTOR3(-m_DefaultSize.x, 0.1f, -m_DefaultSize.y), D3DXVECTOR2(0, 1), Normal, Tangent, Binormal },
		VERTEX{ D3DXVECTOR3( m_DefaultSize.x, 0.1f, -m_DefaultSize.y), D3DXVECTOR2(1, 1), Normal, Tangent, Binormal }
	};

	for (int i = 0; i < VERTEX_NUM; i++)
	{
		m_pVertexData[i].Pos = VertexData[i].Pos;
		m_pVertexData[i].UV = VertexData[i].UV;
		m_pVertexData[i].Normal = VertexData[i].Normal;
		m_pVertexData[i].Tangent = VertexData[i].Tangent;
		m_pVertexData[i].Binormal = VertexData[i].Binormal;
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

	if (FAILED(pGraphicdDevice->GetDevice()->CreateBuffer(
		&BufferDesc,
		&ResourceData,
		&m_pVertexBuffer)))
	{
		OutputErrorLog("頂点バッファの生成に失敗しました");
		return false;
	}


	MAP_VERTEX WaveVertexData[VERTEX_NUM] =
	{
		MAP_VERTEX{ D3DXVECTOR3(-m_WaveTextureWidth / 2, -m_WaveTextureHeight / 2, 0), D3DXVECTOR2(0, 0) },
		MAP_VERTEX{ D3DXVECTOR3( m_WaveTextureWidth / 2, -m_WaveTextureHeight / 2, 0), D3DXVECTOR2(1, 0) },
		MAP_VERTEX{ D3DXVECTOR3(-m_WaveTextureWidth / 2,  m_WaveTextureHeight / 2, 0), D3DXVECTOR2(0, 1) },
		MAP_VERTEX{ D3DXVECTOR3( m_WaveTextureWidth / 2,  m_WaveTextureHeight / 2, 0), D3DXVECTOR2(1, 1) }
	};

	for (int i = 0; i < VERTEX_NUM; i++)
	{
		m_pWaveVertexData[i].Pos = WaveVertexData[i].Pos;
		m_pWaveVertexData[i].UV = WaveVertexData[i].UV;
	}

	// 頂点バッファの設定.
	D3D11_BUFFER_DESC WaveBufferDesc;
	ZeroMemory(&WaveBufferDesc, sizeof(D3D11_BUFFER_DESC));
	WaveBufferDesc.ByteWidth = sizeof(MAP_VERTEX) * VERTEX_NUM;
	WaveBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	WaveBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	WaveBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	WaveBufferDesc.MiscFlags = 0;
	WaveBufferDesc.StructureByteStride = 0;

	// 頂点バッファに格納するデータの設定.
	D3D11_SUBRESOURCE_DATA WaveResourceData;
	ZeroMemory(&WaveResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	WaveResourceData.pSysMem = m_pWaveVertexData;

	if (FAILED(pGraphicdDevice->GetDevice()->CreateBuffer(
		&WaveBufferDesc,
		&WaveResourceData,
		&m_pWaveVertexBuffer)))
	{
		OutputErrorLog("頂点バッファの生成に失敗しました");
		return false;
	}

	return true;
}

bool Water::CreateShader()
{
	// 頂点シェーダー読み込み
	auto VertexShaderLoad = [this](LPCTSTR _pShaderName, LPCTSTR _pFuncName, int* _pIndex)
	{
		return SINGLETON_INSTANCE(Lib::ShaderManager)->LoadVertexShader(
			TEXT(_pShaderName),
			TEXT(_pFuncName),
			_pIndex);
	};

	// ピクセルシェーダー読み込み
	auto PixelShaderLoad = [this](LPCTSTR _pShaderName, LPCTSTR _pFuncName, int* _pIndex)
	{
		return SINGLETON_INSTANCE(Lib::ShaderManager)->LoadPixelShader(
			TEXT(_pShaderName),
			TEXT(_pFuncName),
			_pIndex);
	};


	if (!VertexShaderLoad("Resource\\Effect\\WaterCube.fx", "VS", &m_CubeVertexShaderIndex))
	{
		OutputErrorLog("キューブマップ頂点シェーダーの読み込みに失敗しました");
		return false;
	}

	if (!PixelShaderLoad("Resource\\Effect\\WaterCube.fx", "PS", &m_CubePixelShaderIndex))
	{
		OutputErrorLog("キューブマップピクセルシェーダーの読み込みに失敗しました");
		return false;
	}

	if (!VertexShaderLoad("Resource\\Effect\\WaterReflect.fx", "VS", &m_ReflectVertexShaderIndex))
	{
		OutputErrorLog("反射頂点シェーダーの読み込みに失敗しました");
		return false;
	}

	if (!PixelShaderLoad("Resource\\Effect\\WaterReflect.fx", "PS", &m_ReflectPixelShaderIndex))
	{
		OutputErrorLog("反射ピクセルシェーダーの読み込みに失敗しました");
		return false;
	}

	if (!VertexShaderLoad("Resource\\Effect\\Wave.fx", "VS", &m_WaveVertexShaderIndex))
	{
		OutputErrorLog("波マップと法線マップ生成用頂点シェーダーの読み込みに失敗しました");
		return false;
	}

	if (!PixelShaderLoad("Resource\\Effect\\Wave.fx", "PS_WAVEMAP", &m_WavePixelShaderIndex))
	{
		OutputErrorLog("波マップ生成用ピクセルシェーダーの読み込みに失敗しました");
		return false;
	}

	if (!PixelShaderLoad("Resource\\Effect\\Wave.fx", "PS_BUMPMAP", &m_BumpPixelShaderIndex))
	{
		OutputErrorLog("法線マップ生成用ピクセルシェーダーの読み込みに失敗しました");
		return false;
	}

	return true;
}

bool Water::CreateVertexLayout()
{
	Lib::GraphicsDevice* pGraphicdDevice = SINGLETON_INSTANCE(Lib::GraphicsDevice);
	Lib::ShaderManager* pShaderManager = SINGLETON_INSTANCE(Lib::ShaderManager);

	D3D11_INPUT_ELEMENT_DESC InputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (FAILED(pGraphicdDevice->GetDevice()->CreateInputLayout(
		InputElementDesc,
		sizeof(InputElementDesc) / sizeof(InputElementDesc[0]),
		pShaderManager->GetCompiledVertexShader(m_CubeVertexShaderIndex)->GetBufferPointer(),
		pShaderManager->GetCompiledVertexShader(m_CubeVertexShaderIndex)->GetBufferSize(),
		&m_pVertexLayout)))
	{
		OutputErrorLog("入力レイアウトの生成に失敗しました");
		return false;
	}


	D3D11_INPUT_ELEMENT_DESC WaveInputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (FAILED(pGraphicdDevice->GetDevice()->CreateInputLayout(
		WaveInputElementDesc,
		sizeof(WaveInputElementDesc) / sizeof(WaveInputElementDesc[0]),
		pShaderManager->GetCompiledVertexShader(m_WaveVertexShaderIndex)->GetBufferPointer(),
		pShaderManager->GetCompiledVertexShader(m_WaveVertexShaderIndex)->GetBufferSize(),
		&m_pWaveVertexLayout)))
	{
		OutputErrorLog("入力レイアウトの生成に失敗しました");
		return false;
	}

	return true;
}

bool Water::CreateState()
{
	Lib::GraphicsDevice* pGraphicdDevice = SINGLETON_INSTANCE(Lib::GraphicsDevice);

	// ブレンドステートの生成.
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

	if (FAILED(pGraphicdDevice->GetDevice()->CreateBlendState(
		&BlendDesc,
		&m_pBlendState)))
	{
		OutputErrorLog("ブレンドステートの生成に失敗しました");
		return false;
	}

	D3D11_DEPTH_STENCIL_DESC DepthStencilDesc;
	ZeroMemory(&DepthStencilDesc, sizeof(DepthStencilDesc));
	DepthStencilDesc.DepthEnable = TRUE;
	DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	DepthStencilDesc.StencilEnable = FALSE;

	if (FAILED(pGraphicdDevice->GetDevice()->CreateDepthStencilState(
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
	Lib::GraphicsDevice* pGraphicdDevice = SINGLETON_INSTANCE(Lib::GraphicsDevice);

	// 水の定数バッファ生成.
	D3D11_BUFFER_DESC WaterConstantBufferDesc;
	WaterConstantBufferDesc.ByteWidth = sizeof(CONSTANT_BUFFER);
	WaterConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	WaterConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	WaterConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	WaterConstantBufferDesc.MiscFlags = 0;
	WaterConstantBufferDesc.StructureByteStride = 0;

	if (FAILED(pGraphicdDevice->GetDevice()->CreateBuffer(
		&WaterConstantBufferDesc,
		nullptr,
		&m_pConstantBuffer)))
	{
		OutputErrorLog("水の描画に関する定数バッファ生成に失敗しました");
		return false;
	}

	// キューブマップの定数バッファ生成.
	D3D11_BUFFER_DESC CubeMapConstantBufferDesc;
	CubeMapConstantBufferDesc.ByteWidth = sizeof(CUBE_CONSTANT_BUFFER);
	CubeMapConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	CubeMapConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	CubeMapConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	CubeMapConstantBufferDesc.MiscFlags = 0;
	CubeMapConstantBufferDesc.StructureByteStride = 0;

	if (FAILED(pGraphicdDevice->GetDevice()->CreateBuffer(
		&CubeMapConstantBufferDesc,
		nullptr, 
		&m_pCubeMapConstantBuffer)))
	{
		OutputErrorLog("キューブマップ作成に関する定数バッファ生成に失敗しました");
		return false;
	}

	// 反射マップの定数バッファ生成.
	D3D11_BUFFER_DESC ReflectMapConstantBufferDesc;
	ReflectMapConstantBufferDesc.ByteWidth = sizeof(REFLECT_CONSTANT_BUFFER);
	ReflectMapConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	ReflectMapConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ReflectMapConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ReflectMapConstantBufferDesc.MiscFlags = 0;
	ReflectMapConstantBufferDesc.StructureByteStride = 0;

	if (FAILED(pGraphicdDevice->GetDevice()->CreateBuffer(
		&ReflectMapConstantBufferDesc,
		nullptr,
		&m_pReflectMapConstantBuffer)))
	{
		OutputErrorLog("反射マップ作成に関する定数バッファ生成に失敗しました");
		return false;
	}

	return true;
}

bool Water::CreateTexture()
{
	// テクスチャの読み込み.
	auto LoadTexture = [this](LPCTSTR _pTextureName, int* _pIndex)
	{
		return SINGLETON_INSTANCE(Lib::TextureManager)->LoadTexture(
			TEXT(_pTextureName),
			TEXT(_pIndex));
	};


	// 水たまりテクスチャの読み込み.
	if (!LoadTexture("Resource\\Texture\\Water.png", &m_PuddleTextureIndex))
	{
		OutputErrorLog("水たまりテクスチャの読み込みに失敗しました");
		return false;
	}

	// 空のカラーテーブル読み込み.
	if (!LoadTexture("Resource\\Texture\\MainLightCLUT.png", &m_SkyCLUTIndex))
	{
		OutputErrorLog("空のカラーテーブルの読み込みに失敗しました");
		return false;
	}

	// 空のカラーテクスチャ読み込み.
	if (!LoadTexture("Resource\\Texture\\WaterColor.png", &m_WaterColorIndex))
	{
		OutputErrorLog("空のカラーテクスチャの読み込みに失敗しました");
		return false;
	}

	if (!CreateCubeMapTexture())
	{
		return false;
	}

	if (!CreateWaveMapTexture())
	{
		return false;
	}

	if (!CreateReflectMapTexture())
	{
		return false;
	}

	return true;
}

bool Water::CreateCubeMapTexture()
{
	Lib::GraphicsDevice* pGraphicdDevice = SINGLETON_INSTANCE(Lib::GraphicsDevice);

	// キューブマップテクスチャ.
	D3D11_TEXTURE2D_DESC CubeTextureDesc;
	ZeroMemory(&CubeTextureDesc, sizeof(CubeTextureDesc));
	CubeTextureDesc.Width = static_cast<UINT>(m_CubeTextureWidth);
	CubeTextureDesc.Height = static_cast<UINT>(m_CubeTextureHeight);
	CubeTextureDesc.MipLevels = 1;
	CubeTextureDesc.ArraySize = 6;	// キューブマップなので6方向分用意.
	CubeTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	CubeTextureDesc.SampleDesc.Count = 1;
	CubeTextureDesc.SampleDesc.Quality = 0;
	CubeTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	CubeTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	CubeTextureDesc.CPUAccessFlags = 0;
	CubeTextureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE; // キューブマップ指定.

	if (FAILED(pGraphicdDevice->GetDevice()->CreateTexture2D(
		&CubeTextureDesc,
		nullptr,
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
	CubeRenderTargetDesc.Texture2DArray.ArraySize = 6;	// キューブマップなので6つ.

	if (FAILED(pGraphicdDevice->GetDevice()->CreateRenderTargetView(
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
	CubeResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;	// キューブテクスチャ指定.
	CubeResourceDesc.TextureCube.MipLevels = CubeTextureDesc.MipLevels;
	CubeResourceDesc.TextureCube.MostDetailedMip = 0;

	if (FAILED(pGraphicdDevice->GetDevice()->CreateShaderResourceView(
		m_pCubeTexture,
		&CubeResourceDesc,
		&m_pCubeTextureResource)))
	{
		OutputErrorLog("シェーダーリソースビューの生成に失敗しました");
		return false;
	}


	// 深度ステンシルテクスチャの生成.
	D3D11_TEXTURE2D_DESC DepthStencilTextureDesc;
	DepthStencilTextureDesc.Width = static_cast<UINT>(m_CubeTextureWidth);
	DepthStencilTextureDesc.Height = static_cast<UINT>(m_CubeTextureHeight);
	DepthStencilTextureDesc.MipLevels = 1;
	DepthStencilTextureDesc.ArraySize = 6;
	DepthStencilTextureDesc.Format = DXGI_FORMAT_D32_FLOAT;
	DepthStencilTextureDesc.SampleDesc.Count = 1;
	DepthStencilTextureDesc.SampleDesc.Quality = 0;
	DepthStencilTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DepthStencilTextureDesc.CPUAccessFlags = 0;
	DepthStencilTextureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	if (FAILED(pGraphicdDevice->GetDevice()->CreateTexture2D(
		&DepthStencilTextureDesc,
		nullptr,
		&m_pDepthStencilTexture)))
	{
		OutputErrorLog("深度ステンシルテクスチャの生成に失敗しました");
		return false;
	}

	// 深度ステンシルビューの生成.
	if (FAILED(pGraphicdDevice->GetDevice()->CreateDepthStencilView(
		m_pDepthStencilTexture,
		nullptr,
		&m_pDepthStencilView)))
	{
		OutputErrorLog("深度ステンシルビューの生成に失敗しました");
		return false;
	}

	// ビューポート設定.
	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;
	m_ViewPort.Width = m_CubeTextureWidth;
	m_ViewPort.Height = m_CubeTextureHeight;
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;

	// グラフィックデバイスに描画先として深度テクスチャを設定.
	pGraphicdDevice->SetRenderTarget(&m_pCubeTextureRenderTarget, m_CubeRenderTargetStage);
	pGraphicdDevice->SetDepthStencil(&m_pDepthStencilView, m_CubeRenderTargetStage);
	pGraphicdDevice->SetClearColor(m_ClearColor, m_CubeRenderTargetStage);
	pGraphicdDevice->SetViewPort(&m_ViewPort, m_CubeRenderTargetStage);


	D3DXVECTOR3 EyePos = D3DXVECTOR3(0, 0, 0);
	D3DXVECTOR3 LookPos = D3DXVECTOR3(0, 0, 0);
	D3DXVECTOR3 UpVec = D3DXVECTOR3(0, 1, 0);
	float ViewAngle = 90;

	m_pCamera = new Lib::Camera(m_CubeTextureWidth, m_CubeTextureHeight, 1, 700);

	// 6方面View行列を生成する.
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
		OutputErrorLog("定数バッファの書き込みに失敗しました");
		return false;
	}

	return true;
}

bool Water::CreateWaveMapTexture()
{
	Lib::GraphicsDevice* pGraphicdDevice = SINGLETON_INSTANCE(Lib::GraphicsDevice);

	for (int i = 0; i < WAVE_TEXTURE_NUM; i++)
	{
		// 波テクスチャの生成処理.
		D3D11_TEXTURE2D_DESC WaveMapTextureDesc;
		ZeroMemory(&WaveMapTextureDesc, sizeof(WaveMapTextureDesc));
		WaveMapTextureDesc.Width = static_cast<UINT>(m_WaveTextureWidth);
		WaveMapTextureDesc.Height = static_cast<UINT>(m_WaveTextureHeight);
		WaveMapTextureDesc.MipLevels = 1;
		WaveMapTextureDesc.ArraySize = 1;
		WaveMapTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		WaveMapTextureDesc.SampleDesc.Count = 1;
		WaveMapTextureDesc.SampleDesc.Quality = 0;
		WaveMapTextureDesc.Usage = D3D11_USAGE_DEFAULT;
		WaveMapTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		WaveMapTextureDesc.CPUAccessFlags = 0;
		WaveMapTextureDesc.MiscFlags = 0;

		if (FAILED(pGraphicdDevice->GetDevice()->CreateTexture2D(
			&WaveMapTextureDesc,
			nullptr,
			&m_pWaveTexture[i])))
		{
			OutputErrorLog("波テクスチャ生成に失敗しました");
			return false;
		}

		if (FAILED(pGraphicdDevice->GetDevice()->CreateRenderTargetView(
			m_pWaveTexture[i],
			nullptr,
			&m_pWaveRenderTarget[i])))
		{
			OutputErrorLog("波テクスチャのレンダーターゲットビューの設定に失敗しました");
			return false;
		}

		if (FAILED(pGraphicdDevice->GetDevice()->CreateShaderResourceView(
			m_pWaveTexture[i],
			nullptr,
			&m_pWaveShaderResourceView[i])))
		{
			OutputErrorLog("波テクスチャのシェーダーリソースビューの生成に失敗しました");
			return false;
		}

		D3D11_TEXTURE2D_DESC DepthStencilDesc;
		DepthStencilDesc.Width = static_cast<UINT>(m_WaveTextureHeight);
		DepthStencilDesc.Height = static_cast<UINT>(m_WaveTextureWidth);
		DepthStencilDesc.MipLevels = 1;
		DepthStencilDesc.ArraySize = 1;
		DepthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
		DepthStencilDesc.SampleDesc.Count = 1;
		DepthStencilDesc.SampleDesc.Quality = 0;
		DepthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		DepthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		DepthStencilDesc.CPUAccessFlags = 0;
		DepthStencilDesc.MiscFlags = 0;

		if (FAILED(pGraphicdDevice->GetDevice()->CreateTexture2D(
			&DepthStencilDesc,
			nullptr,
			&m_pWaveDepthStencilTexture[i])))
		{
			OutputErrorLog("深度ステンシルテクスチャ生成に失敗しました");
			return false;
		}

		if (FAILED(pGraphicdDevice->GetDevice()->CreateDepthStencilView(
			m_pWaveDepthStencilTexture[i],
			nullptr,
			&m_pWaveDepthStencilView[i])))
		{
			OutputErrorLog("深度ステンシルテクスチャのデプスステンシルビューの生成に失敗しました");
			return false;
		}
	}

	// 波テクスチャをグラフィックデバイスに設定.
	pGraphicdDevice->SetRenderTarget(&m_pWaveRenderTarget[m_WaveRenderIndex], m_WaveRenderTargetStage);
	pGraphicdDevice->SetDepthStencil(&m_pWaveDepthStencilView[m_WaveRenderIndex], m_WaveRenderTargetStage);
	pGraphicdDevice->SetClearColor(m_WaterClearColor, m_WaveRenderTargetStage);
	pGraphicdDevice->SetViewPort(&m_ViewPort, m_WaveRenderTargetStage);
	pGraphicdDevice->BeginScene(m_WaveRenderTargetStage);

	// 2つ目の波テクスチャもグラフィックデバイスに設定.
	pGraphicdDevice->SetRenderTarget(&m_pWaveRenderTarget[m_WaveRenderIndex ^ 1], m_WaveRenderTargetStage);
	pGraphicdDevice->SetDepthStencil(&m_pWaveDepthStencilView[m_WaveRenderIndex ^ 1], m_WaveRenderTargetStage);
	pGraphicdDevice->SetClearColor(m_WaterClearColor, m_WaveRenderTargetStage);
	pGraphicdDevice->SetViewPort(&m_ViewPort, m_WaveRenderTargetStage);
	pGraphicdDevice->BeginScene(m_WaveRenderTargetStage);



	// 法線マップ生成処理.
	D3D11_TEXTURE2D_DESC BumpMapTextureDesc;
	ZeroMemory(&BumpMapTextureDesc, sizeof(BumpMapTextureDesc));
	BumpMapTextureDesc.Width = static_cast<UINT>(m_WaveTextureWidth);
	BumpMapTextureDesc.Height = static_cast<UINT>(m_WaveTextureHeight);
	BumpMapTextureDesc.MipLevels = 1;
	BumpMapTextureDesc.ArraySize = 1;
	BumpMapTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	BumpMapTextureDesc.SampleDesc.Count = 1;
	BumpMapTextureDesc.SampleDesc.Quality = 0;
	BumpMapTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	BumpMapTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	BumpMapTextureDesc.CPUAccessFlags = 0;
	BumpMapTextureDesc.MiscFlags = 0;

	if (FAILED(pGraphicdDevice->GetDevice()->CreateTexture2D(
		&BumpMapTextureDesc,
		nullptr,
		&m_pBumpTexture)))
	{
		OutputErrorLog("法線マップ生成に失敗しました");
		return false;
	}

	if (FAILED(pGraphicdDevice->GetDevice()->CreateRenderTargetView(
		m_pBumpTexture,
		nullptr,
		&m_pBumpRenderTarget)))
	{
		OutputErrorLog("法線マップのレンダーターゲットビューの設定に失敗しました");
		return false;
	}

	if (FAILED(pGraphicdDevice->GetDevice()->CreateShaderResourceView(
		m_pBumpTexture,
		nullptr,
		&m_pBumpShaderResourceView)))
	{
		OutputErrorLog("法線マップのシェーダーリソースビューの生成に失敗しました");
		return false;
	}

	return true;
}

bool Water::CreateReflectMapTexture()
{
	Lib::GraphicsDevice* pGraphicdDevice = SINGLETON_INSTANCE(Lib::GraphicsDevice);

	// 波テクスチャの生成処理.
	D3D11_TEXTURE2D_DESC ReflectMapTextureDesc;
	ZeroMemory(&ReflectMapTextureDesc, sizeof(ReflectMapTextureDesc));
	ReflectMapTextureDesc.Width = static_cast<UINT>(m_ReflectTextureWidth);
	ReflectMapTextureDesc.Height = static_cast<UINT>(m_ReflectTextureHeight);
	ReflectMapTextureDesc.MipLevels = 1;
	ReflectMapTextureDesc.ArraySize = 1;
	ReflectMapTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	ReflectMapTextureDesc.SampleDesc.Count = 1;
	ReflectMapTextureDesc.SampleDesc.Quality = 0;
	ReflectMapTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	ReflectMapTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	ReflectMapTextureDesc.CPUAccessFlags = 0;
	ReflectMapTextureDesc.MiscFlags = 0;

	if (FAILED(pGraphicdDevice->GetDevice()->CreateTexture2D(
		&ReflectMapTextureDesc,
		nullptr,
		&m_pReflectTexture)))
	{
		OutputErrorLog("反射テクスチャ生成に失敗しました");
		return false;
	}

	if (FAILED(pGraphicdDevice->GetDevice()->CreateRenderTargetView(
		m_pReflectTexture,
		nullptr,
		&m_pReflectRenderTarget)))
	{
		OutputErrorLog("反射テクスチャのレンダーターゲットビューの設定に失敗しました");
		return false;
	}

	if (FAILED(pGraphicdDevice->GetDevice()->CreateShaderResourceView(
		m_pReflectTexture,
		nullptr,
		&m_pReflectShaderResourceView)))
	{
		OutputErrorLog("反射テクスチャのシェーダーリソースビューの生成に失敗しました");
		return false;
	}

	D3D11_TEXTURE2D_DESC DepthStencilDesc;
	DepthStencilDesc.Width = static_cast<UINT>(m_ReflectTextureWidth);
	DepthStencilDesc.Height = static_cast<UINT>(m_ReflectTextureHeight);
	DepthStencilDesc.MipLevels = 1;
	DepthStencilDesc.ArraySize = 1;
	DepthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	DepthStencilDesc.SampleDesc.Count = 1;
	DepthStencilDesc.SampleDesc.Quality = 0;
	DepthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DepthStencilDesc.CPUAccessFlags = 0;
	DepthStencilDesc.MiscFlags = 0;

	if (FAILED(pGraphicdDevice->GetDevice()->CreateTexture2D(
		&DepthStencilDesc,
		nullptr,
		&m_pReflectDepthStencilTexture)))
	{
		OutputErrorLog("深度ステンシルテクスチャ生成に失敗しました");
		return false;
	}

	if (FAILED(pGraphicdDevice->GetDevice()->CreateDepthStencilView(
		m_pReflectDepthStencilTexture,
		nullptr,
		&m_pReflectDepthStencilView)))
	{
		OutputErrorLog("深度ステンシルテクスチャのデプスステンシルビューの生成に失敗しました");
		return false;
	}

	// ビューポート設定.
	m_ReflectViewPort.TopLeftX = 0;
	m_ReflectViewPort.TopLeftY = 0;
	m_ReflectViewPort.Width = m_ReflectTextureWidth;
	m_ReflectViewPort.Height = m_ReflectTextureHeight;
	m_ReflectViewPort.MinDepth = 0.0f;
	m_ReflectViewPort.MaxDepth = 1.0f;

	pGraphicdDevice->SetRenderTarget(&m_pReflectRenderTarget, m_ReflectRenderTargetStage);
	pGraphicdDevice->SetDepthStencil(&m_pReflectDepthStencilView, m_ReflectRenderTargetStage);
	pGraphicdDevice->SetClearColor(m_ClearColor, m_ReflectRenderTargetStage);
	pGraphicdDevice->SetViewPort(&m_ReflectViewPort, m_ReflectRenderTargetStage);


	if (!WriteReflectMapConstantBuffer())
	{
		OutputErrorLog("定数バッファの書き込みに失敗しました");
		return false;
	}

	return true;
}

bool Water::CreateFontObject()
{
	m_pFont = new Lib::Font();
	if (!m_pFont->Initialize(SINGLETON_INSTANCE(Lib::GraphicsDevice)))
	{
		return false;
	}

	if (!m_pFont->CreateVertexBuffer(&m_DefaultFontSize, &m_DefaultFontColor))
	{
		return false;
	}

	return true;
}

void Water::ReleaseVertexBuffer()
{
	SafeRelease(m_pWaveVertexBuffer);
	SafeRelease(m_pVertexBuffer);
}

void Water::ReleaseShader()
{
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleasePixelShader(m_BumpPixelShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleasePixelShader(m_WavePixelShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleaseVertexShader(m_WaveVertexShaderIndex);

	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleasePixelShader(m_ReflectPixelShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleaseVertexShader(m_ReflectVertexShaderIndex);

	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleasePixelShader(m_CubePixelShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleaseVertexShader(m_CubeVertexShaderIndex);
}

void Water::ReleaseVertexLayout()
{
	SafeRelease(m_pWaveVertexLayout);
	SafeRelease(m_pVertexLayout);
}

void Water::ReleaseState()
{
	SafeRelease(m_pDepthStencilState);
	SafeRelease(m_pBlendState);
}

void Water::ReleaseConstantBuffer()
{
	SafeRelease(m_pReflectMapConstantBuffer);
	SafeRelease(m_pCubeMapConstantBuffer);
	SafeRelease(m_pConstantBuffer);
}

void Water::ReleaseTexture()
{
	ReleaseReflectMapTexture();
	ReleaseWaveMapTexture();
	ReleaseCubeMapTexture();

	SINGLETON_INSTANCE(Lib::TextureManager)->ReleaseTexture(m_WaterColorIndex);
	SINGLETON_INSTANCE(Lib::TextureManager)->ReleaseTexture(m_SkyCLUTIndex);
	SINGLETON_INSTANCE(Lib::TextureManager)->ReleaseTexture(m_PuddleTextureIndex);
}

void Water::ReleaseCubeMapTexture()
{
	delete m_pCamera;
	SafeRelease(m_pDepthStencilView);
	SafeRelease(m_pDepthStencilTexture);
	SafeRelease(m_pCubeTextureResource);
	SafeRelease(m_pCubeTextureRenderTarget);
	SafeRelease(m_pCubeTexture);
}

void Water::ReleaseWaveMapTexture()
{
	SafeRelease(m_pBumpShaderResourceView);
	SafeRelease(m_pBumpRenderTarget);
	SafeRelease(m_pBumpTexture);

	for (int i = 0; i < WAVE_TEXTURE_NUM; i++)
	{
		SafeRelease(m_pWaveDepthStencilView[i]);
		SafeRelease(m_pWaveDepthStencilTexture[i]);
		SafeRelease(m_pWaveShaderResourceView[i]);
		SafeRelease(m_pWaveRenderTarget[i]);
		SafeRelease(m_pWaveTexture[i]);
	}
}

void Water::ReleaseReflectMapTexture()
{
	SafeRelease(m_pReflectDepthStencilView);
	SafeRelease(m_pReflectDepthStencilTexture);
	SafeRelease(m_pReflectShaderResourceView);
	SafeRelease(m_pReflectRenderTarget);
	SafeRelease(m_pReflectTexture);
}

void Water::ReleaseFontObject()
{
	m_pFont->ReleaseVertexBuffer();
	m_pFont->Finalize();
	delete m_pFont;
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

		D3DXMATRIX MapMatWorld, MapMatTranslate;
		D3DXMatrixIdentity(&MapMatWorld);
		D3DXMatrixScaling(&MapMatWorld, 1.0f, 1.0f, 1.0f);
		D3DXMatrixTranslation(&MapMatTranslate, m_WaveTextureWidth / 2, m_WaveTextureHeight / 2, 0.0f);
		D3DXMatrixMultiply(&MapMatWorld, &MapMatWorld, &MapMatTranslate);
		D3DXMatrixTranspose(&MapMatWorld, &MapMatWorld);

		CONSTANT_BUFFER ConstantBuffer;
		ConstantBuffer.World = MatWorld;
		ConstantBuffer.TexelOffset = D3DXVECTOR4(1 / m_WaveTextureWidth, 1 / m_WaveTextureHeight, 0, 0);
		ConstantBuffer.AddWavePos = D3DXVECTOR4(m_AddWavePos.x, m_AddWavePos.y, 0, 0);
		ConstantBuffer.AddWaveHeight = D3DXVECTOR4(m_AddWaveHeight, 0, 0, 0);
		ConstantBuffer.MapWorld = MapMatWorld;

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
			m_CubeConstantBuffer.View[i] = m_ViewMat[i];
			D3DXMatrixTranspose(&m_CubeConstantBuffer.View[i], &m_CubeConstantBuffer.View[i]);
		}

		m_CubeConstantBuffer.Proj = m_ProjMat;
		D3DXMatrixTranspose(&m_CubeConstantBuffer.Proj, &m_CubeConstantBuffer.Proj);

		memcpy_s(
			SubResourceData.pData,
			SubResourceData.RowPitch,
			reinterpret_cast<void*>(&m_CubeConstantBuffer),
			sizeof(m_CubeConstantBuffer));

		SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->Unmap(m_pCubeMapConstantBuffer, 0);

		return true;
	}

	return false;
}

bool Water::WriteReflectMapConstantBuffer()
{
	D3D11_MAPPED_SUBRESOURCE SubResourceData;
	if (SUCCEEDED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->Map(
		m_pReflectMapConstantBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&SubResourceData)))
	{
		D3DXMATRIX ScalingMat;
		D3DXMATRIX TranslateMat;
		D3DXMatrixScaling(&ScalingMat, 0.5f, -0.5f, 1.0f);
		D3DXMatrixTranslation(&TranslateMat, 0.5f, 0.5f, 0.0f);

		m_ReflectConstantBuffer.ScalingMat = ScalingMat;
		m_ReflectConstantBuffer.TranslateMat = TranslateMat;

		D3DXMatrixTranspose(&m_ReflectConstantBuffer.ScalingMat, &m_ReflectConstantBuffer.ScalingMat);
		D3DXMatrixTranspose(&m_ReflectConstantBuffer.TranslateMat, &m_ReflectConstantBuffer.TranslateMat);

		memcpy_s(
			SubResourceData.pData,
			SubResourceData.RowPitch,
			reinterpret_cast<void*>(&m_ReflectConstantBuffer),
			sizeof(m_ReflectConstantBuffer));

		SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->Unmap(m_pReflectMapConstantBuffer, 0);

		return true;
	}

	return false;
}

void Water::CubeMapBeginScene()
{
	Lib::GraphicsDevice* pGraphicdDevice = SINGLETON_INSTANCE(Lib::GraphicsDevice);
	ID3D11DeviceContext* pDeviceContext = SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext();

	pGraphicdDevice->BeginScene(m_CubeRenderTargetStage);
	
	// キューブマップ定数バッファの更新と設定.
	WriteCubeMapConstantBuffer();
	pDeviceContext->VSSetConstantBuffers(5, 1, &m_pCubeMapConstantBuffer);
	pDeviceContext->GSSetConstantBuffers(5, 1, &m_pCubeMapConstantBuffer);
	pDeviceContext->PSSetConstantBuffers(5, 1, &m_pCubeMapConstantBuffer);
}

void Water::ReflectMapBeginScene()
{
	Lib::GraphicsDevice* pGraphicdDevice = SINGLETON_INSTANCE(Lib::GraphicsDevice);
	ID3D11DeviceContext* pDeviceContext = SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext();

	pGraphicdDevice->BeginScene(m_ReflectRenderTargetStage);

	// 反射マップ定数バッファの更新と設定.
	WriteReflectMapConstantBuffer();
	pDeviceContext->VSSetConstantBuffers(5, 1, &m_pReflectMapConstantBuffer);
	pDeviceContext->PSSetConstantBuffers(5, 1, &m_pReflectMapConstantBuffer);
}



//----------------------------------------------------------------------
// Inner Class Constructor Destructor
//----------------------------------------------------------------------
Water::CubeDrawBeginTask::CubeDrawBeginTask(Water* _pWater) :
	m_pWater(_pWater)
{
}

Water::CubeDrawBeginTask::~CubeDrawBeginTask()
{
}


//----------------------------------------------------------------------
// Inner Class Public Function
//----------------------------------------------------------------------
void Water::CubeDrawBeginTask::Run()
{
	m_pWater->CubeMapBeginScene();
}


//----------------------------------------------------------------------
// Inner Class Constructor Destructor
//----------------------------------------------------------------------
Water::ReflectDrawBeginTask::ReflectDrawBeginTask(Water* _pWater) :
	m_pWater(_pWater)
{
}

Water::ReflectDrawBeginTask::~ReflectDrawBeginTask()
{
}


//----------------------------------------------------------------------
// Inner Class Public Function
//----------------------------------------------------------------------
void Water::ReflectDrawBeginTask::Run()
{
	m_pWater->ReflectMapBeginScene();
}
