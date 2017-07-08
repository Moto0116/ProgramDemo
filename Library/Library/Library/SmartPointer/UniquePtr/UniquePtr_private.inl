/**
 * @file	UniquePtr_private.inl
 * @brief	ユニークポインタクラス実装
 * @author	morimoto
 */


namespace Lib
{
	//----------------------------------------------------------------------
	// Constructor
	//----------------------------------------------------------------------
	template <typename Type>
	UniquePtr<Type>::UniquePtr(Type* _ptr) :
		m_Ptr(_ptr),
		m_pReleaseFunc(&SafeDelete<Type>)
	{
	}

	template <typename Type>
	UniquePtr<Type>::UniquePtr(Type* _ptr, bool _isArray) :
		m_Ptr(_ptr)
	{
		if (_isArray)	// 配列かどうかで解放関数を切り替える.
			m_pReleaseFunc = &SafeDeleteArray<Type>;
		else
			m_pReleaseFunc = &SafeDelete<Type>;
	}

	template <typename Type>
	UniquePtr<Type>::UniquePtr(Type* _ptr, void(*_pReleaseFunc)(Type*&)) :
		m_Ptr(_ptr),
		m_pReleaseFunc(_pReleaseFunc)
	{
	}


	//----------------------------------------------------------------------
	// Destructor
	//----------------------------------------------------------------------
	template <typename Type>
	UniquePtr<Type>::~UniquePtr()
	{
		Release();
	}


	//----------------------------------------------------------------------
	// Private Functions
	//----------------------------------------------------------------------
	template <typename Type>
	void UniquePtr<Type>::Reset(Type* _ptr)
	{
		Release();	// 既に所有しているポインタは解放.

		m_Ptr = _ptr;
	}

	template <typename Type>
	Type* UniquePtr<Type>::GetPtr()
	{
		return m_Ptr;
	}

	template <typename Type>
	Type** UniquePtr<Type>::GetPtrPtr()
	{
		return &m_Ptr;
	}

	template <typename Type>
	void UniquePtr<Type>::Release()
	{
		m_pReleaseFunc(m_Ptr);
	}


	//----------------------------------------------------------------------
	// Friend Functions
	//----------------------------------------------------------------------
	template <typename Type>
	void Reset(UniquePtr<Type>& _ptr, Type* _src)
	{
		_ptr.Reset(_src);
	}

	template <typename Type>
	Type* GetPtr(UniquePtr<Type>& _ptr)
	{
		return _ptr.GetPtr();
	}

	template <typename Type>
	Type** GetPtrPtr(UniquePtr<Type>& _ptr)
	{
		return _ptr.GetPtrPtr();
	}
}

