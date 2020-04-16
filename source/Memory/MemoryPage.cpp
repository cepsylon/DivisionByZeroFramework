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

	return MemoryPage{ aSize, selectionMethod };
}

void MemoryPage::Destroy(MemoryPage& aMemoryPage)
{
	aMemoryPage.myFreeBlockIndex = UINT32_MAX;
	aMemoryPage.myInUseBlockIndex = UINT32_MAX;
	aMemoryPage.myUnusedBlockIndices = UINT32_MAX;
	aMemoryPage.myBlocks.clear();
}

uint32_t MemoryPage::Allocate(uint32_t aSize)
{
	uint32_t allocationOffset = UINT32_MAX;

	uint32_t parentIndex = UINT32_MAX;
	uint32_t index = (this->*mySelectionMethodFn)(aSize, parentIndex);
	if (index != UINT32_MAX)
	{
		allocationOffset = myBlocks[index].myOffset;

		if (myBlocks[index].mySize == aSize)
		{
			uint32_t* toUpdate = parentIndex != UINT32_MAX ? &myBlocks[parentIndex].myNext : &myFreeBlockIndex;
			*toUpdate = myBlocks[index].myNext;
			myBlocks[index].myNext = myInUseBlockIndex;
			myInUseBlockIndex = index;
		}
		else
		{
			// Update new block
			uint32_t blockIndex = GetUnusedBlockIndex();
			myBlocks[blockIndex].myOffset = myBlocks[index].myOffset;
			myBlocks[blockIndex].mySize = aSize;
			myBlocks[blockIndex].myNext = myInUseBlockIndex;

			// Update head of used blocks
			myInUseBlockIndex = blockIndex;

			// Update existing block
			myBlocks[index].myOffset += aSize;
			myBlocks[index].mySize -= aSize;
		}
	}

	return allocationOffset;
}

bool MemoryPage::Free(uint32_t anOffset)
{
	// Find block
	uint32_t toFreeIndex = UINT32_MAX;
	uint32_t parentIndex = UINT32_MAX;
	uint32_t index = myInUseBlockIndex;
	while (index != UINT32_MAX)
	{
		const Block& block = myBlocks[index];
		if (block.myOffset == anOffset)
		{
			toFreeIndex = index;
			uint32_t* toUpdateIndex = parentIndex != UINT32_MAX ? &myBlocks[parentIndex].myNext : &myInUseBlockIndex;
			*toUpdateIndex = block.myNext;
			break;
		}

		parentIndex = index;
		index = block.myNext;
	}

	// Requested offset does not belong to this page
	if (toFreeIndex == UINT32_MAX)
		return false;

	// Merge freed block with pre-existing free block if possible
	Block& blockToFree = myBlocks[toFreeIndex];
	uint32_t nextOffset = anOffset + blockToFree.mySize;
	uint32_t mergedBlockParentIndex = UINT32_MAX;
	parentIndex = UINT32_MAX;
	index = myFreeBlockIndex;
	bool mergedBlock = false;
	while (index != UINT32_MAX)
	{
		Block& block = myBlocks[index];

		// Merge to existing block
		if (block.myOffset + block.mySize == anOffset)
		{
			if (mergedBlock)
			{
				uint32_t* toUpdateIndex = mergedBlockParentIndex != UINT32_MAX ? &myBlocks[mergedBlockParentIndex].myNext : &myFreeBlockIndex;
				Block& blockToFree = myBlocks[*toUpdateIndex];
				block.mySize += blockToFree.mySize;
				*toUpdateIndex = blockToFree.myNext;

				// Release block
				blockToFree.myNext = myUnusedBlockIndices;
				myUnusedBlockIndices = *toUpdateIndex;

				break;
			}

			block.mySize += blockToFree.mySize;
			mergedBlock = true;
			mergedBlockParentIndex = parentIndex;
		}

		// Merge existing block to freed one
		if (block.myOffset == nextOffset)
		{
			if (mergedBlock)
			{
				uint32_t* toUpdateIndex = mergedBlockParentIndex != UINT32_MAX ? &myBlocks[mergedBlockParentIndex].myNext : &myFreeBlockIndex;
				Block& blockToFree = myBlocks[*toUpdateIndex];
				block.mySize += blockToFree.mySize;
				block.myOffset = blockToFree.myOffset;
				*toUpdateIndex = blockToFree.myNext;

				// Release block
				blockToFree.myNext = myUnusedBlockIndices;
				myUnusedBlockIndices = *toUpdateIndex;

				break;
			}

			block.myOffset -= blockToFree.mySize;
			block.mySize += blockToFree.mySize;
			mergedBlock = true;
			mergedBlockParentIndex = parentIndex;
		}

		parentIndex = index;
		index = block.myNext;
	}

	if (mergedBlock)
	{
		blockToFree.myNext = myUnusedBlockIndices;
		myUnusedBlockIndices = toFreeIndex;
	}
	else
	{
		// Could not merge blocks, so we just push it to the front
		blockToFree.myNext = myFreeBlockIndex;
		myFreeBlockIndex = toFreeIndex;
	}

	return true;
}

