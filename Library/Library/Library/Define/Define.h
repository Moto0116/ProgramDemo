/**
 * @file	Define.h
 * @brief	define��`
 * @author	morimoto
 */
#ifndef DEFINE_H
#define DEFINE_H


//----------------------------------------------------------------------
// Define
//----------------------------------------------------------------------

// �R�s�[�֎~�}�N��
#define DISALLOW_COPY_AND_ASSIGN(Type)	Type(const Type&); \
										void operator=(const Type&)


template <typename Type>
inline void SafeDelete(Type*& _type)
{
	if (_type != NULL)
	{
		delete _type;
		_type = NULL;
	}
}

template <typename Type>
inline void SafeDeleteArray(Type*& _type)
{
	if (_type != NULL)
	{
		delete[] _type;
		_type = NULL;
	}
}


#endif // DEFINE_H