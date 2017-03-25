#ifndef MAINLIGHT_H
#define MAINLIGHT_H

#include "DirectX11\Light\Light.h"
#include "ObjectBase\ObjectBase.h"
#include "TaskManager\TaskBase\DrawTask\DrawTask.h"
#include "TaskManager\TaskBase\UpdateTask\UpdateTask.h"
#include "DirectX11\Vertex2D\Vertex2D.h"


class MainLight : public Lib::ObjectBase
{
public:
	/**
	 * コンストラクタ
	 */
	MainLight();

	/**
	 * デストラクタ
	 */
	virtual ~MainLight();

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
	static const D3DXVECTOR3 m_DefaultLightPos;
	static const D3DXVECTOR3 m_DefaultLightDirPos;
	static const float m_NearPoint;
	static const float m_FarPoint;
	static const float m_ViewAngle;
	static const float m_ClearColor[4];

	struct LIGHT_CONSTANT_BUFFER
	{
		D3DXVECTOR4 LightPos;
		D3DXVECTOR4 LightDir;
		D3DXMATRIX	LightView;
		D3DXMATRIX	LightProj;
	};

	/**
	 * 定数バッファ初期化
	 */
	bool CreateConstantBuffer();

	/**
	 * 深度テクスチャ初期化
	 */
	bool CreateDepthTexture();

	/**
	 * 定数バッファの解放
	 */
	void ReleaseConstantBuffer();

	/**
	 * 深度テクスチャ解放
	 */
	void ReleaseDepthTexture();

	/**
	 * 定数バッファへの書き込み
	 */
	void WriteConstantBuffer();


	Lib::DrawTask*				m_pDrawTask;
	Lib::UpdateTask*			m_pUpdateTask;
	Lib::Light*					m_pLight;
	ID3D11Buffer*				m_pConstantBuffer;
	D3DXVECTOR3					m_LightDir;
	D3DXMATRIX					m_LightView;
	D3DXMATRIX					m_LightProj;
	ID3D11Texture2D*			m_pDepthTexture;
	ID3D11RenderTargetView*		m_pRenderTarget;
	ID3D11ShaderResourceView*	m_pShaderResourceView;
	ID3D11Texture2D*			m_pDepthStencilTexture;
	ID3D11DepthStencilView*		m_pDepthStencilView;

	Lib::Vertex2D*				m_pVertex2D;

};


#endif // MAINLIGHT_H
