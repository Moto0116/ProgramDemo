/**
 * @file	Rain.h
 * @brief	雨の管理クラス定義
 * @author	morimoto
 */
#ifndef RAIN_H
#define RAIN_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include <D3DX11.h>
#include <D3DX10.h>
#include <random>

#include "ObjectManagerBase\ObjectBase\ObjectBase.h"
#include "InputDeviceManager\InputDeviceManager.h"
#include "..\MainCamera\MainCamera.h"


namespace Lib
{
	class DrawTask;
	class UpdateTask;
	class Font;
}


/**
 * 雨の管理クラス
 */
class Rain : public Lib::ObjectBase
{
public:
	/**
	 * コンストラクタ
	 * @param[in] _pCamera カメラオブジェクト
	 */
	Rain(MainCamera* _pCamera);

	/**
	 * デストラクタ
	 */
	virtual ~Rain();

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
		VERTEX_NUM = 4,	//!< 頂点数.
		RAIN_NUM = 2500	//!< 雨の数.
	};


	/**
	 * ポイントスプライト用の頂点構造体
	 */
	struct VERTEX
	{
		D3DXVECTOR3	Pos;	//!< 頂点座標.
		D3DXVECTOR2 UV;		//!< テクスチャ座標.
		D3DXCOLOR	Color;	//!< 頂点カラー値.
	};

	/**
	 * インスタンス別データ構造体
	 */
	struct INSTANCE_DATA
	{
		D3DXMATRIX Mat;		//!< 変換行列.
		D3DXVECTOR3 Pos;	//!< 位置座標.
	};

	/**
	 * 雨のデータ構造体
	 */
	struct RAIN_DATA
	{
		D3DXVECTOR3 Pos;	//!< 雨の座標.
		D3DXVECTOR3	Scale;	//!< 雨のスケーリング値.
		bool IsFall;		//!< 雨が落下中か.
		int Time;			//!< 落下してからの時間.
	};


	static const D3DXVECTOR2	m_DefaultSize;		//!< デフォルトの頂点サイズ.
	static const D3DXVECTOR2	m_DefaultFontPos;	//!< フォントの座標.
	static const D3DXVECTOR2	m_DefaultFontSize;	//!< フォントのサイズ.
	static const D3DXCOLOR		m_DefaultFontColor;	//!< フォントのカラー値.
	static const D3DXVECTOR2	m_XRange;			//!< xの範囲.
	static const D3DXVECTOR2	m_YRange;			//!< yの範囲.
	static const D3DXVECTOR2	m_ZRange;			//!< zの範囲.


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
	 * サウンドの初期化
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	bool CreateSound();

	/**
	 * フォントオブジェクトの初期化
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	bool CreateFontObject();


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
	 * サウンドの解放 
	 */
	void ReleaseSound();

	/**
	 * フォントオブジェクトの解放
	 */
	void ReleaseFontObject();


	//----------------------------------------------------------------------
	// その他処理
	//----------------------------------------------------------------------

	/**
	 * インスタンスバッファへの書き込み
	 * @return 成功したらtrue 失敗したらfalse
	 */
	bool WriteInstanceBuffer();


	Lib::DrawTask*				m_pDrawTask;				//!< 描画タスクオブジェクト.
	Lib::UpdateTask*			m_pUpdateTask;				//!< 更新タスクオブジェクト.
	int							m_VertexShaderIndex;		//!< 頂点シェーダーインデックス.
	int							m_PixelShaderIndex;			//!< ピクセルシェーダーインデックス.
	ID3D11Buffer*				m_pVertexBuffer;			//!< 頂点バッファ.
	ID3D11Buffer*				m_pInstanceBuffer;			//!< インスタンシングバッファ.
	ID3D11InputLayout*			m_pVertexLayout;			//!< 頂点入力レイアウト.
	ID3D11DepthStencilState*	m_pDepthStencilState;		//!< 深度ステンシルステート.
	ID3D11BlendState*			m_pBlendState;				//!< ブレンドステート.
	VERTEX						m_pVertexData[VERTEX_NUM];	//!< 頂点データ.
	INSTANCE_DATA				m_pInstanceData[RAIN_NUM];	//!< インスタンスデータ.

	RAIN_DATA					m_RainData[RAIN_NUM];		//!< 雨粒のデータ配列.
	MainCamera*					m_pCamera;					//!< カメラオブジェクト.
	std::random_device			m_RandDevice;				//!< 乱数生成デバイス.
	std::mt19937				m_MersenneTwister;			//!< 乱数生成オブジェクト.

	int							m_TextureIndex;				//!< テクスチャインデックス.
	int							m_SoundIndex;				//!< サウンドインデックス.

	const Lib::KeyDevice::KEYSTATE* m_pKeyState;			//!< キーの状態.
	bool							m_IsActive;				//!< このオブジェクトの活動状態.

	Lib::Font*					m_pFont;

};



#endif // RAIN_H
