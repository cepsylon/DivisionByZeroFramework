#pragma once

struct Vector3
{
	Vector3()
		: x(0.0f)
		, y(0.0f)
		, z(0.0f)
	{ }

	Vector3(float aX, float aY, float aZ)
		: x(aX)
		, y(aY)
		, z(aZ)
	{ }

	Vector3(float aValue)
		: x(aValue)
		, y(aValue)
		, z(aValue)
	{ }

	inline Vector3 operator+(const Vector3& aVector) const { return Vector3{ x + aVector.x, y + aVector.y, z + aVector.z }; }
	inline Vector3 operator-(const Vector3& aVector) const { return Vector3{ x - aVector.x, y - aVector.y, z - aVector.z };	}
	inline Vector3 operator*(float aScalar) const { return Vector3{ x * aScalar, y * aScalar, z * aScalar }; }
	inline friend Vector3 operator*(float aScalar, const Vector3& aVector) { return aVector * aScalar; }
	inline Vector3 operator/(float aScalar) const { return Vector3{ x / aScalar, y / aScalar, z / aScalar }; }
	inline Vector3 operator-() const { return Vector3{ -x, -y, -z }; }

	inline Vector3& operator+=(const Vector3& aVector) { x += aVector.x; y += aVector.y; z += aVector.z; return *this; }
	inline Vector3& operator-=(const Vector3& aVector) { x -= aVector.x; y -= aVector.y; z -= aVector.z; return *this; }
	inline Vector3& operator*=(float aScalar) { x *= aScalar; y *= aScalar; z *= aScalar; return *this; }
	inline Vector3& operator/=(float aScalar) { x /= aScalar; y /= aScalar; z /= aScalar; return *this; }

	inline float LengthSquare() const { return x * x + y * y + z * z; }
	inline float Length() const { return sqrtf(LengthSquare()); }
	inline Vector3 Normalize() const { return *this / Length(); }
	inline Vector3 Cross(const Vector3& aVector) const { return Vector3{ y * aVector.z - aVector.y * z, z * aVector.x - x * aVector.z, x * aVector.y - y * aVector.x }; }
	inline float Dot(const Vector3& aVector) const { return x * aVector.x + y * aVector.y + z * aVector.z; }

	float x;
	float y;
	float z;
};
