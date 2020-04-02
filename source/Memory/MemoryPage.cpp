#include "MemoryPage.h"

namespace dbz
{

MemoryPage MemoryPage::Create(uint32_t aSize, SelectionMethod aSelectionMethod)
{
	SelectionMethodFn selectionMethod = nullptr;

	switch (aSelectionMethod)
	{
	case SelectionMethod::FIRST_FIT:
		selectionMethod = &FirstFit;
		break;
	case SelectionMethod::BEST_FIT:
		selectionMethod = &BestFit;
		break;
	}

	return MemoryPage{ new Block{ 0u, aSize, nullptr }, selectionMethod };
}

void MemoryPage::Destroy(MemoryPage& aMemoryPage)
{
	Block* iterator = aMemoryPage.myFreeBlocks;
	aMemoryPage.myFreeBlocks = nullptr;
	while (iterator)
	{
		Block* next = iterator->myNext;
		delete iterator;
		iterator = next;
	}

	iterator = aMemoryPage.myInUseBlocks;
	aMemoryPage.myInUseBlocks = nullptr;
	while (iterator)
	{
		Block* next = iterator->myNext;
		delete iterator;
		iterator = next;
	}
}

uint32_t MemoryPage::Allocate(uint32_t aSize)
{
	uint32_t allocationOffset = UINT32_MAX;

	Block* parentNode = nullptr;
	if (Block* allocateFromBlock = (this->*mySelectionMethodFn)(aSize, parentNode))
	{
		allocationOffset = allocateFromBlock->myOffset;

		if (allocateFromBlock->mySize == aSize)
		{
			Block** toUpdate = parentNode ? &parentNode->myNext : &myFreeBlocks;
			*toUpdate = allocateFromBlock->myNext;
			allocateFromBlock->myNext = myInUseBlocks;
			myInUseBlocks = allocateFromBlock;
		}
		else
		{
			myInUseBlocks = new Block{ allocateFromBlock->myOffset, aSize, myInUseBlocks };
			allocateFromBlock->myOffset += aSize;
			allocateFromBlock->mySize -= aSize;
		}
	}

	return allocationOffset;
}

bool MemoryPage::Free(uint32_t anOffset)
{
	// Find block
	Block* toFree = nullptr;
	Block* iteratorParent = nullptr;
	Block* iterator = myInUseBlocks;
	while (iterator)
	{
		if (iterator->myOffset == anOffset)
		{
			toFree = iterator;
			Block** toUpdate = iteratorParent ? &iteratorParent->myNext : &myInUseBlocks;
			*toUpdate = iterator->myNext;

			break;
		}

		iteratorParent = iterator;
		iterator = iterator->myNext;
	}

	// Requested offset does not belong to this page
	if (toFree == nullptr)
		return false;

	// Merge freed block with pre-existing free block if possible
	uint32_t nextOffset = anOffset + toFree->mySize;
	Block* toMergeNodeParent = nullptr;
	iteratorParent = nullptr;
	iterator = myFreeBlocks;
	bool mergedBlock = false;
	while (iterator)
	{
		// Merge to existing block
		if (iterator->myOffset + iterator->mySize == anOffset)
		{
			if (mergedBlock)
			{
				Block** toUpdate = toMergeNodeParent ? &toMergeNodeParent->myNext : &myFreeBlocks;
				Block* blockToFree = *toUpdate;
				iterator->mySize += blockToFree->mySize;
				*toUpdate = blockToFree->myNext;
				delete blockToFree;

				break;
			}

			iterator->mySize += toFree->mySize;
			mergedBlock = true;
			toMergeNodeParent = iteratorParent;
		}

		// Merge existing block to freed one
		if (iterator->myOffset == nextOffset)
		{
			if (mergedBlock)
			{
				Block** toUpdate = toMergeNodeParent ? &toMergeNodeParent->myNext : &myFreeBlocks;
				Block* blockToFree = *toUpdate;
				*toUpdate = blockToFree->myNext;
				iterator->mySize += blockToFree->mySize;
				iterator->myOffset = blockToFree->myOffset;
				delete blockToFree;

				break;
			}

			iterator->myOffset -= toFree->mySize;
			iterator->mySize += toFree->mySize;
			mergedBlock = true;
			toMergeNodeParent = iteratorParent;
		}

		iteratorParent = iterator;
		iterator = iterator->myNext;
	}

	if (mergedBlock)
	{
		delete toFree;
	}
	else
	{
		// Could not merge blocks, so we just push it to the front
		toFree->myNext = myFreeBlocks;
		myFreeBlocks = toFree;
	}

	return true;
}

MemoryPage::Block* MemoryPage::FirstFit(uint32_t aSize, Block*& aParentNodeOut) const
{
	Block* iterator = myFreeBlocks;
	Block* parentIterator = nullptr;

	while (iterator)
	{
		if (iterator->mySize >= aSize)
			break;

		parentIterator = iterator;
		iterator = iterator->myNext;
	}

	aParentNodeOut = parentIterator;
	return iterator;
}

MemoryPage::Block* MemoryPage::BestFit(uint32_t aSize, Block*& aParentNodeOut) const
{
	uint32_t minSpareMemory = UINT32_MAX;
	Block* bestBlock = nullptr;
	Block* iterator = myFreeBlocks;
	Block* parentIterator = nullptr;

	while (iterator)
	{
		if (iterator->mySize >= aSize && minSpareMemory > (iterator->mySize - aSize))
		{
			bestBlock = iterator;
			minSpareMemory = iterator->mySize - aSize;
		}

		iterator = iterator->myNext;
	}

	aParentNodeOut = parentIterator;
	return bestBlock;
}

MemoryPage::MemoryPage(Block* aFreeBlock, SelectionMethodFn aSelectionMethodFn)
	: myFreeBlocks(aFreeBlock)
	, myInUseBlocks(nullptr)
	, mySelectionMethodFn(aSelectionMethodFn)
{ }

#if IS_DEVELOPMENT_BUILD

void MemoryPage::Print(std::ostream& anOutputStream) const
{
	anOutputStream << "In use blocks:\n";
	anOutputStream << "Head";
	Block* iterator = myInUseBlocks;
	while (iterator)
	{
		anOutputStream << " -> [Offset: " << iterator->myOffset << ", Size: " << iterator->mySize << "]";
		iterator = iterator->myNext;
	}
	anOutputStream << " -> nullptr\n";

	anOutputStream << "Free blocks:\n";
	anOutputStream << "Head";
	iterator = myFreeBlocks;
	while (iterator)
	{
		anOutputStream << " -> [Offset: " << iterator->myOffset << ", Size: " << iterator->mySize << "]";
		iterator = iterator->myNext;
	}
	anOutputStream << " -> nullptr\n";
}

#endif // IS_DEBUG_BUILD

}
