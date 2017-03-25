﻿/**
 * @file	SingletonBase_private.h
 * @brief	Singleton基底クラステンプレート実装
 * @author	morimoto
 */
#ifndef LIB_SINGLETONBASE_PRIVATE_H
#define LIB_SINGLETONBASE_PRIVATE_H


namespace Lib
{
	//----------------------------------------------------------------------
	// Static Private Variables
	//----------------------------------------------------------------------
	template <typename Type>
	Type* SingletonBase<Type>::m_pInstance = NULL;


	//----------------------------------------------------------------------
	// Constructor	Destructor
	//----------------------------------------------------------------------
	template <typename Type>
	inline SingletonBase<Type>::SingletonBase()
	{
	}

	template <typename Type>
	inline SingletonBase<Type>::~SingletonBase()
	{
	}
}


#endif // LIB_SINGLETONBASE_PRIVATE_H
