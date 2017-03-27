/**
 * @file	ShaderManager.h
 * @brief	シェーダーの管理クラス定義
 * @author	morimoto
 */
#ifndef LIB_SHADERMANAGER_H
#define LIB_SHADERMANAGER_H


//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include <D3DX11.h>
#include <D3DX10.h>
#include <vector>

#include "..\GraphicsDevice\GraphicsDevice.h"
#include "..\..\SingletonBase\SingletonBase.h"


namespace Lib
{
	/**
	 * シェーダーの管理クラス
	 */
	class ShaderManager : public SingletonBase<ShaderManager>
	{
	public:
		friend SingletonBase<ShaderManager>;

		/**
		 * 初期化処理
		 * @return 初期化に成功したか
		 */
		bool Initialize(GraphicsDevice* _pGraphicsDevice);

		/**
		 * 終了処理
		 */
		void Finalize();

		/**
		 * 頂点シェーダーを読み込む(バージョンはvs_5_0)
		 * @param[in] _pFilePath 読み込むエフェクトファイルのパス
		 * @param[in] _pFuncName 頂点シェーダーの関数名
		 * @param[out] _pIndex 頂点シェーダーとコンパイルデータの格納先インデックスが入る
		 * @return 成功したらtrue 失敗したらfalse
		 */
		bool LoadVertexShader(LPCTSTR _pFilePath, LPCTSTR _pFuncName, int* _pIndex);

		/**
		 * ピクセルシェーダーを読み込む(バージョンはps_5_0)
		 * @param[in] _pFilePath 読み込むエフェクトファイルのパス
		 * @param[in] _pFuncName ピクセルシェーダーの関数名
		 * @param[out] _pIndex ピクセルシェーダーとコンパイルデータの格納先インデックスが入る
		 * @return 成功したらtrue 失敗したらfalse
		 */
		bool LoadPixelShader(LPCTSTR _pFilePath, LPCTSTR _pFuncName, int* _pIndex);

		/**
		 * 頂点シェーダーとそのコンパイルデータを解放する
		 * @param[in] _index 解放する頂点シェーダーのインデックス
		 */
		void ReleaseVertexShader(int _index);

		/**
		 * 頂点シェーダーとそのコンパイルデータを全て解放する
		 */
		void ReleaseVertexShader();

		/**
		 * ピクセルシェーダーとそのコンパイルデータを解放する
		 * @param[in] _index 解放するピクセルシェーダーのインデックス
		 */
		void ReleasePixelShader(int _index);

		/**
		 * ピクセルシェーダーとそのコンパイルデータを全て解放する
		 */
		void ReleasePixelShader();

		/**
		 * 頂点シェーダーを取得する
		 * @param[in] _index 取得したい頂点シェーダーのインデックス
		 * @return 頂点シェーダー
		 */
		inline ID3D11VertexShader* GetVertexShader(int _index) const
		{
			return m_pVertexShader[_index];
		}

		/**
		 * 頂点シェーダーのコンパイルデータを取得する
		 * @param[in] _index 取得したいコンパイルデータのインデックス
		 * @return 頂点シェーダーのコンパイルデータ
		 */
		inline ID3DBlob* GetCompiledVertexShader(int _index) const
		{
			return m_pCompiledVertexShader[_index];
		}

		/**
		 * ピクセルシェーダーを取得する
		 * @param[in] _index 取得したいピクセルシェーダーのインデックス
		 * @return ピクセルシェーダー
		 */
		inline ID3D11PixelShader* GetPixelShader(int _index) const
		{
			return m_pPixelShader[_index];
		}

		/**
		 * ピクセルシェーダーのコンパイルデータを取得する
		 * @param[in] _index 取得したいコンパイルデータのインデックス
		 * @return ピクセルシェーダーのコンパイルデータ
		 */
		inline ID3DBlob* GetCompiledPixelShader(int _index) const
		{
			return m_pCompiledPixelShader[_index];
		}

		/**
		 * シェーダーのデータを確保しているバッファをクリアする
		 */
		inline void ClearShaderBuffer()
		{
			m_pVertexShader.clear();
			m_pCompiledVertexShader.clear();
			m_pPixelShader.clear();
			m_pCompiledPixelShader.clear();
		}


	private:
		static const int m_InvalidIndex;	//!< インデックスのエラー値.

		/**
		 * コンストラクタ
		 */
		ShaderManager();
	
		/**
		 * デストラクタ
		 */
		~ShaderManager();

		GraphicsDevice*						m_pGraphicsDevice;		//!< グラフィックデバイス.
		std::vector<ID3D11VertexShader*>	m_pVertexShader;		//!< 頂点シェーダーを格納するコンテナ.
		std::vector<ID3D11PixelShader*>		m_pPixelShader;			//!< ピクセルシェーダーを格納するコンテナ.
		std::vector<ID3DBlob*>				m_pCompiledVertexShader;//!< 頂点シェーダのコンパイル情報を格納するコンテナ.
		std::vector<ID3DBlob*>				m_pCompiledPixelShader;	//!< ピクセルシェーダのコンパイル情報を格納するコンテナ.
	};
}



#endif // LIB_SHADERMANAGER_H
