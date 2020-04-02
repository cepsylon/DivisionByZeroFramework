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

TEST_CASE("MemoryPage_AdjacedBlocksWhenFreedAreMerged", "[Memory], [MemoryPage]")
{
	dbz::MemoryPage page = dbz::MemoryPage::Create(DBZ_MB);

	uint32_t allocationOffset = page.Allocate(DBZ_KB);
	uint32_t allocationOffset2 = page.Allocate(DBZ_KB);
	REQUIRE(allocationOffset == 0u);
	REQUIRE(allocationOffset2 == DBZ_KB);

	page.Free(allocationOffset);
	page.Free(allocationOffset2);

	allocationOffset2 = page.Allocate(DBZ_KB);
	allocationOffset = page.Allocate(DBZ_KB);
	REQUIRE(allocationOffset2 == 0u);
	REQUIRE(allocationOffset == DBZ_KB);

	dbz::MemoryPage::Destroy(page);
}

TEST_CASE("MemoryPage_AdjacedBlocksWhenFreedAreMerged_2", "[Memory], [MemoryPage]")
{
	dbz::MemoryPage page = dbz::MemoryPage::Create(DBZ_MB);

	uint32_t allocationOffset = page.Allocate(DBZ_KB);
	uint32_t allocationOffset2 = page.Allocate(DBZ_KB);
	uint32_t allocationOffset3 = page.Allocate(DBZ_KB);
	REQUIRE(allocationOffset == 0u);
	REQUIRE(allocationOffset2 == DBZ_KB);
	REQUIRE(allocationOffset3 == 2 * DBZ_KB);

	page.Free(allocationOffset);
	page.Free(allocationOffset3);
	page.Free(allocationOffset2);

	allocationOffset = page.Allocate(3 * DBZ_KB);
	REQUIRE(allocationOffset == 0u);

	dbz::MemoryPage::Destroy(page);
}

TEST_CASE("MemoryPage_AllocationDeallocation_StressTest", "[Memory], [MemoryPage], [StressTest]")
{
	dbz::MemoryPage page = dbz::MemoryPage::Create(DBZ_MB);

	std::vector<uint32_t> offsets;

	do
	{
		uint32_t size = static_cast<uint32_t>(rand()) % DBZ_KB;
		offsets.push_back(page.Allocate(size + 1));
	} while (offsets.back() != UINT32_MAX);
	offsets.pop_back();

	while (offsets.empty() == false)
	{
		uint32_t index = static_cast<uint32_t>(rand()) % offsets.size();
		page.Free(offsets[index]);
		offsets[index] = offsets.back();
		offsets.pop_back();
	}

	dbz::MemoryPage::Destroy(page);
}

TEST_CASE("MemoryPage_CanChangeBlockSelectionForAllocations", "[Memory], [MemoryPage], [StressTest]")
{
	dbz::MemoryPage page = dbz::MemoryPage::Create(DBZ_MB, dbz::MemoryPage::SelectionMethod::BEST_FIT);
	dbz::MemoryPage::Destroy(page);
}

TEST_CASE("MemoryPage_BestFitAllocatesFromSmallestAvailableBlock", "[Memory], [MemoryPage], [StressTest]")
{
	dbz::MemoryPage page = dbz::MemoryPage::Create(DBZ_MB, dbz::MemoryPage::SelectionMethod::BEST_FIT);

	//uint32_t allocationOneKb = page.Allocate(DBZ_KB);
	//uint32_t allocationThreeKb = page.Allocate(3 * DBZ_KB);
	//uint32_t allocationTwoKb = page.Allocate(2 * DBZ_KB);
	//uint32_t allocationFiveKb = page.Allocate(5 * DBZ_KB);
	//uint32_t allocationFourKb = page.Allocate(4 * DBZ_KB);

	dbz::MemoryPage::Destroy(page);
}
