#pragma once

#include "Platform/PlatformDefines.h"

#if IS_WINDOWS_PLATFORM

#include <xmmintrin.h>
#include <immintrin.h>

namespace SIMD
{
	using Vector = __m128;

	inline Vector Load(float aX, float aY, float aZ, float aW)
	{
		alignas(16) float vector[] = { aX, aY, aZ, aW };
		return _mm_load_ps(vector);
	}

	inline Vector Load(float aValue)
	{
		alignas(16) float vector[] = { aValue, aValue, aValue, aValue };
		return _mm_load_ps(vector);
	}

	inline Vector Add(const Vector& aLeft, const Vector& aRight)
	{
		return _mm_add_ps(aLeft, aRight);
	}

	inline Vector Subtract(const Vector& aLeft, const Vector& aRight)
	{
		return _mm_sub_ps(aLeft, aRight);
	}

	inline Vector Multiply(const Vector& aLeft, const Vector& aRight)
	{
		return _mm_mul_ps(aLeft, aRight);
	}

	inline Vector Multiply(const Vector& aVector, float aScalar)
	{
		return Multiply(aVector, Load(aScalar));
	}

	inline Vector Divide(const Vector& aLeft, const Vector& aRight)
	{
		return _mm_div_ps(aLeft, aRight);
	}

	inline Vector Divide(const Vector& aVector, float aScalar)
	{
		return Divide(aVector, Load(aScalar));
	}

	inline Vector MultiplyAdd(const Vector& aLeftMultiply, const Vector& aRightMultiply, const Vector& anAddVector)
	{
		return _mm_fmadd_ps(aLeftMultiply, aRightMultiply, anAddVector);
	}

	// Creates vector with values (aLeft[anIndex0], aLeft[anIndex1], aRight[anIndex2], aRight[anIndex3])
	template <unsigned int Index0, unsigned int Index1, unsigned int Index2, unsigned int Index3>
	Vector Shuffle(const Vector& aLeft, const Vector& aRight)
	{
		return _mm_shuffle_ps(aLeft, aRight, _MM_SHUFFLE(Index3, Index2, Index1, Index0));
	}
}

#define SIMD_VECTOR_INDEX_OPERATOR(aVector, anIndex) aVector.m128_f32[anIndex]

#else

// TODO: have this working
namespace SIMD
{
	using Vector = __m128;

	inline Vector Load(float aX, float aY, float aZ, float aW)
	{
		alignas(16) float vector[] = { aX, aY, aZ, aW };
		return __mm_load_ps(vector);
	}

	inline Vector Load(float aValue)
	{
		alignas(16) float vector[] = { aValue, aValue, aValue, aValue };
		return __mm_load_ps(vector);
	}

	inline Vector Add(const Vector& aLeft, const Vector& aRight)
	{
		return __mm_add_ps(aLeft, aRight);
	}

	inline Vector Subtract(const Vector& aLeft, const Vector& aRight)
	{
		return __mm_sub_ps(aLeft, aRight);
	}
}

#endif // IS_WINDOWS_PLATFORM
