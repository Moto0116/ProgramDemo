﻿/**
 * @file	UniquePtr.h
 * @brief	ユニークポインタクラス定義
 * @author	morimoto
 */
#ifndef LIB_UNIQUEPTR_H
#define LIB_UNIQUEPTR_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "Define\Define.h"


namespace Lib
{
	/**
	 * ユニークポインタクラス
	 */
	template <typename Type>
	class UniquePtr
	{
	public:
		template <typename Type>
		friend void Reset(UniquePtr<Type>& _ptr, Type* _src);

		template <typename Type>
		friend Type* GetPtr(UniquePtr<Type>& _ptr);

		template <typename Type>
		friend Type** GetPtrPtr(UniquePtr<Type>& _ptr);


		/**
		 * コンストラクタ
		 * @param[in] _ptr 設定するポインタ
		 */
		UniquePtr(Type* _ptr);

		/**
		 * コンストラクタ
		 * @param[in] _ptr 設定するポインタ
		 * @param[in] _isArray 設定するポインタが配列であるか
		 */
		UniquePtr(Type* _ptr, bool _isArray);

		/**
		 * コンストラクタ
		 * @param[in] _ptr 設定するポインタ
		 * @param[in] _pReleaseFunc ポインタの解放関数
		 */
		UniquePtr(Type* _ptr, void(*_pReleaseFunc)(Type*&));

		/**
		 * デストラクタ
		 */
		~UniquePtr();


		// キャスト.
		operator bool()
		{
			return m_Ptr != NULL;
		}


		// 単項演算子.
		Type& operator * ()
		{
			return *m_Ptr;
		}

		Type* operator -> ()
		{
			return m_Ptr;
		}


		// 比較演算子.
		bool operator == (Type* _ptr)
		{
			return m_Ptr == _ptr;
		}

		bool operator != (Type* _ptr)
		{
			return m_Ptr != _ptr;
		}

		// 添字演算子.
		Type& operator [] (int _index)
		{
			return m_Ptr[_index];	// 配列であるかどうかは考慮しない.
		}


	private:
		/**
		 * ポインタを設定
		 * @param[in] _ptr 設定するポインタ
		 */
		void Reset(Type* _ptr);

		/**
		 * ポインタを取得
		 * @return 管理しているポインタ取得
		 */
		Type* GetPtr();

		/**
		 * ポインタのアドレスを取得
		 * @return 管理しているポインタアドレス取得
		 */
		Type** GetPtrPtr();

		/**
		 * ポインタの解放
		 */
		void Release();


		Type*		m_Ptr;				//!< 管理するポインタ.
		void(*m_pReleaseFunc)(Type*&);	//!< 解放関数.


		DISALLOW_COPY_AND_ASSIGN(UniquePtr<Type>);	// コピー禁止.

	};


	/**
	 * ポインタの所有権を再設定
	 * @param[in] _ptr 再設定するポインタ管理オブジェクト
	 * @param[in] _src 設定するポインタ
	 */
	template <typename Type>
	void Reset(UniquePtr<Type>& _ptr, Type* _src = nullptr);

	/**
	 * ポインタの取得
	 * @param[in] _ptr 取得するポインタの管理オブジェクト
	 */
	template <typename Type>
	Type* GetPtr(UniquePtr<Type>& _ptr);

	/**
	 * ポインタアドレスの取得
	 * @param[in] _ptr 取得するポインタアドレスの管理オブジェクト
	 */
	template <typename Type>
	Type** GetPtrPtr(UniquePtr<Type>& _ptr);

}


#include "UniquePtr_private.inl"


#endif // !LIB_UNIQUEPTR_H
