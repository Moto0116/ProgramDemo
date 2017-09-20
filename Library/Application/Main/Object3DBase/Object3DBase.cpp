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
#include "DirectX11\GraphicsDevice\Dx11GraphicsDevice.h"
#include "DirectX11\ShaderManager\Dx11ShaderManager.h"
#include "DirectX11\TextureManager\Dx11TextureManager.h"
#include "DirectX11\TextureManager\ITexture\Dx11ITexture.h"


//----------------------------------------------------------------------
// Constructor	Destructor
//----------------------------------------------------------------------
Object3DBase::Object3DBase() : 
	m_pVertexLayout(nullptr),
	m_pDepthStencilState(nullptr),
	m_pConstantBuffer(nullptr),
	m_Pos(D3DXVECTOR3(0, 0, 0)),
	m_Scale(D3DXVECTOR3(1, 1, 1)),
	m_Rotate(D3DXVECTOR3(0, 0, 0))
{
	// タスク生成処理.
	m_pDrawTask = new Lib::DrawTask();
	m_pUpdateTask = new Lib::UpdateTask();
	m_pDepthDrawTask = new DepthDrawTask();
	m_pMapDrawTask = new MapDrawTask();
	m_pCubeMapDrawTask = new CubeMapDrawTask();
	m_pReflectMapDrawTask = new ReflectMapDrawTask();

	// タスクにオブジェクト設定.
	m_pDrawTask->SetDrawObject(this);
	m_pUpdateTask->SetUpdateObject(this);
	m_pDepthDrawTask->SetDrawObject(this);
	m_pMapDrawTask->SetDrawObject(this);
	m_pCubeMapDrawTask->SetDrawObject(this);
	m_pReflectMapDrawTask->SetDrawObject(this);
}

Object3DBase::~Object3DBase()
{
	delete m_pReflectMapDrawTask;
	delete m_pCubeMapDrawTask;
	delete m_pMapDrawTask;
	delete m_pDepthDrawTask;
	delete m_pUpdateTask;
	delete m_pDrawTask;
}


//----------------------------------------------------------------------
// Public Functions
//----------------------------------------------------------------------
bool Object3DBase::Initialize()
{
	return true;
}

void Object3DBase::Finalize()
{
}

void Object3DBase::UpdateStartUp()
{
}

void Object3DBase::Update()
{
}

void Object3DBase::DrawStartUp()
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

void Object3DBase::CubeMapDraw()
{
}

void Object3DBase::ReflectMapDraw()
{
}


//----------------------------------------------------------------------
// Protected Functions
//----------------------------------------------------------------------
void Object3DBase::ShaderSetup()
{
	ID3D11DeviceContext* pContext = SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDeviceContext();

	pContext->VSSetShader(
		SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->GetVertexShader(m_VertexShaderIndex), 
		nullptr, 
		0);
	
	pContext->PSSetShader(
		SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->GetPixelShader(m_PixelShaderIndex), 
		nullptr,
		0);

	pContext->GSSetShader(
		nullptr,
		nullptr,
		0);
}

void Object3DBase::TextureSetup()
{
	ID3D11DeviceContext* pContext = SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDeviceContext();

	ID3D11ShaderResourceView* pResource =
		SINGLETON_INSTANCE(Lib::Dx11::TextureManager)->GetTexture(m_SkyCLUTIndex)->Get();

	pContext->PSSetShaderResources(3, 1, &pResource);
}

void Object3DBase::VertexLayoutSetup()
{
	ID3D11DeviceContext* pContext = SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDeviceContext();

	pContext->IASetInputLayout(m_pVertexLayout);
}

void Object3DBase::DepthStencilStateSetup()
{
	ID3D11DeviceContext* pContext = SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDeviceContext();

	pContext->OMSetDepthStencilState(m_pDepthStencilState, 0);
}

void Object3DBase::ConstantBufferSetup()
{
	ID3D11DeviceContext* pContext = SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDeviceContext();

	pContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	pContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);
}

bool Object3DBase::CreateShader()
{
	if (!SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadVertexShader(
		TEXT("Resource\\Effect\\DefaultEffect.fx"), 
		"VS", 
		&m_VertexShaderIndex))
	{
		OutputErrorLog("頂点シェーダーの読み込みに失敗しました");
		return false;
	}

	if (!SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->LoadPixelShader(
		TEXT("Resource\\Effect\\DefaultEffect.fx"),
		"PS",
		&m_PixelShaderIndex))
	{
		OutputErrorLog("ピクセルシェーダの読み込みに失敗しました");
		return false;
	}

	return true;
}

bool Object3DBase::CreateTexture()
{
	if (!SINGLETON_INSTANCE(Lib::Dx11::TextureManager)->LoadTexture(
		TEXT("Resource\\Texture\\MainLightCLUT.png"),
		&m_SkyCLUTIndex))
	{
		OutputErrorLog("テクスチャの読み込みに失敗しました");
		return false;
	}

	return true;
}

bool Object3DBase::CreateVertexLayout()
{
	Lib::Dx11::ShaderManager* pShaderManager = Lib::Dx11::ShaderManager::GetInstance();

	// 入力レイアウトの設定.
	D3D11_INPUT_ELEMENT_DESC InputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// 入力レイアウトの生成.
	if (FAILED(SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDevice()->CreateInputLayout(
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

bool Object3DBase::CreateDepthStencilState()
{
	// 深度ステンシルステートの設定.
	D3D11_DEPTH_STENCIL_DESC DepthStencilDesc;
	ZeroMemory(&DepthStencilDesc, sizeof(DepthStencilDesc));
	DepthStencilDesc.DepthEnable = TRUE;
	DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	DepthStencilDesc.StencilEnable = FALSE;

	// 深度ステンシルステートの生成.
	if (FAILED(SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDevice()->CreateDepthStencilState(
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
	// 定数バッファの設定.
	D3D11_BUFFER_DESC ConstantBufferDesc;
	ConstantBufferDesc.ByteWidth = sizeof(CONSTANT_BUFFER);
	ConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ConstantBufferDesc.MiscFlags = 0;
	ConstantBufferDesc.StructureByteStride = 0;

	// 定数バッファの生成.
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

bool Object3DBase::WriteConstantBuffer()
{
	ID3D11DeviceContext* pContext = SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDeviceContext();

	D3D11_MAPPED_SUBRESOURCE SubResourceData;
	if (SUCCEEDED(pContext->Map(
		m_pConstantBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&SubResourceData)))
	{
		D3DXMATRIX MatWorld, MatTranslate, MatRotateX, MatRotateY;
		D3DXMatrixIdentity(&MatWorld);
		D3DXMatrixScaling(&MatWorld, m_Scale.x, m_Scale.y, m_Scale.z);
		D3DXMatrixRotationX(&MatRotateX, m_Rotate.x);
		D3DXMatrixMultiply(&MatWorld, &MatWorld, &MatRotateX);
		D3DXMatrixRotationY(&MatRotateY, m_Rotate.y);
		D3DXMatrixMultiply(&MatWorld, &MatWorld, &MatRotateY);
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

		pContext->Unmap(m_pConstantBuffer, 0);

		return true;
	}

	return false;
}

void Object3DBase::ReleaseShader()
{
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleaseVertexShader(m_VertexShaderIndex);
	SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->ReleasePixelShader(m_PixelShaderIndex);
}

void Object3DBase::ReleaseTexture()
{
	SINGLETON_INSTANCE(Lib::Dx11::TextureManager)->ReleaseTexture(m_SkyCLUTIndex);
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