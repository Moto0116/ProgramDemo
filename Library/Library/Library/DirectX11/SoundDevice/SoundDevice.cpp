/**
 * @file	SoundDevice.cpp
 * @brief	サウンドデバイスの管理クラス定義
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "SoundDevice.h"


namespace Lib
{
	//----------------------------------------------------------------------
	// Constructor	Destructor
	//----------------------------------------------------------------------
	SoundDevice::SoundDevice() : 
		m_pDSound8(NULL),
		m_hWnd(NULL)
	{
	}

	SoundDevice::~SoundDevice()
	{
		Finalize();
	}


	//----------------------------------------------------------------------
	// Public Functions
	//----------------------------------------------------------------------
	bool SoundDevice::Initialize(HWND _hWnd)
	{
		if (m_pDSound8 != NULL)
		{
			return false;
		}

		m_hWnd = _hWnd;
		DirectSoundCreate8(NULL, &m_pDSound8, NULL);
		m_pDSound8->SetCooperativeLevel(m_hWnd, DSSCL_NORMAL);

		return true;
	}

	void SoundDevice::Finalize()
	{
		if (m_pDSound8 != NULL)
		{
			m_pDSound8->Release();
			m_pDSound8 = NULL;
		}
	}
}

