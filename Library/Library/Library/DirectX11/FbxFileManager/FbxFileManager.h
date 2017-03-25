﻿/**
 * @file   FbxFileManager.h
 * @brief  FbxFileManagerクラスの定義
 * @author morimoto
 */
#ifndef LIB_FBXFILEMANAGER_H
#define LIB_FBXFILEMANAGER_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include <D3D11.h>
#include <vector>

#include "DirectX11\GraphicsDevice\GraphicsDevice.h"
#include "FbxModel\FbxModel.h"
#include "SingletonBase\SingletonBase.h"


namespace Lib
{
	class FbxLoader;

	/**
	 *  Fbxファイルを管理するクラス
	 */
	class FbxFileManager : public SingletonBase<FbxFileManager>
	{
	public:
		friend SingletonBase<FbxFileManager>;


		/**
		 * 初期化処理
		 * @param[in] _pGraphicsDevice グラフィックデバイス
		 * @return 初期化に成功したらtrue
		 */
		bool Initialize(GraphicsDevice* _pGraphicsDevice);

		/**
		 * 終了処理
		 */
		void Finalize();

		/**
		 * Fbxモデルを読み込む
		 * @param[in] _pFileName 読み込むモデルのファイルパス
		 * @param[out] _pIndex 読み込んだモデルの格納先インデックス
		 * @return 読み込みに成功したらtrue
		 */
		bool LoadFbxModel(LPCTSTR _pFileName, int* _pIndex);

		/**
		 * Fbxモデルを取得する
		 * @param[in] _index 取得するモデルのインデックス
		 * @return Fbxのデータを格納したFbxModelクラス
		 */
		inline FbxModel* GetFbxModel(int _index) const
		{
			return m_pFbxModel[_index];
		}

		/**
		 * Fbxモデルを解放する
		 * @param[in] _index 解放するモデルのインデックス
		 */
		void ReleaseFbxModel(int _index);

		/**
		 * モデルを確保しているバッファをクリアする
		 *
		 * vectorを使用しているのでバッファ領域は解放されない。\n
		 * バッファ領域はFbxFileManagerが破棄されたときに解放される。
		 */
		inline void ClearBuffer()
		{
			m_pFbxModel.clear();
		}

	private:
		static const int m_InvalidIndex; //!< FbxFileManagerクラスがとるインデックスのエラー値

		/**
		 * FbxFileManagerクラスのコンストラクタ
		 */
		FbxFileManager();

		/**
		 * FbxFileManagerクラスのデストラクタ
		 */
		~FbxFileManager();


		GraphicsDevice*				m_pGraphicsDevice;
		FbxLoader*					m_pFbxLoader;
		std::vector<FbxModel*>		m_pFbxModel;

	};
}


#endif // LIB_FBXFILEMANAGER_H
