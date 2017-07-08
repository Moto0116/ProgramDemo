/**
 * @file	SharedPtr.h
 * @brief	シェアドポインタクラス定義
 * @author	morimoto
 */
#ifndef LIB_SHAREDPTR_H
#define LIB_SHAREDPTR_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "Define\Define.h"


namespace Lib
{
	template <typename Type>
	class SharedPtr
	{
	public:
		template <typename Type>
		friend void Reset(SharedPtr<Type>& _ptr, Type* _src);

		template <typename Type>
		friend Type* GetPtr(SharedPtr<Type>& _ptr);

		template <typename Type>
		friend Type** GetPtrPtr(SharedPtr<Type>& _ptr);

		template <typename Type>
		friend int GetCounter(SharedPtr<Type>& _ptr);


		/**
		 * コンストラクタ
		 * @param[in] _ptr 設定するポインタ
		 */
		SharedPtr(Type* _ptr);

		/**
		 * コンストラクタ
		 * @param[in] _ptr 設定するポインタ
		 * @param[in] _isArray 設定するポインタが配列であるか
		 */
		SharedPtr(Type* _ptr, bool _isArray);

		/**
		 * コンストラクタ
		 * @param[in] _ptr 設定するポインタ
		 * @param[in] _pReleaseFunc ポインタの解放関数
		 */
		SharedPtr(Type* _ptr, void(*_pReleaseFunc)(Type*&));

		/**
		 * コピーコンストラクタ
		 */
		SharedPtr(const SharedPtr<Type>& _src);

		/**
		 * デストラクタ
		 */
		~SharedPtr();


		// キャスト.
		operator bool()
		{
			return m_Ptr != NULL;
		}


		// 代入演算子.
		SharedPtr<Type>& operator = (const SharedPtr<Type>& _src)
		{
			// 同じポインタ同士のコピーは行わない.
			if (m_Ptr == _src.m_Ptr) return (*this);

			Release(); // 既に所有しているポインタは解放.

			m_Ptr = _src.m_Ptr;
			m_pRefCount = _src.m_pRefCount;

			AddRef();

			return (*this);
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
		 * 参照カウンタの取得
		 * @return 参照カウンタ
		 */
		int GetCounter();

		/**
		 * 参照カウンタの増分
		 */
		void AddRef();

		/**
		 * ポインタの解放
		 */
		void Release();


		Type*		m_Ptr;				//!< 管理するポインタ.
		int*		m_pRefCount;		//!< 参照カウンタ.
		void(*m_pReleaseFunc)(Type*&);	//!< 解放関数.

	};


	/**
	 * ポインタの所有権を再設定
	 * @param[in] _ptr 再設定するポインタ管理オブジェクト
	 * @param[in] _src 設定するポインタ
	 */
	template <typename Type>
	void Reset(SharedPtr<Type>& _ptr, Type* _src = nullptr);

	/**
	 * ポインタの取得
	 * @param[in] _ptr 取得するポインタの管理オブジェクト
	 */
	template <typename Type>
	Type* GetPtr(SharedPtr<Type>& _ptr);

	/**
	 * ポインタアドレスの取得
	 * @param[in] _ptr 取得するポインタアドレスの管理オブジェクト
	 */
	template <typename Type>
	Type** GetPtrPtr(SharedPtr<Type>& _ptr);

	/**
	 * ポインタの参照数の取得
	 * @param[in] _ptr 取得するポインタ参照数の管理オブジェクト
	 */
	template <typename Type>
	int GetCounter(SharedPtr<Type>& _ptr);

}


#include "SharedPtr_private.inl"


#endif // !LIB_SHAREDPTR_H
