#include "MemoryPage.h"

namespace dbz
{

MemoryPage MemoryPage::Create(uint32_t aSize, SelectionMethod aSelectionMethod)
{
	return MemoryPage{ new Block{ 0u, aSize, nullptr } };
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
	Block* iteratorParent = nullptr;
	Block* iterator = myFreeBlocks;
	while (iterator)
	{
		if (iterator->mySize >= aSize)
		{
			allocationOffset = iterator->myOffset;

			if (iterator->mySize == aSize)
			{
				Block** toUpdate = iteratorParent ? &iteratorParent->myNext : &myFreeBlocks;
				*toUpdate = iterator->myNext;
				iterator->myNext = myInUseBlocks;
				myInUseBlocks = iterator;
			}
			else
			{
				myInUseBlocks = new Block{ iterator->myOffset, aSize, myInUseBlocks };
				iterator->myOffset += aSize;
				iterator->mySize -= aSize;
			}

			break;
		}

		iteratorParent = iterator;
		iterator = iterator->myNext;
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
	iterator = myFreeBlocks;
	while (iterator)
	{
		if (iterator->myOffset + iterator->mySize == anOffset)
		{
			iterator->mySize += toFree->mySize;
			delete toFree;
			toFree = nullptr;
			break;
		}

		iterator = iterator->myNext;
	}

	// Could not merge blocks, so we just push it to the front
	if (toFree)
	{
		toFree->myNext = myFreeBlocks;
		myFreeBlocks = toFree;
	}

	return true;
}

MemoryPage::MemoryPage(Block* aFreeBlock)
	: myFreeBlocks(aFreeBlock)
	, myInUseBlocks(nullptr)
{ }

}
