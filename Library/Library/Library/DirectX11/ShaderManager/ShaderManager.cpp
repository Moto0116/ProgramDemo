﻿/**
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
			OutputErrorLog("fxファイルの読み込みに失敗しました");
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
			OutputErrorLog("頂点シェーダーの生成に失敗しました");
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
			OutputErrorLog("fxファイルの読み込みに失敗しました");
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
			OutputErrorLog("ピクセルシェーダーの生成に失敗しました");
			SafeRelease(pCompiledShader);
			*_pIndex = m_InvalidIndex;
			return false;
		}

		*_pIndex = m_pPixelShader.size();
		m_pPixelShader.push_back(pPixelShader);
		m_pCompiledPixelShader.push_back(pCompiledShader);

		return true;
	}


	bool ShaderManager::LoadGeometryShader(LPCTSTR _pFilePath, LPCTSTR _pFuncName, int* _pIndex)
	{
		if (m_pGraphicsDevice == NULL)
		{
			OutputErrorLog("グラフィックデバイスがありません");
			return false;
		}

		ID3D11GeometryShader* pGeometryShader = NULL;
		ID3DBlob* pShaderErrors = NULL;
		ID3DBlob* pCompiledShader = NULL;
		if (FAILED(D3DX11CompileFromFile(
			_pFilePath,
			NULL,
			NULL,
			_pFuncName,
			"gs_5_0",
			D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION,
			0,
			NULL,
			&pCompiledShader,
			&pShaderErrors,
			NULL)))
		{
			OutputErrorLog("fxファイルの読み込みに失敗しました");
			SafeRelease(pShaderErrors);
			*_pIndex = m_InvalidIndex;
			return false;
		}

		SafeRelease(pShaderErrors);

		if (FAILED(m_pGraphicsDevice->GetDevice()->CreateGeometryShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			NULL,
			&pGeometryShader)))
		{
			OutputErrorLog("ジオメトリシェーダーの生成に失敗しました");
			SafeRelease(pCompiledShader);
			*_pIndex = m_InvalidIndex;
			return false;
		}

		*_pIndex = m_pGeometryShader.size();
		m_pGeometryShader.push_back(pGeometryShader);
		m_pCompiledGeometryShader.push_back(pCompiledShader);

		return true;
	}

	bool ShaderManager::LoadHullShader(LPCTSTR _pFilePath, LPCTSTR _pFuncName, int* _pIndex)
	{
		if (m_pGraphicsDevice == NULL)
		{
			OutputErrorLog("グラフィックデバイスがありません");
			return false;
		}

		ID3D11HullShader* pHullShader = NULL;
		ID3DBlob* pShaderErrors = NULL;
		ID3DBlob* pCompiledShader = NULL;
		if (FAILED(D3DX11CompileFromFile(
			_pFilePath,
			NULL,
			NULL,
			_pFuncName,
			"hs_5_0",
			D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION,
			0,
			NULL,
			&pCompiledShader,
			&pShaderErrors,
			NULL)))
		{
			OutputErrorLog("fxファイルの読み込みに失敗しました");
			SafeRelease(pShaderErrors);
			*_pIndex = m_InvalidIndex;
			return false;
		}

		SafeRelease(pShaderErrors);

		if (FAILED(m_pGraphicsDevice->GetDevice()->CreateHullShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			NULL,
			&pHullShader)))
		{
			OutputErrorLog("ハルシェーダーの生成に失敗しました");
			SafeRelease(pCompiledShader);
			*_pIndex = m_InvalidIndex;
			return false;
		}

		*_pIndex = m_pHullShader.size();
		m_pHullShader.push_back(pHullShader);
		m_pCompiledHullShader.push_back(pCompiledShader);

		return true;
	}

	bool ShaderManager::LoadDomainShader(LPCTSTR _pFilePath, LPCTSTR _pFuncName, int* _pIndex)
	{
		if (m_pGraphicsDevice == NULL)
		{
			OutputErrorLog("グラフィックデバイスがありません");
			return false;
		}

		ID3D11DomainShader* pDomainShader = NULL;
		ID3DBlob* pShaderErrors = NULL;
		ID3DBlob* pCompiledShader = NULL;
		if (FAILED(D3DX11CompileFromFile(
			_pFilePath,
			NULL,
			NULL,
			_pFuncName,
			"ds_5_0",
			D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION,
			0,
			NULL,
			&pCompiledShader,
			&pShaderErrors,
			NULL)))
		{
			OutputErrorLog("fxファイルの読み込みに失敗しました");
			SafeRelease(pShaderErrors);
			*_pIndex = m_InvalidIndex;
			return false;
		}

		SafeRelease(pShaderErrors);

		if (FAILED(m_pGraphicsDevice->GetDevice()->CreateDomainShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			NULL,
			&pDomainShader)))
		{
			OutputErrorLog("ドメインシェーダーの生成に失敗しました");
			SafeRelease(pCompiledShader);
			*_pIndex = m_InvalidIndex;
			return false;
		}

		*_pIndex = m_pDomainShader.size();
		m_pDomainShader.push_back(pDomainShader);
		m_pCompiledDomainShader.push_back(pCompiledShader);

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

	void ShaderManager::ReleaseGeometryShader(int _index)
	{
		SafeRelease(m_pGeometryShader[_index]);
		SafeRelease(m_pCompiledGeometryShader[_index]);
	}

	void ShaderManager::ReleaseGeometryShader()
	{
		for (auto itr = m_pGeometryShader.begin(); itr != m_pGeometryShader.end(); itr++)
		{
			SafeRelease(*itr);
		}

		for (auto itr = m_pCompiledGeometryShader.begin(); itr != m_pCompiledGeometryShader.end(); itr++)
		{
			SafeRelease(*itr);
		}
	}

	void ShaderManager::ReleaseHullShader(int _index)
	{
		SafeRelease(m_pHullShader[_index]);
		SafeRelease(m_pCompiledHullShader[_index]);
	}

	void ShaderManager::ReleaseHullShader()
	{
		for (auto itr = m_pHullShader.begin(); itr != m_pHullShader.end(); itr++)
		{
			SafeRelease(*itr);
		}

		for (auto itr = m_pCompiledHullShader.begin(); itr != m_pCompiledHullShader.end(); itr++)
		{
			SafeRelease(*itr);
		}
	}

	void ShaderManager::ReleaseDomainShader(int _index)
	{
		SafeRelease(m_pDomainShader[_index]);
		SafeRelease(m_pCompiledDomainShader[_index]);
	}

	void ShaderManager::ReleaseDomainShader()
	{
		for (auto itr = m_pDomainShader.begin(); itr != m_pDomainShader.end(); itr++)
		{
			SafeRelease(*itr);
		}

		for (auto itr = m_pCompiledDomainShader.begin(); itr != m_pCompiledDomainShader.end(); itr++)
		{
			SafeRelease(*itr);
		}
	}
}

