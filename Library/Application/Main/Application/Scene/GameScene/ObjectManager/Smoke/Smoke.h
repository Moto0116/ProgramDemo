/**
 * @file	Smoke.h
 * @brief	煙クラス定義
 * @author	morimoto
 */
#ifndef SMOKE_H
#define SMOKE_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "ObjectManagerBase\ObjectBase\ObjectBase.h"


/**
 * 煙クラス
 */
class Smoke : public Lib::ObjectBase
{
public:
	/**
	 * コンストラクタ
	 */
	Smoke();

	/**
	 * デストラクタ
	 */
	virtual ~Smoke();

	/**
	 * 初期化処理
	 * @return 初期化に成功したか
	 */
	virtual bool Initialize();

	/**
	 * 終了処理
	 */
	virtual void Finalize();

	/**
	 * オブジェクトの更新
	 */
	virtual void Update();

	/**
	 * オブジェクトの描画
	 */
	virtual void Draw();

};



#endif // SMOKE_H
