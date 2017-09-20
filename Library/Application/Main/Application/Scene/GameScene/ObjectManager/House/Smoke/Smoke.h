/**
 * @file	Smoke.h
 * @brief	煙クラス定義
 * @author	morimoto
 */
#ifndef SMOKE_H
#define SMOKE_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include <D3DX11.h>
#include <D3DX10.h>
#include <random>

#include "ObjectManagerBase\ObjectBase\ObjectBase.h"
#include "TaskManager\TaskBase\UpdateTask\UpdateTask.h"
#include "TaskManager\TaskBase\DrawTask\DrawTask.h"


class MainCamera;


/**
 * 煙クラス
 */
class Smoke : public Lib::ObjectBase
{
public:
	/**
	 * コンストラクタ
	 * @param[in] _pCamera カメラオブジェクト
	 * @param[in] _pPos 煙の発生座標
	 */
	Smoke(MainCamera* _pCamera, D3DXVECTOR3* _pPos);

	/**
	 * デストラクタ
	 */
	virtual ~Smoke();

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

private:
	enum
	{
		VERTEX_NUM = 4,		//!< 頂点数.
		PARTICLE_NUM = 600	//!< パーティクルの数.
	};

	/**
	 * ポイントスプライト用の頂点構造体
	 */
	struct VERTEX
	{
		D3DXVECTOR3	Pos;	//!< 頂点座標.
		D3DXVECTOR2 UV;		//!< テクスチャ座標.
	};

	/**
	 * インスタンス別データ構造体
	 */
	struct INSTANCE_DATA
	{
		D3DXMATRIX Mat;		//!< 変換行列.
		D3DXCOLOR Color;	//!< カラー値.
	};

	/**
	 * 煙のデータ構造体 
	 */
	struct SMOKE_DATA
	{
		D3DXVECTOR3 Pos;		//!< 煙の座標.
		D3DXVECTOR3	Scale;		//!< 煙のスケーリング値.
		D3DXCOLOR	Color;		//!< 煙のカラー値.
		D3DXVECTOR3 Vec;		//!< 移動速度.
		int			Life;		//!< 消滅するまで寿命.
		bool		IsActive;	//!< アクティブになっているか.
	};

	/**
	 * 煙パーティクルのエミッタ構造体
	 */
	struct EMITTER_DATA
	{
		D3DXVECTOR3 Pos;		//!< エミッタの座標.
		D3DXVECTOR3 Vec;		//!< パーティクルの初速.
		float AngleRange;		//!< 範囲.
	};

	/**
	 * コンピュートシェーダーバッファ
	 */
	struct COMPUTESHADER_BUFFER
	{
		D3DXVECTOR4 Pos;	//!< パーティクルの座標.
		D3DXVECTOR4 Scale;	//!< パーティクルの拡大率.
		D3DXCOLOR Color;	//!< パーティクルのカラー値.
		D3DXVECTOR4 Life;	//!< パーティクルのライフ値.
	};


	static const D3DXVECTOR2 m_DefaultSize;	//!< デフォルトの頂点サイズ.
	static const D3DXVECTOR2 m_LifeRange;	//!< 寿命の範囲.


	//----------------------------------------------------------------------
	// 生成処理
	//----------------------------------------------------------------------

	/**
	 * タスクオブジェクト初期化
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	bool CreateTask();

	/**
	 * 頂点バッファの生成
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	bool CreateVertexBuffer();

	/**
	 * シェーダーの初期化
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	bool CreateShader();

	/**
	 * 頂点入力レイアウトの初期化
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	bool CreateVertexLayout();

	/**
	 * 描画ステートの初期化
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	bool CreateState();

	/**
	 * テクスチャの初期化
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	bool CreateTexture();

	/**
	 * コンピュートシェーダーバッファの初期化
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	bool CreateComputeShaderBuffer();


	//----------------------------------------------------------------------
	// 解放処理
	//----------------------------------------------------------------------

	/**
	 * タスクオブジェクトの解放
	 */
	void ReleaseTask();

	/**
	 * 頂点バッファの解放
	 */
	void ReleaseVertexBuffer();

	/**
	 * シェーダーの解放
	 */
	void ReleaseShader();

	/**
	 * 頂点入力レイアウトの解放
	 */
	void ReleaseVertexLayout();

	/**
	 * ステートの解放
	 */
	void ReleaseState();

	/**
	 * テクスチャの解放
	 */
	void ReleaseTexture();

	/**
	 * コンピュートシェーダバッファの解放 
	 */
	void ReleaseComputeShaderBuffer();


	//----------------------------------------------------------------------
	// その他処理
	//----------------------------------------------------------------------

	/**
	 * インスタンスバッファへの書き込み
	 * @return 成功したらtrue 失敗したらfalse
	 */
	bool WriteInstanceBuffer();



	//--------------------タスクオブジェクト--------------------
	Lib::Draw3DTask*			m_pDrawTask;					//!< 描画タスクオブジェクト.
	Lib::UpdateTask*			m_pUpdateTask;					//!< 更新タスクオブジェクト.


	//--------------------その他オブジェクト--------------------
	MainCamera*					m_pCamera;						//!< カメラオブジェクト.


	//--------------------描画関連--------------------
	int							m_VertexShaderIndex;			//!< 頂点シェーダーインデックス.
	int							m_PixelShaderIndex;				//!< ピクセルシェーダーインデックス.
	int							m_ComputeShaderIndex;			//!< コンピュートシェーダーインデックス.
	ID3D11Buffer*				m_pVertexBuffer;				//!< 頂点バッファ.
	ID3D11Buffer*				m_pInstanceBuffer;				//!< インスタンシングバッファ.
	ID3D11Buffer*				m_pComputeShaderBuffer;			//!< コンピュートシェーダーで使用するバッファ.
	ID3D11UnorderedAccessView*	m_pComputeShaderBufferAccess;	//!< コンピュートシェーダーバッファのアクセスビュー.
	ID3D11InputLayout*			m_pVertexLayout;				//!< 頂点入力レイアウト.
	ID3D11DepthStencilState*	m_pDepthStencilState;			//!< 深度ステンシルステート.
	ID3D11BlendState*			m_pBlendState;					//!< ブレンドステート.
	VERTEX						m_pVertexData[VERTEX_NUM];		//!< 頂点データ.
	INSTANCE_DATA				m_pInstanceData[PARTICLE_NUM];	//!< インスタンスデータ.

	int							m_SmokeTextureIndex;			//!< 煙のテクスチャインデックス.
	int							m_SkyCLUTIndex;					//!< ライトのカラールックアップテーブル.

	bool						m_IsActive;						//!< このオブジェクトの活動状態.
	bool						m_IsComputeShader;				//!< コンピュートシェーダを使用するか.
	EMITTER_DATA				m_EmitterData;					//!< エミッタデータ構造体.
	SMOKE_DATA					m_SmokeData[PARTICLE_NUM];		//!< 煙のデータ配列.
	COMPUTESHADER_BUFFER		m_ComputeData[PARTICLE_NUM];	//!< コンピュートシェーダで使用するデータ.
	std::random_device			m_RandDevice;					//!< 乱数生成デバイス.
	std::mt19937				m_MersenneTwister;				//!< 乱数生成オブジェクト.

};



#endif // !SMOKE_H
