/**
 * @file   Object3DBase.h
 * @brief  Object3DBaseクラスの定義
 * @author morimoto
 */
#ifndef OBJECT3DBASE_H
#define OBJECT3DBASE_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include <D3DX11.h>
#include <D3DX10.h>

#include "ObjectManagerBase\ObjectBase\ObjectBase.h"
#include "TaskManager\TaskBase\DrawTask\DrawTask.h"
#include "TaskManager\TaskBase\UpdateTask\UpdateTask.h"
#include "Main\Application\Scene\GameScene\Task\DepthDrawTask\DepthDrawTask.h"
#include "Main\Application\Scene\GameScene\Task\MapDrawTask\MapDrawTask.h"
#include "Main\Application\Scene\GameScene\Task\CubeMapDrawTask\CubeMapDrawTask.h"


/**
 * 3Dオブジェクトの基底クラス
 */
class Object3DBase : public Lib::ObjectBase
{
public:
	/**
	 * コンストラクタ
	 */
	Object3DBase();

	/**
	 * デストラクタ
	 */
	virtual ~Object3DBase();

	/**
	 * 初期化処理
	 * @return 初期化に成功したか
	*/
	virtual bool Initialize();

	/**
	 * 終了処理
	 */
	virtual void Finalize();

	/**
	 * オブジェクトの更新
	 */
	virtual void Update();

	/**
	 * オブジェクトの描画
	 */
	virtual void Draw();

	/**
	 * Z値をテクスチャに描画
	 */
	virtual void DepthDraw();

	/**
	 * マップへの描画
	 */
	virtual void MapDraw();

	/**
	 * キューブマップへの描画
	 */
	virtual void CubeMapDraw();

protected:
	/**
	 * 定数バッファ
	 */
	struct CONSTANT_BUFFER
	{
		D3DXMATRIX World;
	};

	/**
	 * シェーダーの準備
	 */
	void ShaderSetup();

	/**
	 * テクスチャの準備
	 */
	void TextureSetup();

	/**
	 * 入力レイアウトの準備
	 */
	void VertexLayoutSetup();

	/**
	 * 深度ステンシルステートの準備
	 */
	void DepthStencilStateSetup();

	/**
	 * 定数バッファの準備
	 */
	void ConstantBufferSetup();

	/**
	 * シェーダーの生成
	 * @return 成功したらtrue失敗したらfalse
	 */
	bool CreateShader();

	/**
	 * テクスチャの生成
	 * @return 成功したらtrue失敗したらfalse
	 */
	bool CreateTexture();

	/**
	 * 頂点入力レイアウトの生成
	 * @return 成功したらtrue失敗したらfalse
	 */
	bool CreateVertexLayout();

	/**
	 * 深度ステンシルステートの生成
	 * @return 成功したらtrue失敗したらfalse
	 */
	bool CreateDepthStencilState();

	/**
	 * 定数バッファの生成
	 * @return 成功したらtrue失敗したらfalse
	 */
	bool CreateConstantBuffer();

	/**
	 * 定数バッファへの書き込み
	 * @return 成功したらtrue失敗したらfalse
	 */
	bool WriteConstantBuffer();

	/**
	 * シェーダーの解放
	 */
	void ReleaseShader();

	/**
	 * テクスチャの解放
	 */
	void ReleaseTexture();

	/**
	 * 頂点入力レイアウトの解放
	 */
	void ReleaseVertexLayout();

	/**
	 * 深度ステンシルステートの解放
	 */
	void ReleaseDepthStencilState();

	/**
	 * 定数バッファの解放
	 */
	void ReleaseConstantBuffer();



	Lib::DrawTask*				m_pDrawTask;		//!< 描画タスクオブジェクト
	Lib::UpdateTask*			m_pUpdateTask;		//!< 更新タスクオブジェクト
	DepthDrawTask*				m_pDepthDrawTask;	//!< 深度バッファ描画タスクオブジェクト
	MapDrawTask*				m_pMapDrawTask;		//!< マップ描画タスクオブジェクト
	CubeMapDrawTask*			m_pCubeMapDrawTask;	//!< キューブマップ描画タスク

	int							m_VertexShaderIndex;	//!< 頂点シェーダーインデックス
	int							m_PixelShaderIndex;		//!< ピクセルシェーダーインデックス
	int							m_SkyCLUTIndex;			//!< 空の色テーブルテクスチャインデックス
	ID3D11InputLayout*			m_pVertexLayout;		//!< 頂点入力レイアウト
	ID3D11DepthStencilState*	m_pDepthStencilState;	//!< 深度ステンシルステート
	ID3D11Buffer*				m_pConstantBuffer;		//!< 定数バッファ

	D3DXVECTOR3					m_Pos;		//!< 座標
	D3DXVECTOR3					m_Scale;	//!< スケール
	D3DXVECTOR3					m_Rotate;	//!< 回転

};


#endif // OBJECT3DBASE_H
