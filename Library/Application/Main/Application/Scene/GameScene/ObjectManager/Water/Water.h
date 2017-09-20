/**
 * @file	Water.h
 * @brief	水の管理クラス定義
 * @author	morimoto
 */
#ifndef WATER_H
#define WATER_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include <D3DX11.h>
#include <D3DX10.h>
#include <random>

#include "ObjectManagerBase\ObjectBase\ObjectBase.h"
#include "TaskManager\TaskBase\UpdateTask\UpdateTask.h"
#include "TaskManager\TaskBase\DrawTask\DrawTask.h"
#include "InputDeviceManager\InputDeviceManager.h"
#include "WaterDebugFont\WaterDebugFont.h"


namespace Lib
{
	namespace Dx11
	{
		class Camera;
		class Texture;
		class Vertex2D;
	}
}


/**
 * 水の管理クラス
 */
class Water : public Lib::ObjectBase
{
public:
	/**
	 * コンストラクタ
	 */
	Water();

	/**
	 * デストラクタ
	 */
	virtual ~Water();

	/**
	 * 初期化処理
	 * @return 初期化に成功したらtrue 失敗したらfalse
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
	/**
	 * キューブマップ描画前処理のタスク
	 */
	class CubeDrawStartUp : public Lib::TaskBase<>
	{
	public:
		/**
		 * コンストラクタ
		 * @param[in] _pWater 水オブジェクト
		 */
		CubeDrawStartUp(Water* _pWater);

		/**
		 * デストラクタ
		 */
		virtual ~CubeDrawStartUp();

		/**
		 * タスクの実行
		 */
		virtual void Run();


	private:
		Water* m_pWater;	//!< 描画前処理タスク.

	};

	/**
	 * 反射マップ描画前処理のタスク
	 */
	class ReflectDrawStartUp : public Lib::TaskBase<>
	{
	public:
		/**
		 * コンストラクタ
		 * @param[in] _pWater 水オブジェクト
		 */
		ReflectDrawStartUp(Water* _pWater);

		/**
		 * デストラクタ
		 */
		virtual ~ReflectDrawStartUp();

		/**
		 * タスクの実行
		 */
		virtual void Run();


	private:
		Water* m_pWater;	//!< 描画前処理タスク.

	};

	enum
	{
		VERTEX_NUM = 4,			//!< 頂点数.
		WAVE_TEXTURE_NUM = 2	//!< 波テクスチャの数.
	};


	/**
	 * 水オブジェクト描画の定数バッファ
	 */
	struct CONSTANT_BUFFER
	{
		D3DXMATRIX  World;			//!< ワールド変換行列.
		D3DXVECTOR4 TexelOffset;	//!< 1ピクセルのオフセット値.
		D3DXVECTOR4 AddWavePos;		//!< 追加する波の座標.
		D3DXVECTOR4 AddWaveHeight;	//!< 追加する波の高さ.
		D3DXMATRIX	MapWorld;		//!< 波マップ 法線マップ描画行列.
	};

	/**
	 * 水のキューブマップ作成に関する定数バッファ
	 */
	struct CUBE_CONSTANT_BUFFER
	{
		D3DXMATRIX View[6];		//!< ビュー変換行列.
		D3DXMATRIX Proj;		//!< プロジェクション変換行列.
	};

	/**
	 * 水の反射マップ作成に関する定数バッファ
	 */
	struct REFLECT_CONSTANT_BUFFER
	{
		D3DXMATRIX	ScalingMat;		//!< スケーリング行列.
		D3DXMATRIX	TranslateMat;	//!< 平行移動行列.
	};

	/**
	 * 水描画用の頂点構造体
	 */
	struct VERTEX
	{
		D3DXVECTOR3	Pos;		//!< 頂点座標.
		D3DXVECTOR2 UV;			//!< テクスチャ座標.
		D3DXVECTOR3 Normal;		//!< 法線.
		D3DXVECTOR3 Tangent;	//!< 接線.
		D3DXVECTOR3 Binormal;	//!< 従法線.
	};

	/**
	 * 波マップ 法線マップ描画用の頂点構造体
	 */
	struct MAP_VERTEX
	{
		D3DXVECTOR3	Pos;		//!< 頂点座標.
		D3DXVECTOR2 UV;			//!< テクスチャ座標.
	};


