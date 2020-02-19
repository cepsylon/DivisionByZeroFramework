#include <catch/catch.hpp>

#include "Math/Quaternion.h"

TEST_CASE("Quaternion_CanBeDefaultConstructed", "[Math], [Quaternion]")
{
	Quaternion quaternion;

	REQUIRE(quaternion.myAxis.x == Approx(0.0f));
	REQUIRE(quaternion.myAxis.y == Approx(0.0f));
	REQUIRE(quaternion.myAxis.z == Approx(0.0f));
	REQUIRE(quaternion.myValue == 1.0f);
}
