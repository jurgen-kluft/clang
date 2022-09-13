#ifndef __XLANG_IALLOCATOR_H
#define __XLANG_IALLOCATOR_H
#include "cbase/c_target.h"
#ifdef USE_PRAGMA_ONCE 
#pragma once 
#endif

#include "clang/private/c_BasicTypes.h"


namespace clang
{
	/**
	\brief Interface describing a general-purpose memory allocator.

	This interface class defines the interface expected of allocators used by clang.
	In particular, it characterizes the type of allocator that can be assigned as
	clang's internal allocator with \ref AllocatorManager::SetAllocator.

	By implementing the IAllocator interface on a custom allocator implementation
	(or wrapping an existing allocator in a custom wrapper that implements IAllocator),
	users can provide a custom allocator to be used by clang. When set via
	\ref AllocatorManager::SetAllocator, then the allocator replaces the \ref
	DefaultAllocator that is used within clang by default.

	By replacing the default allocator, users can control, cache, or optimize the
	allocation of memory within clang. clang guarantees that all of its internal
	heap allocations are allocated via the allocator set with
	\ref AllocatorManager::SetAllocator, as long as the allocator is set before
	any clang objects are constructed.

	\see <a href="http://www.theron-library.com/index.php?t=page&p=CustomAllocator">Using a custom allocator</a>
	*/
	class IAllocator
	{
	public:

		/// \brief Defines an integer type used for specifying sizes of memory allocations.
		typedef u32 SizeType;

		/// \brief Default constructor
		inline IAllocator()
		{
		}

		/// \brief Virtual destructor
		inline virtual ~IAllocator()
		{
		}

		/// \brief Allocates a piece of contiguous memory.
		/// \param size The size of the memory to allocate, in bytes.
		/// \return A pointer to the allocated memory.
		virtual void *Allocate(const SizeType size) = 0;

		/// \brief Allocates a piece of contiguous memory aligned to a given byte-multiple boundary.
		/// \param size The size of the memory to allocate, in bytes.
		/// \param alignment The alignment of the memory to allocate, in bytes.
		/// \return A pointer to the allocated memory.
		/// \note The default implementation, which implementors can override,
		/// simply calls \ref Allocate, ignoring the alignment. This provides backwards
		/// compatibility with legacy implementations that don't implement AllocateAligned.
		inline virtual void *AllocateAligned(const SizeType size, const SizeType /*alignment*/)
		{
			return Allocate(size);
		}

		/// \brief Frees a previously allocated piece of contiguous memory.
		/// \param memory A pointer to the memory to be deallocated.
		virtual void Free(void *const memory) = 0;

	private:

		IAllocator(const IAllocator &other);
		IAllocator &operator=(const IAllocator &other);
	};


} // namespace clang


#endif // XLANG_IALLOCATOR_H

