/**
 * @file	Define.h
 * @brief	define定義
 * @author	morimoto
 */
#ifndef DEFINE_H
#define DEFINE_H

//----------------------------------------------------------------------
// Define
//----------------------------------------------------------------------

// コピー禁止マクロ.
#define DISALLOW_COPY_AND_ASSIGN(Type)	Type(const Type&);			\
										void operator=(const Type&)

// インターフェースマクロ.
#define interface struct


//----------------------------------------------------------------------
// Template Functions
//----------------------------------------------------------------------
template <typename Type>
inline void SafeDelete(Type*& _type)
{
	delete _type;
	_type = nullptr;
}

template <typename Type>
inline void SafeDeleteArray(Type*& _type)
{
	delete[] _type;
	_type = nullptr;
}

template <typename Type>
inline void SafeRelease(Type*& _type)
{
	if (_type != nullptr)
	{
		_type->Release();
		_type = nullptr;
	}
}

template <typename Type>
inline void SafeDestroy(Type*& _type)
{
	if (_type != nullptr)
	{
		_type->Destroy();
		_type = nullptr;
	}
}


#endif // !DEFINE_H