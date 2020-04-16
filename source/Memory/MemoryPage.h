#pragma once

#include "GlobalDefines.h"

#include <stdint.h>
#include <vector>

#if IS_DEVELOPMENT_BUILD
#include <iostream>
#endif // IS_DEVELOPMENT_BUILD

namespace dbz
{

class MemoryPage
{
public:
	enum class SelectionMethod
	{
		FIRST_FIT = 0,
		BEST_FIT
	};

	static MemoryPage Create(uint32_t aSize, SelectionMethod aSelectionMethod = SelectionMethod::FIRST_FIT);
	static void Destroy(MemoryPage& aPage);

	// Dummy constructor, does nothing
	MemoryPage() = default;

	uint32_t Allocate(uint32_t aSize);
	bool Free(uint32_t anOffset);

private:
	struct Block
	{
		uint32_t myOffset = 0u;
		uint32_t mySize = 0u;
		uint32_t myNext = UINT32_MAX;
	};

	uint32_t FirstFit(uint32_t aSize, uint32_t& aParentNodeOut) const;
	uint32_t BestFit(uint32_t aSize, uint32_t& aParentNodeOut) const;

	uint32_t GetUnusedBlockIndex();

	using SelectionMethodFn = uint32_t (MemoryPage::*)(uint32_t, uint32_t&) const;
	MemoryPage(uint32_t aSize, SelectionMethodFn aSelectionMethod);

	SelectionMethodFn mySelectionMethodFn = nullptr;
	std::vector<Block> myBlocks;
	uint32_t myFreeBlockIndex = UINT32_MAX;
	uint32_t myInUseBlockIndex = UINT32_MAX;
	uint32_t myUnusedBlockIndices = UINT32_MAX;

#if IS_DEVELOPMENT_BUILD
public:
	void Print(std::ostream& anOutputStream) const;
#endif // IS_DEVELOPMENT_BUILD
};

}
