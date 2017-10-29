/**
 * @file	test.h
 * @brief	テストクラス定義
 * @author	morimoto
 */
#ifndef TEST_H
#define TEST_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "Main\Object2DBase\Object2DBase.h"


/*** テストクラス */
class test : public Object2DBase
{
public:
	/*** コンストラクタ */
	test();

	/*** デストラクタ */
	virtual ~test();

	/**
	* 初期化処理
	* @return 初期化に成功したらtrue 失敗したらfalse
	*/
	virtual bool Initialize();

	/*** 終了処理 */
	virtual void Finalize();

	/*** 更新処理 */
	virtual void Update();

	/*** 描画処理 */
	virtual void Draw();

};


#endif // !TEST_H
