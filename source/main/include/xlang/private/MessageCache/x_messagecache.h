#ifndef __XLANG_PRIVATE_MESSAGECACHE_MESSAGECACHE_H
#define __XLANG_PRIVATE_MESSAGECACHE_MESSAGECACHE_H
#include "xbase/x_target.h"
#ifdef USE_PRAGMA_ONCE 
#pragma once 
#endif

#include "xlang/private/x_BasicTypes.h"
#include "xlang/private/Debug/x_Assert.h"
#include "xlang/private/MessageCache/x_Pool.h"
#include "xlang/private/Threading/x_Lock.h"
#include "xlang/private/Threading/x_Mutex.h"

#include "xlang/x_AllocatorManager.h"
#include "xlang/x_Defines.h"


namespace xlang
{
	namespace detail
	{
		// A global cache of free message memory blocks of different sizes.
		class MessageCache
		{
		public:
			static const u32 MessageAlignment = sizeof(void*);

			/// Gets a reference to the single global instance.
			inline static MessageCache &Instance();

			/// Default constructor.
			inline MessageCache();

			/// Destructor.
			inline ~MessageCache();

			/// References the singleton instance.
			inline void Reference();

			/// Dereferences the singleton instance.
			/// Any cached memory blocks are freed on last dereference.
			inline void Dereference();

			/// Allocates a memory block of the given size.
			inline void *Allocate(const u32 size, const u32 alignment);

			/// Frees a previously allocated memory block.
			inline void Free(void *const block, const u32 size);

		private:

			/// Hashes a block size to a pool index.
			inline static u32 MapBlockSizeToPool(const u32 size);

			MessageCache(const MessageCache &other);
			MessageCache &operator=(const MessageCache &other);

			/// Number of memory block pools maintained.
			/// Each pool holds memory blocks of a specific size.
			/// The number of pools dictates the maximum block size that can be cached.
			static const u32 MAX_POOLS = 32;

			static MessageCache smInstance;			///< Single, static instance of the class.

			Mutex		mReferenceCountMutex;		///< Synchronizes access to the reference count.
			u32			mReferenceCount;			///< Tracks how many clients exist.
			Pool		mPools[MAX_POOLS];			///< Pools of memory blocks of different sizes.
		};


		XLANG_FORCEINLINE MessageCache &MessageCache::Instance()
		{
			return smInstance;
		}


		XLANG_FORCEINLINE MessageCache::MessageCache()
			: mReferenceCountMutex()
			, mReferenceCount(0)
		{
		}


		inline MessageCache::~MessageCache()
		{
			// Check that the pools were all emptied when the cache became unreferenced.
			// If these asserts fail it probably means a xlang object (either an actor
			// or a receiver) wasn't destructed prior to the application ending.
			for (u32 index = 0; index < MAX_POOLS; ++index)
			{
				XLANG_ASSERT(mPools[index].Empty());
			}
		}


		XLANG_FORCEINLINE void MessageCache::Reference()
		{
			Lock lock(mReferenceCountMutex);
			if (mReferenceCount++ == 0)
			{
				// Check that the pools were all left empty from the last use, if any.
				for (u32 index = 0; index < MAX_POOLS; ++index)
				{
					XLANG_ASSERT(mPools[index].Empty());
				}
			}
		}


		XLANG_FORCEINLINE void MessageCache::Dereference()
		{
			Lock lock(mReferenceCountMutex);
			if (--mReferenceCount == 0)
			{
				// Free any remaining blocks in the pools.
				for (u32 index = 0; index < MAX_POOLS; ++index)
				{
					mPools[index].Clear();
				}
			}
		}


		XLANG_FORCEINLINE void *MessageCache::Allocate(const u32 size, const u32 alignment)
		{
			// Alignment values are expected to be powers of two.
			XLANG_ASSERT(size);
			XLANG_ASSERT(alignment);
			XLANG_ASSERT((alignment & (alignment - 1)) == 0);

			// Find the index of the pool containing blocks of this size.
			const u32 poolIndex(MapBlockSizeToPool(size));

			// We can't cache blocks bigger than a certain maximum size.
			if (poolIndex < MAX_POOLS)
			{
				// Search the pool for a block of the right alignment.
				Pool &pool(mPools[poolIndex]);
				if (void *const block = pool.FetchAligned(alignment))
				{
					return block;
				}
			}

			// We didn't find a cached block so we need to allocate a new one from the user allocator.
			return AllocatorManager::Instance().GetAllocator()->AllocateAligned(size, alignment);
		}


		XLANG_FORCEINLINE void MessageCache::Free(void *const block, const u32 size)
		{
			XLANG_ASSERT(block);
			XLANG_ASSERT(size);

			// Find the index of the pool containing blocks of this size.
			const u32 poolIndex(MapBlockSizeToPool(size));

			// We can't cache blocks bigger than a certain maximum size.
			if (poolIndex < MAX_POOLS)
			{
				// Add the block to the pool, if there is space left in the pool.
				if (mPools[poolIndex].Add(block))
				{
					return;
				}
			}

			// Can't cache this block; return it to the user allocator.
			AllocatorManager::Instance().GetAllocator()->Free(block);
		}


		XLANG_FORCEINLINE u32 MessageCache::MapBlockSizeToPool(const u32 size)
		{
			// We assume that all allocations are non-zero multiples of four bytes!
			XLANG_ASSERT((size & 3) == 0);

			// Because all allocation sizes are multiples of four, we divide by four.
			const u32 index((size + (MessageAlignment - 1)) / MessageAlignment);

			// Because the minimum size is four bytes, we subtract one.
			XLANG_ASSERT(index > 0);
			return index - 1;
		}


	} // namespace detail
} // namespace xlang


#endif // __XLANG_PRIVATE_MESSAGECACHE_MESSAGECACHE_H

