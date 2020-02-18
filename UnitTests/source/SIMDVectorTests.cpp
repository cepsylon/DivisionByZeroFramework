#include <catch/catch.hpp>

#include "Math/SIMDVector.h"

namespace
{
	static constexpr int locStressTestCount = 10000;
}

TEST_CASE("SIMDVector_CanBeLoadedWithAllValues", "[Common], [SIMDVector]")
{
	SIMD::Vector vector = SIMD::Load(1.0f, 2.0f, 3.0f, 4.0f);

	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(vector, 0) == 1.0f);
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(vector, 1) == 2.0f);
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(vector, 2) == 3.0f);
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(vector, 3) == 4.0f);
}

TEST_CASE("SIMDVector_CanBeLoadedWithAllValues_StressTest", "[Common], [SIMDVector], [StressTest]")
{
	for (int i = 0; i < locStressTestCount; ++i)
	{
		float x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float w = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		SIMD::Vector vector = SIMD::Load(x, y, z, w);

		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(vector, 0) == x);
		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(vector, 1) == y);
		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(vector, 2) == z);
		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(vector, 3) == w);
	}
}

TEST_CASE("SIMDVector_CanBeLoadedWithSingleValue", "[Common], [SIMDVector]")
{
	SIMD::Vector vector = SIMD::Load(1.0f);

	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(vector, 0) == 1.0f);
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(vector, 1) == 1.0f);
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(vector, 2) == 1.0f);
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(vector, 3) == 1.0f);
}

TEST_CASE("SIMDVector_CanBeLoadedWithSingleValue_StressTest", "[Common], [SIMDVector], [StressTest]")
{
	for (int i = 0; i < locStressTestCount; ++i)
	{
		float value = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		SIMD::Vector vector = SIMD::Load(value);

		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(vector, 0) == value);
		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(vector, 1) == value);
		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(vector, 2) == value);
		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(vector, 3) == value);
	}
}

TEST_CASE("SIMDVector_CanAddVectors", "[Common], [SIMDVector]")
{
	SIMD::Vector vector0 = SIMD::Load(1.0f, 2.0f, 3.0f, 4.0f);
	SIMD::Vector vector1 = SIMD::Load(4.0f, 3.0f, 2.0f, 1.0f);
	SIMD::Vector result = SIMD::Add(vector0, vector1);

	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 0) == Approx(5.0f));
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 1) == Approx(5.0f));
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 2) == Approx(5.0f));
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 3) == Approx(5.0f));
}

TEST_CASE("SIMDVector_CanAddVectors_StressTest", "[Common], [SIMDVector], [StressTest]")
{
	for (int i = 0; i < locStressTestCount; ++i)
	{
		float x0 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float y0 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float z0 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float w0 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		float x1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float y1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float z1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float w1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		SIMD::Vector vector0 = SIMD::Load(x0, y0, z0, w0);
		SIMD::Vector vector1 = SIMD::Load(x1, y1, z1, w1);
		SIMD::Vector result = SIMD::Add(vector0, vector1);

		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 0) == Approx(x0 + x1));
		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 1) == Approx(y0 + y1));
		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 2) == Approx(z0 + z1));
		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 3) == Approx(w0 + w1));
	}
}

TEST_CASE("SIMDVector_CanSubtractVectors", "[Common], [SIMDVector]")
{
	SIMD::Vector vector0 = SIMD::Load(1.0f, 2.0f, 3.0f, 4.0f);
	SIMD::Vector vector1 = SIMD::Load(4.0f, 3.0f, 2.0f, 1.0f);
	SIMD::Vector result = SIMD::Subtract(vector0, vector1);

	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 0) == Approx(-3.0f));
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 1) == Approx(-1.0f));
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 2) == Approx(1.0f));
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 3) == Approx(3.0f));
}

TEST_CASE("SIMDVector_CanSubtractVectors_StressTest", "[Common], [SIMDVector], [StressTest]")
{
	for (int i = 0; i < locStressTestCount; ++i)
	{
		float x0 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float y0 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float z0 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float w0 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		float x1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float y1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float z1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float w1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		SIMD::Vector vector0 = SIMD::Load(x0, y0, z0, w0);
		SIMD::Vector vector1 = SIMD::Load(x1, y1, z1, w1);
		SIMD::Vector result = SIMD::Subtract(vector0, vector1);

		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 0) == Approx(x0 - x1));
		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 1) == Approx(y0 - y1));
		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 2) == Approx(z0 - z1));
		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 3) == Approx(w0 - w1));
	}
}

