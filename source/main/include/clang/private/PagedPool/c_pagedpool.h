#ifndef __XLANG_PRIVATE_PAGEDPOOL_PAGEDPOOL_H
#define __XLANG_PRIVATE_PAGEDPOOL_PAGEDPOOL_H
#include "xbase/x_target.h"
#ifdef USE_PRAGMA_ONCE 
#pragma once 
#endif

#include "xlang/private/x_BasicTypes.h"
#include "xlang/private/Debug/x_Assert.h"
#include "xlang/private/PagedPool/x_FreeList.h"
#include "xlang/private/PagedPool/x_Page.h"

#include "xlang/x_AllocatorManager.h"
#include "xlang/x_IAllocator.h"
#include "xlang/x_Defines.h"


namespace xlang
{
	namespace detail
	{
		/// A growable pool in which objects can be allocated.
		template <class Entry, u32 MAX_ENTRIES>
		class PagedPool
		{
		public:

			/// Default Constructor.
			inline PagedPool() : mEntryCount(0), mMaxPageIndex(0)
			{
			}

			/// Returns the number of allocated entries.
			XLANG_FORCEINLINE u32 Count() const
			{
				return mEntryCount;
			}

			/// Allocates an entity in the pool and returns its unique index.
			XLANG_FORCEINLINE bool Allocate(u32 &index)
			{
				u32 pageIndex(0);
				u32 entryIndex(0);

				if (mEntryCount < MAX_ENTRIES)
				{
					// Check each initialized page in turn for a free entry.
					while (pageIndex < MAX_PAGES)
					{
						FreeList &freeList(mFreeLists[pageIndex]);
						PageType &page(mPageTable[pageIndex]);

						if (!page.IsInitialized())
						{
							break;
						}

						if (page.Allocate(freeList, entryIndex))
						{
							index = Index(pageIndex, entryIndex);
							++mEntryCount;

							// Update the maximum page index.
							if (pageIndex > mMaxPageIndex)
							{
								mMaxPageIndex = pageIndex;
							}

							return true;
						}

						++pageIndex;
					}

					// All allocated pages are full. Initialize the first uninitialized page.
					if (pageIndex < MAX_PAGES)
					{
						FreeList &freeList(mFreeLists[pageIndex]);
						PageType &page(mPageTable[pageIndex]);

						if (page.Initialize(freeList))
						{
							if (page.Allocate(freeList, entryIndex))
							{
								index = Index(pageIndex, entryIndex);
								++mEntryCount;
								return true;
							}
						}
					}
				}

				return false;
			}

			/// Frees the entry at the given index and returns its memory to the pool.
			XLANG_FORCEINLINE bool Free(const u32 index)
			{
				const u32 pageIndex(PageIndex(index));
				const u32 entryIndex(EntryIndex(index));

				XLANG_ASSERT(pageIndex < MAX_PAGES);

				// Since we're being asked to free the memory the page should exist!
				FreeList &freeList(mFreeLists[pageIndex]);
				PageType &page(mPageTable[pageIndex]);

				XLANG_ASSERT(page.IsInitialized());
				if (page.Free(freeList, entryIndex))
				{
					--mEntryCount;

					// If the page has become unused then deallocate it.
					if (freeList.Count() == ENTRIES_PER_PAGE)
					{
						page.Release(freeList);
					}

					return true;
				}

				return false;
			}

			/// Gets a pointer to the entry at the given index.
			XLANG_FORCEINLINE void *GetEntry(const u32 index) const
			{
				const u32 pageIndex(PageIndex(index));
				const u32 entryIndex(EntryIndex(index));

				XLANG_ASSERT(pageIndex < MAX_PAGES);

				// If the address is stale then the page may not even exist any more.
				const PageType &page(mPageTable[pageIndex]);
				if (page.IsInitialized())
				{
					return page.GetEntry(entryIndex);
				}

				return 0;
			}

			/// Gets the index of the entry addressed by the given pointer.
			/// Returns MAX_ENTRIES if the entry is not found.
			XLANG_FORCEINLINE u32 GetIndex(void *const entry) const
			{
				XLANG_ASSERT(entry);

				// Search all pages that have ever been allocated for one which contains the entry.
				u32 pageIndex(0);
				while (pageIndex <= mMaxPageIndex)
				{
					const PageType &page(mPageTable[pageIndex]);
					if (page.IsInitialized())
					{
						const u32 entryIndex(page.GetIndex(entry));
						if (entryIndex < ENTRIES_PER_PAGE)
						{
							return Index(pageIndex, entryIndex);
						}
					}

					++pageIndex;
				}

				return MAX_ENTRIES;
			}

		private:

			static const u32 ENTRIES_PER_PAGE = 64;
			static const u32 MAX_PAGES = (MAX_ENTRIES + ENTRIES_PER_PAGE - 1) / ENTRIES_PER_PAGE;
			static const u32 ENTRY_INDEX_MASK = ENTRIES_PER_PAGE - 1;
			static const u32 PAGE_INDEX_MASK = ~ENTRY_INDEX_MASK;
			static const u32 PAGE_INDEX_SHIFT = 6;

			typedef Page<Entry, ENTRIES_PER_PAGE> PageType;

			XLANG_FORCEINLINE static u32 PageIndex(const u32 index)
			{
				return ((index & PAGE_INDEX_MASK) >> PAGE_INDEX_SHIFT);
			}

			XLANG_FORCEINLINE static u32 EntryIndex(const u32 index)
			{
				return (index & ENTRY_INDEX_MASK);
			}

			XLANG_FORCEINLINE static u32 Index(const u32 pageIndex, const u32 entryIndex)
			{
				return ((pageIndex << PAGE_INDEX_SHIFT) | entryIndex);
			}

			PagedPool(const PagedPool &other);
			PagedPool &operator=(const PagedPool &other);

			PageType mPageTable[MAX_PAGES];     ///< Table of allocated pages, each of which is basically a pointer to a buffer.
			FreeList mFreeLists[MAX_PAGES];     ///< Each page has its own dedicated list of free entries, so we can favour low-index pages.
			u32 mEntryCount;               ///< Number of allocated entries in the entire pool.
			u32 mMaxPageIndex;             ///< Maximum index of any allocated page.
		};


	} // namespace detail
} // namespace xlang


#endif // __XLANG_PRIVATE_PAGEDPOOL_PAGEDPOOL_H