	static const D3DXVECTOR3 m_DefaultPos;		//!< 描画する波の位置.
	static const D3DXVECTOR2 m_DefaultSize;		//!< 描画する波のサイズ.
	static const float m_ClearColor[4];			//!< 初期化色.
	static const float m_WaterClearColor[4];	//!< 波マップ初期化色.
	static const float m_CubeTextureWidth;		//!< キューブマップテクスチャの幅.
	static const float m_CubeTextureHeight;		//!< キューブマップテクスチャの高さ.
	static const float m_WaveTextureWidth;		//!< 波マップテクスチャの幅.
	static const float m_WaveTextureHeight;		//!< 波マップテクスチャの高さ.
	static const float m_ReflectTextureWidth;	//!< 反射マップテクスチャの幅.
	static const float m_ReflectTextureHeight;	//!< 反射マップテクスチャの高さ.
	static const int m_CubeRenderTargetStage;	//!< キューブマップレンダーターゲットステージ.
	static const int m_WaveRenderTargetStage;	//!< 波マップレンダーターゲットステージ.
	static const int m_BumpRenderTargetStage;	//!< 法線マップレンダーターゲットステージ.
	static const int m_ReflectRenderTargetStage;//!< 反射マップレンダーターゲットステージ.


	//----------------------------------------------------------------------
	// 生成処理
	//----------------------------------------------------------------------

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
	 * 定数バッファ初期化
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	bool CreateConstantBuffer();
	
	/**
	 * テクスチャの生成
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	bool CreateTexture();

	/** 
	 * キューブマップテクスチャの生成
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	bool CreateCubeMapTexture();

	/**
	 * 波マップテクスチャの生成
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	bool CreateWaveMapTexture();

	/**
	 * 反射マップテクスチャの生成
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	bool CreateReflectMapTexture();


	//----------------------------------------------------------------------
	// 解放処理
	//----------------------------------------------------------------------

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
	 * 定数バッファの解放
	 */
	void ReleaseConstantBuffer();

	/**
	 * テクスチャの解放
	 */
	void ReleaseTexture();

	/**
	 * キューブマップテクスチャの解放
	 */
	void ReleaseCubeMapTexture();

	/**
	 * 波マップテクスチャの解放
	 */
	void ReleaseWaveMapTexture();

	/**
	 * 反射マップテクスチャの解放
	 */
	void ReleaseReflectMapTexture();


	//----------------------------------------------------------------------
	// その他処理
	//----------------------------------------------------------------------

	/**
	 * 定数バッファへの書き込み
	 * @return 成功したらtrue 失敗したらfalse
	 */
	bool WriteConstantBuffer();

	/**
	 * キューブマップ作成に関する定数バッファへの書き込み
	 * @return 成功したらtrue 失敗したらfalse
	 */
	bool WriteCubeMapConstantBuffer();

	/**
	 * 反射マップ作成に関する定数バッファへの書き込み
	 * @return 成功したらtrue 失敗したらfalse
	 */
	bool WriteReflectMapConstantBuffer();

	/**
	 * キューブマップ描画前処理
	 */
	void CubeMapBeginScene();

	/**
	 * 反射マップ描画前処理
	 */
	void ReflectMapBeginScene();

	/**
	 * 波マップの描画
	 */
	void WaveDraw();

	/**
	 * 法線マップの描画
	 */
	void BumpDraw();


	//--------------------タスクオブジェクト--------------------
	Lib::Draw3DTask*			m_pDraw3DTask;				//!< 3D描画タスクオブジェクト.
	Lib::UpdateTask*			m_pUpdateTask;				//!< 更新タスクオブジェクト.
	CubeDrawStartUp*			m_pCubeDrawStartUp;			//!< キューブマップ描画タスクオブジェクト.
	ReflectDrawStartUp*			m_pReflectDrawStartUp;		//!< 反射マップ描画タスクオブジェクト.


	//--------------------その他オブジェクト--------------------
	Lib::Dx11::Camera*			m_pCamera;					//!< カメラオブジェクト.
	WaterDebugFont*				m_pDebugFont;				//!< 水デバッグフォントクラス.	


	//--------------------描画関連--------------------
	int							m_CubeVertexShaderIndex;		//!< キューブマップ頂点シェーダーインデックス.
	int							m_CubePixelShaderIndex;			//!< キューブマップピクセルシェーダーインデックス.
	int							m_ReflectVertexShaderIndex;		//!< 反射頂点シェーダーインデックス.
	int							m_ReflectPixelShaderIndex;		//!< 反射ピクセルシェーダーインデックス.
	ID3D11Buffer*				m_pVertexBuffer;				//!< 頂点バッファ.
	ID3D11InputLayout*			m_pVertexLayout;				//!< 頂点入力レイアウト.
	ID3D11DepthStencilState*	m_pDepthStencilState;			//!< 深度ステンシルステート.
	ID3D11BlendState*			m_pBlendState;					//!< ブレンドステート.
	VERTEX						m_pVertexData[VERTEX_NUM];		//!< 頂点データ.

