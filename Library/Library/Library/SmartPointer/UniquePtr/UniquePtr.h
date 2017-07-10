/**
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
	 * @tparam Type 管理させるポインタの型
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
		UniquePtr(Type* _ptr = nullptr);

		/**
		 * ムーブコンストラクタ
		 * @param[in] _src ムーブ元
		 */
		UniquePtr(UniquePtr<Type>&& _src);

		/**
		 * デストラクタ
		 */
		~UniquePtr();


		// キャスト.
		explicit operator bool()
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


		Type* m_Ptr; //!< 管理するポインタ.


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

	/**
 	 * ユニークポインタ生成関数
	 * @tparam Type 生成するポインタ型
	 * @tparam CreateFunctor 生成処理ファンクタ
	 * @tparam Args 生成する型のコンストラクタ引数の型
	 * @param _args 生成する型のコンストラクタ引数
	 */
	template <
		typename Type, 
		typename CreateFunctor = CreateFunctor,
		typename... Args>
	UniquePtr<Type> CreateUniquePtr(Args... _args);

}


#include "UniquePtr_private.inl"


#endif // !LIB_UNIQUEPTR_H
