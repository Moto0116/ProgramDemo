/**
 * @file	InputDeviceManager.cpp
 * @brief	入力デバイス管理クラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "InputDeviceManager.h"


namespace Lib
{
	//----------------------------------------------------------------------
	// Constructor	Destructor
	//----------------------------------------------------------------------
	InputDeviceManager::InputDeviceManager() : 
		m_pGamePad(NULL),
		m_pKeyDevice(NULL),
		m_pMouseDevice(NULL),
		m_pDInput8(NULL),
		m_hWnd(NULL)
	{
	}

	InputDeviceManager::~InputDeviceManager()
	{
		Finalize();
	}


	//----------------------------------------------------------------------
	// Public Functions
	//----------------------------------------------------------------------
	bool InputDeviceManager::Initialize(HWND _hWnd)
	{
		if (FAILED(DirectInput8Create(
			GetModuleHandle(NULL),
			DIRECTINPUT_VERSION,
			IID_IDirectInput8,
			reinterpret_cast<void**>(&m_pDInput8),
			NULL)))
		{
			return false;
		}

		m_hWnd = _hWnd;

		return true;
	}

	void InputDeviceManager::Finalize()
	{
		if (m_pGamePad != NULL)
		{
			delete m_pGamePad;
			m_pGamePad = NULL;
		}

		if (m_pKeyDevice != NULL)
		{
			m_pKeyDevice->Finalize();
			delete m_pKeyDevice;
			m_pKeyDevice = NULL;
		}

		if (m_pMouseDevice != NULL)
		{
			m_pMouseDevice->Finalize();
			delete m_pMouseDevice;
			m_pMouseDevice = NULL;
		}

		if (m_pDInput8 != NULL)
		{
			m_pDInput8->Release();
			m_pDInput8 = NULL;
		}
	}
	
	bool InputDeviceManager::CreateDevice(INPUTDEVICE_TYPE _deviceType)
	{
		bool Result = false;

		switch (_deviceType)
		{
		case GAMEPAD_TYPE:
			Result = CreateGamePad();
			break;
		case KEYDEVICE_TYPE:
			Result = CreateKeyDevice();
			break;
		case MOUSEDEVICE_TYPE:
			Result = CreateMouseDevice();
			break;
		}

		return Result;
	}

	void InputDeviceManager::ReleaseDevice(INPUTDEVICE_TYPE _deviceType)
	{
		switch (_deviceType)
		{
		case GAMEPAD_TYPE:
			ReleaseGamePad();
			break;
		case KEYDEVICE_TYPE:
			ReleaseKeyDevice();
			break;
		case MOUSEDEVICE_TYPE:
			ReleaseMouseDevice();
			break;
		}
	}

	void InputDeviceManager::KeyUpdate()
	{
		m_pKeyDevice->Update();
	}

	void InputDeviceManager::MouseUpdate()
	{
		m_pMouseDevice->Update();
	}

	void InputDeviceManager::KeyCheck(int _dik)
	{
		m_pKeyDevice->KeyCheck(_dik);
	}

	const KeyDevice::KEYSTATE* InputDeviceManager::GetKeyState() const
	{
		return m_pKeyDevice->GetKeyState();
	}

	const MouseDevice::MOUSESTATE InputDeviceManager::GetMouseState() const
	{
		return m_pMouseDevice->GetMouseState();
	}


	//----------------------------------------------------------------------
	// Private Functions
	//----------------------------------------------------------------------
	bool InputDeviceManager::CreateGamePad()
	{
		if (m_pGamePad != NULL)
		{
			return false;
		}

		m_pGamePad = new GamePad();

		return true;
	}

	bool InputDeviceManager::CreateKeyDevice()
	{
		if (m_pKeyDevice != NULL)
		{
			return false;
		}

		m_pKeyDevice = new KeyDevice();
		if (!m_pKeyDevice->Initialize(m_pDInput8, m_hWnd))
		{
			delete m_pKeyDevice;
			m_pKeyDevice = NULL;
			return false;
		}

		return true;
	}

	bool InputDeviceManager::CreateMouseDevice()
	{
		if (m_pMouseDevice != NULL)
		{
			return false;
		}

		m_pMouseDevice = new MouseDevice();
		if (!m_pMouseDevice->Initialize(m_pDInput8, m_hWnd))
		{
			delete m_pMouseDevice;
			m_pMouseDevice = NULL;
			return false;
		}

		return true;
	}

	void InputDeviceManager::ReleaseGamePad()
	{
		if (m_pGamePad != NULL)
		{
			delete m_pGamePad;
			m_pGamePad = NULL;
		}
	}

	void InputDeviceManager::ReleaseKeyDevice()
	{
		if (m_pKeyDevice != NULL)
		{
			m_pKeyDevice->Finalize();
			delete m_pKeyDevice;
			m_pKeyDevice = NULL;
		}
	}

	void InputDeviceManager::ReleaseMouseDevice()
	{
		if (m_pMouseDevice != NULL)
		{
			m_pMouseDevice->Finalize();
			delete m_pMouseDevice;
			m_pMouseDevice = NULL;
		}
	}
}
