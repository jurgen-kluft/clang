#ifndef __XLANG_PRIVATE_ALLOCATORS_DEFAULTALLOCATOR_H
#define __XLANG_PRIVATE_ALLOCATORS_DEFAULTALLOCATOR_H
#include "xbase\x_target.h"
#ifdef USE_PRAGMA_ONCE 
#pragma once 
#endif

/**
\file DefaultAllocator.h
The allocator used within xlang by default.
*/


#include "xlang\private\x_BasicTypes.h"
#include "xlang\private\Debug\x_Assert.h"
#include "xlang\private\Threading\x_Lock.h"
#include "xlang\private\Threading\x_Mutex.h"

#include "xlang\x_Align.h"
#include "xlang\x_Defines.h"
#include "xlang\x_IAllocator.h"


namespace xlang
{


	/**
	\brief A simple general purpose memory allocator used by default.

	This is the allocator implementation used by default within xlang.
	It is a simple wrapper around global new and delete, adding alignment and
	some simple, optional, allocation checking.

	The DefaultAllocator is used by xlang for its internal allocations, unless it
	is replaced by a custom allocator via \ref AllocatorManager::SetAllocator.
	The DefaultAllocator may also be used for allocations within user application code,
	if desired, in which case it can be accessed via \ref AllocatorManager::GetAllocator.

	The DefaultAllocator respects aligned allocation requests, and allocates
	memory blocks whose start addresses are aligned to a boundary of a specified
	multiple of bytes, when requested. For example, when requested, it can allocate
	a block of memory starting at an address that is a multiple of 16 bytes.

	Because this allocator supports alignment, it can be safely used by users needing
	to align their actor types (using \ref XLANG_ALIGN_ACTOR) or message types (using
	\ref XLANG_ALIGN_MESSAGE).

	The \ref XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS define, defined in \ref Defines.h,
	controls the enabling of various checking code, which is intended to be enabled
	only for debugging. It is defined to 1 by default in debug builds and to 0 in
	optimized (non-debug) builds.

	The checks performed when \ref XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS is enabled include:
	- Guardband checking of allocated memory blocks.
	- Tracking of current and peak allocated memory (in bytes).
	- Detection and reporting of memory leaks on application exit via asserts.

	The checking of allocated memory, if enabled, is made thread-safe by means of an
	internal Mutex object, which unavoidably adds to the overhead of the checking.
	Generally the checking shouldn't be enabled in production code.

	\note This default allocator can be replaced with a custom allocator implementation
	using \ref AllocatorManager::SetAllocator.

	\see AllocatorManager
	\see <a href="http://www.theron-library.com/index.php?t=page&p=CustomAllocator">Using a custom allocator</a>
	*/
	class DefaultAllocator : public IAllocator
	{
	public:

		static const SizeType MIN_ALIGNMENT = 8;
		static const u32 GUARD_VALUE = 0xdddddddd;

		/**
		\brief Default constructor
		*/
		inline DefaultAllocator();

		/**
		\brief Virtual destructor.

		When enabled via \ref XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS, the destructor
		checks that the amount of memory still allocated has been reduced to zero, in
		order to detect memory leaks.
		*/
		inline virtual ~DefaultAllocator();

		/**
		\brief Allocates a block of contiguous memory.

		\ref Allocate is called by xlang for its internal allocations, unless the
		DefaultAllocator is replaced by a custom allocator via \ref AllocatorManager::SetAllocator.
		The DefaultAllocator may also be used for allocations within user application code,
		if desired, in which case it can be accessed via \ref AllocatorManager::GetAllocator.

		\note The returned memory is typically aligned to at least four-byte boundaries.
		For larger alignments, use \ref AllocateAligned.

		\param size The size of the memory block to allocate, in bytes, which must be a non-zero multiple of four bytes.
		\return A pointer to the allocated memory.
		*/
		inline virtual void *Allocate(const SizeType size);

