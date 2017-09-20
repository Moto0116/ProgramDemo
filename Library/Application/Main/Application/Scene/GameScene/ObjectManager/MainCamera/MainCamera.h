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

#include "DirectX11\Camera\Dx11Camera.h"
#include "InputDeviceManager\InputDeviceManager.h"
#include "ObjectManagerBase\ObjectBase\ObjectBase.h"


namespace Lib
{
	class UpdateTask;
}


/**
 * カメラを操作するクラス
 */
class MainCamera : public Lib::ObjectBase
{
public:
	/**
	 * コンストラクタ
	 */
	MainCamera();

	/**
	 * デストラクタ
	 */
	virtual ~MainCamera();

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

	/**
	 * カメラ座標を取得
	 * @return カメラ座標
	 */
	inline D3DXVECTOR3 GetPos()
	{
		return m_Pos;
	}
	
	/**
	 * ビルボード回転行列取得
	 * @param[in] _pBillPos ビルボードオブジェクトの座標位置
	 * @param[out] _pRotation 回転行列の出力先
	 */
	void GetBillBoardRotation(D3DXVECTOR3* _pBillPos, D3DXMATRIX* _pRotation);

private:
	/**
	 * カメラの定数バッファ
	 */
	struct CAMERA_CONSTANT_BUFFER
	{
		D3DXMATRIX	View;			//!< メインカメラのビュー行列.
		D3DXMATRIX	Proj;			//!< メインカメラのプロジェクション行列.
		D3DXVECTOR4	CameraPos;		//!< メインカメラ座標.
		D3DXVECTOR4	CameraDir;		//!< メインカメラの向き.
		D3DXVECTOR4 Aspect;			//!< アスペクト比.
		D3DXMATRIX	ReflectView;	//!< 反射カメラのビュー行列.
		D3DXMATRIX	ReflectProj;	//!< 反射カメラのプロジェクション行列.
	};


	static const float	m_NearPoint;			//!< 最近点.
	static const float	m_FarPoint;				//!< 最遠点.
	static const float	m_ViewAngle;			//!< 視野角.
	static const float	m_ReflectViewAngle;		//!< 反射カメラの視野角.
	static const float	m_MaxAngle;				//!< カメラの最大傾き.
	static const float	m_MinAngle;				//!< カメラの最小傾き.
	static const float	m_MaxLength;			//!< カメラと注視点の最大距離.
	static const float	m_MinLength;			//!< カメラと注視点の最小距離.
	static const float	m_MoveSpeedWeight;		//!< 移動速度を計算する際の重み.
	static const float	m_ZoomSpeedWeight;		//!< ズーム速度を計算する際の重み.
	static const float	m_RotateSpeedWeight;	//!< ズーム速度を計算する際の重み.



	//----------------------------------------------------------------------
	// カメラの操作処理
	//----------------------------------------------------------------------

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


	//----------------------------------------------------------------------
	// 生成処理
	//----------------------------------------------------------------------

	/**
	 * 定数バッファ初期化関数
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	bool CreateConstantBuffer();


	//----------------------------------------------------------------------
	// 解放処理
	//----------------------------------------------------------------------

	/**
	 * 定数バッファの開放関数
	 */
	void ReleaseConstantBuffer();


	//----------------------------------------------------------------------
	// その他処理
	//----------------------------------------------------------------------

	/**
	 * 定数バッファへの書き込み関数
	 * @return 書き込みに成功したらtrue 失敗したらfalse
	 */
	bool WriteConstantBuffer();



	//--------------------タスクオブジェクト--------------------
	Lib::UpdateTask*				m_pUpdateTask;		//!< 更新タスクオブジェクト.


	//--------------------その他オブジェクト--------------------
	Lib::Dx11::Camera*				m_pCamera;			//!< カメラオブジェクト.


	//--------------------カメラのステータス--------------------
	D3DXVECTOR3						m_Pos;				//!< カメラ座標.
	D3DXVECTOR3						m_LookPoint;		//!< カメラの注視点.
	D3DXVECTOR3						m_UpVec;			//!< カメラの上方向ベクトル.
	D3DXVECTOR2						m_CameraAngle;		//!< カメラの画角.
	float							m_MoveSpeed;		//!< カメラの移動速度.
	float							m_ZoomSpeed;		//!< カメラのズーム速度.
	float							m_CameraLength;		//!< カメラの注視点の距離.
	bool							m_IsCameraControl;	//!< カメラを操作したか.


	//--------------------カメラの定数バッファ--------------------
	ID3D11Buffer*					m_pConstantBuffer;	//!< 定数バッファ.


	//--------------------入力デバイスの状態--------------------
	Lib::MouseDevice::MOUSESTATE	m_MouseState;		//!< マウスの状態.
	const Lib::KeyDevice::KEYSTATE* m_pKeyState;		//!< キーの状態.

};


#endif // !MAINCAMERA_H
