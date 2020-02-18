#pragma once

#include "Math/MathCommon.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"

struct Matrix44;

class Camera
{
public:
	Matrix44 ViewMatrix() const;
	Matrix44 ProjectionMatrix(float anAspectRatio) const;

	void SetFieldOfViewY(Math::Radian aRadian) { myFieldOfViewY = aRadian; }
	void SetFieldOfViewY(Math::Degree aDegree) { myFieldOfViewY = aDegree.ToRadian(); }
	void SetFarPlane(float aFarPlane) { myFarPlane = aFarPlane; }
	void SetNearPlane(float aNearPlane) { myNearPlane = aNearPlane; }

	Math::Radian GetFieldOfViewY() const { return myFieldOfViewY; }

private:
	Quaternion myOrientation;
	Vector3 myPosition;
	Math::Radian myFieldOfViewY = Math::DegreeToRadian(60.0f);
	float myFarPlane = 100.0f;
	float myNearPlane = 0.1f;
};
