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
#include "ObjectManagerBase\ObjectManagerBase.h"


/**
 * フィールド管理クラス
 */
class FieldManager : public Lib::ObjectManagerBase
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

};


#endif // !FIELDMANAGER_H
