/**
 * @file	ShaderManager.cpp
 * @brief	シェーダーの管理クラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "ShaderManager.h"

#include "..\..\Debugger\Debugger.h"


namespace Lib
{
	//----------------------------------------------------------------------
	// Static Private Variables
	//----------------------------------------------------------------------
	const int ShaderManager::m_InvalidIndex = 0;


	//----------------------------------------------------------------------
	// Constructor	Destructor
	//----------------------------------------------------------------------
	ShaderManager::ShaderManager() : 
		m_pGraphicsDevice(NULL)
	{
		// 読み込みに失敗した際に参照する値としてNULLを追加
		m_pVertexShader.push_back(NULL);
		m_pPixelShader.push_back(NULL);
		m_pCompiledVertexShader.push_back(NULL);
		m_pCompiledPixelShader.push_back(NULL);
	}

	ShaderManager::~ShaderManager()
	{
		Finalize();
	}

	//----------------------------------------------------------------------
	// Public Functions
	//----------------------------------------------------------------------
	bool ShaderManager::Initialize(GraphicsDevice* _pGraphicsDevice)
	{
		m_pGraphicsDevice = _pGraphicsDevice;
		return true;
	}

	void ShaderManager::Finalize()
	{
		ReleaseVertexShader();
		ReleasePixelShader();
	}

	bool ShaderManager::LoadVertexShader(LPCTSTR _pFilePath, LPCTSTR _pFuncName, int* _pIndex)
	{
		MyAssert(m_pGraphicsDevice != NULL, "GraphicsDeviceがありません");


		ID3D11VertexShader* pVertexShader = NULL;
		ID3DBlob* pShaderErrors = NULL;
		ID3DBlob* pCompiledShader = NULL;
		if (FAILED(D3DX11CompileFromFile(
			_pFilePath,
			NULL,
			NULL,
			_pFuncName,
			"vs_5_0",
			D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION,
			0,
			NULL,
			&pCompiledShader,
			&pShaderErrors,
			NULL)))
		{
			if (pShaderErrors != NULL) pShaderErrors->Release();
			*_pIndex = m_InvalidIndex;
			return false;
		}

		if (pShaderErrors != NULL) pShaderErrors->Release();


		if (FAILED(m_pGraphicsDevice->GetDevice()->CreateVertexShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			NULL,
			&pVertexShader)))
		{
			pCompiledShader->Release();
			*_pIndex = m_InvalidIndex;
			return false;
		}

		*_pIndex = m_pVertexShader.size();
		m_pVertexShader.push_back(pVertexShader);
		m_pCompiledVertexShader.push_back(pCompiledShader);

		return true;
	}

	bool ShaderManager::LoadPixelShader(LPCTSTR _pFilePath, LPCTSTR _pFuncName, int* _pIndex)
	{
		MyAssert(m_pGraphicsDevice != NULL, "DirectX11デバイスがありません");


		ID3D11PixelShader* pPixelShader = NULL;
		ID3DBlob* pShaderErrors = NULL;
		ID3DBlob* pCompiledShader = NULL;
		if (FAILED(D3DX11CompileFromFile(
			_pFilePath,
			NULL,
			NULL,
			_pFuncName,
			"ps_5_0",
			D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION,
			0,
			NULL,
			&pCompiledShader,
			&pShaderErrors,
			NULL)))
		{
			if (pShaderErrors != NULL) pShaderErrors->Release();
			*_pIndex = m_InvalidIndex;
			return false;
		}

		if (pShaderErrors != NULL) pShaderErrors->Release();

		if (FAILED(m_pGraphicsDevice->GetDevice()->CreatePixelShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			NULL,
			&pPixelShader)))
		{
			pCompiledShader->Release();
			*_pIndex = m_InvalidIndex;
			return false;
		}

		*_pIndex = m_pPixelShader.size();
		m_pPixelShader.push_back(pPixelShader);
		m_pCompiledPixelShader.push_back(pCompiledShader);

		return true;
	}

	void ShaderManager::ReleaseVertexShader(int _index)
	{
		if (m_pVertexShader[_index] != NULL)
		{
			m_pVertexShader[_index]->Release();
			m_pVertexShader[_index] = NULL;
			m_pCompiledVertexShader[_index]->Release();
			m_pCompiledVertexShader[_index] = NULL;
		}
	}

	void ShaderManager::ReleaseVertexShader()
	{
		for (auto itr = m_pVertexShader.begin(); itr != m_pVertexShader.end(); itr++)
		{
			if ((*itr) != NULL)
			{
				(*itr)->Release();
				(*itr) = NULL;
			}
		}

		for (auto itr = m_pCompiledVertexShader.begin(); itr != m_pCompiledVertexShader.end(); itr++)
		{
			if ((*itr) != NULL)
			{
				(*itr)->Release();
				(*itr) = NULL;
			}
		}
	}

	void ShaderManager::ReleasePixelShader(int _index)
	{
		if (m_pPixelShader[_index] != NULL)
		{
			m_pPixelShader[_index]->Release();
			m_pPixelShader[_index] = NULL;
			m_pCompiledPixelShader[_index]->Release();
			m_pCompiledPixelShader[_index] = NULL;
		}
	}

	void ShaderManager::ReleasePixelShader()
	{
		for (auto itr = m_pPixelShader.begin(); itr != m_pPixelShader.end(); itr++)
		{
			if ((*itr) != NULL)
			{
				(*itr)->Release();
				(*itr) = NULL;
			}
		}

		for (auto itr = m_pCompiledPixelShader.begin(); itr != m_pCompiledPixelShader.end(); itr++)
		{
			if ((*itr) != NULL)
			{
				(*itr)->Release();
				(*itr) = NULL;
			}
		}
	}
}

