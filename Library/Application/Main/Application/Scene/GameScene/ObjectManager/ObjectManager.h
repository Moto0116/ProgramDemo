/**
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

#include "ObjectManagerBase\ObjectManagerBase.h"
#include "ObjectManagerBase\ObjectBase\ObjectBase.h"


/**
 * オブジェクト管理クラス
 */
class ObjectManager : public Lib::ObjectManagerBase
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

};


#endif // OBJECTMANAGER_H
