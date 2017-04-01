/**
 * @file   FbxFileManager.cpp
 * @brief  FbxFileManagerクラスの実装
 * @author morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "FbxFileManager.h"

#include "Debugger\Debugger.h"
#include "FbxLoader\FbxLoader.h"


namespace Lib
{
	//----------------------------------------------------------------------------------------------------
	// Static Public Variables
	//----------------------------------------------------------------------------------------------------
	const int FbxFileManager::m_InvalidIndex = 0;


	//----------------------------------------------------------------------------------------------------
	// Constructor	Destructor
	//----------------------------------------------------------------------------------------------------
	FbxFileManager::FbxFileManager() :
		m_pFbxLoader(NULL)
	{
		m_pFbxModel.push_back(NULL);	// 読み込みに失敗した際に参照する値としてNULLを追加.
	}

	FbxFileManager::~FbxFileManager()
	{
	}


	//----------------------------------------------------------------------------------------------------
	// Public Functions
	//----------------------------------------------------------------------------------------------------
	bool FbxFileManager::Initialize(GraphicsDevice* _pGraphicsDevice)
	{
		if (m_pFbxLoader != NULL)
		{
			OutputErrorLog("FbxFileManagerクラスはすでに生成されています");
			return false;
		}

		m_pGraphicsDevice = _pGraphicsDevice;
		m_pFbxLoader = new FbxLoader(m_pGraphicsDevice);

		return m_pFbxLoader->Initialize();
	}

	void FbxFileManager::Finalize()
	{
		if (m_pFbxLoader != NULL)
		{
			m_pFbxLoader->Finalize();
			SafeDelete(m_pFbxLoader);
		}
	}

	bool FbxFileManager::LoadFbxModel(LPCTSTR _pFileName, int* _pIndex)
	{
		FbxModel* pModel = new FbxModel(m_pGraphicsDevice);
		if (!m_pFbxLoader->LoadFbxModel(pModel, _pFileName))
		{
			OutputErrorLog("Fbxモデルの読み込みに失敗しました");

			*_pIndex = m_InvalidIndex;
			SafeDelete(pModel);

			return false;
		}

		if (!pModel->Initialize())
		{
			OutputErrorLog("Fbxモデルの初期化に失敗しました");

			pModel->Finalize();
			SafeDelete(pModel);

			return false;
		}

		*_pIndex = m_pFbxModel.size();
		m_pFbxModel.push_back(pModel);

		return true;
	}

	void FbxFileManager::ReleaseFbxModel(int _index)
	{
		if (m_pFbxModel[_index] != NULL)
		{
			m_pFbxModel[_index]->Finalize();
			SafeDelete(m_pFbxModel[_index]);
		}
	}
}
