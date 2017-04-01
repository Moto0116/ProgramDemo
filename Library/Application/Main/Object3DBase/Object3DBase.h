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
#include "Main\Application\Scene\GameScene\DepthDrawTask\DepthDrawTask.h"
#include "Main\Application\Scene\GameScene\MapDrawTask\MapDrawTask.h"


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
	 */
	bool CreateShader();

	/**
	 * 頂点入力レイアウトの生成
	 */
	bool CreateVertexLayout();

	/**
	 * 深度ステンシルステートの生成
	 */
	bool CreateDepthStencilState();

	/**
	 * 定数バッファの生成
	 */
	bool CreateConstantBuffer();

	/**
	 * 定数バッファへの書き込み
	 */
	bool WriteConstantBuffer();

	/**
	 * シェーダーの解放
	 */
	void ReleaseShader();

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

	Lib::DrawTask*				m_pDrawTask;
	Lib::UpdateTask*			m_pUpdateTask;
	DepthDrawTask*				m_pDepthDrawTask;
	MapDrawTask*				m_pMapDrawTask;

	int							m_VertexShaderIndex;
	int							m_PixelShaderIndex;
	ID3D11InputLayout*			m_pVertexLayout;
	ID3D11DepthStencilState*	m_pDepthStencilState;
	ID3D11Buffer*				m_pConstantBuffer;

	D3DXVECTOR3					m_Pos;
	D3DXVECTOR3					m_Scale;
	D3DXVECTOR3					m_Rotate;

};


#endif // OBJECT3DBASE_H