TEST_CASE("SIMDVector_CanMultiplyVectors", "[Common], [SIMDVector]")
{
	SIMD::Vector vector0 = SIMD::Load(1.0f, 2.0f, 3.0f, 4.0f);
	SIMD::Vector vector1 = SIMD::Load(4.0f, 3.0f, 2.0f, 1.0f);
	SIMD::Vector result = SIMD::Multiply(vector0, vector1);

	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 0) == Approx(4.0f));
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 1) == Approx(6.0f));
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 2) == Approx(6.0f));
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 3) == Approx(4.0f));
}

TEST_CASE("SIMDVector_CanMultiplyVectors_StressTest", "[Common], [SIMDVector], [StressTest]")
{
	for (int i = 0; i < locStressTestCount; ++i)
	{
		float x0 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float y0 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float z0 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float w0 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		float x1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float y1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float z1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float w1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		SIMD::Vector vector0 = SIMD::Load(x0, y0, z0, w0);
		SIMD::Vector vector1 = SIMD::Load(x1, y1, z1, w1);
		SIMD::Vector result = SIMD::Multiply(vector0, vector1);

		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 0) == Approx(x0 * x1));
		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 1) == Approx(y0 * y1));
		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 2) == Approx(z0 * z1));
		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 3) == Approx(w0 * w1));
	}
}

TEST_CASE("SIMDVector_CanMultiplyVectorWithSingleValue", "[Common], [SIMDVector]")
{
	SIMD::Vector vector = SIMD::Load(1.0f, 2.0f, 3.0f, 4.0f);
	SIMD::Vector result = SIMD::Multiply(vector, 2.0f);

	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 0) == Approx(1.0f * 2.0f));
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 1) == Approx(2.0f * 2.0f));
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 2) == Approx(3.0f * 2.0f));
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 3) == Approx(4.0f * 2.0f));
}

TEST_CASE("SIMDVector_CanMultiplyVectorWithSingleValue_StressTest", "[Common], [SIMDVector], [StressTest]")
{
	for (int i = 0; i < locStressTestCount; ++i)
	{
		float x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float w = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		float multiplier = static_cast<float>(rand()) / static_cast<float>(RAND_MAX); // +1 so we don't divide by 0

		SIMD::Vector vector = SIMD::Load(x, y, z, w);
		SIMD::Vector result = SIMD::Multiply(vector, multiplier);

		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 0) == Approx(x * multiplier));
		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 1) == Approx(y * multiplier));
		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 2) == Approx(z * multiplier));
		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 3) == Approx(w * multiplier));
	}
}

TEST_CASE("SIMDVector_CanDivideVectors", "[Common], [SIMDVector]")
{
	SIMD::Vector vector0 = SIMD::Load(1.0f, 2.0f, 3.0f, 4.0f);
	SIMD::Vector vector1 = SIMD::Load(4.0f, 3.0f, 2.0f, 1.0f);
	SIMD::Vector result = SIMD::Divide(vector0, vector1);

	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 0) == Approx(1.0f / 4.0f));
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 1) == Approx(2.0f / 3.0f));
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 2) == Approx(3.0f / 2.0f));
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 3) == Approx(4.0f / 1.0f));
}

TEST_CASE("SIMDVector_CanDivideVectors_StressTest", "[Common], [SIMDVector], [StressTest]")
{
	for (int i = 0; i < locStressTestCount; ++i)
	{
		float x0 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float y0 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float z0 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float w0 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		float x1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) + 1.0f; // +1 so we do not divide by 0
		float y1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) + 1.0f;
		float z1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) + 1.0f;
		float w1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) + 1.0f;

		SIMD::Vector vector0 = SIMD::Load(x0, y0, z0, w0);
		SIMD::Vector vector1 = SIMD::Load(x1, y1, z1, w1);
		SIMD::Vector result = SIMD::Divide(vector0, vector1);

		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 0) == Approx(x0 / x1));
		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 1) == Approx(y0 / y1));
		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 2) == Approx(z0 / z1));
		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 3) == Approx(w0 / w1));
	}
}

TEST_CASE("SIMDVector_CanDivideVectorWithSingleValue", "[Common], [SIMDVector]")
{
	SIMD::Vector vector = SIMD::Load(1.0f, 2.0f, 3.0f, 4.0f);
	SIMD::Vector result = SIMD::Divide(vector, 2.0f);

	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 0) == Approx(1.0f / 2.0f));
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 1) == Approx(2.0f / 2.0f));
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 2) == Approx(3.0f / 2.0f));
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 3) == Approx(4.0f / 2.0f));
}

