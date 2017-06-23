﻿/**
 * @file	SoundDevice.cpp
 * @brief	サウンドデバイスの管理クラス定義
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "SoundDevice.h"

#include "Debugger\Debugger.h"


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
		MyAssert(m_pDSound8 == NULL, "SoundDeviceオブジェクトは既に初期化されています");

		m_hWnd = _hWnd;
		DirectSoundCreate8(NULL, &m_pDSound8, NULL);
		m_pDSound8->SetCooperativeLevel(m_hWnd, DSSCL_NORMAL);

		return true;
	}

	void SoundDevice::Finalize()
	{
		SafeRelease(m_pDSound8);
	}
}

