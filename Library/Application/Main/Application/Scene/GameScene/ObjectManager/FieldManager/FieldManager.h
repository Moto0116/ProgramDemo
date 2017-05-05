/**
 * @file	FieldManager.h
 * @brief	フィールドクラス定義
 * @author	morimoto
 */
#ifndef FIELDMANAGER_H
#define FIELDMANAGER_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "ObjectManagerBase\ObjectManagerBase.h"


/**
 * フィールドクラス
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


#endif // FIELDMANAGER_H
