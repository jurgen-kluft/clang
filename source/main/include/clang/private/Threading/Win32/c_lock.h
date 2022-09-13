#ifndef __XLANG_PRIVATE_THREADING_WIN32_LOCK_H
#define __XLANG_PRIVATE_THREADING_WIN32_LOCK_H

#include "clang/private/Debug/c_Assert.h"
#include "clang/private/Threading/Win32/c_Mutex.h"

#include "clang/c_Defines.h"


namespace clang
{
	namespace detail
	{
		/// Object that locks a Mutex, implemented using Win32 threads.
		class Lock
		{
		public:

			/// Constructor.
			/// Creates a locked lock around the given mutex object.
			XLANG_FORCEINLINE explicit Lock(Mutex &mutex) : mMutex(mutex)
			{
				mMutex.Lock();
			}

			/// Destructor.
			/// Unlocks the mutex prior to destruction.
			XLANG_FORCEINLINE ~Lock()
			{
				mMutex.Unlock();
			}

			/// Relocks the lock
			XLANG_FORCEINLINE void Relock()
			{
				mMutex.Lock();
			}

			/// Unlocks the lock
			XLANG_FORCEINLINE void Unlock()
			{
				mMutex.Unlock();
			}

		private:

			Lock(const Lock &other);
			Lock &operator=(const Lock &other);

			Mutex &mMutex;              ///< Referenced Mutex object.
		};


	} // namespace detail
} // namespace clang


#endif // __XLANG_PRIVATE_THREADING_WIN32_LOCK_H

