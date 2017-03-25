/**
 * @file	Sound.h
 * @brief	サウンドクラス定義
 * @author	morimoto
 */
#ifndef LIB_SOUND_H
#define LIB_SOUND_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include <dsound.h>

#include "..\..\..\Define\Define.h"
#include "..\SoundManager.h"
#include "..\ISound\ISound.h"


namespace Lib
{
	class SoundDevice;

	/**
	 * サウンドクラス
	 */
	class Sound : public ISound
	{
	public:
		/**
		 * コンストラクタ
		 */
		Sound(SoundDevice* _pSoundDevice, LPSTR _pSoundPath);

		/**
		 * デストラクタ
		 */
		virtual ~Sound();

		/**
		 * サウンドの操作
		 */
		virtual void SoundOperation(SoundManager::SOUND_OPERATION _operation);

		/**
		 * サウンドの取得
		 * @return サウンド
		 */
		inline LPDIRECTSOUNDBUFFER8 Get()
		{
			return m_pSound;
		}


	private:
		/**
		 * サウンドの読み込み
		 * @param[in] _pSoundPath テクスチャのパス
		 */
		void Load(LPSTR _pSoundPath);

		/**
		 * サウンドの解放
		 */
		void Release();

		/**
		 * waveファイルの読み込み
		 */
		bool WaveLoad(LPSTR _pFilepath, WAVEFORMATEX* _pWaveFormat, char** _pWaveData, DWORD* _pDataSize);


		SoundDevice*			m_pSoundDevice;
		LPDIRECTSOUNDBUFFER8	m_pSound;

		DISALLOW_COPY_AND_ASSIGN(Sound);

	};
}



#endif // LIB_SOUND_H
