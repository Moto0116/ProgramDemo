/**
 * @file	KeyDevice.cpp
 * @brief	キーデバイスクラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "KeyDevice.h"

#include "..\..\Define\Define.h"
#include "..\..\Debugger\Debugger.h"


namespace Lib
{
	//----------------------------------------------------------------------
	// Constructor	Destructor
	//----------------------------------------------------------------------
	KeyDevice::KeyDevice() :
		m_pDInput8(NULL),
		m_pDInputDevice8(NULL),
		m_hWnd(NULL)
	{
	}

	KeyDevice::~KeyDevice()
	{
		Finalize();
	}


	//----------------------------------------------------------------------
	// Public Functions
	//----------------------------------------------------------------------
	bool KeyDevice::Initialize(LPDIRECTINPUT8 _pDInput8, HWND _hWnd)
	{
		if (_pDInput8 == NULL)
		{
			OutputErrorLog("DirectInputオブジェクトがありません");
			return false;
		}


		m_pDInput8 = _pDInput8;
		m_hWnd = _hWnd;

		if (FAILED(m_pDInput8->CreateDevice(GUID_SysKeyboard, &m_pDInputDevice8, NULL)))
		{
			OutputErrorLog("デバイスの生成に失敗しました");
			return false;
		}

		if (FAILED(m_pDInputDevice8->SetDataFormat(&c_dfDIKeyboard)))
		{
			OutputErrorLog("デバイスのフォーマットに失敗しました");
			SafeRelease(m_pDInputDevice8);
			return false;
		}

		DIPROPDWORD DiProp;
		DiProp.diph.dwSize = sizeof(DiProp);
		DiProp.diph.dwHeaderSize = sizeof(DiProp.diph);
		DiProp.diph.dwObj = 0;
		DiProp.diph.dwHow = DIPH_DEVICE;
		DiProp.dwData = 1000;
		if (FAILED(m_pDInputDevice8->SetProperty(DIPROP_BUFFERSIZE, &DiProp.diph)))
		{
			OutputErrorLog("デバイスのプロパティ設定に失敗しました");
			SafeRelease(m_pDInputDevice8);
			return false;
		}

		if (FAILED(m_pDInputDevice8->SetCooperativeLevel(m_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND)))
		{
			OutputErrorLog("デバイスの協調レベルの設定に失敗しました");
			SafeRelease(m_pDInputDevice8);
			return false;
		}

		m_pDInputDevice8->Acquire();

		return true;
	}

	void KeyDevice::Finalize()
	{
		SafeRelease(m_pDInputDevice8);
	}

	void KeyDevice::Update()
	{
		HRESULT Result = m_pDInputDevice8->Acquire();
		if ((Result == DI_OK) || (Result == S_FALSE))
		{
			m_pDInputDevice8->GetDeviceState(sizeof(m_pDIKeyState), &m_pDIKeyState);
		}
	}

	void KeyDevice::KeyCheck(int _dik)
	{
		if (m_pDIKeyState[_dik] & 0x80)
		{
			if (m_pOldDIKeyState[_dik] == KEY_OFF)
			{
				m_pKeyState[_dik] = KEY_PUSH;
			}
			else
			{
				m_pKeyState[_dik] = KEY_ON;
			}
			m_pOldDIKeyState[_dik] = KEY_ON;
		}
		else
		{
			if (m_pOldDIKeyState[_dik] == KEY_ON)
			{
				m_pKeyState[_dik] = KEY_RELEASE;
			}
			else
			{
				m_pKeyState[_dik] = KEY_OFF;
			}
			m_pOldDIKeyState[_dik] = KEY_OFF;
		}
	}
}
