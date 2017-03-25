/**
 * @file	Animation.h
 * @brief	2Dアニメーションクラス定義
 * @author	morimoto
 */
#ifndef LIB_ANIMATION_H
#define LIB_ANIMATION_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include <vector>
#include <Windows.h>

#include "..\IAnimation\IAnimation.h"


namespace Lib
{
	/**
	 * 2Dアニメーションクラス
	 */
	class Animation : public IAnimation
	{
	public:
		/**
		 * アニメーションのフレーム情報
		 */
		struct ANIMATION_FRAME
		{
			int FrameID;	//!< フレームのID

			float MinTu;	//!< tuの最小値
			float MinTv;	//!< tvの最小値
			float MaxTu;	//!< tuの最大値
			float MaxTv;	//!< tvの最大値
		};


		/**
		 * コンストラクタ
		 * @param[in] _pAnimationPath アニメーションファイルのパス
		 */
		Animation(LPCTSTR _pAnimationPath);

		/**
		 * デストラクタ
		 */
		~Animation();

		/**
		 * アニメーションの更新
		 * @return アニメーションが終了したらtrue 終了していなかったらfalse
		 */
		bool Update();

		/**
		 * アニメーションのパターンを設定する(設定前のフレーム等は初期化される)
		 * @param[in] _animationPattern アニメーションのパターン
		 */
		virtual void SetAnimationPattern(ANIMATION_PATTERN _animationPattern);

		/**
		 * アニメーションの速度を設定する
		 * @param[in] _animationSpeed アニメーションの速度
		 */
		virtual void SetAnimationSpeed(float _animationSpeed);

		/**
		 * 現在のフレームを取得する
		 * @return 現在のアニメーションフレーム
		 */
		inline ANIMATION_FRAME* GetCurrentFrame()
		{
			return m_pFrame[m_CurrentFrame];
		}

	private:
		/**
		 * 読み込み処理
		 * @param[in] _pAnimationPath アニメーションファイルのパス
		 */
		void Load(LPCTSTR _pAnimationPath);

		/**
		 * 解放処理
		 */
		void Release();

		/**
		 * アニメーション制御
		 */
		bool OneAnimationControl();

		/**
		 * ループアニメーション制御
		 */
		bool LoopAnimationControl();

		/**
		 * 逆アニメーション制御
		 */
		bool ReverseOneAnimationControl();

		/**
		 * 逆ループアニメーション制御
		 */
		bool ReverseLoopAnimationControl();


		static const float				m_AnimationTime;	//!< アニメーションを行う時間
		std::vector<ANIMATION_FRAME*>	m_pFrame;			//!< アニメーションフレーム
		int								m_CurrentFrame;		//!< 現在のアニメーションフレーム
		float							m_AnimationCounter;	//!< アニメーションを行うかを判断するためのカウンタ
		float							m_AnimationSpeed;	//!< アニメーションを行うスピード
		ANIMATION_PATTERN				m_AnimationPattern;	//!< アニメーションのパターン
		bool							m_IsReverse;		//!< 反転しているか
		bool(Animation::*m_pAnimationFunc)();

	};
}


#endif // LIB_ANIMATION_H
