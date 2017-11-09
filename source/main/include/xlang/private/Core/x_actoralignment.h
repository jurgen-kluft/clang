#ifndef __XLANG_PRIVATE_CORE_ACTORALIGNMENT_H
#define __XLANG_PRIVATE_CORE_ACTORALIGNMENT_H

#include "xlang/private/x_BasicTypes.h"

namespace xlang
{
	namespace detail
	{
		/// \brief Traits struct template that stores alignment information about actors.
		/// Users can specialize this template for their own actor types in order to tell
		/// xlang about any specialized alignment requirements of those classes.
		/// xlang uses the alignment value defined for each actor type to request memory
		/// with the correct alignment from the general allocator registered with the
		/// \ref AllocatorManager. The default alignment is four bytes, implying that
		/// instances of the actor class should be allocated starting at 4-byte boundaries.
		/// \note Note that although xlang will request memory allocated with the correct
		/// alignment, whether or not the allocator respects the alignment request is up
		/// to the allocator implementation. The default allocator, DefaultAllocator,
		/// supports alignment and returns correctly aligned allocations. Users with
		/// specialized alignment requirements should ensure that any custom allocator,
		/// implementing the \ref IAllocator interface, and enabled with
		/// \ref xlang::AllocatorManager::SetAllocator, supports aligned allocations.
		/// \tparam ActorType The actor type for which the alignment traits are defined.
		template <class ActorType>
		struct ActorAlignment
		{
			/// \brief Describes the memory alignment requirement of the actor type, in bytes.
			/// The default alignment is four bytes.
			static const u32 ALIGNMENT = 4;
		};


	} // namespace detail
} // namespace xlang


#endif // __XLANG_PRIVATE_CORE_ACTORALIGNMENT_H

