#pragma once

#include <math.h>

namespace Math
{
	constexpr float PI = 3.14159265358979323846f;

	struct Radian;
	struct Degree;

	float DegreeToRadian(float aDegree);
	float DegreeToRadian(const Degree& aDegree);
	float RadianToDegree(float aRadian);
	float RadianToDegree(const Radian& aRadian);

	struct Radian
	{
		inline Radian(float aRadian)
			: myValue(aRadian)
		{ }

		inline explicit Radian(const Degree& aDegree)
			: myValue(DegreeToRadian(aDegree))
		{ }

		inline Radian operator*(float aScalar) const { return Radian{ myValue * aScalar }; }
		inline friend Radian operator*(float aScalar, const Radian& aDegree) { return aDegree * aScalar; }
		inline Radian operator/(float aScalar) const { return Radian{ myValue / aScalar }; }
		inline friend Radian operator/(float aScalar, const Radian& aDegree) { return aDegree * aScalar; }

		inline float ToDegree() const { return RadianToDegree(myValue); }

		float myValue;
	};

	struct Degree
	{
		inline Degree(float aDegree)
			: myValue(aDegree)
		{ }

		inline explicit Degree(const Radian& aRadian)
			: myValue(RadianToDegree(aRadian))
		{ }

		inline Degree operator*(float aScalar) const { return Degree{ myValue * aScalar }; }
		inline friend Degree operator*(float aScalar, const Degree& aDegree) { return aDegree * aScalar; }
		inline Degree operator/(float aScalar) const { return Degree{ myValue / aScalar }; }
		inline friend Degree operator/(float aScalar, const Degree& aDegree) { return aDegree * aScalar; }

		inline float ToRadian() const { return DegreeToRadian(myValue); }

		float myValue;
	};

	inline float DegreeToRadian(float aDegree) { return aDegree * (PI / 180.0f); }
	inline float DegreeToRadian(const Degree& aDegree) { return DegreeToRadian(aDegree.myValue); }
	inline float RadianToDegree(float aRadian) { return aRadian * (180.0f / PI); }
	inline float RadianToDegree(const Radian& aRadian) { return RadianToDegree(aRadian.myValue); }
	inline float Cos(const Radian& aRadian) { return cosf(aRadian.myValue); }
	inline float Cos(const Degree& aDegree) { return cosf(aDegree.ToRadian()); }
	inline float Sin(const Radian& aRadian) { return sinf(aRadian.myValue); }
	inline float Sin(const Degree& aDegree) { return sinf(aDegree.ToRadian()); }
	inline float Tan(const Radian& aRadian) { return tanf(aRadian.myValue); }
	inline float Tan(const Degree& aDegree) { return tanf(aDegree.ToRadian()); }
}
