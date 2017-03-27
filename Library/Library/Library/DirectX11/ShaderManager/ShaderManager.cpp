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
		// 読み込みに失敗した際に参照する値としてNULLを追加.
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
		if (m_pGraphicsDevice == NULL)
		{
			OutputErrorLog("グラフィックデバイスがありません");
			return false;
		}

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
			SafeRelease(pShaderErrors);
			*_pIndex = m_InvalidIndex;
			return false;
		}

		SafeRelease(pShaderErrors);


		if (FAILED(m_pGraphicsDevice->GetDevice()->CreateVertexShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			NULL,
			&pVertexShader)))
		{
			SafeRelease(pCompiledShader);
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
		if (m_pGraphicsDevice == NULL)
		{
			OutputErrorLog("グラフィックデバイスがありません");
			return false;
		}

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
			SafeRelease(pShaderErrors);
			*_pIndex = m_InvalidIndex;
			return false;
		}

		SafeRelease(pShaderErrors);

		if (FAILED(m_pGraphicsDevice->GetDevice()->CreatePixelShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			NULL,
			&pPixelShader)))
		{
			SafeRelease(pCompiledShader);
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
		SafeRelease(m_pVertexShader[_index]);
		SafeRelease(m_pCompiledVertexShader[_index]);
	}

	void ShaderManager::ReleaseVertexShader()
	{
		for (auto itr = m_pVertexShader.begin(); itr != m_pVertexShader.end(); itr++)
		{
			SafeRelease(*itr);
		}

		for (auto itr = m_pCompiledVertexShader.begin(); itr != m_pCompiledVertexShader.end(); itr++)
		{
			SafeRelease(*itr);
		}
	}

	void ShaderManager::ReleasePixelShader(int _index)
	{
		SafeRelease(m_pPixelShader[_index]);
		SafeRelease(m_pCompiledPixelShader[_index]);
	}

	void ShaderManager::ReleasePixelShader()
	{
		for (auto itr = m_pPixelShader.begin(); itr != m_pPixelShader.end(); itr++)
		{
			SafeRelease(*itr);
		}

		for (auto itr = m_pCompiledPixelShader.begin(); itr != m_pCompiledPixelShader.end(); itr++)
		{
			SafeRelease(*itr);
		}
	}
}

