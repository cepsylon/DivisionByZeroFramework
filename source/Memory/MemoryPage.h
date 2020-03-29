#pragma once

#include <stdint.h>

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

	MemoryPage(Block* aFreeBlock);

	Block* myFreeBlocks = nullptr;
	Block* myInUseBlocks = nullptr;
};

}
