/**
 * @file	Font.cpp
 * @brief	フォントクラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "Font.h"


namespace Lib
{
	//----------------------------------------------------------------------
	// Static Private Variables
	//----------------------------------------------------------------------
	const float Font::m_FontTu = 1.0f / 95;
	const int Font::m_SpaceAsciiCode = 32;

	//----------------------------------------------------------------------
	// Constructor	Destructor
	//----------------------------------------------------------------------
	Font::Font() :
		m_pGraphicsDevice(NULL),
		m_pVertexShader(NULL),
		m_pVertexCompiledShader(NULL),
		m_pVertexLayout(NULL),
		m_pPixelShader(NULL),
		m_pPixelCompiledShader(NULL),
		m_pBlendState(NULL),
		m_pSamplerState(NULL),
		m_pVertexBuffer(NULL),
		m_pConstantBuffer(NULL),
		m_pShaderResource(NULL),
		m_WindowWidth(0),
		m_WindowHeight(0)
	{
	}

	Font::~Font()
	{
		Finalize();
	}


	//----------------------------------------------------------------------
	// Public Functions
	//----------------------------------------------------------------------
	bool Font::Initialize(GraphicsDevice* _pGraphicsDevice)
	{
		if (m_pGraphicsDevice != NULL)
		{
			return false;
		}

		m_pGraphicsDevice = _pGraphicsDevice;

		RECT WindowRect;
		GetWindowRect(m_pGraphicsDevice->GetMainWindowHandle(), &WindowRect);
		m_WindowWidth = static_cast<float>(WindowRect.right);
		m_WindowHeight = static_cast<float>(WindowRect.bottom);


		if (!InitVertexShader())
		{
			return false;
		}

		if (!InitVertexLayout())
		{
			ReleaseVertexShader();
			return false;
		}

		if (!InitPixelShader())
		{
			ReleaseVertexLayout();
			ReleaseVertexShader();
			return false;
		}

		if (!InitState())
		{
			ReleasePixelShader();
			ReleaseVertexLayout();
			ReleaseVertexShader();
			return false;
		}

		if (!InitResourceView())
		{
			ReleaseState();
			ReleasePixelShader();
			ReleaseVertexLayout();
			ReleaseVertexShader();
			return false;
		}

		return true;
	}

	void Font::Finalize()
	{
		ReleaseResourceView();
		ReleaseState();
		ReleasePixelShader();
		ReleaseVertexLayout();
		ReleaseVertexShader();
	}

	bool Font::CreateVertexBuffer(const D3DXVECTOR2* _pRect, const D3DXCOLOR* _pColor)
	{
		m_FontSize = *_pRect;
		FONT_VERTEX FontVertex[] =
		{
			D3DXVECTOR3(-m_FontSize.x / 2, -m_FontSize.y / 2, 0), D3DXVECTOR2(0,        0), *_pColor,
			D3DXVECTOR3( m_FontSize.x / 2, -m_FontSize.y / 2, 0), D3DXVECTOR2(m_FontTu, 0), *_pColor,
			D3DXVECTOR3(-m_FontSize.x / 2,  m_FontSize.y / 2, 0), D3DXVECTOR2(0,        1), *_pColor,
			D3DXVECTOR3( m_FontSize.x / 2,  m_FontSize.y / 2, 0), D3DXVECTOR2(m_FontTu, 1), *_pColor,
		};

		D3D11_BUFFER_DESC BufferDesc;
		ZeroMemory(&BufferDesc, sizeof(BufferDesc));
		BufferDesc.ByteWidth = sizeof(FontVertex);
		BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		BufferDesc.CPUAccessFlags = 0;
		BufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA ResourceData;
		ZeroMemory(&ResourceData, sizeof(ResourceData));
		ResourceData.pSysMem = reinterpret_cast<void*>(FontVertex);
		ResourceData.SysMemPitch = 0;
		ResourceData.SysMemSlicePitch = 0;

		if (FAILED(m_pGraphicsDevice->GetDevice()->CreateBuffer(&BufferDesc, &ResourceData, &m_pVertexBuffer)))
		{
			return false;
		}

		return true;
	}

	void Font::ReleaseVertexBuffer()
	{
		if (m_pVertexBuffer != NULL)
		{
			m_pVertexBuffer->Release();
			m_pVertexBuffer = NULL;
		}
	}

	void Font::Draw(const D3DXVECTOR2* _pDrawPos, LPCTSTR _pStr)
	{
		m_pGraphicsDevice->GetDeviceContext()->VSSetShader(m_pVertexShader, NULL, 0);
		m_pGraphicsDevice->GetDeviceContext()->PSSetShader(m_pPixelShader, NULL, 0);

		m_pGraphicsDevice->GetDeviceContext()->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
		m_pGraphicsDevice->GetDeviceContext()->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);

		m_pGraphicsDevice->GetDeviceContext()->IASetInputLayout(m_pVertexLayout);
		m_pGraphicsDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		m_pGraphicsDevice->GetDeviceContext()->PSSetSamplers(0, 1, &m_pSamplerState);
		m_pGraphicsDevice->GetDeviceContext()->PSSetShaderResources(0, 1, &m_pShaderResource);

		m_pGraphicsDevice->GetDeviceContext()->OMSetBlendState(m_pBlendState, NULL, 0xffffffff);

		UINT Stride = sizeof(FONT_VERTEX);
		UINT Offset = 0;
		m_pGraphicsDevice->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &Stride, &Offset);


		D3DXMATRIX MatWorld, MatTranslate, MatRotate;
		D3DXMatrixIdentity(&MatWorld);
		D3DXMatrixScaling(&MatWorld, 1.0f, 1.0f, 1.0f);
		D3DXMatrixRotationZ(&MatRotate, 0.0f);
		D3DXMatrixMultiply(&MatWorld, &MatWorld, &MatRotate);
		D3DXMatrixTranslation(&MatTranslate, _pDrawPos->x, _pDrawPos->y, 0.0f);
		D3DXMatrixMultiply(&MatWorld, &MatWorld, &MatTranslate);

		D3D11_MAPPED_SUBRESOURCE MappedResource;
		CONSTANT_BUFFER ConstantBuffer;
		for (int i = 0; i < static_cast<int>(strlen(_pStr)); i++)
		{
			m_pGraphicsDevice->GetDeviceContext()->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);

			ConstantBuffer.MatWorld = MatWorld;
			D3DXMatrixTranspose(&ConstantBuffer.MatWorld, &ConstantBuffer.MatWorld);

			ConstantBuffer.WindowSize.x = m_WindowWidth;
			ConstantBuffer.WindowSize.y = m_WindowHeight;

			ConstantBuffer.TexelOffset.x = static_cast<float>(_pStr[i] - m_SpaceAsciiCode) * m_FontTu;
			ConstantBuffer.TexelOffset.y = 0.0f;
			ConstantBuffer.TexelOffset.z = 0.0f;
			ConstantBuffer.TexelOffset.w = 0.0f;

			ConstantBuffer.PosOffset.x = i * m_FontSize.x;
			ConstantBuffer.PosOffset.y = 0;
			ConstantBuffer.PosOffset.z = 0;
			ConstantBuffer.PosOffset.w = 0;

			memcpy_s(
				MappedResource.pData,
				MappedResource.RowPitch,
				reinterpret_cast<void*>(&ConstantBuffer),
				sizeof(ConstantBuffer));

			m_pGraphicsDevice->GetDeviceContext()->Unmap(m_pConstantBuffer, 0);

			m_pGraphicsDevice->GetDeviceContext()->Draw(VERTEX_NUM, 0);
		}
	}

	//----------------------------------------------------------------------
	// Private Functions
	//----------------------------------------------------------------------
	bool Font::InitVertexShader()
	{
		ID3DBlob* pShaderErrors = NULL;
		if (FAILED(D3DX11CompileFromFile(
			TEXT("Library\\DirectX11\\Font\\Effect\\Font.fx"),
			NULL,
			NULL,
			"VS",
			"vs_5_0",
			D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION,
			0,
			NULL,
			&m_pVertexCompiledShader,
			&pShaderErrors,
			NULL)))
		{
			if (pShaderErrors != NULL) pShaderErrors->Release();
			return false;
		}

		if (pShaderErrors != NULL) pShaderErrors->Release();

		if (FAILED(m_pGraphicsDevice->GetDevice()->CreateVertexShader(
			m_pVertexCompiledShader->GetBufferPointer(),
			m_pVertexCompiledShader->GetBufferSize(),
			NULL,
			&m_pVertexShader)))
		{
			m_pVertexCompiledShader->Release();
			m_pVertexCompiledShader = NULL;
			return false;
		}

		return true;
	}

	bool Font::InitVertexLayout()
	{
		D3D11_INPUT_ELEMENT_DESC InputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		if (FAILED(m_pGraphicsDevice->GetDevice()->CreateInputLayout(
			InputElementDesc,
			sizeof(InputElementDesc) / sizeof(InputElementDesc[0]),
			m_pVertexCompiledShader->GetBufferPointer(),
			m_pVertexCompiledShader->GetBufferSize(),
			&m_pVertexLayout)))
		{
			return false;
		}

		return true;
	}

	bool Font::InitPixelShader()
	{
		ID3DBlob* pShaderErrors = NULL;
		if (FAILED(D3DX11CompileFromFile(
			TEXT("Library\\DirectX11\\Font\\Effect\\Font.fx"),
			NULL,
			NULL,
			"PS",
			"ps_5_0",
			D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION,
			0,
			NULL,
			&m_pPixelCompiledShader,
			&pShaderErrors,
			NULL)))
		{
			if (pShaderErrors != NULL) pShaderErrors->Release();
			return false;
		}

		if (pShaderErrors != NULL) pShaderErrors->Release();

		if (FAILED(m_pGraphicsDevice->GetDevice()->CreatePixelShader(
			m_pPixelCompiledShader->GetBufferPointer(),
			m_pPixelCompiledShader->GetBufferSize(),
			NULL,
			&m_pPixelShader)))
		{
			m_pPixelCompiledShader->Release();
			m_pPixelCompiledShader = NULL;
			return false;
		}

		return true;
	}

	bool Font::InitState()
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
		if (FAILED(m_pGraphicsDevice->GetDevice()->CreateBlendState(&BlendDesc, &m_pBlendState)))
		{
			return false;
		}

		D3D11_SAMPLER_DESC SamplerDesc;
		ZeroMemory(&SamplerDesc, sizeof(D3D11_SAMPLER_DESC));
		SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		if (FAILED(m_pGraphicsDevice->GetDevice()->CreateSamplerState(&SamplerDesc, &m_pSamplerState)))
		{
			return false;
		}

		D3D11_BUFFER_DESC ConstantBufferDesc;
		ZeroMemory(&ConstantBufferDesc, sizeof(D3D11_BUFFER_DESC));
		ConstantBufferDesc.ByteWidth = sizeof(CONSTANT_BUFFER);
		ConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		ConstantBufferDesc.MiscFlags = 0;
		ConstantBufferDesc.StructureByteStride = 0;
		if (FAILED(m_pGraphicsDevice->GetDevice()->CreateBuffer(&ConstantBufferDesc, NULL, &m_pConstantBuffer)))
		{
			return false;
		}

		return true;
	}

	bool Font::InitResourceView()
	{
		D3DX11_IMAGE_LOAD_INFO LoadInfo;
		ZeroMemory(&LoadInfo, sizeof(D3DX11_IMAGE_LOAD_INFO));
		LoadInfo.Width = D3DX11_DEFAULT;
		LoadInfo.Height = D3DX11_DEFAULT;
		LoadInfo.Depth = D3DX11_DEFAULT;
		LoadInfo.FirstMipLevel = D3DX11_DEFAULT;
		LoadInfo.MipLevels = 1;
		LoadInfo.Usage = D3D11_USAGE_DEFAULT;
		LoadInfo.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		LoadInfo.CpuAccessFlags = 0;
		LoadInfo.MiscFlags = 0;
		LoadInfo.Format = DXGI_FORMAT_FROM_FILE;
		LoadInfo.Filter = D3DX11_FILTER_POINT;
		LoadInfo.MipFilter = D3DX11_FILTER_POINT;
		LoadInfo.pSrcInfo = NULL;

		if (FAILED(D3DX11CreateShaderResourceViewFromFile(
			m_pGraphicsDevice->GetDevice(),
			TEXT("Library\\DirectX11\\Font\\Texture\\Font.png"),
			&LoadInfo,
			NULL,
			&m_pShaderResource,
			NULL)))
		{
			return false;
		}

		return true;
	}

	void Font::ReleaseVertexShader()
	{
		if (m_pVertexShader != NULL)
		{
			m_pVertexShader->Release();
			m_pVertexShader = NULL;
		}

		if (m_pVertexCompiledShader != NULL)
		{
			m_pVertexCompiledShader->Release();
			m_pVertexCompiledShader = NULL;
		}
	}

	void Font::ReleaseVertexLayout()
	{
		if (m_pVertexLayout != NULL)
		{
			m_pVertexLayout->Release();
			m_pVertexLayout = NULL;
		}
	}

	void Font::ReleasePixelShader()
	{
		if (m_pPixelShader != NULL)
		{
			m_pPixelShader->Release();
			m_pPixelShader = NULL;
		}

		if (m_pPixelCompiledShader != NULL)
		{
			m_pPixelCompiledShader->Release();
			m_pPixelCompiledShader = NULL;
		}
	}

	void Font::ReleaseState()
	{
		if (m_pConstantBuffer != NULL)
		{
			m_pConstantBuffer->Release();
			m_pConstantBuffer = NULL;
		}

		if (m_pSamplerState != NULL)
		{
			m_pSamplerState->Release();
			m_pSamplerState = NULL;
		}

		if (m_pBlendState != NULL)
		{
			m_pBlendState->Release();
			m_pBlendState = NULL;
		}
	}

	void Font::ReleaseResourceView()
	{
		if (m_pShaderResource != NULL)
		{
			m_pShaderResource->Release();
			m_pShaderResource = NULL;
		}
	}
}
