/**
 * @file	KeyDevice.h
 * @brief	キーデバイスクラス定義
 * @author	morimoto
 */
#ifndef LIB_KEYDEVICE_H
#define LIB_KEYDEVICE_H

//----------------------------------------------------------------------
// Define
//----------------------------------------------------------------------
/**
 * DirectInputのバージョン指定
 */
#define DIRECTINPUT_VERSION 0x0800


//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include <dinput.h>


namespace Lib
{
	/**
	 * キーデバイスクラス
	 */
	class KeyDevice
	{
	public:
		/**
		 * キーの状態を表した列挙子
		 */
		enum KEYSTATE
		{
			KEY_PUSH,		//!< キーが押された瞬間の状態.
			KEY_RELEASE,	//!< キーが離された瞬間の状態.
			KEY_ON,			//!< キーが押され続けている状態.
			KEY_OFF			//!< キーが押されていない状態.
		};

		/** 
		 * コンストラクタ
		 */
		KeyDevice();

		/**
		 * デストラクタ
		 */
		~KeyDevice();

		/**
		 * 初期化処理
		 * @param[in] _pDInput8 DirectInput8オブジェクト
		 * @param[in] _hWnd デバイスに対応させるウィンドウハンドル
		 * @return 初期化に成功したらtrue 初期化に失敗したらfalse
		 */
		bool Initialize(LPDIRECTINPUT8 _pDInput8, HWND _hWnd);

		/**
		 * 終了処理
		 */
		void Finalize();

		/**
		 * デバイスの状態を更新する
		 */
		void Update();

		/**
		 * キーの状態を更新する
		 * @param[in] 更新したいキーのDIK
		 */
		void KeyCheck(int _dik);

		/**
		 * キーの状態が格納されている配列を取得する
		 * @return キーの状態が格納されている配列
		 */
		inline const KEYSTATE* GetKeyState() const 
		{
			return m_pKeyState;
		}

	private:
		LPDIRECTINPUT8			m_pDInput8;				//!< DirectInput8オブジェクト.
		LPDIRECTINPUTDEVICE8	m_pDInputDevice8;		//!< DirectInput8デバイス.
		HWND					m_hWnd;					//!< 対応するウィンドウハンドル.
		KEYSTATE				m_pKeyState[256];		//!< キーの状態を格納する構造体.
		BYTE					m_pDIKeyState[256];		//!< キーの状態を格納している配列.
		BYTE					m_pOldDIKeyState[256];	//!< キーの古い状態を格納している配列.

	};
}


#endif // LIB_KEYDEVICE_H
