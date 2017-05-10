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
#include "Debugger\Debugger.h"
#include "Main\Application\MyDefine.h"


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
	m_IsActive(true),
	m_RandDevice(),
	m_MersenneTwister(m_RandDevice())
{
	m_EmitterData.Frequency = 1;					//!< パーティクル発生頻度.
	m_EmitterData.Pos = *_pPos;						//!< パーティクル発生座標.
	m_EmitterData.Vec = D3DXVECTOR3(0.1f, 0.5f, 0);	//!< パーティクルの初速.
	m_EmitterData.AngleRange = 6;					//!< パーティクルの移動範囲.

	for (int i = 0; i < PARTICLE_NUM; i++)
	{
		m_SmokeData[i].Pos = D3DXVECTOR3(0, -1, 0);				//!< パーティクルの座標.
		m_SmokeData[i].Scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);	//!< パーティクルのスケーリング.
		m_SmokeData[i].AlphaColor = 1;							//!< パーティクルのアルファ値.
		m_SmokeData[i].Color = 0xffffffff;						//!< パーティクルのカラー.
		m_SmokeData[i].Life = 300 - i;							//!< パーティクルの寿命.
		m_SmokeData[i].IsActive = false;						//!< パーティクルの活動状態.
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
	if (!CreateTask())
	{
		return false;
	}

	if (!CreateVertexBuffer())
	{
		return false;
	}

	if (!WriteInstanceBuffer())
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

	if (!CreateTexture())
	{
		return false;
	}

	return true;
}

void Smoke::Finalize()
{
	ReleaseTexture();
	ReleaseState();
	ReleaseVertexLayout();
	ReleaseShader();
	ReleaseVertexBuffer();
	ReleaseTask();
}

void Smoke::Update()
{
	if (m_IsActive)
	{
		for (int i = 0; i < PARTICLE_NUM; i++)
		{
			if (m_SmokeData[i].IsActive)
			{
				m_SmokeData[i].Life--;
				if (m_SmokeData[i].Life <= 0)
				{
					m_SmokeData[i].IsActive = false;
					m_SmokeData[i].Scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
					m_SmokeData[i].Color = 0xffffffff;
					m_SmokeData[i].AlphaColor = 1;
					m_SmokeData[i].Pos = D3DXVECTOR3(0, -1, 0);
				}

				m_SmokeData[i].Pos += m_SmokeData[i].Vec;				// 座標更新.
				m_SmokeData[i].Scale += D3DXVECTOR3(0.01f, 0.01f, 0);	// サイズ更新.
				m_SmokeData[i].Color -= 0x01000000;						// カラー更新.
				m_SmokeData[i].AlphaColor -= 0.001f;
			}
			else
			{
				m_SmokeData[i].Life++;
				if (m_SmokeData[i].Life >= 300)	// 生成する条件は発生頻度を利用して実装する
				{
					double DegX = m_MersenneTwister() %
						(static_cast<int>(m_EmitterData.AngleRange) * 2) -
						m_EmitterData.AngleRange - 90;
					float RadX = static_cast<float>(D3DXToRadian(DegX));

					double DegY = m_MersenneTwister() %
						(static_cast<int>(m_EmitterData.AngleRange) * 2) - 
						m_EmitterData.AngleRange;
					float RadY = static_cast<float>(D3DXToRadian(DegY));

					m_SmokeData[i].Pos = m_EmitterData.Pos;
					m_SmokeData[i].IsActive = true;
					m_SmokeData[i].Vec.x = cos(RadX) * m_EmitterData.Vec.x;
					m_SmokeData[i].Vec.y = fabs(sin(RadY) * m_EmitterData.Vec.y);
					m_SmokeData[i].Vec.z = 0;
				}
			}
		}
		WriteInstanceBuffer();
	}
}

