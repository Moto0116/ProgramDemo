/**
 * @file   FbxModel.cpp
 * @brief  FbxModelクラスの実装
 * @author morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "FbxModel.h"

#include "Debugger\Debugger.h"


namespace Lib
{
	//----------------------------------------------------------------------------------------------------
	// Constructor	Destructor
	//----------------------------------------------------------------------------------------------------
	FbxModel::FbxModel(GraphicsDevice* _pGraphicsDevice) :
		m_pGraphicsDevice(_pGraphicsDevice),
		m_ppIndexBuffer(NULL),
		m_ppVertexBuffer(NULL),
		m_ppVertexData(NULL),
		m_pSamplerState(NULL),
		m_pMaterialConstantBuffer(NULL)
	{
	}

	FbxModel::~FbxModel()
	{
	}


	//----------------------------------------------------------------------------------------------------
	// Public Functions
	//----------------------------------------------------------------------------------------------------
	void FbxModel::AddMeshData(MESH_DATA* _pMeshData)
	{
		m_MeshData.push_back(*_pMeshData);
	}

	bool FbxModel::Initialize()
	{
		if (m_MeshData.size() == 0)
		{
			OutputErrorLog("メッシュデータがありません\n");
			return false;
		}

		if (!InitIndexBuffer())
		{
			OutputErrorLog("インデックスバッファの初期化に失敗しました\n");
			return false;
		}

		if (!InitVertexBuffer())
		{
			OutputErrorLog("頂点バッファの初期化に失敗しました\n");

			ReleaseIndexBuffer();

			return false;
		}

		if (!InitSamplerState())
		{
			OutputErrorLog("サンプラステートの初期化に失敗しました\n");

			ReleaseVertexBuffer();
			ReleaseIndexBuffer();

			return false;
		}

		if (!InitConstantBuffer())
		{
			OutputErrorLog("定数バッファの初期化に失敗しました\n");

			ReleaseSamplerState();
			ReleaseVertexBuffer();
			ReleaseIndexBuffer();
		}

		WriteConstantBuffer();	///@todo 現状は複数メッシュの対応はしない

		return true;
	}

	void FbxModel::Finalize()
	{
		ReleaseConstantBuffer();
		ReleaseSamplerState();
		ReleaseVertexBuffer();
		ReleaseIndexBuffer();

		for (unsigned int i = 0; i < m_MeshData.size(); i++)
		{
			delete[] m_MeshData[i].pMaterialData->pTextureName;
			m_MeshData[i].pMaterialData->pTextureName = NULL;

			delete[] m_MeshData[i].pMaterialData->pTextureUVSetName;
			m_MeshData[i].pMaterialData->pTextureUVSetName = NULL;

			for (int n = 0; n < m_MeshData[i].pMaterialData->TextureCount; n++)
			{
				m_MeshData[i].pMaterialData->pTextureView[n]->Release();
			}

			delete[] m_MeshData[i].pMaterialData->pTextureView;
			m_MeshData[i].pMaterialData->pTextureView = NULL;

			delete[] m_MeshData[i].pMaterialData;
			m_MeshData[i].pMaterialData = NULL;

			for (int n = 0; n < m_MeshData[i].pTextureData->TextureUVCount; n++)
			{
				delete[] m_MeshData[i].pTextureData->pTextureUVData[n].pTextureUV;
				m_MeshData[i].pTextureData->pTextureUVData[n].pTextureUV = NULL;
			}

			delete[] m_MeshData[i].pTextureData->pTextureUVData;
			m_MeshData[i].pTextureData->pTextureUVData = NULL;

			delete m_MeshData[i].pTextureData;
			m_MeshData[i].pTextureData = NULL;

			delete[] m_MeshData[i].pNormalData->pNormalVec;
			m_MeshData[i].pNormalData->pNormalVec = NULL;

			delete m_MeshData[i].pNormalData;
			m_MeshData[i].pNormalData = NULL;

			delete[] m_MeshData[i].pVertexData->pVertex;
			m_MeshData[i].pVertexData->pVertex = NULL;

			delete[] m_MeshData[i].pVertexData->pIndexAry;
			m_MeshData[i].pVertexData->pIndexAry = NULL;

			delete m_MeshData[i].pVertexData;
			m_MeshData[i].pVertexData = NULL;
		}
	}

	void FbxModel::Draw(int _meshNum)
	{
		UINT stride = sizeof(FBXMODEL_VERTEX);
		UINT offset = 0;

		m_pGraphicsDevice->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_ppVertexBuffer[_meshNum], &stride, &offset);
		m_pGraphicsDevice->GetDeviceContext()->IASetIndexBuffer(m_ppIndexBuffer[_meshNum], DXGI_FORMAT_R32_UINT, 0);
		m_pGraphicsDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_pGraphicsDevice->GetDeviceContext()->PSSetSamplers(0, 1, &m_pSamplerState);

		m_pGraphicsDevice->GetDeviceContext()->OMSetBlendState(NULL, NULL, 0xffffffff);

		m_pGraphicsDevice->GetDeviceContext()->VSSetConstantBuffers(3, 1, &m_pMaterialConstantBuffer);
		m_pGraphicsDevice->GetDeviceContext()->PSSetConstantBuffers(3, 1, &m_pMaterialConstantBuffer);

		for (int i = 0; i < m_MeshData[_meshNum].pMaterialData->TextureCount; i++)
		{
			m_pGraphicsDevice->GetDeviceContext()->PSSetShaderResources(i, 1, &m_MeshData[_meshNum].pMaterialData->pTextureView[i]);
		}

		m_pGraphicsDevice->GetDeviceContext()->DrawIndexed(m_MeshData[_meshNum].pVertexData->PolygonVertexNum, 0, 0);
	}

	void FbxModel::AnimationDraw()
	{
		/// @todo アニメーション情報の取得がまだなので未実装
	}

	void FbxModel::SetAnimationFrame(int _setFrame)
	{
		/// @todo アニメーション情報の取得がまだなので未実装
	}

	int FbxModel::GetAnimationFrame()
	{
		/// @todo アニメーション情報の取得がまだなので未実装(とりあえず0だけ返す)
		return 0;
	}


	//----------------------------------------------------------------------------------------------------
	// Private Functions
	//----------------------------------------------------------------------------------------------------
	bool FbxModel::InitIndexBuffer()
	{
		m_ppIndexBuffer = new ID3D11Buffer*[m_MeshData.size()];

		for (unsigned int MeshIndex = 0; MeshIndex < m_MeshData.size(); MeshIndex++)
		{
			int PolygonVertexNum = m_MeshData[MeshIndex].pVertexData->PolygonVertexNum;	// メッシュの総頂点数

			D3D11_BUFFER_DESC BufferDesc;
			ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
			BufferDesc.ByteWidth = sizeof(int) * PolygonVertexNum;
			BufferDesc.Usage = D3D11_USAGE_DEFAULT;
			BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			BufferDesc.CPUAccessFlags = 0;
			BufferDesc.MiscFlags = 0;
			BufferDesc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA ResourceData;
			ZeroMemory(&ResourceData, sizeof(ResourceData));
			ResourceData.pSysMem = m_MeshData[MeshIndex].pVertexData->pIndexAry;
			ResourceData.SysMemPitch = 0;
			ResourceData.SysMemSlicePitch = 0;

			if (FAILED(m_pGraphicsDevice->GetDevice()->CreateBuffer(&BufferDesc, &ResourceData, &m_ppIndexBuffer[MeshIndex])))
			{
				OutputErrorLog(TEXT("インデックスバッファの作成に失敗しました\n"));
				for (unsigned int i = 0; i < MeshIndex; i++)
				{
					m_ppIndexBuffer[i]->Release();
				}
				delete[] m_ppIndexBuffer;
				m_ppIndexBuffer = NULL;
				return false;
			}
		}

		return true;
	}

	bool FbxModel::InitVertexBuffer()
	{
		m_ppVertexBuffer = new ID3D11Buffer*[m_MeshData.size()];
		m_ppVertexData = new FBXMODEL_VERTEX*[m_MeshData.size()];

		for (unsigned int MeshIndex = 0; MeshIndex < m_MeshData.size(); MeshIndex++)
		{
			int ControlPositionNum = m_MeshData[MeshIndex].pVertexData->ControlPositionNum;	// メッシュのインデックスバッファが指す総頂点数
			D3DXVECTOR3* pVertex = m_MeshData[MeshIndex].pVertexData->pVertex;				// メッシュが持つ頂点座標の配列
			D3DXVECTOR3* pNormal = m_MeshData[MeshIndex].pNormalData->pNormalVec;			// メッシュが持つ法線ベクトルの配列

			/// @todo テクスチャ座標は今のところ最初のだけ適用
			D3DXVECTOR2* pUVVertex = m_MeshData[MeshIndex].pTextureData->pTextureUVData[0].pTextureUV;	// メッシュが持つテクスチャ座標の配列

			m_ppVertexData[MeshIndex] = new FBXMODEL_VERTEX[ControlPositionNum];

			for (int i = 0; i < ControlPositionNum; i++)
			{
				m_ppVertexData[MeshIndex][i].Pos = pVertex[i];
				m_ppVertexData[MeshIndex][i].Normal = pNormal[i];
				m_ppVertexData[MeshIndex][i].Texel = pUVVertex[i];
			}

			D3D11_BUFFER_DESC BufferDesc;
			ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
			BufferDesc.ByteWidth = sizeof(FBXMODEL_VERTEX) * ControlPositionNum;
			BufferDesc.Usage = D3D11_USAGE_DEFAULT;
			BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			BufferDesc.CPUAccessFlags = 0;
			BufferDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA ResourceData;
			ZeroMemory(&ResourceData, sizeof(ResourceData));
			ResourceData.pSysMem = m_ppVertexData[MeshIndex];
			ResourceData.SysMemPitch = 0;
			ResourceData.SysMemSlicePitch = 0;

			if (FAILED(m_pGraphicsDevice->GetDevice()->CreateBuffer(&BufferDesc, &ResourceData, &m_ppVertexBuffer[MeshIndex])))
			{
				OutputErrorLog("頂点バッファの作成に失敗しました\n");
				for (unsigned int i = 0; i < MeshIndex; i++)
				{
					m_ppVertexBuffer[i]->Release();
				}
				delete[] m_ppVertexBuffer;
				m_ppVertexBuffer = NULL;

				return false;
			}
		}

		return true;
	}

	bool FbxModel::InitSamplerState()
	{
		D3D11_SAMPLER_DESC SamplerDesc;
		ZeroMemory(&SamplerDesc, sizeof(SamplerDesc));
		SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		SamplerDesc.MipLODBias = 0;
		SamplerDesc.MaxAnisotropy = 0;
		SamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		for (int i = 0; i < 4; i++)
		{
			SamplerDesc.BorderColor[i] = 0.0f;
		}
		SamplerDesc.MinLOD = 0;
		SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		if (FAILED(m_pGraphicsDevice->GetDevice()->CreateSamplerState(&SamplerDesc, &m_pSamplerState)))
		{
			OutputErrorLog("サンプラステートの生成に失敗");
			return false;
		}

		return true;
	}

	bool FbxModel::InitConstantBuffer()
	{
		D3D11_BUFFER_DESC ConstantBufferDesc;
		ConstantBufferDesc.ByteWidth = sizeof(MATERIAL_CONSTANT_BUFFER);
		ConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		ConstantBufferDesc.MiscFlags = 0;
		ConstantBufferDesc.StructureByteStride = 0;

		if (FAILED(m_pGraphicsDevice->GetDevice()->CreateBuffer(
			&ConstantBufferDesc, 
			NULL, 
			&m_pMaterialConstantBuffer)))
		{
			OutputErrorLog("定数バッファ生成に失敗しました");
			return false;
		}

		return true;
	}

	void FbxModel::ReleaseIndexBuffer()
	{
		if (m_ppIndexBuffer != NULL)
		{
			for (unsigned int i = 0; i < m_MeshData.size(); i++)
			{
				m_ppIndexBuffer[i]->Release();
			}
			delete[] m_ppIndexBuffer;
			m_ppIndexBuffer = NULL;
		}
	}

	void FbxModel::ReleaseVertexBuffer()
	{
		if (m_ppVertexBuffer != NULL)
		{
			for (unsigned int i = 0; i < m_MeshData.size(); i++)
			{
				delete m_ppVertexData[i];
				m_ppVertexData[i] = NULL;
			}
			delete[] m_ppVertexData;
			m_ppVertexData = NULL;
		}

		if (m_ppVertexBuffer != NULL)
		{
			for (unsigned int i = 0; i < m_MeshData.size(); i++)
			{
				m_ppVertexBuffer[i]->Release();
				delete[] m_ppVertexBuffer;
				m_ppVertexBuffer = NULL;
			}
		}
	}

	void FbxModel::ReleaseSamplerState()
	{
		if (m_pSamplerState != NULL)
		{
			m_pSamplerState->Release();
			m_pSamplerState = NULL;
		}
	}

	void FbxModel::ReleaseConstantBuffer()
	{
		if (m_pMaterialConstantBuffer != NULL)
		{
			m_pMaterialConstantBuffer->Release();
			m_pMaterialConstantBuffer = NULL;
		}
	}

	void FbxModel::WriteConstantBuffer(int _meshNum)
	{
		D3D11_MAPPED_SUBRESOURCE SubResourceData;
		if (SUCCEEDED(m_pGraphicsDevice->GetDeviceContext()->Map(
			m_pMaterialConstantBuffer, 
			0, 
			D3D11_MAP_WRITE_DISCARD, 
			0,
			&SubResourceData)))
		{
			MATERIAL_CONSTANT_BUFFER ConstantBuffer;
			ConstantBuffer.Diffuse = static_cast<D3DXVECTOR4>(m_MeshData[_meshNum].pMaterialData->pMaterial.Diffuse);
			ConstantBuffer.Ambient = static_cast<D3DXVECTOR4>(m_MeshData[_meshNum].pMaterialData->pMaterial.Ambient);
			ConstantBuffer.Specular = static_cast<D3DXVECTOR4>(m_MeshData[_meshNum].pMaterialData->pMaterial.Specular);
			ConstantBuffer.Emissive = static_cast<D3DXVECTOR4>(m_MeshData[_meshNum].pMaterialData->pMaterial.Emissive);

			memcpy_s(
				SubResourceData.pData,
				SubResourceData.RowPitch,
				reinterpret_cast<void*>(&ConstantBuffer),
				sizeof(ConstantBuffer));

			m_pGraphicsDevice->GetDeviceContext()->Unmap(m_pMaterialConstantBuffer, 0);
		}
	}
}