uint32_t MemoryPage::FirstFit(uint32_t aSize, uint32_t& aParentIndexOut) const
{
	uint32_t index = myFreeBlockIndex;
	uint32_t parentIndex = UINT32_MAX;

	while (index != UINT32_MAX)
	{
		const Block& block = myBlocks[index];
		if (block.mySize >= aSize)
			break;

		parentIndex = index;
		index = block.myNext;
	}

	aParentIndexOut = parentIndex;
	return index;
}

uint32_t MemoryPage::BestFit(uint32_t aSize, uint32_t& aParentIndexOut) const
{
	uint32_t minSpareMemory = UINT32_MAX;
	uint32_t bestBlockIndex = UINT32_MAX;
	uint32_t index = myFreeBlockIndex;
	uint32_t parentIndex = UINT32_MAX;

	while (index != UINT32_MAX)
	{
		const Block& block = myBlocks[index];
		if (block.mySize >= aSize && minSpareMemory > (block.mySize - aSize))
		{
			bestBlockIndex = index;
			aParentIndexOut = parentIndex;
			minSpareMemory = block.mySize - aSize;
		}

		parentIndex = index;
		index = block.myNext;
	}

	return bestBlockIndex;
}

uint32_t MemoryPage::GetUnusedBlockIndex()
{
	uint32_t index = myUnusedBlockIndices;
	if (myUnusedBlockIndices == UINT32_MAX)
	{
		index = myBlocks.size();
		myBlocks.emplace_back();
	}
	else
	{
		myUnusedBlockIndices = myBlocks[index].myNext;
	}

	return index;
}

MemoryPage::MemoryPage(uint32_t aSize, SelectionMethodFn aSelectionMethodFn)
	: mySelectionMethodFn(aSelectionMethodFn)
	, myFreeBlockIndex(0)
{
	myBlocks.emplace_back(Block{ 0, aSize, UINT32_MAX });
}

#if IS_DEVELOPMENT_BUILD

void MemoryPage::Print(std::ostream& anOutputStream) const
{
	anOutputStream << "In use blocks:\n";
	anOutputStream << "Head";
	uint32_t index = myInUseBlockIndex;
	while (index != UINT32_MAX)
	{
		anOutputStream << " -> [Offset: " << myBlocks[index].myOffset << ", Size: " << myBlocks[index].mySize << "]";
		index = myBlocks[index].myNext;
	}
	anOutputStream << " -> nullptr\n";

	anOutputStream << "Free blocks:\n";
	anOutputStream << "Head";
	index = myFreeBlockIndex;
	while (index != UINT32_MAX)
	{
		anOutputStream << " -> [Offset: " << myBlocks[index].myOffset << ", Size: " << myBlocks[index].mySize << "]";
		index = myBlocks[index].myNext;
	}
	anOutputStream << " -> nullptr\n";
}

#endif // IS_DEBUG_BUILD

}
