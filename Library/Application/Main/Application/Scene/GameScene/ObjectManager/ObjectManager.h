﻿/**
 * @file	ObjectManager.h
 * @brief	オブジェクト管理クラス定義
 * @author	morimoto
 */
#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include <vector>

#include "ObjectBase\ObjectBase.h"


/**
 * オブジェクト管理クラス
 */
class ObjectManager
{
public:
	/**
	 * コンストラクタ
	 */
	ObjectManager();

	/**
	 * デストラクタ
	 */
	virtual ~ObjectManager();

	/**
	 * 初期化処理
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	virtual bool Initialize();

	/**
	 * 終了処理
	 */
	virtual void Finalize();

private:
	std::vector<Lib::ObjectBase*> m_pObjects;

};


#endif // !OBJECTMANAGER_H
