/**
 * @file	MainLight.cpp
 * @brief	メインライトクラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "MainLight.h"

#include "Debugger\Debugger.h"
#include "TaskManager\TaskBase\DrawStartUpTask\DrawStartUpTask.h"
#include "TaskManager\TaskBase\DrawTask\DrawTask.h"
#include "TaskManager\TaskBase\UpdateTask\UpdateTask.h"
#include "DirectX11\GraphicsDevice\Dx11GraphicsDevice.h"
#include "DirectX11\TextureManager\Dx11TextureManager.h"
#include "DirectX11\TextureManager\ITexture\Dx11ITexture.h"
#include "DirectX11\ShaderManager\Dx11ShaderManager.h"
#include "Main\Application\Scene\GameScene\Task\DepthDrawTask\DepthDrawTask.h"
#include "..\MainCamera\MainCamera.h"


//----------------------------------------------------------------------
// Static Private Variables
//----------------------------------------------------------------------
const D3DXVECTOR3 MainLight::m_DefaultLightPos = D3DXVECTOR3(150, 160, 170);
const D3DXVECTOR3 MainLight::m_DefaultLightDirPos = D3DXVECTOR3(0, 0, 0);
const D3DXVECTOR2 MainLight::m_DefaultSize = D3DXVECTOR2(50, 50);
const float MainLight::m_NearPoint = 1.f;
const float MainLight::m_FarPoint = 700.f;
const float MainLight::m_ViewAngle = 50.f;
const float MainLight::m_ClearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
const float MainLight::m_DepthTextureWidth = 9600.f;
const float MainLight::m_DepthTextureHeight = 5400.f;
const int MainLight::m_RenderTargetStage = 1;


//----------------------------------------------------------------------
// Constructor	Destructor
//----------------------------------------------------------------------
MainLight::MainLight(MainCamera* _pCamera) :
m_pLight(nullptr),
m_pCamera(_pCamera),
m_pDepthTexture(nullptr),
m_pRenderTarget(nullptr),
m_LightState(m_DefaultLightPos, 0.0f)
{
}

MainLight::~MainLight()
{
}


//----------------------------------------------------------------------
// Public Functions
//----------------------------------------------------------------------
bool MainLight::Initialize()
{
	if (!CreateTask())				return false;
	if (!CreateLight())				return false;
	if (!CreateConstantBuffer())	return false;
	if (!WriteConstantBuffer())		return false;
	if (!CreateDepthTexture())		return false;
	if (!CreateVertex())			return false;
	if (!CreateShader())			return false;
	if (!CreateVertexLayout())		return false;
	if (!CreateState())				return false;
	if (!CreateLightTexture())		return false;

	return true;
}

void MainLight::Finalize()
{
	ReleaseLightTexture();
	ReleaseState();
	ReleaseVertexLayout();
	ReleaseShader();
	ReleaseVertex();
	ReleaseDepthTexture();
	ReleaseConstantBuffer();
	ReleaseLight();
	ReleaseTask();
}

void MainLight::Update()
{
	m_LightState.Time += 0.03f;
	if (m_LightState.Time >= 360 || m_LightState.Pos.y <= 10)
	{
		m_LightState.Time = -40;	// 時間が一定に達するまたは太陽が落ちたら初期化.
	}

	// 現在の時間からライトの位置を更新.
	float Radian = static_cast<float>(D3DXToRadian(m_LightState.Time));
	m_LightState.Pos.x = m_DefaultLightPos.x * cos(Radian) - m_DefaultLightPos.y * sin(Radian);
	m_LightState.Pos.y = m_DefaultLightPos.y * sin(Radian) + m_DefaultLightPos.y * cos(Radian);

	m_pLight->SetPos(&m_LightState.Pos);
	m_pLight->SetDirectionPos(&m_DefaultLightDirPos);
	D3DXMatrixLookAtLH(&m_LightView, &m_LightState.Pos, &m_DefaultLightDirPos, &D3DXVECTOR3(0, 1, 0));
	WriteConstantBuffer();
}

void MainLight::DrawStartUp()
{
	SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDeviceContext()->PSSetShaderResources(2, 1, &m_pDepthStencilResource);
}

void MainLight::Draw()
{
	//--------------------描画--------------------
	ID3D11DeviceContext* pContext = SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDeviceContext();

	pContext->VSSetShader(SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->GetVertexShader(m_VertexShaderIndex), nullptr, 0);
	pContext->PSSetShader(SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->GetPixelShader(m_PixelShaderIndex), nullptr, 0);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	pContext->IASetInputLayout(m_pVertexLayout);
	pContext->OMSetDepthStencilState(m_pDepthStencilState, 0);
	pContext->OMSetBlendState(m_pBlendState, nullptr, 0xffffffff);

	UINT Stride = sizeof(VERTEX);
	UINT Offset = 0;
	pContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &Stride, &Offset);

	Lib::Dx11::TextureManager* pTextureManager = SINGLETON_INSTANCE(Lib::Dx11::TextureManager);
	ID3D11ShaderResourceView* pResource = pTextureManager->GetTexture(m_LightTextureIndex)->Get();
	ID3D11ShaderResourceView* pResource2 = pTextureManager->GetTexture(m_BloomTextureIndex)->Get();
	pContext->PSSetShaderResources(0, 1, &pResource);
	pContext->PSSetShaderResources(1, 1, &pResource2);

	pContext->Draw(VERTEX_NUM, 0);
}


//----------------------------------------------------------------------
// Private Functions
//----------------------------------------------------------------------
bool MainLight::CreateTask()
{
	// タスクオブジェクトの生成初期化処理.
	m_pDrawTask = new Lib::Draw3DTask();
	m_pUpdateTask = new Lib::UpdateTask();
	m_pDrawStartUpTask = new Lib::DrawStartUpTask();
	m_pDepthDrawStartUp = new DepthDrawStartUp(this);

	m_pDrawTask->SetObject(this);
	m_pUpdateTask->SetObject(this);
	m_pDrawStartUpTask->SetObject(this);

	m_pDrawTask->SetName("MainLight");
	m_pUpdateTask->SetName("MainLight");
	m_pDrawStartUpTask->SetName("MainLight");
	m_pDepthDrawStartUp->SetName("MainLight");

	// タスクオブジェクトを管理クラスに追加.
	SINGLETON_INSTANCE(Lib::Draw3DTaskManager)->AddTask(m_pDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->AddTask(m_pUpdateTask);
	SINGLETON_INSTANCE(Lib::Draw3DTaskManager)->AddStartUpTask(m_pDrawStartUpTask);
	SINGLETON_INSTANCE(DepthDrawTaskManager)->AddStartUpTask(m_pDepthDrawStartUp);

	return true;
}

bool MainLight::CreateLight()
{
	// ライトオブジェクトの生成 初期化.
	m_pLight = new Lib::Dx11::Light();
	m_pLight->SetPos(&m_LightState.Pos);
	m_pLight->SetDirectionPos(&m_DefaultLightDirPos);


	// ライト視点の行列生成.
	D3DXMatrixLookAtLH(&m_LightView, &m_LightState.Pos, &m_DefaultLightDirPos, &D3DXVECTOR3(0, 1, 0));

	const RECT* pWindowRect = SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetMainWindowRect();
	float WindowWidth = static_cast<float>(pWindowRect->right - pWindowRect->left);
	float WindowHeight = static_cast<float>(pWindowRect->bottom - pWindowRect->top);
	float Aspect = WindowWidth / WindowHeight;

	D3DXMatrixPerspectiveFovLH(
		&m_LightProj,
		static_cast<float>(D3DXToRadian(m_ViewAngle)),
		Aspect,
		m_NearPoint,
		m_FarPoint);

	return true;
}

bool MainLight::CreateConstantBuffer()
{
	// 定数バッファ生成初期化処理.
	D3D11_BUFFER_DESC ConstantBufferDesc;
	ConstantBufferDesc.ByteWidth = sizeof(LIGHT_CONSTANT_BUFFER);
	ConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ConstantBufferDesc.MiscFlags = 0;
	ConstantBufferDesc.StructureByteStride = 0;

	if (FAILED(SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDevice()->CreateBuffer(
		&ConstantBufferDesc, 
		nullptr,
		&m_pConstantBuffer)))
	{
		OutputErrorLog("定数バッファ生成に失敗しました");
		return false;
	}

	return true;
}

bool MainLight::CreateDepthTexture()
{
	// 深度テクスチャ生成初期化処理.
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

	if (FAILED(SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDevice()->CreateTexture2D(
		&DepthTextureDesc,
		nullptr,
		&m_pDepthTexture)))
	{
		OutputErrorLog("Z値テクスチャ生成に失敗しました");
		return false;
	}

	if (FAILED(SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDevice()->CreateRenderTargetView(
		m_pDepthTexture, 
		nullptr,
		&m_pRenderTarget)))
	{
		OutputErrorLog("Z値テクスチャから描画ターゲットの生成に失敗しました");
		return false;
	}

	if (FAILED(SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDevice()->CreateShaderResourceView(
		m_pDepthTexture, 
		nullptr, 
		&m_pDepthStencilResource)))
	{
		OutputErrorLog("シェーダーリソースビューの生成に失敗しました");
		return false;
	}


	// 深度ステンシルテクスチャ生成初期化処理.
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

	if (FAILED(FAILED(SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDevice()->CreateTexture2D(
		&DepthStencilDesc,
		nullptr,
		&m_pDepthStencilTexture))))
	{
		OutputErrorLog("深度ステンシルテクスチャ生成に失敗しました");
		return false;
	}

	if (FAILED(FAILED(SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDevice()->CreateDepthStencilView(
		m_pDepthStencilTexture,
		nullptr,
		&m_pDepthStencilView))))
	{
		OutputErrorLog("深度ステンシルテクスチャのデプスステンシルビューの生成に失敗しました");
		return false;
	}

	// ビューポート設定.
	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;
	m_ViewPort.Width = m_DepthTextureWidth;
	m_ViewPort.Height = m_DepthTextureHeight;
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;

	// グラフィックデバイスに描画先として深度テクスチャを設定.
	SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->SetRenderTarget(&m_pRenderTarget, m_RenderTargetStage);
	SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->SetDepthStencil(&m_pDepthStencilView, m_RenderTargetStage);
	SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->SetClearColor(m_ClearColor, m_RenderTargetStage);
	SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->SetViewPort(&m_ViewPort, m_RenderTargetStage);

	return true;
}

bool MainLight::CreateVertex()
{
	VERTEX VertexData[VERTEX_NUM] =
	{
		VERTEX{ D3DXVECTOR3(-m_DefaultSize.x / 2, -m_DefaultSize.y / 2, 0), D3DXVECTOR2(0, 0) },
		VERTEX{ D3DXVECTOR3( m_DefaultSize.x / 2, -m_DefaultSize.y / 2, 0), D3DXVECTOR2(1, 0) },
		VERTEX{ D3DXVECTOR3(-m_DefaultSize.x / 2,  m_DefaultSize.y / 2, 0), D3DXVECTOR2(0, 1) },
		VERTEX{ D3DXVECTOR3( m_DefaultSize.x / 2,  m_DefaultSize.y / 2, 0), D3DXVECTOR2(1, 1) }
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

	if (FAILED(SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDevice()->CreateBuffer(
		&BufferDesc,
		&ResourceData,
		&m_pVertexBuffer)))
	{
		OutputErrorLog("頂点バッファの生成に失敗しました");
		return false;
	}

	return true;
}

bool MainLight::CreateShader()
{
	if (!SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadVertexShader(
		TEXT("Resource\\Effect\\MainLight.fx"),
		"VS",
		&m_VertexShaderIndex))
	{
		OutputErrorLog("頂点シェーダーの読み込みに失敗しました");
		return false;
	}

	if (!SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadPixelShader(
		TEXT("Resource\\Effect\\MainLight.fx"),
		"PS",
		&m_PixelShaderIndex))
	{
		OutputErrorLog("ピクセルシェーダーの読み込みに失敗しました");
		return false;
	}

	return true;
}

bool MainLight::CreateVertexLayout()
{
	D3D11_INPUT_ELEMENT_DESC InputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (FAILED(SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDevice()->CreateInputLayout(
		InputElementDesc,
		sizeof(InputElementDesc) / sizeof(InputElementDesc[0]),
		SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->GetCompiledVertexShader(m_VertexShaderIndex)->GetBufferPointer(),
		SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->GetCompiledVertexShader(m_VertexShaderIndex)->GetBufferSize(),
		&m_pVertexLayout)))
	{
		OutputErrorLog("入力レイアウトの生成に失敗しました");
		return false;
	}

	return true;
}

bool MainLight::CreateState()
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
	if (FAILED(SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDevice()->CreateBlendState(
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
	if (FAILED(SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDevice()->CreateDepthStencilState(
		&DepthStencilDesc,
		&m_pDepthStencilState)))
	{
		OutputErrorLog("深度ステンシルステートの生成に失敗しました");
		return false;
	}

	return true;
}

bool MainLight::CreateLightTexture()
{
	if(!SINGLETON_INSTANCE(Lib::Dx11::TextureManager)->LoadTexture(
		"Resource\\Texture\\Light.png",
		&m_LightTextureIndex))
	{
		OutputErrorLog("テクスチャの生成に失敗しました");
		return false;
	}

	if (!SINGLETON_INSTANCE(Lib::Dx11::TextureManager)->LoadTexture(
		"Resource\\Texture\\Bloom.png",
		&m_BloomTextureIndex))
	{
		OutputErrorLog("テクスチャの生成に失敗しました");
		return false;
	}

	return true;
}

void MainLight::ReleaseTask()
{
	SINGLETON_INSTANCE(DepthDrawTaskManager)->RemoveStartUpTask(m_pDepthDrawStartUp);
	SINGLETON_INSTANCE(Lib::Draw3DTaskManager)->RemoveStartUpTask(m_pDrawStartUpTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->RemoveTask(m_pUpdateTask);
	SINGLETON_INSTANCE(Lib::Draw3DTaskManager)->RemoveTask(m_pDrawTask);

	SafeDelete(m_pDepthDrawStartUp);
	SafeDelete(m_pDrawStartUpTask);
	SafeDelete(m_pUpdateTask);
	SafeDelete(m_pDrawTask);
}

void MainLight::ReleaseLight()
{
	SafeDelete(m_pLight);
}

void MainLight::ReleaseConstantBuffer()
{
	SafeRelease(m_pConstantBuffer);
}

void MainLight::ReleaseDepthTexture()
{
	SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->SetViewPort(nullptr, m_RenderTargetStage);
	SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->SetClearColor(0xffffffff, m_RenderTargetStage);
	SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->SetDepthStencil(nullptr, m_RenderTargetStage);
	SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->SetRenderTarget(nullptr, m_RenderTargetStage);

	SafeRelease(m_pDepthStencilView);
	SafeRelease(m_pDepthStencilTexture);
	SafeRelease(m_pDepthStencilResource);
	SafeRelease(m_pRenderTarget);
	SafeRelease(m_pDepthTexture);
}

void MainLight::ReleaseVertex()
{
	SafeRelease(m_pVertexBuffer);
}

void MainLight::ReleaseShader()
{
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleasePixelShader(m_PixelShaderIndex);
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleaseVertexShader(m_VertexShaderIndex);
}

void MainLight::ReleaseVertexLayout()
{
	SafeRelease(m_pVertexLayout);
}

void MainLight::ReleaseState()
{
	SafeRelease(m_pDepthStencilState);
	SafeRelease(m_pBlendState);
}

void MainLight::ReleaseLightTexture()
{
	SINGLETON_INSTANCE(Lib::Dx11::TextureManager)->ReleaseTexture(m_BloomTextureIndex);
	SINGLETON_INSTANCE(Lib::Dx11::TextureManager)->ReleaseTexture(m_LightTextureIndex);
}

bool MainLight::WriteConstantBuffer()
{
	D3D11_MAPPED_SUBRESOURCE SubResourceData;
	if (SUCCEEDED(SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDeviceContext()->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResourceData)))
	{
		D3DXVECTOR3 LightPos = m_pLight->GetPos();
		D3DXVECTOR3 LightDir;
		D3DXVec3Normalize(&LightDir, &D3DXVECTOR3(m_pLight->GetPos() - m_pLight->GetDirectionPos()));
		D3DXMATRIX LightView = m_LightView;
		D3DXMatrixTranspose(&LightView, &LightView);
		D3DXMATRIX LightProj = m_LightProj;
		D3DXMatrixTranspose(&LightProj, &LightProj);

		D3DXMATRIX MatWorld, MatRotate, MatTranslate;
		D3DXMatrixIdentity(&MatWorld);
		m_pCamera->GetBillBoardRotation(&m_LightState.Pos, &MatRotate);
		D3DXMatrixMultiply(&MatWorld, &MatWorld, &MatRotate);
	
		D3DXMatrixTranslation(&MatTranslate, m_LightState.Pos.x, m_LightState.Pos.y, m_LightState.Pos.z);
		D3DXMatrixMultiply(&MatWorld, &MatWorld, &MatTranslate);
		D3DXMatrixTranspose(&MatWorld, &MatWorld);

		LIGHT_CONSTANT_BUFFER ConstantBuffer;
		ConstantBuffer.LightPos = D3DXVECTOR4(LightPos.x, LightPos.y, LightPos.z, 1.0);
		ConstantBuffer.LightDir = D3DXVECTOR4(LightDir.x, LightDir.y, LightDir.z, 1.0);
		ConstantBuffer.LightView = LightView;
		ConstantBuffer.LightProj = LightProj;
		ConstantBuffer.Matrix = MatWorld;

		D3DXVECTOR3 Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		D3DXVECTOR3 InvDir = LightPos;
		D3DXVec3Normalize(&InvDir, &InvDir);
		float Dot = D3DXVec3Dot(&InvDir, &Up);
		ConstantBuffer.LightDot.x = (1.0f + Dot) * 0.5f;

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

void MainLight::MainLightBeginScene()
{
	SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->BeginScene(m_RenderTargetStage);

	WriteConstantBuffer();

	SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDeviceContext()->VSSetConstantBuffers(2, 1, &m_pConstantBuffer);
	SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDeviceContext()->PSSetConstantBuffers(2, 1, &m_pConstantBuffer);
	SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDeviceContext()->GSSetConstantBuffers(2, 1, &m_pConstantBuffer);
}


//----------------------------------------------------------------------
// 深度バッファ描画前タスク Constructor Destructor
//----------------------------------------------------------------------
MainLight::DepthDrawStartUp::DepthDrawStartUp(MainLight* _pMainLight) :
	m_pMainLight(_pMainLight)
{
}

MainLight::DepthDrawStartUp::~DepthDrawStartUp()
{
}


//----------------------------------------------------------------------
// 深度バッファ描画前タスク Public Function
//----------------------------------------------------------------------
void MainLight::DepthDrawStartUp::Run()
{
	m_pMainLight->MainLightBeginScene();
}

