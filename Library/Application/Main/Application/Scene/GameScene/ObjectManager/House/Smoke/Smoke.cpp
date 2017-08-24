/**
 * @file	Smoke.cpp
 * @brief	煙クラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "Smoke.h"

#include "DirectX11\GraphicsDevice\GraphicsDevice.h"
#include "DirectX11\ShaderManager\ShaderManager.h"
#include "DirectX11\TextureManager\TextureManager.h"
#include "DirectX11\TextureManager\ITexture\ITexture.h"
#include "TaskManager\TaskBase\DrawTask\DrawTask.h"
#include "TaskManager\TaskBase\UpdateTask\UpdateTask.h"
#include "Debugger\Debugger.h"
#include "Main\Application\MyDefine.h"
#include "..\..\MainCamera\MainCamera.h"


//----------------------------------------------------------------------
// Static Private Variables
//----------------------------------------------------------------------
const D3DXVECTOR2 Smoke::m_DefaultSize = D3DXVECTOR2(1.0f, 1.0f);
const D3DXVECTOR2 Smoke::m_LifeRange = D3DXVECTOR2(180, 240);


//----------------------------------------------------------------------
// Constructor	Destructor
//----------------------------------------------------------------------
Smoke::Smoke(MainCamera* _pCamera, D3DXVECTOR3* _pPos) :
	m_pCamera(_pCamera),
	m_VertexShaderIndex(Lib::Dx11::ShaderManager::m_InvalidIndex),
	m_PixelShaderIndex(Lib::Dx11::ShaderManager::m_InvalidIndex),
	m_ComputeShaderIndex(Lib::Dx11::ShaderManager::m_InvalidIndex),
	m_SmokeTextureIndex(Lib::Dx11::TextureManager::m_InvalidIndex),
	m_SkyCLUTIndex(Lib::Dx11::TextureManager::m_InvalidIndex),
	m_IsActive(true),
	m_IsComputeShader(false),
	m_RandDevice(),
	m_MersenneTwister(m_RandDevice())
{
	m_EmitterData.Pos = *_pPos;							// パーティクル発生座標.
	m_EmitterData.Vec = D3DXVECTOR3(0.08f, 0.5f, 0.08f);// パーティクルの初速.
	m_EmitterData.AngleRange = 6;						// パーティクルの移動範囲.

	// パーティクルデータの初期化.
	for (int i = 0; i < PARTICLE_NUM; i++)
	{
		double DegX = m_MersenneTwister() %
			(static_cast<int>(m_EmitterData.AngleRange) * 2) -
			m_EmitterData.AngleRange - 90;
		float RadX = static_cast<float>(D3DXToRadian(DegX));

		double DegY = m_MersenneTwister() %
			(static_cast<int>(m_EmitterData.AngleRange) * 2) -
			m_EmitterData.AngleRange;
		float RadY = static_cast<float>(D3DXToRadian(DegY));

		double DegZ = m_MersenneTwister() %
			(static_cast<int>(m_EmitterData.AngleRange) * 2) -
			m_EmitterData.AngleRange - 90;
		float RadZ = static_cast<float>(D3DXToRadian(DegZ));

		m_SmokeData[i].Vec.x = cos(RadX) * m_EmitterData.Vec.x;
		m_SmokeData[i].Vec.y = fabs(sin(RadY) * m_EmitterData.Vec.y);
		m_SmokeData[i].Vec.z = cos(RadZ) * m_EmitterData.Vec.z;

		m_SmokeData[i].Pos = D3DXVECTOR3(0, -1, 0);				// パーティクルの座標.
		m_SmokeData[i].Scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);	// パーティクルのスケーリング.
		m_SmokeData[i].Color = 0xffffffff;						// パーティクルのカラー.
		m_SmokeData[i].Life = 300 - i;							// パーティクルの寿命.
		m_SmokeData[i].IsActive = false;						// パーティクルの活動状態.

		m_ComputeData[i].Pos = D3DXVECTOR4(m_SmokeData[i].Pos, 0);
		m_ComputeData[i].Scale = D3DXVECTOR4(m_SmokeData[i].Scale, 0);
		m_ComputeData[i].Color = m_SmokeData[i].Color;
		m_ComputeData[i].Life = D3DXVECTOR4(static_cast<float>(m_SmokeData[i].Life), 0, 0, 0);
	}
}

Smoke::~Smoke()
{
}


//----------------------------------------------------------------------
// Public Functions
//----------------------------------------------------------------------
bool Smoke::Initialize()
{
	if (!CreateTask())					return false;
	if (!CreateVertexBuffer())			return false;
	if (!WriteInstanceBuffer())			return false;
	if (!CreateShader())				return false;
	if (!CreateVertexLayout())			return false;
	if (!CreateState())					return false;
	if (!CreateTexture())				return false;
	if (!CreateComputeShaderBuffer())	return false;

	return true;
}

void Smoke::Finalize()
{
	ReleaseComputeShaderBuffer();
	ReleaseTexture();
	ReleaseState();
	ReleaseVertexLayout();
	ReleaseShader();
	ReleaseVertexBuffer();
	ReleaseTask();
}

void Smoke::Update()
{
	if (!m_IsActive)
	{
		return;
	}


	if (!m_IsComputeShader)
	{
		for (int i = 0; i < PARTICLE_NUM; i++)
		{
			if (m_SmokeData[i].IsActive)
			{
				m_SmokeData[i].Life--;
				if (m_SmokeData[i].Life <= 0)
				{
					m_SmokeData[i].IsActive = false;
				}

				m_SmokeData[i].Pos += m_SmokeData[i].Vec;				// 座標更新.
				m_SmokeData[i].Scale += D3DXVECTOR3(0.01f, 0.01f, 0);	// サイズ更新.
				m_SmokeData[i].Color -= 0x01000000;						// カラー更新.
			}
			else
			{
				m_SmokeData[i].Life++;
				if (m_SmokeData[i].Life == 300)	// 一定時間が経過したらアクティブ状態に遷移.
				{
					m_SmokeData[i].Pos = m_EmitterData.Pos;
					m_SmokeData[i].Color = 0xffffffff;
					m_SmokeData[i].Scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
					m_SmokeData[i].IsActive = true;
				}
			}
		}
		WriteInstanceBuffer();
	}
	else
	{

	}
}

void Smoke::Draw()
{
	Lib::Dx11::GraphicsDevice* pGraphicsDevice = SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice);
	ID3D11DeviceContext* pDeviceContext = SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDeviceContext();
	Lib::Dx11::TextureManager* pTextureManageer = SINGLETON_INSTANCE(Lib::Dx11::TextureManager);
	Lib::Dx11::ShaderManager*	pShaderManager = SINGLETON_INSTANCE(Lib::Dx11::ShaderManager);

	if (m_IsActive)
	{
		pDeviceContext->VSSetShader(pShaderManager->GetVertexShader(m_VertexShaderIndex), nullptr, 0);
		pDeviceContext->PSSetShader(pShaderManager->GetPixelShader(m_PixelShaderIndex), nullptr, 0);
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		pDeviceContext->IASetInputLayout(m_pVertexLayout);

		pDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 0);
		pDeviceContext->OMSetBlendState(m_pBlendState, nullptr, 0xffffffff);

		ID3D11Buffer* pBuffer[2] = { m_pVertexBuffer, m_pInstanceBuffer };
		UINT Stride[2] = { sizeof(VERTEX), sizeof(INSTANCE_DATA) };
		UINT Offset[2] = { 0, 0 };
		pDeviceContext->IASetVertexBuffers(0, 2, pBuffer, Stride, Offset);

		ID3D11ShaderResourceView* pResource = pTextureManageer->GetTexture(m_SmokeTextureIndex)->Get();
		pDeviceContext->PSSetShaderResources(0, 1, &pResource);

		ID3D11ShaderResourceView* pResource2 = pTextureManageer->GetTexture(m_SkyCLUTIndex)->Get();
		pDeviceContext->PSSetShaderResources(3, 1, &pResource2);

		pDeviceContext->DrawInstanced(VERTEX_NUM, PARTICLE_NUM, 0, 0);
	}
}


//----------------------------------------------------------------------
// Private Functions
//----------------------------------------------------------------------
bool Smoke::CreateTask()
{
	// タスク生成処理.
	m_pDrawTask = new Lib::DrawTask();
	m_pUpdateTask = new Lib::UpdateTask();

	// タスクにオブジェクト設定.
	m_pDrawTask->SetDrawObject(this);
	m_pUpdateTask->SetUpdateObject(this);

	m_pDrawTask->SetPriority(TRANSPARENT_OBJECT);

	SINGLETON_INSTANCE(Lib::DrawTaskManager)->AddTask(m_pDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->AddTask(m_pUpdateTask);

	return true;
}

bool Smoke::CreateVertexBuffer()
{
	Lib::Dx11::GraphicsDevice* pGraphicsDevice = SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice);

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

	if (FAILED(pGraphicsDevice->GetDevice()->CreateBuffer(
		&BufferDesc,
		&ResourceData,
		&m_pVertexBuffer)))
	{
		OutputErrorLog("頂点バッファの生成に失敗しました");
		return false;
	}


	for (int i = 0; i < PARTICLE_NUM; i++)
	{
		D3DXMatrixIdentity(&m_pInstanceData[i].Mat);
		D3DXMatrixTranspose(&m_pInstanceData[i].Mat, &m_pInstanceData[i].Mat);
	}

	// インスタンスバッファの設定.
	D3D11_BUFFER_DESC InstanceBufferDesc;
	ZeroMemory(&InstanceBufferDesc, sizeof(D3D11_BUFFER_DESC));
	InstanceBufferDesc.ByteWidth = sizeof(INSTANCE_DATA) * PARTICLE_NUM;
	InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	InstanceBufferDesc.MiscFlags = 0;
	InstanceBufferDesc.StructureByteStride = 0;

	// インスタンスバッファに格納するデータの設定.
	D3D11_SUBRESOURCE_DATA InstanceResourceData;
	ZeroMemory(&InstanceResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	InstanceResourceData.pSysMem = m_pInstanceData;

	if (FAILED(pGraphicsDevice->GetDevice()->CreateBuffer(
		&InstanceBufferDesc,
		&InstanceResourceData,
		&m_pInstanceBuffer)))
	{
		OutputErrorLog("インスタンスバッファの生成に失敗しました");
		return false;
	}

	return true;
}

bool Smoke::CreateShader()
{
	if (!SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadVertexShader(
		TEXT("Resource\\Effect\\Smoke.fx"),
		"VS",
		&m_VertexShaderIndex))
	{
		OutputErrorLog("頂点シェーダーの読み込みに失敗しました");
		return false;
	}

	if (!SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadPixelShader(
		TEXT("Resource\\Effect\\Smoke.fx"),
		"PS",
		&m_PixelShaderIndex))
	{
		OutputErrorLog("ピクセルシェーダーの読み込みに失敗しました");
		return false;
	}

	if (!SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadComputeShader(
		TEXT("Resource\\Effect\\Compute.fx"),
		"CS",
		&m_ComputeShaderIndex))
	{
		OutputErrorLog("コンピュートシェーダーシェーダーの読み込みに失敗しました");
		return false;
	}

	return true;
}

bool Smoke::CreateVertexLayout()
{
	Lib::Dx11::GraphicsDevice* pGraphicsDevice = SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice);
	Lib::Dx11::ShaderManager*	pShaderManager = SINGLETON_INSTANCE(Lib::Dx11::ShaderManager);

	D3D11_INPUT_ELEMENT_DESC InputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 12, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "MATRIX",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,  0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "MATRIX",   1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "MATRIX",   2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "MATRIX",   3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};

	if (FAILED(pGraphicsDevice->GetDevice()->CreateInputLayout(
		InputElementDesc,
		sizeof(InputElementDesc) / sizeof(InputElementDesc[0]),
		pShaderManager->GetCompiledVertexShader(m_VertexShaderIndex)->GetBufferPointer(),
		pShaderManager->GetCompiledVertexShader(m_VertexShaderIndex)->GetBufferSize(),
		&m_pVertexLayout)))
	{
		OutputErrorLog("入力レイアウトの生成に失敗しました");
		return false;
	}

	return true;
}

bool Smoke::CreateState()
{
	Lib::Dx11::GraphicsDevice* pGraphicsDevice = SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice);

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
	if (FAILED(pGraphicsDevice->GetDevice()->CreateBlendState(
		&BlendDesc,
		&m_pBlendState)))
	{
		OutputErrorLog("ブレンドステートの生成に失敗しました");
		return false;
	}

	// 深度ステンシルステートの生成.
	D3D11_DEPTH_STENCIL_DESC DepthStencilDesc;
	ZeroMemory(&DepthStencilDesc, sizeof(DepthStencilDesc));
	DepthStencilDesc.DepthEnable = TRUE;
	DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	DepthStencilDesc.StencilEnable = FALSE;
	if (FAILED(pGraphicsDevice->GetDevice()->CreateDepthStencilState(
		&DepthStencilDesc,
		&m_pDepthStencilState)))
	{
		OutputErrorLog("深度ステンシルステートの生成に失敗しました");
		return false;
	}

	return true;
}

bool Smoke::CreateTexture()
{
	if (!SINGLETON_INSTANCE(Lib::Dx11::TextureManager)->LoadTexture(
		TEXT("Resource\\Texture\\smoke.png"),
		&m_SmokeTextureIndex))
	{
		OutputErrorLog("煙テクスチャの読み込みに失敗しました");
		return false;
	}

	if (!SINGLETON_INSTANCE(Lib::Dx11::TextureManager)->LoadTexture(
		TEXT("Resource\\Texture\\MainLightCLUT.png"),
		&m_SkyCLUTIndex))
	{
		OutputErrorLog("カラールックアップテーブルテクスチャの読み込みに失敗しました");
		return false;
	}

	return true;
}

bool Smoke::CreateComputeShaderBuffer()
{
	// コンピュートシェーダーバッファの生成.
	D3D11_BUFFER_DESC BufferDesc;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	BufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	BufferDesc.ByteWidth = sizeof(COMPUTESHADER_BUFFER) * PARTICLE_NUM;
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	BufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	BufferDesc.StructureByteStride = sizeof(COMPUTESHADER_BUFFER);

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = m_ComputeData;

	if (FAILED(SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDevice()->CreateBuffer(
		&BufferDesc,
		&InitData,
		&m_pComputeShaderBuffer)))
	{
		OutputErrorLog("コンピュートシェーダーバッファの生成に失敗しました");
		return false;
	}


	// コンピュートシェーダーバッファのアクセスビュー生成.
	D3D11_UNORDERED_ACCESS_VIEW_DESC AccessViewDesc;
	ZeroMemory(&AccessViewDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	AccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	AccessViewDesc.Buffer.FirstElement = 0;
	AccessViewDesc.Format = DXGI_FORMAT_UNKNOWN;
	AccessViewDesc.Buffer.NumElements = PARTICLE_NUM;
	if (FAILED(SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDevice()->CreateUnorderedAccessView(
		m_pComputeShaderBuffer,
		&AccessViewDesc,
		&m_pComputeShaderBufferAccess)))
	{
		OutputErrorLog("コンピュートシェーダーバッファアクセスビューの生成に失敗しました");
		return false;
	}

	return true;
}

void Smoke::ReleaseTask()
{
	SINGLETON_INSTANCE(Lib::DrawTaskManager)->RemoveTask(m_pDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->RemoveTask(m_pUpdateTask);

	delete m_pUpdateTask;
	delete m_pDrawTask;
}

void Smoke::ReleaseVertexBuffer()
{
	SafeRelease(m_pInstanceBuffer);
	SafeRelease(m_pVertexBuffer);
}

void Smoke::ReleaseShader()
{
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleaseComputeShader(m_ComputeShaderIndex);
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleasePixelShader(m_PixelShaderIndex);
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleaseVertexShader(m_VertexShaderIndex);
}

void Smoke::ReleaseVertexLayout()
{
	SafeRelease(m_pVertexLayout);
}

void Smoke::ReleaseState()
{
	SafeRelease(m_pDepthStencilState);
	SafeRelease(m_pBlendState);
}

void Smoke::ReleaseTexture()
{
	SINGLETON_INSTANCE(Lib::Dx11::TextureManager)->ReleaseTexture(m_SkyCLUTIndex);
	SINGLETON_INSTANCE(Lib::Dx11::TextureManager)->ReleaseTexture(m_SmokeTextureIndex);
}

void Smoke::ReleaseComputeShaderBuffer()
{
	SafeRelease(m_pComputeShaderBufferAccess);
	SafeRelease(m_pComputeShaderBuffer);
}

bool Smoke::WriteInstanceBuffer()
{
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	if (SUCCEEDED(SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDeviceContext()->Map(
		m_pInstanceBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&MappedResource)))
	{
		INSTANCE_DATA* pInstanceData = reinterpret_cast<INSTANCE_DATA*>(MappedResource.pData);

		// インスタンスバッファへのデータ書き込み.
		for (int i = 0; i < PARTICLE_NUM; i++)
		{
			D3DXMATRIX MatWorld, MatRotate, MatTranslate;
			D3DXMatrixIdentity(&MatWorld);

			m_pCamera->GetBillBoardRotation(&m_SmokeData[i].Pos, &MatRotate);
			D3DXMatrixScaling(&MatWorld, m_SmokeData[i].Scale.x, m_SmokeData[i].Scale.y, m_SmokeData[i].Scale.z);
			D3DXMatrixMultiply(&MatWorld, &MatWorld, &MatRotate);
			D3DXMatrixTranslation(&MatTranslate, m_SmokeData[i].Pos.x, m_SmokeData[i].Pos.y, m_SmokeData[i].Pos.z);
			D3DXMatrixMultiply(&MatWorld, &MatWorld, &MatTranslate);

			D3DXMatrixTranspose(&MatWorld, &MatWorld);
			pInstanceData[i].Mat = MatWorld;

			pInstanceData[i].Color.a = m_SmokeData[i].Color.a;
		}

		SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDeviceContext()->Unmap(m_pInstanceBuffer, 0);

		return true;
	}

	return false;
}