void Smoke::Draw()
{
	if (m_IsActive)
	{
		ID3D11DeviceContext* pContext = SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext();
		pContext->VSSetShader(SINGLETON_INSTANCE(Lib::ShaderManager)->GetVertexShader(m_VertexShaderIndex), NULL, 0);
		pContext->PSSetShader(SINGLETON_INSTANCE(Lib::ShaderManager)->GetPixelShader(m_PixelShaderIndex), NULL, 0);
		pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		pContext->IASetInputLayout(m_pVertexLayout);

		pContext->OMSetDepthStencilState(m_pDepthStencilState, 0);
		pContext->OMSetBlendState(m_pBlendState, NULL, 0xffffffff);

		ID3D11Buffer* pBuffer[2] = { m_pVertexBuffer, m_pInstanceBuffer };
		UINT Stride[2] = { sizeof(VERTEX), sizeof(INSTANCE_DATA) };
		UINT Offset[2] = { 0, 0 };
		pContext->IASetVertexBuffers(0, 2, pBuffer, Stride, Offset);

		ID3D11ShaderResourceView* pResource = 
			SINGLETON_INSTANCE(Lib::TextureManager)->GetTexture(m_SmokeTextureIndex)->Get();
		pContext->PSSetShaderResources(0, 1, &pResource);

		pContext->DrawInstanced(VERTEX_NUM, PARTICLE_NUM, 0, 0);
	}
	else
	{

	}
}


//----------------------------------------------------------------------
// Private Functions
//----------------------------------------------------------------------
bool Smoke::CreateTask()
{
	// タスク生成処理
	m_pDrawTask = new Lib::DrawTask();
	m_pUpdateTask = new Lib::UpdateTask();

	// タスクにオブジェクト設定
	m_pDrawTask->SetDrawObject(this);
	m_pUpdateTask->SetUpdateObject(this);

	m_pDrawTask->SetPriority(TRANSPARENT_OBJECT);

	SINGLETON_INSTANCE(Lib::DrawTaskManager)->AddTask(m_pDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->AddTask(m_pUpdateTask);

	return true;
}

bool Smoke::CreateVertexBuffer()
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

	if (FAILED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDevice()->CreateBuffer(
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
	if (!SINGLETON_INSTANCE(Lib::ShaderManager)->LoadVertexShader(
		TEXT("Resource\\Effect\\Smoke.fx"),
		"VS",
		&m_VertexShaderIndex))
	{
		OutputErrorLog("頂点シェーダーの読み込みに失敗しました");
		return false;
	}

	if (!SINGLETON_INSTANCE(Lib::ShaderManager)->LoadPixelShader(
		TEXT("Resource\\Effect\\Smoke.fx"),
		"PS",
		&m_PixelShaderIndex))
	{
		OutputErrorLog("ピクセルシェーダーの読み込みに失敗しました");
		return false;
	}

	return true;
}

bool Smoke::CreateVertexLayout()
{
	D3D11_INPUT_ELEMENT_DESC InputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 12, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "MATRIX",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,  0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "MATRIX",   1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "MATRIX",   2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "MATRIX",   3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "COLOR",	  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
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

bool Smoke::CreateState()
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
	DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
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

bool Smoke::CreateTexture()
{
	if(!SINGLETON_INSTANCE(Lib::TextureManager)->LoadTexture(
		TEXT("Resource\\Texture\\smoke.png"),
		&m_SmokeTextureIndex))
	{
		OutputErrorLog("テクスチャの読み込みに失敗しました");
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
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleasePixelShader(m_PixelShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleaseVertexShader(m_VertexShaderIndex);
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
	SINGLETON_INSTANCE(Lib::TextureManager)->ReleaseTexture(m_SmokeTextureIndex);
}

bool Smoke::WriteInstanceBuffer()
{
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	if (SUCCEEDED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->Map(
		m_pInstanceBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&MappedResource)))
	{
		INSTANCE_DATA* pInstanceData = reinterpret_cast<INSTANCE_DATA*>(MappedResource.pData);

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
			//pInstanceData[i].Color.a = m_SmokeData[i].AlphaColor;
		}

		SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->Unmap(m_pInstanceBuffer, 0);

		return true;
	}

	return false;
}


