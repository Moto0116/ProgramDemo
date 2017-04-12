﻿/**
 * @file	Rain.cpp
 * @brief	雨の管理クラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "Rain.h"

#include "Debugger\Debugger.h"
#include "DirectX11\GraphicsDevice\GraphicsDevice.h"
#include "DirectX11\ShaderManager\ShaderManager.h"
#include "DirectX11\SoundManager\SoundManager.h"
#include "DirectX11\SoundManager\ISound\ISound.h"
#include "DirectX11\TextureManager\TextureManager.h"
#include "DirectX11\TextureManager\ITexture\ITexture.h"


//----------------------------------------------------------------------
// Static Private Variables
//----------------------------------------------------------------------
const D3DXVECTOR2 Rain::m_DefaultSize = D3DXVECTOR2(0.2f, 0.2f);
const D3DXVECTOR2 Rain::m_XRange = D3DXVECTOR2(-55, 130);
const D3DXVECTOR2 Rain::m_YRange = D3DXVECTOR2(80, 100);
const D3DXVECTOR2 Rain::m_ZRange = D3DXVECTOR2(-55, 130);


//----------------------------------------------------------------------
// Constructor	Destructor
//----------------------------------------------------------------------
Rain::Rain(MainCamera* _pCamera) : 
	m_pCamera(_pCamera),
	m_RandDevice(),
	m_MersenneTwister(m_RandDevice())
{
	for (int i = 0; i < RAIN_NUM; i++)
	{
		int Data = m_MersenneTwister();
		m_RainData[i].Pos.x = m_XRange.x + (Data % static_cast<int>(m_XRange.y));
		Data = m_MersenneTwister();
		m_RainData[i].Pos.y = m_YRange.x + (Data % static_cast<int>(m_YRange.y));
		Data = m_MersenneTwister();
		m_RainData[i].Pos.z = m_ZRange.x + (Data % static_cast<int>(m_ZRange.y));

		m_RainData[i].Scale.x = 1.0f;
		m_RainData[i].Scale.y = 45.0f;
		m_RainData[i].Scale.z = 1.0f;

		m_RainData[i].IsFall = true;
		m_RainData[i].Time = 0;
	}
}

Rain::~Rain()
{
}


//----------------------------------------------------------------------
// Public Functions
//----------------------------------------------------------------------
bool Rain::Initialize()
{
	// タスク生成処理
	m_pDrawTask = new Lib::DrawTask();
	m_pUpdateTask = new Lib::UpdateTask();

	// タスクにオブジェクト設定
	m_pDrawTask->SetDrawObject(this);
	m_pUpdateTask->SetUpdateObject(this);

	SINGLETON_INSTANCE(Lib::DrawTaskManager)->AddTask(m_pDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->AddTask(m_pUpdateTask);


	if (!CreateVertexBuffer())
	{
		return false;
	}

	if (!WriteInstanceBuffer())
	{
		return false;
	}

	if (!CreateVertexShader())
	{
		return false;
	}

	if (!CreateVertexLayout())
	{
		return false;
	}

	if (!CreatePixelShader())
	{
		return false;
	}

	if (!CreateState())
	{
		return false;
	}


	SINGLETON_INSTANCE(Lib::TextureManager)->LoadTexture("Resource\\Texture\\Ring.png", &m_TextureIndex);

	SINGLETON_INSTANCE(Lib::SoundManager)->LoadSound("Resource\\Sound\\Rain.wav", &m_SoundIndex);
	SINGLETON_INSTANCE(Lib::SoundManager)->GetSound(m_SoundIndex)->SoundOperation(Lib::SoundManager::PLAY_LOOP);


	return true;
}

void Rain::Finalize()
{
	SINGLETON_INSTANCE(Lib::SoundManager)->GetSound(m_SoundIndex)->SoundOperation(Lib::SoundManager::STOP);
	SINGLETON_INSTANCE(Lib::SoundManager)->ReleaseSound(m_SoundIndex);

	SINGLETON_INSTANCE(Lib::TextureManager)->ReleaseTexture(m_TextureIndex);

	ReleaseState();
	ReleasePixelShader();
	ReleaseVertexLayout();
	ReleaseVertexShader();
	ReleaseVertexBuffer();

	SINGLETON_INSTANCE(Lib::DrawTaskManager)->RemoveTask(m_pDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->RemoveTask(m_pUpdateTask);

	delete m_pUpdateTask;
	delete m_pDrawTask;
}

void Rain::Update()
{
	// 雨の操作
	for (int i = 0; i < RAIN_NUM; i++)
	{
		if (m_RainData[i].IsFall == true)
		{
			// 落下中であれば座標移動を行う
			m_RainData[i].Pos.y -= 3.5f;

			if (m_RainData[i].Pos.y <= -4.5f)
			{
				m_RainData[i].Pos.y = 0;
				m_RainData[i].IsFall = false;

				m_RainData[i].Scale.x = 2.0f;
				m_RainData[i].Scale.y = 2.0f;
				m_RainData[i].Scale.z = 2.0f;
			}
		}
		else
		{
			// 落下中でなければ波紋を出す
			m_RainData[i].Time++;

			m_RainData[i].Scale.x += 0.6f;
			m_RainData[i].Scale.y += 0.6f;
			m_RainData[i].Scale.z += 0.6f;

			if (m_RainData[i].Time == 35)
			{
				m_RainData[i].Time = 0;
				m_RainData[i].IsFall = true;

				m_RainData[i].Scale.x = 1.0f;
				m_RainData[i].Scale.y = 45.0f;
				m_RainData[i].Scale.z = 1.0f;

				int Data = m_MersenneTwister();
				m_RainData[i].Pos.x = m_XRange.x + (Data % static_cast<int>(m_XRange.y));
				Data = m_MersenneTwister();
				m_RainData[i].Pos.y = m_YRange.x + (Data % static_cast<int>(m_YRange.y));
				Data = m_MersenneTwister();
				m_RainData[i].Pos.z = m_ZRange.x + (Data % static_cast<int>(m_ZRange.y));
			}
		}
	}

	WriteInstanceBuffer();
}

void Rain::Draw()
{
	ID3D11DeviceContext* pContext = SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext();
	pContext->VSSetShader(SINGLETON_INSTANCE(Lib::ShaderManager)->GetVertexShader(m_VertexShaderIndex), NULL, 0);
	pContext->PSSetShader(SINGLETON_INSTANCE(Lib::ShaderManager)->GetPixelShader(m_PixelShaderIndex), NULL, 0);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	pContext->IASetInputLayout(m_pVertexLayout);

	pContext->OMSetDepthStencilState(m_pDepthStencilState, 0);
	pContext->OMSetBlendState(m_pBlendState, NULL, 0xffffffff);

	ID3D11Buffer* pBuffer[2] = { m_pVertexBuffer , m_pInstanceBuffer};
	UINT Stride[2] = { sizeof(VERTEX), sizeof(INSTANCE_DATA) };
	UINT Offset[2] = { 0, 0 };
	pContext->IASetVertexBuffers(0, 2, pBuffer, Stride, Offset);

	ID3D11ShaderResourceView* pResource = SINGLETON_INSTANCE(Lib::TextureManager)->GetTexture(m_TextureIndex)->Get();
	pContext->PSSetShaderResources(0, 1, &pResource);

	pContext->DrawInstanced(VERTEX_NUM, RAIN_NUM, 0, 0);
}


//----------------------------------------------------------------------
// Private Functions
//----------------------------------------------------------------------
bool Rain::CreateVertexBuffer()
{
	VERTEX VertexData[VERTEX_NUM] =
	{
		VERTEX{ D3DXVECTOR3(-m_DefaultSize.x / 2, -m_DefaultSize.y / 2, 0), D3DXVECTOR2(0, 0), 0x10aaaaaa },
		VERTEX{ D3DXVECTOR3(m_DefaultSize.x  / 2, -m_DefaultSize.y / 2, 0), D3DXVECTOR2(1, 0), 0x10aaaaaa },
		VERTEX{ D3DXVECTOR3(-m_DefaultSize.x / 2,  m_DefaultSize.y / 2, 0), D3DXVECTOR2(0, 1), 0x0faaaaaa },
		VERTEX{ D3DXVECTOR3(m_DefaultSize.x  / 2,  m_DefaultSize.y / 2, 0), D3DXVECTOR2(1, 1), 0x0faaaaaa }
	};

	for (int i = 0; i < VERTEX_NUM; i++)
	{
		m_pVertexData[i].Pos = VertexData[i].Pos;
		m_pVertexData[i].UV = VertexData[i].UV;
		m_pVertexData[i].Color = VertexData[i].Color;
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


	for (int i = 0; i < RAIN_NUM; i++)
	{
		D3DXMatrixIdentity(&m_pInstanceData[i].Mat);
		m_pInstanceData[i].Pos = D3DXVECTOR3(0, 0, 0);
	}

	// インスタンスバッファの設定.
	D3D11_BUFFER_DESC InstanceBufferDesc;
	ZeroMemory(&InstanceBufferDesc, sizeof(D3D11_BUFFER_DESC));
	InstanceBufferDesc.ByteWidth = sizeof(INSTANCE_DATA) * RAIN_NUM;
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

bool Rain::CreateVertexShader()
{
	if (!SINGLETON_INSTANCE(Lib::ShaderManager)->LoadVertexShader(
		TEXT("Resource\\Effect\\PointSprite.fx"),
		"VS",
		&m_VertexShaderIndex))
	{
		OutputErrorLog("頂点シェーダーの読み込みに失敗しました");
		return false;
	}

	return true;
}

bool Rain::CreateVertexLayout()
{
	D3D11_INPUT_ELEMENT_DESC InputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		 0, 12, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "COLOR",	  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "MATRIX",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0,  D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "MATRIX",   1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "MATRIX",   2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "MATRIX",   3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "POS",	  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
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

bool Rain::CreatePixelShader()
{
	if(!SINGLETON_INSTANCE(Lib::ShaderManager)->LoadPixelShader(
		TEXT("Resource\\Effect\\PointSprite.fx"),
		"PS",
		&m_PixelShaderIndex))
	{
		OutputErrorLog("ピクセルシェーダーの読み込みに失敗しました");
		return false;
	}

	return true;
}

bool Rain::CreateState()
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

void Rain::ReleaseVertexBuffer()
{
	SafeRelease(m_pVertexBuffer);
}

void Rain::ReleaseVertexShader()
{
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleaseVertexShader(m_VertexShaderIndex);
}

void Rain::ReleaseVertexLayout()
{
	SafeRelease(m_pVertexLayout);
}

void Rain::ReleasePixelShader()
{
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleasePixelShader(m_PixelShaderIndex);
}

void Rain::ReleaseState()
{
	SafeRelease(m_pDepthStencilState);
	SafeRelease(m_pBlendState);
}


///@todo 仮コード
void GetBillBoardRotation(D3DXVECTOR3* _pBillPos, D3DXVECTOR3* _pTargetPos, D3DXMATRIX* _pRot)
{
	D3DXMatrixIdentity(_pRot);
	D3DXMatrixLookAtLH(_pRot, _pTargetPos, _pBillPos, &D3DXVECTOR3(0, 1, 0));
	D3DXMatrixInverse(_pRot, NULL, _pRot);

	// 座標移動は消しておく
	_pRot->_41 = 0.0f;
	_pRot->_42 = 0.0f;
	_pRot->_43 = 0.0f;
}

bool Rain::WriteInstanceBuffer()
{
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	if (SUCCEEDED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->Map(
		m_pInstanceBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&MappedResource)))
	{
		INSTANCE_DATA* InstanceData = reinterpret_cast<INSTANCE_DATA*>(MappedResource.pData);

		for (int i = 0; i < RAIN_NUM; i++)
		{
			D3DXMATRIX MatWorld, MatTranslate, MatRotate;
			D3DXMatrixIdentity(&MatWorld);

			if (m_RainData[i].IsFall == true)
			{
				GetBillBoardRotation(&m_RainData[i].Pos, &m_pCamera->GetPos(), &MatRotate);
				D3DXMatrixScaling(&MatWorld, m_RainData[i].Scale.x, m_RainData[i].Scale.y, m_RainData[i].Scale.z);
				D3DXMatrixMultiply(&MatWorld, &MatWorld, &MatRotate);
				D3DXMatrixRotationY(&MatRotate, static_cast<float>(D3DXToRadian(180)));	// こっち側向くように反転
				D3DXMatrixMultiply(&MatWorld, &MatWorld, &MatRotate);
				D3DXMatrixTranslation(&MatTranslate, m_RainData[i].Pos.x, m_RainData[i].Pos.y, m_RainData[i].Pos.z);
				D3DXMatrixMultiply(&MatWorld, &MatWorld, &MatTranslate);
				D3DXMatrixTranspose(&InstanceData[i].Mat, &MatWorld);
			}
			else
			{
				D3DXMatrixScaling(&MatWorld, m_RainData[i].Scale.x, m_RainData[i].Scale.y, m_RainData[i].Scale.z);
				D3DXMatrixRotationX(&MatRotate, static_cast<float>(D3DXToRadian(-90)));	// 上を向くように反転
				D3DXMatrixMultiply(&MatWorld, &MatWorld, &MatRotate);
				D3DXMatrixTranslation(&MatTranslate, m_RainData[i].Pos.x, m_RainData[i].Pos.y, m_RainData[i].Pos.z);
				D3DXMatrixMultiply(&MatWorld, &MatWorld, &MatTranslate);
				D3DXMatrixTranspose(&InstanceData[i].Mat, &MatWorld);
			}

			InstanceData[i].Pos = m_RainData[i].Pos;
		}

		SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->Unmap(m_pInstanceBuffer, 0);

		return true;
	}

	return false;
}

