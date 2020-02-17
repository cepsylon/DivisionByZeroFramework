#pragma once

#include "Vector4.h"

#include <math.h>

struct Matrix44
{
	Matrix44()
		: myXAxis(1.0f, 0.0f, 0.0f, 0.0f)
		, myYAxis(0.0f, 1.0f, 0.0f, 0.0f)
		, myZAxis(0.0f, 0.0f, 1.0f, 0.0f)
		, myPosition(0.0f, 0.0f, 0.0f, 1.0f)
	{ }

	Matrix44( float a00, float a01, float a02, float a03,
						float a10, float a11, float a12, float a13,
						float a20, float a21, float a22, float a23,
						float a30, float a31, float a32, float a33 )
		: myXAxis(a00, a01, a02, a03)
		, myYAxis(a10, a11, a12, a13)
		, myZAxis(a20, a21, a22, a23)
		, myPosition(a30, a31, a32, a33)
	{ }

	Matrix44(float aValue)
		: myXAxis(aValue)
		, myYAxis(aValue)
		, myZAxis(aValue)
		, myPosition(aValue)
	{ }

	inline Matrix44 operator*(const Matrix44& anOther) const
	{
		const SIMD::Vector& X = myXAxis.myVector;
		const SIMD::Vector& Y = myYAxis.myVector;
		const SIMD::Vector& Z = myZAxis.myVector;
		const SIMD::Vector& P = myPosition.myVector;

		Matrix44 result;

		SIMD::Vector vector = anOther.myXAxis.myVector;
		result.myXAxis = SIMD::MultiplyAdd(
			X, SIMD::Shuffle<0, 0, 0, 0>(vector, vector),	SIMD::MultiplyAdd(
				Y, SIMD::Shuffle<1, 1, 1, 1>(vector, vector),	SIMD::MultiplyAdd(
					Z, SIMD::Shuffle<2, 2, 2, 2>(vector, vector), SIMD::Multiply(P, SIMD::Shuffle<3, 3, 3, 3>(vector, vector))
				)
			)
		);

		vector = anOther.myYAxis.myVector;
		result.myYAxis = SIMD::MultiplyAdd(
			X, SIMD::Shuffle<0, 0, 0, 0>(vector, vector), SIMD::MultiplyAdd(
				Y, SIMD::Shuffle<1, 1, 1, 1>(vector, vector), SIMD::MultiplyAdd(
					Z, SIMD::Shuffle<2, 2, 2, 2>(vector, vector), SIMD::Multiply(P, SIMD::Shuffle<3, 3, 3, 3>(vector, vector))
				)
			)
		);

		vector = anOther.myZAxis.myVector;
		result.myZAxis = SIMD::MultiplyAdd(
			X, SIMD::Shuffle<0, 0, 0, 0>(vector, vector), SIMD::MultiplyAdd(
				Y, SIMD::Shuffle<1, 1, 1, 1>(vector, vector), SIMD::MultiplyAdd(
					Z, SIMD::Shuffle<2, 2, 2, 2>(vector, vector), SIMD::Multiply(P, SIMD::Shuffle<3, 3, 3, 3>(vector, vector))
				)
			)
		);

		vector = anOther.myPosition.myVector;
		result.myPosition = SIMD::MultiplyAdd(
			X, SIMD::Shuffle<0, 0, 0, 0>(vector, vector), SIMD::MultiplyAdd(
				Y, SIMD::Shuffle<1, 1, 1, 1>(vector, vector), SIMD::MultiplyAdd(
					Z, SIMD::Shuffle<2, 2, 2, 2>(vector, vector), SIMD::Multiply(P, SIMD::Shuffle<3, 3, 3, 3>(vector, vector))
				)
			)
		);

		return result;
	}

	inline Vector4 operator*(const Vector4& aVector)
	{
		const SIMD::Vector& X = myXAxis.myVector;
		const SIMD::Vector& Y = myYAxis.myVector;
		const SIMD::Vector& Z = myZAxis.myVector;
		const SIMD::Vector& P = myPosition.myVector;

		Vector4 result = SIMD::MultiplyAdd(
			X, SIMD::Shuffle<0, 0, 0, 0>(aVector.myVector, aVector.myVector), SIMD::MultiplyAdd(
				Y, SIMD::Shuffle<1, 1, 1, 1>(aVector.myVector, aVector.myVector), SIMD::MultiplyAdd(
					Z, SIMD::Shuffle<2, 2, 2, 2>(aVector.myVector, aVector.myVector), SIMD::Multiply(P, SIMD::Shuffle<3, 3, 3, 3>(aVector.myVector, aVector.myVector))
				)
			)
		);

		return result;
	}

	inline const Vector4& operator[](unsigned int anIndex) const { return myVectors[anIndex]; }
	inline Vector4& operator[](unsigned int anIndex) { return myVectors[anIndex]; }

	inline static Matrix44 Translate(float aX, float aY, float aZ)
	{
		return Matrix44{ 1.0f, 0.0f, 0.0f, 0.0f,
										 0.0f, 1.0f, 0.0f, 0.0f,
										 0.0f, 0.0f, 1.0f, 0.0f,
										 aX, aY, aZ, 1.0f };
	}

	inline static Matrix44 Scale(float aX, float aY, float aZ)
	{
		return Matrix44{ aX, 0.0f, 0.0f, 0.0f,
										 0.0f, aY, 0.0f, 0.0f,
										 0.0f, 0.0f, aZ, 0.0f,
										 0.0f, 0.0f, 0.0f, 1.0f };
	}

	inline static Matrix44 Perspective(float aFieldOfViewY, float anAspectRatio, float aNear, float aFar)
	{
		float tanHalfFieldOfViewY = tanf(aFieldOfViewY * 0.5f);

		Matrix44 result(0.0f);

		// Our world has a right hand coordinate system with y pointing upwards and x to the right
		// Since Vulkan uses same coordinate system, but with y pointing downwards, that is why we
		// need to negate the y coordinate
		result[0][0] = 1.0f / (anAspectRatio * tanHalfFieldOfViewY);
		result[1][1] = -1.0f / tanHalfFieldOfViewY;
		result[2][2] = aFar / (aFar - aNear);
		result[2][3] = -1.0f;
		result[3][2] = -(aFar * aNear) / (aFar - aNear);

		return result;
	}

	union
	{
		struct
		{
			Vector4 myXAxis;
			Vector4 myYAxis;
			Vector4 myZAxis;
			Vector4 myPosition;
		};

		struct
		{
			Vector4 myVectors[4];
		};
	};
};
