#pragma once

#include "MathCommon.h"
#include "SIMDVector.h"
#include "Vector3.h"

struct Quaternion
{
	Quaternion()
		: myValue(1.0f)
		, myAxis(0.0f)
	{ }

	Quaternion(const Vector3& anAxis, Math::Radian aRadian)
		: myValue(Math::Cos(aRadian))
		, myAxis(anAxis.Normalize() * Math::Sin(aRadian))
	{ }

	Quaternion(const Vector3& anAxis, Math::Degree aDegree)
		: myValue(Math::Cos(aDegree))
		, myAxis(anAxis.Normalize() * Math::Sin(aDegree))
	{ }

	inline Quaternion operator*(const Quaternion& aQuaternion)const
	{
		return Quaternion
		{
			myAxis.Cross(aQuaternion.myAxis) + aQuaternion.myAxis * myValue + myAxis * aQuaternion.myValue,
			aQuaternion.myValue * myValue - myAxis.Dot(aQuaternion.myAxis)
		};
	}

	inline Quaternion Conjugate() const { return Quaternion{ -myAxis, myValue }; }

	union
	{
		struct
		{
			float w;
			float x;
			float y;
			float z;
		};

		struct
		{
			float myValue;
			Vector3 myAxis;
		};

		SIMD::Vector myVector;
	};

private:
	// Needed for conjugate, do not provide this to the users
	Quaternion(const Vector3& aAxis, float aValue)
		: myValue(aValue)
		, myAxis(aAxis)
	{ }
};
