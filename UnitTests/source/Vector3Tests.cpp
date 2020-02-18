#include <catch/catch.hpp>

#include "Math/Vector3.h"

namespace
{
	static constexpr int locStressTestCount = 10000;
}

TEST_CASE("Vector3_DefaultConstructsToAllZeros", "[Math], [Vector3]")
{
	Vector3 vector;

	REQUIRE(vector.x == Approx(0.0f));
	REQUIRE(vector.y == Approx(0.0f));
	REQUIRE(vector.z == Approx(0.0f));
}

TEST_CASE("Vector3_CanBeConstructedWithThreeValues", "[Math], [Vector3]")
{
	Vector3 vector{ 1.0f, 2.0f, 3.0f };

	REQUIRE(vector.x == Approx(1.0f));
	REQUIRE(vector.y == Approx(2.0f));
	REQUIRE(vector.z == Approx(3.0f));
}

TEST_CASE("Vector3_CanBeConstructedWithSingleValue", "[Math], [Vector3]")
{
	Vector3 vector{ 2.0f };

	REQUIRE(vector.x == Approx(2.0f));
	REQUIRE(vector.y == Approx(2.0f));
	REQUIRE(vector.z == Approx(2.0f));
}

TEST_CASE("Vector3_CanAddVectors", "[Math], [Vector3]")
{
	Vector3 vector0{ 1.0f, 2.0f, 3.0f };
	Vector3 vector1{ 4.0f, 5.0f, 6.0f };

	Vector3 result = vector1 + vector0;

	REQUIRE(result.x == Approx(5.0f));
	REQUIRE(result.y == Approx(7.0f));
	REQUIRE(result.z == Approx(9.0f));
}

TEST_CASE("Vector3_CanAddVectors_StressTest", "[Math], [Vector3], [StressTest]")
{
	for (int i = 0; i < locStressTestCount; ++i)
	{
		float x0 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float y0 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float z0 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		float x1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float y1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float z1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		Vector3 vector0{ x0, y0, z0 };
		Vector3 vector1{ x1, y1, z1 };

		Vector3 result = vector1 + vector0;

		REQUIRE(result.x == Approx(x1 + x0));
		REQUIRE(result.y == Approx(y1 + y0));
		REQUIRE(result.z == Approx(z1 + z0));
	}
}

TEST_CASE("Vector3_CanSubtractVectors", "[Math], [Vector3]")
{
	Vector3 vector0{ 1.0f, 2.0f, 3.0f };
	Vector3 vector1{ 4.0f, 5.0f, 6.0f };

	Vector3 result = vector1 - vector0;

	REQUIRE(result.x == Approx(3.0f));
	REQUIRE(result.y == Approx(3.0f));
	REQUIRE(result.z == Approx(3.0f));
}

TEST_CASE("Vector3_CanSubtractVectors_StressTest", "[Math], [Vector3], [StressTest]")
{
	for (int i = 0; i < locStressTestCount; ++i)
	{
		float x0 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float y0 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float z0 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		float x1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float y1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float z1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		Vector3 vector0{ x0, y0, z0 };
		Vector3 vector1{ x1, y1, z1 };

		Vector3 result = vector1 - vector0;

		REQUIRE(result.x == Approx(x1 - x0));
		REQUIRE(result.y == Approx(y1 - y0));
		REQUIRE(result.z == Approx(z1 - z0));
	}
}

TEST_CASE("Vector3_CanMultiplyVectorByScalar", "[Math], [Vector3]")
{
	Vector3 vector{ 1.0f, 2.0f, 3.0f };
	float scalar = 4.0f;

	Vector3 result0 = vector * scalar;
	Vector3 result1 = scalar * vector;

	REQUIRE(result0.x == Approx(4.0f));
	REQUIRE(result0.y == Approx(8.0f));
	REQUIRE(result0.z == Approx(12.0f));
	REQUIRE(result0.x == Approx(result1.x));
	REQUIRE(result0.y == Approx(result1.y));
	REQUIRE(result0.z == Approx(result1.z));
}

TEST_CASE("Vector3_CanMultiplyVectorByScalar_StressTest", "[Math], [Vector3], [StressTest]")
{
	for (int i = 0; i < locStressTestCount; ++i)
	{
		float x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float scalar = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		Vector3 vector{ x, y, z };

		Vector3 result0 = vector * scalar;
		Vector3 result1 = scalar * vector;

		REQUIRE(result0.x == Approx(x * scalar));
		REQUIRE(result0.y == Approx(y * scalar));
		REQUIRE(result0.z == Approx(z * scalar));
		REQUIRE(result0.x == Approx(result1.x));
		REQUIRE(result0.y == Approx(result1.y));
		REQUIRE(result0.z == Approx(result1.z));
	}
}
