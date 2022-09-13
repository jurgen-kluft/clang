#ifndef __XLANG_ALLOCATORMANAGER_H
#define __XLANG_ALLOCATORMANAGER_H
#include "cbase/c_target.h"
#ifdef USE_PRAGMA_ONCE 
#pragma once 
#endif

#include "clang/private/Debug/c_Assert.h"

#include "clang/c_DefaultAllocator.h"
#include "clang/c_Defines.h"
#include "clang/c_IAllocator.h"


namespace clang
{
	/**
	\brief Singleton class that manages allocators for use by clang.

	This class is a singleton, and its single instance can be accessed using the
	static \ref Instance method on the class.

	Non-static \ref SetAllocator and \ref GetAllocator methods on the singleton instance
	allow the allocator used by clang to be set and retreived. Setting the allocator
	replaces the \ref DefaultAllocator, which is used if no custom allocator is explicitly
	set. The \ref GetAllocator method returns a pointer to the currently set allocator,
	which is either the allocator set previously using \ref SetAllocator or the \ref
	DefaultAllocator, if none has been set.

	\code
	class MyAllocator : public clang::IAllocator
	{
	public:
						MyAllocator();
		virtual			~MyAllocator();

		virtual void	*Allocate(const SizeType size);
		virtual void	*AllocateAligned(const SizeType size, const SizeType alignment);
		virtual void	Free(void *const memory);
	};

	MyAllocator allocator;
	clang::AllocatorManager::Instance().SetAllocator(&allocator);
	\endcode

	\note The \ref SetAllocator method can be called at most once, at application start.
	If the \ref DefaultAllocator is replaced with a custom allocator then it must be
	replaced at application start, before any clang objects (\ref Framework "frameworks",
	\ref Actor "actors" or \ref Receiver "receivers") are constructed. \ref GetAllocator
	can be called any number of times, both before and after a call to \ref SetAllocator.

	\see <a href="http://www.theron-library.com/index.php?t=page&p=CustomAllocator">Using a custom allocator</a>
	*/
	class AllocatorManager
	{
	public:

		/**
		\brief Returns a reference to the AllocatorManager singleton instance.
		*/
		XLANG_FORCEINLINE static AllocatorManager &Instance()
		{
			return smInstance;
		}

		/**
		\brief Sets the allocator used for internal allocations, replacing the default allocator.

		Calling this method allows applications to provide custom allocators and hence to control
		how and where memory is allocated by clang. This is useful, for example, in embedded systems
		where memory is scarce or of different available types. The allocator provided to this method
		must implement IAllocator and can be a wrapper around another existing allocator implementation.
		The provided allocator is used for all internal heap allocations, including the allocation of
		instantiated actors. If this method is not called by user code then a default DefaultAllocator
		is used, which is a simple wrapper around global new and delete.

		\code
		MyAllocator allocator;
		clang::AllocatorManager::Instance().SetAllocator(&allocator);
		\endcode

		\note This method should be called once at most, and before any other clang activity.

		\see GetAllocator
		*/
		inline void SetAllocator(IAllocator *const allocator)
		{
			// This method should only be called once, at start of day.
			XLANG_ASSERT_MSG(mAllocator == &mDefaultAllocator, "SetAllocator can only be called once!");
			XLANG_ASSERT_MSG(mDefaultAllocator.GetBytesAllocated() == 0, "SetAllocator can only be called before Framework construction");
			XLANG_ASSERT(allocator != 0);

			mAllocator = allocator;
		}

		/**
		\brief Gets a pointer to the general allocator currently in use by clang.

		\code
		clang::IAllocator *const allocator = clang::AllocatorManager::Instance().GetAllocator();
		clang::DefaultAllocator *const defaultAllocator = dynamic_cast<clang::DefaultAllocator *>(allocator);

		if (defaultAllocator)
		{
			printf("Default allocator has %d bytes currently allocated\n", defaultAllocator->GetBytesAllocated());
		}
		\endcode

		\see SetAllocator
		*/
		XLANG_FORCEINLINE IAllocator *GetAllocator() const
		{
			return mAllocator;
		}

	private:

		/// Default constructor. Private, since the AllocatorManager is a singleton class.
		inline AllocatorManager() 
			: mDefaultAllocator()
			, mAllocator(&mDefaultAllocator)
		{
		}

		AllocatorManager(const AllocatorManager &other);
		AllocatorManager &operator=(const AllocatorManager &other);

		static AllocatorManager smInstance;			///< The single, static instance.

		DefaultAllocator	mDefaultAllocator;		///< Default allocator used if none is explicitly set.
		IAllocator			*mAllocator;			///< Pointer to a general allocator for use in internal allocations.
	};


} // namespace clang


#endif // XLANG_ALLOCATORMANAGER_H

