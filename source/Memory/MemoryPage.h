#pragma once

#include "GlobalDefines.h"

#include <stdint.h>

#if IS_DEVELOPMENT_BUILD
#include <iostream>
#endif // IS_DEBUG_BUILD

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
		Block* myNext = nullptr;
	};

	Block* FirstFit(uint32_t aSize, Block*& aParentNodeOut) const;
	Block* BestFit(uint32_t aSize, Block*& aParentNodeOut) const;

	using SelectionMethodFn = Block* (MemoryPage::*)(uint32_t, Block*&) const;
	MemoryPage(Block* aFreeBlock, SelectionMethodFn aSelectionMethod);

	Block* myFreeBlocks = nullptr;
	Block* myInUseBlocks = nullptr;
	SelectionMethodFn mySelectionMethodFn = nullptr;

#if IS_DEVELOPMENT_BUILD
public:
	void Print(std::ostream& anOutputStream) const;
#endif // IS_DEBUG_BUILD
};

}
