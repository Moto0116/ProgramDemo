#ifndef MINIMAP_H
#define MINIMAP_H

#include "Main\Object2DBase\Object2DBase.h"
#include "TaskManager\TaskBase\TaskBase.h"
#include "DirectX11\Camera\Camera.h"


class MiniMap : public Object2DBase
{
public:
	/**
	 * コンストラクタ
	 */
	MiniMap();

	/**
	 * デストラクタ
	 */
	virtual ~MiniMap();

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
	/**
	 * ミニマップ描画前処理のタスク
	 */
	class BeginTask : public Lib::TaskBase<>
	{
	public:
		/**
		 * コンストラクタ
		 * @param[in] _pMiniMap ミニマップオブジェクト
		 */
		BeginTask(MiniMap* _pMiniMap);

		/**
		 * デストラクタ
		 */
		virtual ~BeginTask();

		/**
		 * タスクの実行
		 */
		virtual void Run();

	private:
		MiniMap* m_pMiniMap;

	};



	static const float	m_NearPoint;	//!< 最近点
	static const float	m_FarPoint;		//!< 最遠点
	static const float	m_ViewAngle;	//!< 視野角
	static const float m_ClearColor[4];
	static const float m_TextureWidth;
	static const float m_TextureHeight;
	static const int m_RenderTargetStage;
	static const D3DXVECTOR3 m_DefaultPos;


	struct MINIMAP_CONSTANT_BUFFER
	{
		D3DXMATRIX View;
		D3DXMATRIX Proj;
	};

	/**
	 * 定数バッファ初期化
	 */
	bool CreateConstantBuffer();

	/**
	 * テクスチャの生成
	 */
	bool CreateTexture();

	/**
	 * 定数バッファの解放
	 */
	void ReleaseConstantBuffer();

	/**
	 * テクスチャの解放
	 */
	void ReleaseTexture();

	/**
	 * 定数バッファへの書き込み
	 */
	void WriteConstantBuffer();

	/**
	 * ミニマップ描画前処理
	 */
	void MiniMapBeginScene();


	ID3D11Texture2D*			m_pMapTexture;
	ID3D11RenderTargetView*		m_pRenderTarget;
	ID3D11ShaderResourceView*	m_pShaderResourceView;
	ID3D11Texture2D*			m_pDepthStencilTexture;
	ID3D11DepthStencilView*		m_pDepthStencilView;
	D3D11_VIEWPORT				m_ViewPort;
	BeginTask*					m_pBeginTask;
	ID3D11Buffer*				m_pConstantBuffer;
	D3DXMATRIX					m_CameraView;
	D3DXMATRIX					m_CameraProj;
	Lib::Camera*				m_pCamera;

};


#endif // MINIMAP_H
