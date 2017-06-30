/**
 * @file	Vector.h
 * @brief	ベクタ構造体定義
 * @author	morimoto
 */
#ifndef LIB_VECTOR_H
#define LIB_VECTOR_H


namespace Lib
{
	/**
	 * 2次元ベクタ構造体
	 */
	struct VECTOR2
	{
	public:
		// Constructor コンストラクタ.
		VECTOR2();
		VECTOR2(const float* _pf);
		VECTOR2(float _x, float _y);

		// Cast キャスト.
		operator float* ();
		operator const float* () const;

		// Assignment operators 代入演算子.
		VECTOR2& operator += (const VECTOR2& _v);
		VECTOR2& operator -= (const VECTOR2& _v);
		VECTOR2& operator *= (float _f);
		VECTOR2& operator /= (float _f);

		// Unary operators 単項演算子.
		VECTOR2 operator + () const;
		VECTOR2 operator - () const;

		// Binary Operator 二項演算子.
		VECTOR2 operator + (const VECTOR2& _v) const;
		VECTOR2 operator - (const VECTOR2& _v) const;
		VECTOR2 operator * (float _f) const;
		VECTOR2 operator / (float _f) const;

		// Comparison Operator 比較演算子.
		bool operator == (const VECTOR2& _v) const;
		bool operator != (const VECTOR2& _v) const;


		float x, y;	//!< 要素.

	};

	/**
	 * 3次元ベクタ構造体
	 */
	struct VECTOR3
	{
	public:
		// Constructor コンストラクタ.
		VECTOR3();
		VECTOR3(const float* _pf);
		VECTOR3(float _x, float _y, float _z);

		// Cast キャスト.
		operator float* ();
		operator const float* () const;

		// Assignment operators 代入演算子.
		VECTOR3& operator += (const VECTOR3& _v);
		VECTOR3& operator -= (const VECTOR3& _v);
		VECTOR3& operator *= (float _f);
		VECTOR3& operator /= (float _f);

		// Unary operators 単項演算子.
		VECTOR3 operator + () const;
		VECTOR3 operator - () const;

		// Binary Operator 二項演算子.
		VECTOR3 operator + (const VECTOR3& _v) const;
		VECTOR3 operator - (const VECTOR3& _v) const;
		VECTOR3 operator * (float _f) const;
		VECTOR3 operator / (float _f) const;

		// Comparison Operator 比較演算子.
		bool operator == (const VECTOR3& _v) const;
		bool operator != (const VECTOR3& _v) const;


		float x, y, z;	//!< 要素.

	};

	/**
	 * 4次元ベクタ構造体
	 */
	struct VECTOR4
	{
	public:
		// Constructor コンストラクタ.
		VECTOR4();
		VECTOR4(const float* _pf);
		VECTOR4(float _x, float _y, float _z, float _w);

		// Cast キャスト.
		operator float* ();
		operator const float* () const;

		// Assignment operators 代入演算子.
		VECTOR4& operator += (const VECTOR4& _v);
		VECTOR4& operator -= (const VECTOR4& _v);
		VECTOR4& operator *= (float _f);
		VECTOR4& operator /= (float _f);

		// Unary operators 単項演算子.
		VECTOR4 operator + () const;
		VECTOR4 operator - () const;

		// Binary Operator 二項演算子.
		VECTOR4 operator + (const VECTOR4& _v) const;
		VECTOR4 operator - (const VECTOR4& _v) const;
		VECTOR4 operator * (float _f) const;
		VECTOR4 operator / (float _f) const;

		// Comparison Operator 比較演算子.
		bool operator == (const VECTOR4& _v) const;
		bool operator != (const VECTOR4& _v) const;


		float x, y, z, w;	//!< 要素.

	};
}


#endif // LIB_VECTOR_H
