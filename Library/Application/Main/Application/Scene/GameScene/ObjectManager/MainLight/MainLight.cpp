/**
 * @file	MainLight.cpp
 * @brief	メインライトオブジェクト実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "MainLight.h"

#include "Debugger\Debugger.h"
#include "DirectX11\GraphicsDevice\GraphicsDevice.h"
#include "DirectX11\TextureManager\TextureManager.h"
#include "DirectX11\ShaderManager\ShaderManager.h"
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
	m_pLight(NULL),
	m_pCamera(_pCamera),
	m_pDepthTexture(NULL),
	m_pRenderTarget(NULL),
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
	if (!CreateTask())
	{
		return false;
	}

	if (!CreateLight())
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

	if (!CreateDepthTexture())
	{
		return false;
	}

	if (!CreateVertex())
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

	if (!CreateLightTexture())
	{
		return false;
	}

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

void MainLight::Draw()
{
	//--------------------描画--------------------
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

	ID3D11ShaderResourceView* pResource = SINGLETON_INSTANCE(Lib::TextureManager)->GetTexture(m_LightTextureIndex)->Get();
	ID3D11ShaderResourceView* pResource2 = SINGLETON_INSTANCE(Lib::TextureManager)->GetTexture(m_BloomTextureIndex)->Get();
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
	m_pDrawTask = new Lib::DrawTask();
	m_pUpdateTask = new Lib::UpdateTask();
	m_pDrawBeginTask = new DrawBeginTask(this);
	m_pDepthDrawBeginTask = new DepthDrawBeginTask(this);

	m_pDrawTask->SetDrawObject(this);
	m_pUpdateTask->SetUpdateObject(this);

	// タスクオブジェクトを管理クラスに追加.
	SINGLETON_INSTANCE(Lib::DrawTaskManager)->AddTask(m_pDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->AddTask(m_pUpdateTask);
	SINGLETON_INSTANCE(Lib::DrawTaskManager)->AddBeginTask(m_pDrawBeginTask);
	SINGLETON_INSTANCE(DepthDrawTaskManager)->AddBeginTask(m_pDepthDrawBeginTask);

	return true;
}

bool MainLight::CreateLight()
{
	// ライトオブジェクトの生成 初期化.
	m_pLight = new Lib::Light();
	m_pLight->SetPos(&m_LightState.Pos);
	m_pLight->SetDirectionPos(&m_DefaultLightDirPos);


	// ライト視点の行列生成.
	D3DXMatrixLookAtLH(&m_LightView, &m_LightState.Pos, &m_DefaultLightDirPos, &D3DXVECTOR3(0, 1, 0));

	const RECT* pWindowRect = SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetMainWindowRect();
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

	if (FAILED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDevice()->CreateBuffer(&ConstantBufferDesc, NULL, &m_pConstantBuffer)))
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

	// ビューポート設定.
	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;
	m_ViewPort.Width = m_DepthTextureWidth;
	m_ViewPort.Height = m_DepthTextureHeight;
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;

	// グラフィックデバイスに描画先として深度テクスチャを設定.
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->SetRenderTarget(&m_pRenderTarget, m_RenderTargetStage);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->SetDepthStencil(&m_pDepthStencilView, m_RenderTargetStage);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->SetClearColor(m_ClearColor, m_RenderTargetStage);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->SetViewPort(&m_ViewPort, m_RenderTargetStage);

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

bool MainLight::CreateShader()
{
	if (!SINGLETON_INSTANCE(Lib::ShaderManager)->LoadVertexShader(
		TEXT("Resource\\Effect\\MainLight.fx"),
		"VS",
		&m_VertexShaderIndex))
	{
		OutputErrorLog("頂点シェーダーの読み込みに失敗しました");
		return false;
	}

	if (!SINGLETON_INSTANCE(Lib::ShaderManager)->LoadPixelShader(
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

bool MainLight::CreateLightTexture()
{
	if(!SINGLETON_INSTANCE(Lib::TextureManager)->LoadTexture(
		"Resource\\Texture\\Light.png",
		&m_LightTextureIndex))
	{
		OutputErrorLog("テクスチャの生成に失敗しました");
		return false;
	}

	if (!SINGLETON_INSTANCE(Lib::TextureManager)->LoadTexture(
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
	SINGLETON_INSTANCE(DepthDrawTaskManager)->RemoveBeginTask(m_pDepthDrawBeginTask);
	SINGLETON_INSTANCE(Lib::DrawTaskManager)->RemoveBeginTask(m_pDrawBeginTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->RemoveTask(m_pUpdateTask);
	SINGLETON_INSTANCE(Lib::DrawTaskManager)->RemoveTask(m_pDrawTask);

	SafeDelete(m_pDepthDrawBeginTask);
	SafeDelete(m_pDrawBeginTask);
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
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->SetViewPort(NULL, m_RenderTargetStage);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->SetClearColor(0xffffffff, m_RenderTargetStage);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->SetDepthStencil(NULL, m_RenderTargetStage);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->SetRenderTarget(NULL, m_RenderTargetStage);

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
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleasePixelShader(m_PixelShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleaseVertexShader(m_VertexShaderIndex);
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
	SINGLETON_INSTANCE(Lib::TextureManager)->ReleaseTexture(m_BloomTextureIndex);
	SINGLETON_INSTANCE(Lib::TextureManager)->ReleaseTexture(m_LightTextureIndex);
}

bool MainLight::WriteConstantBuffer()
{
	D3D11_MAPPED_SUBRESOURCE SubResourceData;
	if (SUCCEEDED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResourceData)))
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

		SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->Unmap(m_pConstantBuffer, 0);

		return true;
	}

	return false;
}

void MainLight::DrawBeginScene()
{
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->PSSetShaderResources(2, 1, &m_pDepthStencilResource);
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
// 通常描画前処理タスク Constructor Destructor
//----------------------------------------------------------------------
MainLight::DrawBeginTask::DrawBeginTask(MainLight* _pMainLight) :
	m_pMainLight(_pMainLight)
{
}

MainLight::DrawBeginTask::~DrawBeginTask()
{
}


//----------------------------------------------------------------------
// 通常描画前処理タスク Public Function
//----------------------------------------------------------------------
void MainLight::DrawBeginTask::Run()
{
	m_pMainLight->DrawBeginScene();
}



//----------------------------------------------------------------------
// 深度バッファ描画前タスク Constructor Destructor
//----------------------------------------------------------------------
MainLight::DepthDrawBeginTask::DepthDrawBeginTask(MainLight* _pMainLight) :
	m_pMainLight(_pMainLight)
{
}

MainLight::DepthDrawBeginTask::~DepthDrawBeginTask()
{
}


//----------------------------------------------------------------------
// 深度バッファ描画前タスク Public Function
//----------------------------------------------------------------------
void MainLight::DepthDrawBeginTask::Run()
{
	m_pMainLight->MainLightBeginScene();
}

