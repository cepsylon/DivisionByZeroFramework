#include "Camera.h"

#include "Math/Matrix44.h"

Matrix44 Camera::ViewMatrix() const
{
	// TODO
	return Matrix44{};
}

Matrix44 Camera::ProjectionMatrix(float anAspectRatio) const
{
	return Matrix44::Perspective(myFieldOfViewY.myValue, anAspectRatio, myNearPlane, myFarPlane);
}
