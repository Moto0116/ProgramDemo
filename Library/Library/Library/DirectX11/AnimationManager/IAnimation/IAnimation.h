/**
 * @file	IAnimation.h
 * @brief	アニメーションインターフェース定義
 * @author	morimoto
 */
#ifndef LIB_IANIMATION_H
#define LIB_IANIMATION_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include <Windows.h>

#include "..\..\..\Define\Define.h"


namespace Lib
{
	/**
	 * アニメーションインターフェース
	 */
	interface IAnimation
	{
	public:
		/**
		 * アニメーションのパターン
		 */
		enum ANIMATION_PATTERN
		{
			ONE_ANIMATION,			//!< アニメーションが終了したらそのまま終了する
			LOOP_ANIMATION,			//!< アニメーションが終了したら最初からループする
			REVERSE_ONE_ANIMATION,	//!< アニメーションが終了したら反転して最初まで戻る
			REVERSE_LOOP_ANIMATION	//!< アニメーションが終了したら反転して最初まで戻るのをする
		};

		/**
		 * コンストラクタ
		 */
		IAnimation();

		/**
		 * デストラクタ
		 */
		virtual ~IAnimation() = 0;

		/**
		 * アニメーションの更新
		 * @return アニメーションが終了したらtrue 終了していなかったらfalse
		 */
		virtual bool Update() = 0;

		/**
		 * アニメーションのパターンを設定する(設定前のフレーム等は初期化される)
		 * @param[in] _animationPattern アニメーションのパターン
		 */
		virtual void SetAnimationPattern(ANIMATION_PATTERN _animationPattern) = 0;

		/**
		 * アニメーションの速度を設定する
		 * @param[in] _animationSpeed アニメーションの速度
		 */
		virtual void SetAnimationSpeed(float _animationSpeed) = 0;

	private:
		DISALLOW_COPY_AND_ASSIGN(IAnimation);

	};
}



#endif // LIB_IANIMATION_H