		/**
		\brief Allocates a block of contiguous memory aligned to a given byte-multiple boundary.

		This method supports alignment, and the returned memory is correctly aligned,
		starting at a memory address that is a multiple of the requested alignment value.

		The support for aligned allocations, as well as guardband checking and tracking of
		allocated memory when enabled, are implemented by allocating an extra preamble
		and postamble before and after each allocated memory block. This naturally adds
		'hidden' overheads to the size of each allocated memory block, making the actual
		amount of memory allocated greater than the total amount of memory allocated from
		the point of view of the caller. The internal layout of each allocated block is
		illustrated below:

		\verbatim
		|-----------|--------|--------|--------|---------------------------------|--------|---------|
		|  padding  | offset |  size  | guard  |          caller block           | guard  |  waste  |
		|-----------|--------|--------|--------|---------------------------------|--------|---------|
		\endverbatim

		When \ref XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS is zero, some checking fields are omitted,
		and allocated blocks have the following structure:

		\verbatim
		|-----------|--------|----------------------------------|---------|
		|  padding  | offset |           caller block           |  waste  |
		|-----------|--------|----------------------------------|---------|
		\endverbatim

		Where:
		- The \em caller \em  block starts at an aligned address.
		- \em padding is a padding field of variable size used to ensure that \em caller \em block is aligned.
		- \em offset is a u32 recording the offset in bytes of \em caller \em block within the block.
		- \em size is a u32 recording the size of \em caller \em block.
		- \em guard is a u32 marker word with known value of <code>0xdddddddd</code>.
		- \em waste is an unused field of variable size left over within the block after offsetting of the \em caller \em block.

		\param size The size of the memory block to allocate, in bytes, which must be a non-zero multiple of four bytes.
		\param alignment The alignment of the memory to allocate, in bytes, which must be a power of two.
		\return A pointer to the allocated memory.
		*/
		inline virtual void *AllocateAligned(const SizeType size, const SizeType alignment);

		/**
		\brief Frees a previously allocated block of contiguous memory.

		\ref Free is called by xlang for its internal allocations, unless the
		DefaultAllocator is replaced by a custom allocator via \ref AllocatorManager::SetAllocator.
		The DefaultAllocator may also be used for allocations within user application code,
		if desired, in which case it can be accessed via \ref AllocatorManager::GetAllocator.

		\param memory Pointer to the memory to be deallocated.

		\note The pointer must not be null, and must address an allocated block of memory.
		*/
		inline virtual void Free(void *const memory);

		/**
		\brief Gets the number of bytes currently allocated by the allocator.

		Returns the total number of bytes of memory currently allocated by calls to
		\ref Allocate or \ref AllocateAligned, but not freed in calls to \ref Free.

		\code
		xlang::IAllocator *const allocator = xlang::AllocatorManager::Instance().GetAllocator();
		xlang::DefaultAllocator *const defaultAllocator = dynamic_cast<xlang::DefaultAllocator *>(allocator);

		if (defaultAllocator)
		{
			printf("Default allocator has %d bytes currently allocated\n", defaultAllocator->GetBytesAllocated());
		}
		\endcode

		This method is only useful when allocation checking is enabled using \ref
		XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS (enabled, by default, in debug builds).
		If allocation checking is disabled then GetBytesAllocated returns zero.

		\note This method counts user allocations and doesn't include internal overheads
		introduced by alignment and memory tracking. The actual amount of memory
		allocated via global new is typically larger.

		\see GetPeakBytesAllocated
		*/
		inline u32 GetBytesAllocated() const;

