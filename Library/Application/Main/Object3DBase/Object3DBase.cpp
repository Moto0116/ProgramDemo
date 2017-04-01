/**
 * @file   Object3DBase.cpp
 * @brief  Object3DBaseクラスの実装
 * @author morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "Object3DBase.h"

#include "Debugger\Debugger.h"
#include "DirectX11\GraphicsDevice\GraphicsDevice.h"
#include "DirectX11\ShaderManager\ShaderManager.h"


Object3DBase::Object3DBase() : 
	m_pVertexLayout(NULL),
	m_pDepthStencilState(NULL),
	m_pConstantBuffer(NULL),
	m_Pos(D3DXVECTOR3(0, 0, 0)),
	m_Scale(D3DXVECTOR3(1, 1, 1)),
	m_Rotate(D3DXVECTOR3(0, 0, 0))
{
	m_pDrawTask = new Lib::DrawTask();
	m_pUpdateTask = new Lib::UpdateTask();
	m_pDepthDrawTask = new DepthDrawTask();
	m_pMapDrawTask = new MapDrawTask();

	m_pDrawTask->SetDrawObject(this);
	m_pUpdateTask->SetUpdateObject(this);
	m_pDepthDrawTask->SetDrawObject(this);
	m_pMapDrawTask->SetDrawObject(this);
}

Object3DBase::~Object3DBase()
{
	delete m_pMapDrawTask;
	delete m_pDepthDrawTask;
	delete m_pUpdateTask;
	delete m_pDrawTask;
}

bool Object3DBase::Initialize()
{
	return true;
}

void Object3DBase::Finalize()
{
}

void Object3DBase::Update()
{
}

void Object3DBase::Draw()
{
}

void Object3DBase::DepthDraw()
{
}

void Object3DBase::MapDraw()
{
}

void Object3DBase::ShaderSetup()
{
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->VSSetShader(
		SINGLETON_INSTANCE(Lib::ShaderManager)->GetVertexShader(m_VertexShaderIndex), 
		NULL, 
		0);
	
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->PSSetShader(
		SINGLETON_INSTANCE(Lib::ShaderManager)->GetPixelShader(m_PixelShaderIndex), 
		NULL,
		0);

	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->GSSetShader(
		NULL,
		NULL,
		0);
}

void Object3DBase::VertexLayoutSetup()
{
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->IASetInputLayout(m_pVertexLayout);
}

void Object3DBase::DepthStencilStateSetup()
{
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->OMSetDepthStencilState(m_pDepthStencilState, 0);
}

void Object3DBase::ConstantBufferSetup()
{
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);
}

bool Object3DBase::CreateShader()
{
	SINGLETON_INSTANCE(Lib::ShaderManager)->LoadVertexShader(TEXT("Resource\\Effect\\DefaultEffect.fx"), "VS", &m_VertexShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->LoadPixelShader(TEXT("Resource\\Effect\\DefaultEffect.fx"), "PS", &m_PixelShaderIndex);

	return true;
}

bool Object3DBase::CreateVertexLayout()
{
	D3D11_INPUT_ELEMENT_DESC InputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(D3DXVECTOR3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(D3DXVECTOR3) + sizeof(D3DXVECTOR3), D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (FAILED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDevice()->CreateInputLayout(
		InputElementDesc,
		sizeof(InputElementDesc) / sizeof(InputElementDesc[0]),
		Lib::ShaderManager::GetInstance()->GetCompiledVertexShader(m_VertexShaderIndex)->GetBufferPointer(),
		Lib::ShaderManager::GetInstance()->GetCompiledVertexShader(m_VertexShaderIndex)->GetBufferSize(),
		&m_pVertexLayout)))
	{
		OutputErrorLog("入力レイアウトの生成に失敗しました");
		return false;
	}

	return true;
}

bool Object3DBase::CreateDepthStencilState()
{
	D3D11_DEPTH_STENCIL_DESC DepthStencilDesc;
	ZeroMemory(&DepthStencilDesc, sizeof(DepthStencilDesc));
	DepthStencilDesc.DepthEnable = TRUE;
	DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
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

bool Object3DBase::CreateConstantBuffer()
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
		OutputErrorLog("定数バッファ生成に失敗しました");
		return false;
	}

	return true;
}

bool Object3DBase::WriteConstantBuffer()
{
	D3D11_MAPPED_SUBRESOURCE SubResourceData;
	if (SUCCEEDED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResourceData)))
	{
		D3DXMATRIX MatWorld, MatTranslate, MatRotate;
		D3DXMatrixIdentity(&MatWorld);
		D3DXMatrixScaling(&MatWorld, m_Scale.x, m_Scale.y, m_Scale.z);
		D3DXMatrixRotationY(&MatRotate, m_Rotate.y);
		D3DXMatrixMultiply(&MatWorld, &MatWorld, &MatRotate);
		D3DXMatrixTranslation(&MatTranslate, m_Pos.x, m_Pos.y, m_Pos.z);
		D3DXMatrixMultiply(&MatWorld, &MatWorld, &MatTranslate);

		CONSTANT_BUFFER ConstantBuffer;
		ConstantBuffer.World = MatWorld;
		D3DXMatrixTranspose(&ConstantBuffer.World, &ConstantBuffer.World);

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

void Object3DBase::ReleaseShader()
{
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleaseVertexShader(m_VertexShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleasePixelShader(m_PixelShaderIndex);
}

void Object3DBase::ReleaseVertexLayout()
{
	SafeRelease(m_pVertexLayout);
}

void Object3DBase::ReleaseDepthStencilState()
{
	SafeRelease(m_pDepthStencilState);
}

void Object3DBase::ReleaseConstantBuffer()
{
	SafeRelease(m_pConstantBuffer);
}