#ifndef __XLANG_PRIVATE_MESSAGECACHE_POOL_H
#define __XLANG_PRIVATE_MESSAGECACHE_POOL_H
#include "xbase/x_target.h"
#ifdef USE_PRAGMA_ONCE 
#pragma once 
#endif

#include "xlang/private/x_BasicTypes.h"
#include "xlang/private/Debug/x_Assert.h"

#include "xlang/x_AllocatorManager.h"
#include "xlang/x_Align.h"
#include "xlang/x_Defines.h"
#include "xlang/x_IAllocator.h"


namespace xlang
{
	namespace detail
	{
		/// A list of free memory blocks.
		class Pool
		{
		public:

			/// Constructor.
			inline Pool();

			/// Returns true if the pool contains no memory blocks.
			inline bool Empty() const;

			/// De-allocates all cached blocks in the pool.
			inline void Clear();

			/// Adds a memory block to the pool.
			inline bool Add(void *memory);

			/// Retreives a memory block from the pool with the given alignment.
			/// \return Zero if no suitable blocks in pool.
			inline void *FetchAligned(const u32 alignment);

			/// Retreives a memory block from the pool with any alignment.
			/// \return Zero if no blocks in pool.
			inline void *Fetch();

		private:

			/// A node representing a free memory block within the pool.
			/// Nodes are created in-place within the free blocks they represent.
			struct Node
			{
				XLANG_FORCEINLINE Node() : mNext(0)
				{
				}

				Node *mNext;                        ///< Pointer to next node in a list.
			};

			static const u32 MAX_BLOCKS = 16;  ///< Maximum number of memory blocks stored per pool.

			Node mHead;                             ///< Dummy node at head of a linked list of nodes in the pool.
			u32 mBlockCount;                   ///< Number of blocks currently cached in the pool.
		};


		XLANG_FORCEINLINE Pool::Pool() 
			: mHead()
			, mBlockCount(0)
		{
		}


		XLANG_FORCEINLINE bool Pool::Empty() const
		{
			XLANG_ASSERT((mBlockCount == 0 && mHead.mNext == 0) || (mBlockCount != 0 && mHead.mNext != 0));
			return (mBlockCount == 0);
		}


		XLANG_FORCEINLINE void Pool::Clear()
		{
			// Free all the blocks we have stashed away.
			Node *node(mHead.mNext);
			while (node)
			{
				Node *const next(node->mNext);
				AllocatorManager::Instance().GetAllocator()->Free(node);
				node = next;
			}

			mHead.mNext = 0;
			mBlockCount = 0;
		}


		XLANG_FORCEINLINE bool Pool::Add(void *const memory)
		{
			XLANG_ASSERT(memory);

			// Just call it a node and link it in.
			Node *const node(reinterpret_cast<Node *>(memory));

			// Below maximum block count limit?
			if (mBlockCount < MAX_BLOCKS)
			{
				node->mNext = mHead.mNext;
				mHead.mNext = node;
				++mBlockCount;
				return true;
			}

			return false;
		}


		XLANG_FORCEINLINE void *Pool::FetchAligned(const u32 alignment)
		{
			Node *previous(&mHead);
			const u32 alignmentMask(alignment - 1);

			// Search the block list.
			Node *node(mHead.mNext);
			while (node)
			{
				// Prefetch.
				Node *const next(node->mNext);

				// This is XLANG_ALIGNED with the alignment mask calculated outside the loop.
				if ((reinterpret_cast<uintptr_t>(node) & alignmentMask) == 0)
				{
					// Remove from list and return as block.
					previous->mNext = next;
					--mBlockCount;
					return reinterpret_cast<void *>(node);
				}

				previous = node;
				node = next;
			}

			// Zero result indicates no correctly aligned block available.
			return 0;
		}


		XLANG_FORCEINLINE void *Pool::Fetch()
		{
			// Grab first block in the list if the list isn't empty.
			Node *const node(mHead.mNext);
			if (node)
			{
				mHead.mNext = node->mNext;
				--mBlockCount;
				return reinterpret_cast<void *>(node);
			}

			// Zero result indicates no correctly aligned block available.
			return 0;
		}


	} // namespace detail
} // namespace xlang


#endif // __XLANG_PRIVATE_MESSAGECACHE_POOL_H