		/**
		\brief Gets the peak number of bytes ever allocated by the allocator at one time.

		Returns the peak number of bytes of memory ever allocated by calls to
		\ref Allocate or \ref AllocateAligned, but not freed in calls to \ref Free.

		\code
		xlang::IAllocator *const allocator = xlang::AllocatorManager::Instance().GetAllocator();
		xlang::DefaultAllocator *const defaultAllocator = dynamic_cast<xlang::DefaultAllocator *>(allocator);

		if (defaultAllocator)
		{
			printf("Default allocator peak allocation was %d bytes\n", defaultAllocator->GetPeakBytesAllocated());
		}
		\endcode

		This method is only useful when allocation checking is enabled using \ref
		XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS (enabled, by default, in debug builds).
		If allocation checking is disabled then GetPeakBytesAllocated returns zero.

		\note This method counts user allocations and doesn't include internal overheads
		introduced by alignment and memory tracking. The actual amount of memory
		allocated via global new is typically larger.

		\see GetBytesAllocated
		*/
		inline u32 GetPeakBytesAllocated() const;

	private:

		DefaultAllocator(const DefaultAllocator &other);
		DefaultAllocator &operator=(const DefaultAllocator &other);

		/// Internal method which is force-inlined to avoid a function call.
		inline void *AllocateInline(const SizeType size, const SizeType alignment);

#if XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS
		detail::Mutex mMutex;           ///< Critical section object used to protect access to the allocation counts.
		u32 mBytesAllocated;       ///< Tracks the number of bytes currently allocated not yet freed.
		u32 mPeakAllocated;        ///< Tracks the peak number of bytes allocated but not yet freed.
#endif // XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS

	};


	inline DefaultAllocator::DefaultAllocator()
	{

#if XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS
		detail::Lock lock(mMutex);

		mBytesAllocated = 0;
		mPeakAllocated = 0;
#endif // XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS

	}


	inline DefaultAllocator::~DefaultAllocator()
	{

#if XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS
		// Memory leak detection.
		// Failures likely indicate xlang bugs, unless the allocator is used by user code.
		if (mBytesAllocated > 0)
		{
			XLANG_FAIL_MSG("DefaultAllocator detected memory leaks");
		}
#endif // XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS

	}


	inline void *DefaultAllocator::Allocate(const SizeType size)
	{
		// Default to sizeof(void*) alignment in 32-bit/64-bit builds.
		// This call is force-inlined.
		return AllocateInline(size, sizeof(void*));
	}


	inline void *DefaultAllocator::AllocateAligned(const SizeType size, const SizeType alignment)
	{
		// This call is force-inlined.
		return AllocateInline(size, alignment);
	}


	inline void DefaultAllocator::Free(void *const memory)
	{
		// We don't expect to have allocated any blocks that aren't at least aligned to the machine word size.
		XLANG_ASSERT_MSG(memory, "Free of null pointer");
		XLANG_ASSERT_MSG(XLANG_ALIGNED(memory, sizeof(int)), "Free of unaligned pointer");

		u32 *const callerBlock(reinterpret_cast<u32 *>(memory));

#if XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS
		// Check the pre-and post-guard fields, bookending the caller block.
		const u32 *const offsetField(reinterpret_cast<u32 *>(callerBlock) - 3);
		const u32 *const sizeField(callerBlock - 2);
		const u32 *const preGuardField(callerBlock - 1);

		const u32 callerBlockSize(*sizeField);
		const u32 *const postGuardField(reinterpret_cast<u32 *>(reinterpret_cast<u8 *>(callerBlock) + callerBlockSize));

		if (*preGuardField != GUARD_VALUE || *postGuardField != GUARD_VALUE)
		{
			XLANG_FAIL_MSG("Corrupted guardband indicates memory corruption");
		}

		{
			detail::Lock lock(mMutex);

			XLANG_ASSERT_MSG(mBytesAllocated >= callerBlockSize, "Unallocated free, suggests duplicate free");
			mBytesAllocated -= callerBlockSize;
		}

#else
		u32 *const offsetField(reinterpret_cast<u32 *>(callerBlock) - 1);
#endif // XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS

		// Address of the internally allocated block.
		const u32 callerBlockOffset(*offsetField);
		u8 *const block(reinterpret_cast<u8 *>(callerBlock) - callerBlockOffset);

		delete [] block;
	}


