#pragma once

#include "MathCommon.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix44.h"

struct Quaternion
{
	Quaternion()
		: myAxis(0.0f)
		, myValue(1.0f)
	{ }

	Quaternion(const Vector3& anAxis, Math::Radian aRadian)
		: myAxis(anAxis.Normalize() * Math::Sin(aRadian * 0.5f))
		, myValue(Math::Cos(aRadian * 0.5f))
	{ }

	Quaternion(const Vector3& anAxis, Math::Degree aDegree)
		: myAxis(anAxis.Normalize() * Math::Sin(aDegree * 0.5f))
		, myValue(Math::Cos(aDegree * 0.5f))
	{ }

	inline Quaternion operator*(const Quaternion& aQuaternion) const
	{
		return Quaternion
		{
			myAxis.Cross(aQuaternion.myAxis) + aQuaternion.myAxis * myValue + myAxis * aQuaternion.myValue,
			aQuaternion.myValue * myValue - myAxis.Dot(aQuaternion.myAxis)
		};
	}

	inline Quaternion Conjugate() const { return Quaternion{ -myAxis, myValue }; }

	inline Matrix44 GetMatrix() const
	{
		// TODO: This can be optimized
		Vector3 axisSquared{ myAxis.x * myAxis.x, myAxis.y * myAxis.y, myAxis.z * myAxis.z };
		float xy = x * y;
		float xz = x * z;
		float yz = y * z;
		float wx = w * x;
		float wy = w * y;
		float wz = w * z;

		return Matrix44
		{
			1.0f - 2.0f * (axisSquared.y + axisSquared.z), 2.0f * (xy + wz), 2.0f * (xz - wy), 0.0f,
			2.0f * (xy - wz), 1.0f - 2.0f * (axisSquared.x + axisSquared.z), 2.0f * (yz + wx), 0.0f,
			2.0f * (xz + wy), 2.0f * (yz - wx), 1.0f - 2.0f * (axisSquared.x + axisSquared.y), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	}

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
			Vector3 myAxis;
			float myValue;
		};

		Vector4 myVector;
	};

private:
	// Needed for conjugate, do not provide this to the users
	Quaternion(const Vector3& aAxis, float aValue)
		: myAxis(aAxis)
		, myValue(aValue)
	{ }
};
