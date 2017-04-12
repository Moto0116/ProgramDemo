/**
 * @file	LightUI.h
 * @brief	ライトUIクラス定義
 * @author	morimoto
 */

#ifndef LIGHTUI_H
#define LIGHTUI_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "Main\Object2DBase\Object2DBase.h"
#include "TaskManager\TaskBase\DrawTask\DrawTask.h"
#include "TaskManager\TaskBase\UpdateTask\UpdateTask.h"


/**
 * ライトUIクラス
 */
class LightUI : public Object2DBase
{
public:
	/**
	 * コンストラクタ
	 */
	LightUI();
	
	/**
	 * デストラクタ
	 */
	virtual ~LightUI();

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


#endif // LIGHTUI_H
