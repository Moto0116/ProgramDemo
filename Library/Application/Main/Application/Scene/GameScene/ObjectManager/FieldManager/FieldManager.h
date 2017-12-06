/**
 * @file	FieldManager.h
 * @brief	フィールド管理クラス定義
 * @author	morimoto
 */
#ifndef FIELDMANAGER_H
#define FIELDMANAGER_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "ObjectBase\ObjectBase.h"

#include <vector>


/**
 * フィールド管理クラス
 */
class FieldManager : public Lib::ObjectBase
{
public:
	/**
	 * コンストラクタ
	 */
	FieldManager();

	/**
	 * デストラクタ
	 */
	virtual ~FieldManager();

	/**
	 * 初期化処理
	 * @return 初期化に成功したか
	 */
	virtual bool Initialize();

	/*** 終了処理 */
	virtual void Finalize();

private:
	std::vector<Lib::ObjectBase*> m_pObjects;

};


#endif // !FIELDMANAGER_H