TEST_CASE("SIMDVector_CanDivideVectorWithSingleValue_StressTest", "[Common], [SIMDVector], [StressTest]")
{
	for (int i = 0; i < locStressTestCount; ++i)
	{
		float x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float w = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		float divisor = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) + 1.0f; // +1 so we don't divide by 0

		SIMD::Vector vector = SIMD::Load(x, y, z, w);
		SIMD::Vector result = SIMD::Divide(vector, divisor);

		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 0) == Approx(x / divisor));
		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 1) == Approx(y / divisor));
		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 2) == Approx(z / divisor));
		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 3) == Approx(w / divisor));
	}
}

TEST_CASE("SIMDVector_CanMultiplyAddVectors", "[Common], [SIMDVector]")
{
	SIMD::Vector vector0 = SIMD::Load(1.0f, 2.0f, 3.0f, 4.0f);
	SIMD::Vector vector1 = SIMD::Load(4.0f, 3.0f, 2.0f, 1.0f);
	SIMD::Vector vector2 = SIMD::Load(3.0f, 4.0f, 2.0f, 1.0f);
	SIMD::Vector result = SIMD::MultiplyAdd(vector0, vector1, vector2);

	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 0) == Approx(7.0f));
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 1) == Approx(10.0f));
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 2) == Approx(8.0f));
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 3) == Approx(5.0f));
}

TEST_CASE("SIMDVector_CanMultiplyAddVectors_StressTest", "[Common], [SIMDVector], [StressTest]")
{
	for (int i = 0; i < locStressTestCount; ++i)
	{
		float x0 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float y0 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float z0 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float w0 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		float x1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float y1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float z1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float w1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		float x2 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float y2 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float z2 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float w2 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		SIMD::Vector vector0 = SIMD::Load(x0, y0, z0, w0);
		SIMD::Vector vector1 = SIMD::Load(x1, y1, z1, w1);
		SIMD::Vector vector2 = SIMD::Load(x2, y2, z2, w2);
		SIMD::Vector result = SIMD::MultiplyAdd(vector0, vector1, vector2);

		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 0) == Approx((x0 * x1) + x2));
		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 1) == Approx((y0 * y1) + y2));
		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 2) == Approx((z0 * z1) + z2));
		REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 3) == Approx((w0 * w1) + w2));
	}
}

TEST_CASE("SIMDVector_CanBeShuffled", "[Common], [SIMDVector]")
{
	SIMD::Vector vector0 = SIMD::Load(1.0f, 2.0f, 3.0f, 4.0f);
	SIMD::Vector vector1 = SIMD::Load(5.0f, 6.0f, 7.0f, 8.0f);
	SIMD::Vector result = SIMD::Shuffle<3, 2, 1, 0>(vector0, vector1);

	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 0) == SIMD_VECTOR_INDEX_OPERATOR(vector0, 3));
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 1) == SIMD_VECTOR_INDEX_OPERATOR(vector0, 2));
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 2) == SIMD_VECTOR_INDEX_OPERATOR(vector1, 1));
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 3) == SIMD_VECTOR_INDEX_OPERATOR(vector1, 0));

	result = SIMD::Shuffle<0, 3, 2, 1>(vector0, vector1);

	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 0) == SIMD_VECTOR_INDEX_OPERATOR(vector0, 0));
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 1) == SIMD_VECTOR_INDEX_OPERATOR(vector0, 3));
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 2) == SIMD_VECTOR_INDEX_OPERATOR(vector1, 2));
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 3) == SIMD_VECTOR_INDEX_OPERATOR(vector1, 1));

	result = SIMD::Shuffle<3, 1, 0, 2>(vector0, vector1);

	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 0) == SIMD_VECTOR_INDEX_OPERATOR(vector0, 3));
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 1) == SIMD_VECTOR_INDEX_OPERATOR(vector0, 1));
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 2) == SIMD_VECTOR_INDEX_OPERATOR(vector1, 0));
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 3) == SIMD_VECTOR_INDEX_OPERATOR(vector1, 2));

	result = SIMD::Shuffle<0, 2, 1, 3>(vector0, vector1);

	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 0) == SIMD_VECTOR_INDEX_OPERATOR(vector0, 0));
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 1) == SIMD_VECTOR_INDEX_OPERATOR(vector0, 2));
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 2) == SIMD_VECTOR_INDEX_OPERATOR(vector1, 1));
	REQUIRE(SIMD_VECTOR_INDEX_OPERATOR(result, 3) == SIMD_VECTOR_INDEX_OPERATOR(vector1, 3));
}
