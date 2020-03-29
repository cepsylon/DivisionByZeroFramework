#include <catch/catch.hpp>

#include "Memory/MemoryPage.h"

#define DBZ_KB (1 << 10)
#define DBZ_MB (1 << 20)
#define DBZ_GB (1 << 30)

TEST_CASE("MemoryPage_PageLifeTimeManagedThroughStaticFunctions", "[Memory], [MemoryPage]")
{
	dbz::MemoryPage page = dbz::MemoryPage::Create(DBZ_MB);
	dbz::MemoryPage::Destroy(page);
}

TEST_CASE("MemoryPage_CanAllocateMemory", "[Memory], [MemoryPage]")
{
	dbz::MemoryPage page = dbz::MemoryPage::Create(DBZ_MB);

	uint32_t allocationOffset = page.Allocate(DBZ_KB);
	REQUIRE(allocationOffset == 0u);

	allocationOffset = page.Allocate(DBZ_KB);
	REQUIRE(allocationOffset == DBZ_KB);

	dbz::MemoryPage::Destroy(page);
}

TEST_CASE("MemoryPage_CanFreeMemory", "[Memory], [MemoryPage]")
{
	dbz::MemoryPage page = dbz::MemoryPage::Create(DBZ_MB);

	uint32_t allocationOffset = page.Allocate(DBZ_KB);
	REQUIRE(allocationOffset == 0u);
	page.Free(allocationOffset);

	allocationOffset = page.Allocate(DBZ_KB);
	REQUIRE(allocationOffset == 0u);

	dbz::MemoryPage::Destroy(page);
}

TEST_CASE("MemoryPage_", "[Memory], [MemoryPage], [StressTest]")
{

}

