#pragma once

#include "SIMDVector.h"

struct Vector4
{
	inline Vector4()
		: myVector(SIMD::Load(0.0f))
	{ }

	inline Vector4(float aX, float aY, float aZ, float aW)
		: myVector(SIMD::Load(aX, aY, aZ, aW))
	{ }

	inline Vector4(float aValue)
		: myVector(SIMD::Load(aValue))
	{ }

	inline Vector4(SIMD::Vector aVector)
		: myVector(aVector)
	{ }

	inline Vector4 operator+(const Vector4& anOther) const { return SIMD::Add(myVector, anOther.myVector); }
	inline Vector4 operator-(const Vector4& anOther) const { return SIMD::Subtract(myVector, anOther.myVector); }
	inline Vector4 operator*(float aScalar) const { return SIMD::Multiply(myVector, aScalar); }
	inline Vector4 operator/(float aScalar) const { return SIMD::Divide(myVector, aScalar); }

	inline Vector4& operator+=(const Vector4& anOther) { myVector = SIMD::Add(myVector, anOther.myVector); return *this; }
	inline Vector4& operator-=(const Vector4& anOther) { myVector = SIMD::Subtract(myVector, anOther.myVector); return *this; }
	inline Vector4& operator*=(float aScalar) { myVector = SIMD::Multiply(myVector, aScalar); return *this; }
	inline Vector4& operator/=(float aScalar) { myVector = SIMD::Divide(myVector, aScalar); return *this; }

	inline const float& operator[](unsigned int anIndex) const { return myValues[anIndex]; }
	inline float& operator[](unsigned int anIndex) { return myValues[anIndex]; }

	union
	{
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};
		struct
		{
			float myValues[4];
		};
		SIMD::Vector myVector;
	};
};
