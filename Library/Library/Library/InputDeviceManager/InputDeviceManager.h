﻿/**
 * @file	InputDeviceManager.h
 * @brief	入力デバイスの管理クラス定義
 * @author	morimoto
 */
#ifndef LIB_INPUTDEVICEMANAGER_H
#define LIB_INPUTDEVICEMANAGER_H

//----------------------------------------------------------------------
// Define
//----------------------------------------------------------------------
#define DIRECTINPUT_VERSION 0x0800


//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include <dinput.h>

#include "GamePad\GamePad.h"
#include "KeyDevice\KeyDevice.h"
#include "MouseDevice\MouseDevice.h"
#include "..\SingletonBase\SingletonBase.h"


namespace Lib
{
	/**
	 * 入力デバイスの管理クラス
	 */
	class InputDeviceManager : public SingletonBase<InputDeviceManager>
	{
	public:
		friend SingletonBase<InputDeviceManager>;

		/**
		 * 管理するデバイスの種類
		 */
		enum INPUTDEVICE_TYPE
		{
			GAMEPAD_TYPE,		//!< コントローラー
			KEYDEVICE_TYPE,		//!< キーボード
			MOUSEDEVICE_TYPE,	//!< マウス
			DEVICE_TYPE_MAX		//!< デバイス最大数
		};

		/**
		 * 初期化処理
		 * @param[in] _hWnd 入力デバイスに対応するウィンドウハンドル
		 * @return 初期化に成功したか
		 */
		bool Initialize(HWND _hWnd);

		/**
		 * 終了処理
		 */
		void Finalize();

		/**
		 * デバイスの生成
		 * @param[in] _deviceType 生成するデバイスの種類
		 * @return 成功したらtrue 失敗したらfalse
		 */
		bool CreateDevice(INPUTDEVICE_TYPE _deviceType);

		/**
		 * デバイスの破棄
		 * @param[in] _deviceType 破棄するデバイスの種類
		 */
		void ReleaseDevice(INPUTDEVICE_TYPE _deviceType);

		/**
		 * KeyDeviceの状態を更新する
		 */
		void KeyUpdate();

		/**
		 * MouseDeviceの状態を更新する
		 */
		void MouseUpdate();

		/**
		 * キーの状態を更新する
		 * @param[in] 更新したいキーのDIK
		 */
		void KeyCheck(int _dik);

		/**
		 * キーの状態が格納されている配列を取得する
		 * @return キーの状態が格納されている配列
		 */
		const KeyDevice::KEYSTATE* GetKeyState() const;

		/**
		 * マウスの状態を取得する
		 * @return マウスの状態が格納されている構造体
		 */
		const MouseDevice::MOUSESTATE GetMouseState() const;

	private:
		/**
		 * コンストラクタ 
		 */
		InputDeviceManager();

		/**
		 * デストラクタ
		 */
		virtual ~InputDeviceManager();

		/**
		 * ゲームパッドの生成
		 * @return 成功したらtrue 失敗したらfalse
		 */
		bool CreateGamePad();

		/**
		 * キーデバイスの生成
		 * @return 成功したらtrue 失敗したらfalse
		 */
		bool CreateKeyDevice();

		/**
		 * マウスデバイスの生成
		 * @return 成功したらtrue 失敗したらfalse
		 */
		bool CreateMouseDevice();

		/**
		 * ゲームパッドの破棄
		 */
		void ReleaseGamePad();

		/**
		 * キーデバイスの破棄
		 */
		void ReleaseKeyDevice();

		/**
		 * マウスデバイスの破棄
		 */
		void ReleaseMouseDevice();


		GamePad*		m_pGamePad;
		KeyDevice*		m_pKeyDevice;
		MouseDevice*	m_pMouseDevice;
		LPDIRECTINPUT8	m_pDInput8;
		HWND			m_hWnd;

	};
}


#endif // LIB_INPUTDEVICEMANAGER_H
