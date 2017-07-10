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
		m_Ptr(_ptr)
	{
	}


	//----------------------------------------------------------------------
	// Move Constructor
	//----------------------------------------------------------------------
	template <typename Type>
	UniquePtr<Type>::UniquePtr(UniquePtr<Type>&& _src)
	{
		m_Ptr = _src.m_Ptr;

		_src.m_Ptr = nullptr;	// 所有権を放棄.
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
		SafeDelete(m_Ptr);
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

	template <typename Type, typename CreateFunctor, typename... Args>
	UniquePtr<Type> CreateUniquePtr(Args... _args)
	{
		CreateFunctor<Type, Args...> Functor;
		Type* pType = Functor(_args...);
		UniquePtr<Type> pSmartPtr(pType);

		return pSmartPtr;
	}
}

