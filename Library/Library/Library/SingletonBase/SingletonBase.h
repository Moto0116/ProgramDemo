/**
 * @file	SingletonBase.h
 * @brief	Singleton基底クラス定義
 * @author	morimoto
 */
#ifndef LIB_SINGLETONBASE_H
#define LIB_SINGLETONBASE_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include <Windows.h>

#include "..\Define\Define.h"


namespace Lib
{
	/**
	 * Singletonの基底クラス
	 * @tparam Type シングルトンを適用するクラス
	 */
	template <typename Type>
	class SingletonBase
	{
	public:
		typedef Type InstanceType;

		/**
		 * インスタンスの生成
		 */
		inline static void Create()
		{
			if (m_pInstance == NULL)
			{
				m_pInstance = new Type();
			}
		}
	
		/**
		 * インスタンスの取得
		 * @return インスタンス
		 */
		inline static Type* GetInstance()
		{
			return m_pInstance;
		}
	
		/**
		 * インスタンスの破棄
		 */
		inline static void Delete()
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
	
	
	protected:
		static Type* m_pInstance;	//!< シングルトンインスタンス
	
		/**
		 * コンストラクタ
		 */
		inline SingletonBase();
	
		/**
		 * デストラクタ
		 */
		inline virtual ~SingletonBase();
	
	private:
		DISALLOW_COPY_AND_ASSIGN(SingletonBase);

	};
}


#include "SingletonBase_private.h"


#define SINGLETON_CREATE(Type) Lib::SingletonBase<Type>::Create()
#define SINGLETON_INSTANCE(Type) Lib::SingletonBase<Type>::GetInstance()
#define SINGLETON_DELETE(Type) Lib::SingletonBase<Type>::Delete()


#endif // LIB_SINGLETONBASE_H
