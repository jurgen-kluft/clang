#ifndef __XLANG_PRIVATE_CORE_MESSAGEALIGNMENT_H
#define __XLANG_PRIVATE_CORE_MESSAGEALIGNMENT_H
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE 
#pragma once 
#endif

#include "clang/private/c_BasicTypes.h"

namespace clang
{
	namespace detail
	{
		/// \brief Traits struct template that stores alignment information about messages.
		/// Users can specialize this template for their own message types in order to tell
		/// clang about any specialized alignment requirements of those classes.
		/// clang uses the alignment value defined for each message type to request memory
		/// with the correct alignment from the general allocator registered with the
		/// \ref AllocatorManager. The default alignment is four bytes, implying that
		/// instances of the message class should be allocated starting at 4-byte boundaries.
		/// \note Note that although clang will request memory allocated with the correct
		/// alignment, whether or not the allocator respects the alignment request is up
		/// to the allocator implementation. The default allocator, DefaultAllocator,
		/// supports alignment and returns correctly aligned allocations. Users with
		/// specialized alignment requirements should ensure that any custom allocator,
		/// implementing the \ref IAllocator interface, and enabled with
		/// \ref clang::AllocatorManager::SetAllocator, supports aligned allocations.
		/// \tparam MessageType The message type for which the alignment traits are defined.
		template <class MessageType>
		struct MessageAlignment
		{
			/// \brief Describes the memory alignment requirement of the message type, in bytes.
			/// The default alignment is four bytes.
			static const u32 ALIGNMENT = 8;
		};


	} // namespace detail
} // namespace clang


#endif // __XLANG_PRIVATE_CORE_MESSAGEALIGNMENT_H

