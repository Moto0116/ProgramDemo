/**
 * @file	Vector.cpp
 * @brief	ベクタ構造体実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "Vector.h"

#include <Windows.h>


namespace Lib
{
	//----------------------------------------------------------------------
	// VECTOR2
	//----------------------------------------------------------------------
	VECTOR2::VECTOR2()
	{
		x = 0;
		y = 0;
	}

	VECTOR2::VECTOR2(const float* _pf)
	{
		x = _pf[0];
		y = _pf[1];
	}

	VECTOR2::VECTOR2(float _fx, float _fy)
	{
		x = _fx;
		y = _fy;
	}


	//----------------------------------------------------------------------
	// Cast
	//----------------------------------------------------------------------
	VECTOR2::operator float* ()
	{
		return reinterpret_cast<float*>(&x);
	}

	VECTOR2::operator const float* () const
	{
		return reinterpret_cast<const float*>(&x);
	}


	//----------------------------------------------------------------------
	// Assignment operators
	//----------------------------------------------------------------------
	VECTOR2& VECTOR2::operator += (const VECTOR2& _v)
	{
		x += _v.x;
		y += _v.y;
		return *this;
	}

	VECTOR2& VECTOR2::operator -= (const VECTOR2& _v)
	{
		x -= _v.x;
		y -= _v.y;
		return *this;
	}

	VECTOR2& VECTOR2::operator *= (float _f)
	{
		x *= _f;
		y *= _f;
		return *this;
	}

	VECTOR2& VECTOR2::operator /= (float _f)
	{
		float fInv = 1.0f / _f;
		x *= fInv;
		y *= fInv;
		return *this;
	}


	//----------------------------------------------------------------------
	// Unary operators
	//----------------------------------------------------------------------
	VECTOR2 VECTOR2::operator + () const
	{
		return *this;
	}

	VECTOR2 VECTOR2::operator - () const
	{
		return VECTOR2(-x, -y);
	}


	//----------------------------------------------------------------------
	// Binary Operator
	//----------------------------------------------------------------------
	VECTOR2	VECTOR2::operator + (const VECTOR2& _v) const
	{
		return VECTOR2(x + _v.x, y + _v.y);
	}

	VECTOR2 VECTOR2::operator - (const VECTOR2& _v) const
	{
		return VECTOR2(x - _v.x, y - _v.y);
	}

	VECTOR2 VECTOR2::operator * (float _f) const
	{
		return VECTOR2(x * _f, y * _f);
	}

	VECTOR2	VECTOR2::operator / (float _f) const
	{
		float fInv = 1.0f / _f;
		return VECTOR2(x * fInv, y * fInv);
	}

	VECTOR2 operator * (float _f, const VECTOR2& _v)
	{
		return VECTOR2(_f * _v.x, _f * _v.y);
	}


	//----------------------------------------------------------------------
	// Comparison Operator
	//----------------------------------------------------------------------
	bool VECTOR2::operator == (const VECTOR2& _v) const
	{
		return
			x == _v.x &&
			y == _v.y;
	}

	bool VECTOR2::operator != (const VECTOR2& _v) const
	{
		return
			x != _v.x ||
			y != _v.y;
	}



	//----------------------------------------------------------------------
	// VECTOR3
	//----------------------------------------------------------------------
	VECTOR3::VECTOR3()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	VECTOR3::VECTOR3(const float* _pf)
	{
		x = _pf[0];
		y = _pf[1];
		z = _pf[2];
	}

	VECTOR3::VECTOR3(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}


	//----------------------------------------------------------------------
	// Cast
	//----------------------------------------------------------------------
	VECTOR3::operator float* ()
	{
		return reinterpret_cast<float*>(&x);
	}

	VECTOR3::operator const float* () const
	{
		return reinterpret_cast<const float*>(&x);
	}


	//----------------------------------------------------------------------
	// Assignment operators
	//----------------------------------------------------------------------
	VECTOR3& VECTOR3::operator += (const VECTOR3& _v)
	{
		x += _v.x;
		y += _v.y;
		z += _v.z;
		return *this;
	}

	VECTOR3& VECTOR3::operator -= (const VECTOR3& _v)
	{
		x -= _v.x;
		y -= _v.y;
		z -= _v.z;
		return *this;
	}

	VECTOR3& VECTOR3::operator *= (float _f)
	{
		x *= _f;
		y *= _f;
		z *= _f;
		return *this;
	}

	VECTOR3& VECTOR3::operator /= (float _f)
	{
		float fInv = 1.0f / _f;
		x *= fInv;
		y *= fInv;
		z *= fInv;
		return *this;
	}


	//----------------------------------------------------------------------
	// Unary operators
	//----------------------------------------------------------------------
	VECTOR3 VECTOR3::operator + () const
	{
		return *this;
	}

	VECTOR3 VECTOR3::operator - () const
	{
		return VECTOR3(-x, -y, -z);
	}


	//----------------------------------------------------------------------
	// Binary Operator
	//----------------------------------------------------------------------
	VECTOR3 VECTOR3::operator + (const VECTOR3& _v) const
	{
		return VECTOR3(x + _v.x, y + _v.y, z + _v.z);
	}

	VECTOR3 VECTOR3::operator - (const VECTOR3& _v) const
	{
		return VECTOR3(x - _v.x, y - _v.y, z - _v.z);
	}

	VECTOR3 VECTOR3::operator * (float _f) const
	{
		return VECTOR3(x * _f, y * _f, z * _f);
	}

	VECTOR3 VECTOR3::operator / (float _f) const
	{
		float fInv = 1.0f / _f;
		return VECTOR3(x * fInv, y * fInv, z * fInv);
	}

	VECTOR3 operator * (float f, const VECTOR3& _v)
	{
		return VECTOR3(f * _v.x, f * _v.y, f * _v.z);
	}


	//----------------------------------------------------------------------
	// Comparison Operator
	//----------------------------------------------------------------------
	bool VECTOR3::operator == (const VECTOR3& _v) const
	{
		return
			x == _v.x &&
			y == _v.y &&
			z == _v.z;
	}

	bool VECTOR3::operator != (const VECTOR3& _v) const
	{
		return
			x != _v.x ||
			y != _v.y ||
			z != _v.z;
	}



	//----------------------------------------------------------------------
	// VECTOR4
	//----------------------------------------------------------------------
	VECTOR4::VECTOR4() :
		x(0),
		y(0),
		z(0),
		w(0)
	{
	}

	VECTOR4::VECTOR4(const float* _pf)
	{
		x = _pf[0];
		y = _pf[1];
		z = _pf[2];
		w = _pf[3];
	}

	VECTOR4::VECTOR4(float _x, float _y, float _z, float _w)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}


	//----------------------------------------------------------------------
	// Cast
	//----------------------------------------------------------------------
	VECTOR4::operator float*()
	{
		return reinterpret_cast<float*>(&x);
	}

	VECTOR4::operator const float*() const
	{
		return reinterpret_cast<const float*>(&x);
	}


	//----------------------------------------------------------------------
	// Assignment operators
	//----------------------------------------------------------------------
	VECTOR4& VECTOR4::operator += (const VECTOR4& _v)
	{
		x += _v.x;
		y += _v.y;
		z += _v.z;
		w += _v.w;
		return *this;
	}

	VECTOR4& VECTOR4::operator -= (const VECTOR4& _v)
	{
		x -= _v.x;
		y -= _v.y;
		z -= _v.z;
		w -= _v.w;
		return *this;
	}

	VECTOR4& VECTOR4::operator *= (float _f)
	{
		x *= _f;
		y *= _f;
		z *= _f;
		w *= _f;
		return *this;
	}

	VECTOR4& VECTOR4::operator /= (float _f)
	{
		float fInv = 1.0f / _f;
		x *= fInv;
		y *= fInv;
		z *= fInv;
		w *= fInv;
		return *this;
	}


	//----------------------------------------------------------------------
	// Unary operators
	//----------------------------------------------------------------------
	VECTOR4 VECTOR4::operator + () const
	{
		return *this;
	}

	VECTOR4 VECTOR4::operator - () const
	{
		return VECTOR4(-x, -y, -z, -w);
	}


	//----------------------------------------------------------------------
	// Binary Operator
	//----------------------------------------------------------------------
	VECTOR4 VECTOR4::operator + (const VECTOR4& _v) const
	{
		return VECTOR4(x + _v.x, y + _v.y, z + _v.z, w + _v.w);
	}

	VECTOR4	VECTOR4::operator - (const VECTOR4& _v) const
	{
		return VECTOR4(x - _v.x, y - _v.y, z - _v.z, w - _v.w);
	}

	VECTOR4	VECTOR4::operator * (float _f) const
	{
		return VECTOR4(x * _f, y * _f, z * _f, w * _f);
	}

	VECTOR4	VECTOR4::operator / (float _f) const
	{
		float fInv = 1.0f / _f;
		return VECTOR4(x * fInv, y * fInv, z * fInv, w * fInv);
	}

	VECTOR4	operator * (float _f, const VECTOR4& _v)
	{
		return VECTOR4(_f * _v.x, _f * _v.y, _f * _v.z, _f * _v.w);
	}


	//----------------------------------------------------------------------
	// Comparison Operator
	//----------------------------------------------------------------------
	bool VECTOR4::operator == (const VECTOR4& _v) const
	{
		return
			x == _v.x &&
			y == _v.y &&
			z == _v.z &&
			w == _v.w;
	}

	bool VECTOR4::operator != (const VECTOR4& _v) const
	{
		return
			x != _v.x ||
			y != _v.y ||
			z != _v.z ||
			w != _v.w;
	}
}

