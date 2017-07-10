/**
 * @file	SmartPointer.h
 * @brief	スマートポインタヘッダ
 * @author	morimoto
 */
#ifndef LIB_SMARTPOINTER_H
#define LIB_SMARTPOINTER_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "SharedPtr\SharedPtr.h"
#include "UniquePtr\UniquePtr.h"
#include "WeakPtr\WeakPtr.h"


template <typename Type>
using SharedPtr = Lib::SharedPtr<Type>;

template <typename Type>
using UniquePtr = Lib::UniquePtr<Type>;

template <typename Type>
using WeakPtr = Lib::WeakPtr<Type>;


#endif // !LIB_SMARTPOINTER_H
