#include <catch/catch.hpp>

#include "Math/Matrix44.h"

namespace
{
	static constexpr int locStressTestCount = 10000;
}

TEST_CASE("Matrix44_CanMultiplyMatrices", "[Math], [Matrix44]")
{
	Matrix44 matrix0{ 1.0f, 2.0f, 3.0f, 4.0f,
										5.0f, 6.0f, 7.0f, 8.0f,
										9.0f, 10.0f, 11.0f, 12.0f,
										13.0f, 14.0f, 15.0f, 16.0f };

	Matrix44 matrix1{ 17.0f, 18.0f, 19.0f, 20.0f,
										21.0f, 22.0f, 23.0f, 24.0f,
										25.0f, 26.0f, 27.0f, 28.0f,
										29.0f, 30.0f, 31.0f, 32.0f };
	Matrix44 result = matrix1 * matrix0;

	REQUIRE(result.myXAxis.x == Approx(250.0f));
	REQUIRE(result.myXAxis.y == Approx(260.0f));
	REQUIRE(result.myXAxis.z == Approx(270.0f));
	REQUIRE(result.myXAxis.w == Approx(280.0f));

	REQUIRE(result.myYAxis.x == Approx(618.0f));
	REQUIRE(result.myYAxis.y == Approx(644.0f));
	REQUIRE(result.myYAxis.z == Approx(670.0f));
	REQUIRE(result.myYAxis.w == Approx(696.0f));

	REQUIRE(result.myZAxis.x == Approx(986.0f));
	REQUIRE(result.myZAxis.y == Approx(1028.0f));
	REQUIRE(result.myZAxis.z == Approx(1070.0f));
	REQUIRE(result.myZAxis.w == Approx(1112.0f));

	REQUIRE(result.myPosition.x == Approx(1354.0f));
	REQUIRE(result.myPosition.y == Approx(1412.0f));
	REQUIRE(result.myPosition.z == Approx(1470.0f));
	REQUIRE(result.myPosition.w == Approx(1528.0f));
}

TEST_CASE("Matrix44_CanMultiplyMatrices_StressTest", "[Math], [Matrix44], [StressTest]")
{
	for (int i = 0; i < locStressTestCount; ++i)
	{
		float a00 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float a01 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float a02 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float a03 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		float a10 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float a11 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float a12 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float a13 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		float a20 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float a21 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float a22 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float a23 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		float a30 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float a31 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float a32 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float a33 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		float b00 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float b01 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float b02 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float b03 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		float b10 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float b11 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float b12 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float b13 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		float b20 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float b21 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float b22 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float b23 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		float b30 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float b31 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float b32 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float b33 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		Matrix44 matrix0{ a00, a01, a02, a03,
											a10, a11, a12, a13,
											a20, a21, a22, a23,
											a30, a31, a32, a32 };

		Matrix44 matrix1{ b00, b01, b02, b03,
											b10, b11, b12, b13,
											b20, b21, b22, b23,
											b30, b31, b32, b32 };
		Matrix44 result = matrix1 * matrix0;

		auto computeCoefficient = [&](int aRow, int aCol) -> float
		{
			return matrix0[aRow][0] * matrix1[0][aCol] + matrix0[aRow][1] * matrix1[1][aCol] + matrix0[aRow][2] * matrix1[2][aCol] + matrix0[aRow][3] * matrix1[3][aCol];
		};

		REQUIRE(result.myXAxis.x == Approx(computeCoefficient(0, 0)));
		REQUIRE(result.myXAxis.y == Approx(computeCoefficient(0, 1)));
		REQUIRE(result.myXAxis.z == Approx(computeCoefficient(0, 2)));
		REQUIRE(result.myXAxis.w == Approx(computeCoefficient(0, 3)));

		REQUIRE(result.myYAxis.x == Approx(computeCoefficient(1, 0)));
		REQUIRE(result.myYAxis.y == Approx(computeCoefficient(1, 1)));
		REQUIRE(result.myYAxis.z == Approx(computeCoefficient(1, 2)));
		REQUIRE(result.myYAxis.w == Approx(computeCoefficient(1, 3)));

		REQUIRE(result.myZAxis.x == Approx(computeCoefficient(2, 0)));
		REQUIRE(result.myZAxis.y == Approx(computeCoefficient(2, 1)));
		REQUIRE(result.myZAxis.z == Approx(computeCoefficient(2, 2)));
		REQUIRE(result.myZAxis.w == Approx(computeCoefficient(2, 3)));

		REQUIRE(result.myPosition.x == Approx(computeCoefficient(3, 0)));
		REQUIRE(result.myPosition.y == Approx(computeCoefficient(3, 1)));
		REQUIRE(result.myPosition.z == Approx(computeCoefficient(3, 2)));
		REQUIRE(result.myPosition.w == Approx(computeCoefficient(3, 3)));
	}
}

TEST_CASE("Matrix44_CanMultiplyWithVector", "[Math], [Matrix44]")
{
	Matrix44 matrix{ 1.0f, 2.0f, 3.0f, 4.0f,
									 5.0f, 6.0f, 7.0f, 8.0f,
									 9.0f, 10.0f, 11.0f, 12.0f,
									 13.0f, 14.0f, 15.0f, 16.0f };

	Vector4 vector{ 17.0f, 18.0f, 19.0f, 20.0f };
	Vector4 result = matrix * vector;

	REQUIRE(result.x == Approx(538.0f));
	REQUIRE(result.y == Approx(612.0f));
	REQUIRE(result.z == Approx(686.0f));
	REQUIRE(result.w == Approx(760.0f));
}

TEST_CASE("Matrix44_CanMultiplyWithVector_StressTest", "[Math], [Matrix44], [StressTest]")
{
	for (int i = 0; i < locStressTestCount; ++i)
	{
		float a00 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float a01 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float a02 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float a03 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		float a10 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float a11 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float a12 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float a13 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		float a20 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float a21 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float a22 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float a23 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		float a30 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float a31 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float a32 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float a33 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		float x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float w = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		Matrix44 matrix{ a00, a01, a02, a03,
										 a10, a11, a12, a13,
										 a20, a21, a22, a23,
										 a30, a31, a32, a32 };

		Vector4 vector{ x, y, z, w };

		Vector4 result = matrix * vector;

		auto computeCoefficient = [&](int aColumn) -> float
		{
			return matrix[0][aColumn] * vector[0] + matrix[1][aColumn] * vector[1] + matrix[2][aColumn] * vector[2] + matrix[3][aColumn] * vector[3];
		};

		REQUIRE(result.x == Approx(computeCoefficient(0)));
		REQUIRE(result.y == Approx(computeCoefficient(1)));
		REQUIRE(result.z == Approx(computeCoefficient(2)));
		REQUIRE(result.w == Approx(computeCoefficient(3)));
	}
}