	XLANG_FORCEINLINE u32 DefaultAllocator::GetBytesAllocated() const
	{

#if XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS
		return mBytesAllocated;
#else
		return 0;
#endif // XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS

	}


	XLANG_FORCEINLINE u32 DefaultAllocator::GetPeakBytesAllocated() const
	{

#if XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS
		return mPeakAllocated;
#else
		return 0;
#endif // XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS

	}


	//XLANG_FORCEINLINE
	void *DefaultAllocator::AllocateInline(const SizeType size, const SizeType alignment)
	{
		// Alignment values are expected to be powers of two greater than or equal to four bytes.
		// This ensures that the size, offset, and guard fields are 4-byte aligned.
		XLANG_ASSERT_MSG(alignment >= MIN_ALIGNMENT, "Actor and message alignments must be at least 4 bytes");
		XLANG_ASSERT_MSG((alignment & (alignment - 1)) == 0, "Actor and message alignments must be powers of two");

		// Allocation sizes are expected to be non-zero multiples of four bytes.
		// This ensures that the trailing guard field is 4-byte aligned.
		XLANG_ASSERT_MSG(size >= MIN_ALIGNMENT, "Allocation of memory block less than four bytes in size");
		XLANG_ASSERT_MSG((size & 0x3) == 0, "Allocation of memory block not a multiple of four bytes in size");

#if XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS
		const u32 numPreFields(4);
		const u32 numPostFields(2);
#else
		const u32 numPreFields(2);
		const u32 numPostFields(0);
#endif // XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS

		// Calculate the size of the internally allocated block.
		// We assume underlying allocations are always MIN_ALIGNMENT aligned, so padding is at most (alignment-MIN_ALIGNMENT) bytes.
		const u32 preambleSize(numPreFields * sizeof(u32));
		const u32 postambleSize(numPostFields * sizeof(u32));
		const u32 internalSize(preambleSize + ((size + (alignment - 1)) & ~(alignment - 1)) + postambleSize);

		u32 *const block = reinterpret_cast<u32 *>(new unsigned char[internalSize]);
		XLANG_ASSERT_MSG(XLANG_ALIGNED(block, MIN_ALIGNMENT), "Global new is assumed to always align to MIN_ALIGNMENT boundaries");

		if (block)
		{
			// Calculate the pre-padding required to offset the caller block to an aligned address.
			// We do this by accounting for the hidden pre-fields and then aligning the pointer.
			u8 *callerBlock(reinterpret_cast<u8 *>(block + numPreFields));
			XLANG_ALIGN(callerBlock, alignment);

#if XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS
			u32 *const offsetField(reinterpret_cast<u32 *>(callerBlock) - 3);
			u32 *const sizeField(reinterpret_cast<u32 *>(callerBlock) - 2);
			u32 *const preGuardField(reinterpret_cast<u32 *>(callerBlock) - 1);
			u32 *const postGuardField(reinterpret_cast<u32 *>(callerBlock + size));

			*sizeField = size;
			*preGuardField = GUARD_VALUE;
			*postGuardField = GUARD_VALUE;

			{
				detail::Lock lock(mMutex);

				mBytesAllocated += size;
				if (mBytesAllocated > mPeakAllocated)
				{
					mPeakAllocated = mBytesAllocated;
				}
			}
#else
			u32 *const offsetField(reinterpret_cast<u32 *>(callerBlock) - 1);
#endif // XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS

			// Offset of the caller block within the internally allocated block, in bytes.
			const u32 callerBlockOffset(static_cast<u32>(callerBlock - reinterpret_cast<u8 *>(block)));
			*offsetField = callerBlockOffset;

			// Caller gets the address of the caller block, which is expected to be aligned.
			XLANG_ASSERT(XLANG_ALIGNED(callerBlock, alignment));
			return callerBlock;
		}

		XLANG_FAIL_MSG("Out of memory in DefaultAllocator!");
		return 0;
	}


} // namespace xlang


#endif // __XLANG_PRIVATE_ALLOCATORS_DEFAULTALLOCATOR_H

