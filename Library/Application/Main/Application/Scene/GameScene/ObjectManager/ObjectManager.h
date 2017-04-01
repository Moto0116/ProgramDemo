#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include <vector>

#include "ObjectManagerBase\ObjectBase\ObjectBase.h"


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
	~ObjectManager();

	/**
	 * 初期化処理
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	bool Initialize();

	/**
	 * 終了処理
	 */
	void Finalize();

private:
	std::vector<Lib::ObjectBase*> m_pObjects;

};


#endif
