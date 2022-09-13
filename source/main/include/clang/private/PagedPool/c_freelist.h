#ifndef __XLANG_PRIVATE_PAGEDPOOL_FREELIST_H
#define __XLANG_PRIVATE_PAGEDPOOL_FREELIST_H
#include "cbase/c_target.h"
#ifdef USE_PRAGMA_ONCE 
#pragma once 
#endif

#include "clang/private/c_BasicTypes.h"
#include "clang/private/Debug/c_Assert.h"

#include "clang/c_Defines.h"

namespace clang
{
	namespace detail
	{
		class FreeList
		{
		public:
			inline FreeList() : mHead(0), mCount(0)
			{
			}

			/// Returns the number of entries currently on this free list.
			XLANG_FORCEINLINE u32 Count() const
			{
				return mCount;
			}

			/// Empties the free list, forgetting all of its entries.
			XLANG_FORCEINLINE void Clear()
			{
				mHead = 0;
				mCount = 0;
			}

			/// Allocates an entry from the free list, if it contains any.
			XLANG_FORCEINLINE void *Get()
			{
				if (mHead)
				{
					XLANG_ASSERT(mCount > 0);

					FreeNode *const node(mHead);
					mHead = mHead->mNext;
					--mCount;

					return node;
				}

				return 0;
			}

			/// Returns an entry to the free list.
			XLANG_FORCEINLINE void Add(void *const entry)
			{
				FreeNode *const node(reinterpret_cast<FreeNode *>(entry));

				node->mNext = mHead;
				mHead = node;
				++mCount;
			}

		private:

			/// Node in a list free of free entries within a block.
			struct FreeNode
			{
				FreeNode *mNext;        ///< Pointer to the next node in the free list.
			};

			FreeNode *mHead;            ///< Pointer to the first in a singly-linked list of free entries.
			u32 mCount;            ///< Number of entries on this free list.
		};


	} // namespace detail
} // namespace clang


#endif // __XLANG_PRIVATE_PAGEDPOOL_FREELIST_H

