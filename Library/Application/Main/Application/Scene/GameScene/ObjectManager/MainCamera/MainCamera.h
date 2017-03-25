/**
 * @file   MainCamera.h
 * @brief  MainCameraクラスの定義
 * @author morimoto
 */
#ifndef MAINCAMERA_H
#define MAINCAMERA_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include <D3DX11.h>
#include <D3DX10.h>

#include "DirectX11\Camera\Camera.h"
#include "InputDeviceManager\InputDeviceManager.h"
#include "ObjectBase\ObjectBase.h"
#include "TaskManager\TaskBase\UpdateTask\UpdateTask.h"


/**
 * カメラを操作するクラス
 */
class MainCamera : public Lib::ObjectBase
{
public:
	/**
	 * MainCameraクラスのコンストラクタ
	 */
	MainCamera();

	/**
	 * MainCameraクラスのデストラクタ
	 */
	virtual ~MainCamera();

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
	 * ビュー行列の取得
	 * @return ビュー行列
	 */
	inline D3DXMATRIX GetViewMatrix()
	{
		return m_pCamera->GetViewMatrix();
	}

	/**
	 * 射影行列を取得
	 * @return 射影行列
	 */
	inline D3DXMATRIX GetProjectionMatrix()
	{
		return m_pCamera->GetProjectionMatrix();
	}

private:
	static const float	m_NearPoint;   //!< 最近点
	static const float	m_FarPoint;    //!< 最遠点
	static const float	m_ViewAngle;   //!< 視野角

	/**
	 * カメラの定数バッファ
	 */
	struct CAMERA_CONSTANT_BUFFER
	{
		D3DXMATRIX View;
		D3DXMATRIX Proj;
	};

	/**
	 * 移動処理
	 */
	void Move();

	/**
	 * 回転処理
	 */
	void Rotate();
	
	/**
	 * 回転の計算
	 */
	void RotateCalculate();

	/**
	 * ズームイン・アウト処理
	 */
	void Zoom();

	/**
	 * カメラを前に移動させる関数
	 */
	void MoveFront();
	
	/**
	 * カメラを左に移動させる関数
	 */
	void MoveLeft();
	
	/**
	 * カメラを後ろ移動させる関数
	 */
	void MoveBack();
	
	/**
	 * カメラを右に移動させる関数
	 */
	void MoveRight();

	/**
	 * カメラの動きを反映させる
	 */
	void Transform();

	/**
	 * 定数バッファ初期化関数
	 */
	bool CreateConstantBuffer();

	/**
	 * 定数バッファの開放関数
	 */
	void ReleaseConstantBuffer();

	/**
	 * 定数バッファへの書き込み関数
	 */
	void WriteConstantBuffer();

	static const float		   m_MaxAngle;	  //!< カメラの最大傾き
	static const float		   m_MinAngle;	  //!< カメラの最小傾き
	static const float		   m_MaxLength;	  //!< カメラと注視点の最大距離
	static const float		   m_MinLength;	  //!< カメラと注視点の最小距離
	static const float		   m_MoveSpeedWeight; //!< 移動速度を計算する際の重み
	static const float		   m_ZoomSpeedWeight; //!< ズーム速度を計算する際の重み
	static const float		   m_RotateSpeedWeight; //!< ズーム速度を計算する際の重み

	Lib::UpdateTask*				m_pUpdateTask;
	Lib::Camera*					m_pCamera;
	D3DXVECTOR3						m_Pos;
	D3DXVECTOR3						m_LookPoint; //!< カメラの注視点
	D3DXVECTOR2						m_CameraAngle;
	ID3D11Buffer*					m_pConstantBuffer;
	Lib::MouseDevice::MOUSESTATE	m_MouseState;
	const Lib::KeyDevice::KEYSTATE* m_pKeyState;
	float							m_MoveSpeed;
	float							m_ZoomSpeed;
	float							m_CameraLength; //!< カメラの注視点の距離
	bool							m_isCameraControl;

};


#endif
