/**
 * @file	SharedPtr_private.inl
 * @brief	シェアドポインタクラス実装
 * @author	morimoto
 */


namespace Lib
{
	//----------------------------------------------------------------------
	// Constructor
	//----------------------------------------------------------------------
	template <typename Type>
	SharedPtr<Type>::SharedPtr(Type* _ptr) :
		m_Ptr(_ptr),
		m_pRefCount(new int),
		m_pReleaseFunc(&SafeDelete<Type>)
	{
		*m_pRefCount = 0;
		AddRef();
	}

	template <typename Type>
	SharedPtr<Type>::SharedPtr(Type* _ptr, bool _isArray) :
		m_Ptr(_ptr),
		m_pRefCount(new int)
	{
		*m_pRefCount = 0;
		AddRef();

		if (_isArray)	// 配列かどうかで解放関数を切り替える.
			m_pReleaseFunc = &SafeDeleteArray<Type>;
		else
			m_pReleaseFunc = &SafeDelete<Type>;
	}

	template <typename Type>
	SharedPtr<Type>::SharedPtr(Type* _ptr, void(*_pReleaseFunc)(Type*&)) :
		m_Ptr(_ptr),
		m_pRefCount(new int),
		m_pReleaseFunc(_pReleaseFunc)
	{
		*m_pRefCount = 0;
		AddRef();
	}


	//----------------------------------------------------------------------
	// Copy Constructor
	//----------------------------------------------------------------------
	template <typename Type>
	SharedPtr<Type>::SharedPtr(const SharedPtr<Type>& _src)
	{
		m_Ptr = _src.m_Ptr;
		m_pRefCount = _src.m_pRefCount;
		m_pReleaseFunc = _src.m_pReleaseFunc;

		AddRef();
	}


	//----------------------------------------------------------------------
	// Destructor
	//----------------------------------------------------------------------
	template <typename Type>
	SharedPtr<Type>::~SharedPtr()
	{
		Release();
	}


	//----------------------------------------------------------------------
	// Private Functions
	//----------------------------------------------------------------------
	template <typename Type>
	void SharedPtr<Type>::Reset(Type* _ptr)
	{
		Release();	// 既に所有しているポインタの所有権は手放す.

		m_Ptr = _ptr;
		m_pRefCount = new int;
		*m_pRefCount = 0;
		AddRef();
	}

	template <typename Type>
	Type* SharedPtr<Type>::GetPtr()
	{
		return m_Ptr;	// 管理するポインタを返す.
	}

	template <typename Type>
	Type** SharedPtr<Type>::GetPtrPtr()
	{
		return &m_Ptr;	// 管理するポインタのアドレスを返す.
	}

	template <typename Type>
	int SharedPtr<Type>::GetCounter()
	{
		return *m_pRefCount;
	}

	template <typename Type>
	void SharedPtr<Type>::AddRef()
	{
		(*m_pRefCount)++;
	}

	template <typename Type>
	void SharedPtr<Type>::Release()
	{
		(*m_pRefCount)--;

		if ((*m_pRefCount) == 0)
		{
			// 参照カウンタが0なら解放関数を呼び出す.
			m_pReleaseFunc(m_Ptr);
			SafeDelete(m_pRefCount);
		}

		m_Ptr = nullptr;
		m_pRefCount = nullptr;
	}



	//----------------------------------------------------------------------
	// Friend Functions
	//----------------------------------------------------------------------
	template <typename Type>
	void Reset(SharedPtr<Type>& _ptr, Type* _src)
	{
		_ptr.Reset(_src);
	}

	template <typename Type>
	Type* GetPtr(SharedPtr<Type>& _ptr)
	{
		return _ptr.GetPtr();
	}

	template <typename Type>
	Type** GetPtrPtr(SharedPtr<Type>& _ptr)
	{
		return _ptr.GetPtrPtr();
	}

	template <typename Type>
	int GetCounter(SharedPtr<Type>& _ptr)
	{
		return _ptr.GetCounter();
	}
}
