#ifndef __XLANG_PRIVATE_PAGEDPOOL_PAGE_H
#define __XLANG_PRIVATE_PAGEDPOOL_PAGE_H
#include "xbase/x_target.h"
#ifdef USE_PRAGMA_ONCE 
#pragma once 
#endif

#include "xlang/private/x_BasicTypes.h"
#include "xlang/private/Debug/x_Assert.h"
#include "xlang/private/PagedPool/x_FreeList.h"

#include "xlang/x_AllocatorManager.h"
#include "xlang/x_IAllocator.h"
#include "xlang/x_Defines.h"


namespace xlang
{
	namespace detail
	{
		template <class Entry, u32 ENTRIES_PER_PAGE>
		class Page
		{
		public:

			inline Page() : mData(0)
			{
			}

			inline ~Page()
			{
				// We expect the page to be unused and hence released.
				XLANG_ASSERT(mData == 0);
			}

			/// Returns true if the page has been allocated by a successful call to Initialize.
			XLANG_FORCEINLINE bool IsInitialized() const
			{
				return (mData != 0);
			}

			/// Initializes the page, allocating its data buffer and marking all entries as free.
			/// \return True, if the page data buffer was successfully allocated.
			inline bool Initialize(FreeList &freeList)
			{
				// Allocate the page data buffer.
				IAllocator *const allocator(AllocatorManager::Instance().GetAllocator());
				mData = reinterpret_cast<Entry *>(allocator->Allocate(ENTRIES_PER_PAGE * sizeof(Entry)));

				if (mData == 0)
				{
					return false;
				}

				// Add all the entries in the page to the free list initially.
				// We add them at the front of the list in reverse order so the list starts at the low end.
				Entry *entry(mData + ENTRIES_PER_PAGE);
				while (--entry >= mData)
				{
					// Add the free entry to the freelist.
					freeList.Add(entry);
				}

				return true;
			}

			/// Releases the page, de-allocating its data buffer and clearing its free list.
			inline void Release(FreeList &freeList)
			{
				XLANG_ASSERT(mData);

				IAllocator *const allocator(AllocatorManager::Instance().GetAllocator());
				allocator->Free(mData);
				mData = 0;

				freeList.Clear();
			}

			/// Allocates a free entry and sets its index, returning true on success.
			XLANG_FORCEINLINE bool Allocate(FreeList &freeList, u32 &index)
			{
				void *const memory(freeList.Get());
				if (memory)
				{
					// Calculate the index of the entry from its address.
					Entry *const entry = reinterpret_cast<Entry *>(memory);
					index = static_cast<u32>(entry - mData);

					return true;
				}

				return false;
			}

			/// Frees a previously allocated entry.
			XLANG_FORCEINLINE bool Free(FreeList &freeList, const u32 index)
			{
				XLANG_ASSERT(mData);
				XLANG_ASSERT(index < ENTRIES_PER_PAGE);

				// Add the free entry to the freelist.
				Entry *const entry(mData + index);
				freeList.Add(entry);

				return true;
			}

			/// Gets a pointer to the entry at the given index.
			XLANG_FORCEINLINE void *GetEntry(const u32 index) const
			{
				XLANG_ASSERT(mData);
				XLANG_ASSERT(index < ENTRIES_PER_PAGE);

				return reinterpret_cast<void *>(mData + index);
			}

			/// Gets the index of the entry addressed by the given pointer.
			/// Returns ENTRIES_PER_PAGE if the pointer isn't the address of an entry in this.
			XLANG_FORCEINLINE u32 GetIndex(void *const ptr) const
			{
				XLANG_ASSERT(ptr);

				Entry *const entry(reinterpret_cast<Entry *>(ptr));
				if (entry < mData)
				{
					return ENTRIES_PER_PAGE;
				}

				if (entry >= mData + ENTRIES_PER_PAGE)
				{
					return ENTRIES_PER_PAGE;
				}

				return (entry - mData);
			}

		private:

			Entry *mData;           ///< A page is really just a pointer to an allocated buffer of entries.
		};


	} // namespace detail
} // namespace xlang


#endif // __XLANG_PRIVATE_PAGEDPOOL_PAGE_H