	ID3D11Buffer*				m_pConstantBuffer;				//!< 水描画の定数バッファ.
	ID3D11Texture2D*			m_pCubeTexture;					//!< 波情報が入ったテクスチャ.
	ID3D11ShaderResourceView*	m_pCubeTextureResource;			//!< 波テクスチャのリソースビュー.
	ID3D11RenderTargetView*		m_pCubeTextureRenderTarget;		//!< 波テクスチャレンダーターゲットビュー.
	ID3D11Buffer*				m_pCubeMapConstantBuffer;		//!< キューブマップ作成の定数バッファ.
	int							m_PuddleTextureIndex;			//!< 水たまりテクスチャインデックス.
	int							m_SkyCLUTIndex;					//!< 空のカラーテーブルテクスチャインデックス.
	int							m_WaterColorIndex;				//!< 水の色テクスチャインデックス.

	ID3D11Texture2D*			m_pDepthStencilTexture;			//!< 深度ステンシルテクスチャ.
	ID3D11DepthStencilView*		m_pDepthStencilView;			//!< 深度ステンシルビュー.
	D3D11_VIEWPORT				m_ViewPort;						//!< ビューポート.

	D3DXMATRIX					m_ViewMat[6];					//!< ビュー変換行列.
	D3DXMATRIX					m_ProjMat;						//!< プロジェクション変換行列.
	CUBE_CONSTANT_BUFFER		m_CubeConstantBuffer;			//!< キューブマップ描画定数バッファデータ.
	
	ID3D11Buffer*				m_pReflectMapConstantBuffer;	//!< 反射マップ作成の定数バッファ.
	REFLECT_CONSTANT_BUFFER		m_ReflectConstantBuffer;		//!< 反射マップ描画定数バッファデータ.

	ID3D11Texture2D*			m_pWaveTexture[2];				//!< 波テクスチャ.
	ID3D11Texture2D*			m_pWaveDepthStencilTexture[2];	//!< 波の深度ステンシルテクスチャ.
	ID3D11RenderTargetView*		m_pWaveRenderTarget[2];			//!< 波のレンダーターゲットビュー.
	ID3D11DepthStencilView*		m_pWaveDepthStencilView[2];		//!< 波の震度ステンシルビュー.
	ID3D11ShaderResourceView*	m_pWaveShaderResourceView[2];	//!< 波のシェーダーリソースビュー.

	ID3D11Texture2D*			m_pBumpTexture;					//!< 法線テクスチャ.
	ID3D11RenderTargetView*		m_pBumpRenderTarget;			//!< 法線テクスチャのレンダーターゲットビュー.
	ID3D11ShaderResourceView*	m_pBumpShaderResourceView;		//!< 法線テクスチャのシェーダーリソースビュー.

	int							m_WaveVertexShaderIndex;		//!< 頂点シェーダーインデックス.
	int							m_WavePixelShaderIndex;			//!< ピクセルシェーダーインデックス.
	int							m_BumpPixelShaderIndex;			//!< ピクセルシェーダーインデックス.
	int							m_WaveRenderIndex;				//!< 描画する波テクスチャのインデックス.

	ID3D11Buffer*				m_pWaveVertexBuffer;			//!< 頂点バッファ.
	ID3D11InputLayout*			m_pWaveVertexLayout;			//!< 頂点入力レイアウト.
	MAP_VERTEX					m_pWaveVertexData[VERTEX_NUM];	//!< 頂点データ.

	D3DXVECTOR2					m_AddWavePos;
	float						m_AddWaveHeight;

	std::random_device			m_RandDevice;				//!< 乱数生成デバイス.
	std::mt19937				m_MersenneTwister;			//!< 乱数生成オブジェクト.
	int							m_WaveAddTime;
	int							m_WaveAddCount;
	bool						m_IsCubeMapDraw;

	ID3D11Texture2D*			m_pReflectTexture;
	ID3D11Texture2D*			m_pReflectDepthStencilTexture;
	ID3D11RenderTargetView*		m_pReflectRenderTarget;
	ID3D11DepthStencilView*		m_pReflectDepthStencilView;
	ID3D11ShaderResourceView*	m_pReflectShaderResourceView;
	D3D11_VIEWPORT				m_ReflectViewPort;


	//--------------------入力デバイスの状態--------------------
	const Lib::KeyDevice::KEYSTATE* m_pKeyState;			//!< キーの状態.

};



#endif // !WATER_H
