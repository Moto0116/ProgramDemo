/**
 * @file   Camera.h
 * @brief  Cameraクラスの定義
 * @author morimoto
 */
#ifndef LIB_CAMERA_H
#define LIB_CAMERA_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include <D3DX10.h>


namespace Lib
{
	/**
	 * カメラ情報の管理クラス
	 */
	class Camera
	{
	public:
		/**
		 * コンストラクタ
		 * @param[in] _windowWidth ウィンドウの幅
		 * @param[in] _windowHeight ウィンドウの高さ
		 * @param[in] _nearZ カメラの最近点
		 * @param[in] _farZ カメラの最遠点
		 */
		Camera(float _windowWidth, float _windowHeight, float _nearZ, float _farZ);

		/**
		 * デストラクタ
		 */
		~Camera();

		/**
		 * 座標変換用行列作成
		 * @param[in] _eyePos カメラの座標
		 * @param[in] _lookPos カメラの注視座標
		 * @param[in] _viewAngle カメラの視野角
		 */
		void TransformView(const D3DXVECTOR3* _pEyePos, const D3DXVECTOR3* _pLookPos, float _viewAngle);

		/**
		 * ビュー座標変換用行列の取得
		 * @return ビュー座標変換用行列
		 */
		inline D3DXMATRIX GetViewMatrix()
		{
			return m_MatView;
		}

		/**
		 * プロジェクション座標変換用行列の取得
		 * @return プロジェクション座標変換用行列
		 */
		inline D3DXMATRIX GetProjectionMatrix()
		{
			return m_MatProj;
		}

	private:
		D3DXMATRIX	m_MatView;
		D3DXMATRIX	m_MatProj;
		float		m_WindowWidth;
		float		m_WindowHeight;
		float		m_NearZ;
		float		m_FarZ;

	};
}


#endif // LIB_CAMERA_H
