/**
 * @file	VertexManager.h
 * @brief	頂点管理クラス定義
 * @author	morimoto
 */
#ifndef LIB_VERTEXMANAGER_H
#define LIB_VERTEXMANAGER_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include <vector>

#include "..\..\SingletonBase\SingletonBase.h"


namespace Lib
{
	class GraphicsDevice;
	interface IVertex;

	/**
	 * 頂点管理クラス
	 */
	class VertexManager
	{
	public:
		/**
		 * コンストラクタ
		 */
		VertexManager();

		/**
		 * デストラクタ
		 */
		~VertexManager();

		/**
		 * 初期化処理
		 * @param[in] _pGraphicsDevice グラフィックデバイス
		 * @return 初期化に成功したらtrue 失敗したらfalse
		 */
		bool Initialize(GraphicsDevice* _pGraphicsDevice);

		/**
		 * 終了処理
		 */
		void Finalize();

	private:
		std::vector<IVertex*>	m_pVertexs;
		GraphicsDevice*			m_pGraphicDevice;

	};
}


#endif // LIB_VERTEXMANAGER_H
