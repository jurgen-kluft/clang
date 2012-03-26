#ifndef __XLANG_PRIVATE_THREADING_WIN32_MUTEX_H
#define __XLANG_PRIVATE_THREADING_WIN32_MUTEX_H

#ifdef _MSC_VER
#pragma warning(push,0)
#endif //_MSC_VER

#include <windows.h>

#ifdef _MSC_VER
#pragma warning(pop)
#endif //_MSC_VER

#include "xlang\x_Defines.h"


namespace xlang
{
	namespace detail
	{
		/// A simple critical section object implemented with Win32 threads.
		class Mutex
		{
		public:

			/// Default constructor.
			XLANG_FORCEINLINE Mutex()
			{
				InitializeCriticalSection(&mCriticalSection);
			}

			/// Destructor.
			XLANG_FORCEINLINE ~Mutex()
			{
				DeleteCriticalSection(&mCriticalSection);
			}

			/// Locks the mutex, guaranteeing exclusive access to a protected resource associated with it.
			/// \note This is a blocking call and should be used with care to avoid deadlocks.
			XLANG_FORCEINLINE void Lock()
			{
				EnterCriticalSection(&mCriticalSection);
			}

			/// Unlocks the mutex, releasing exclusive access to a protected resource associated with it.
			XLANG_FORCEINLINE void Unlock()
			{
				LeaveCriticalSection(&mCriticalSection);
			}

		private:

			Mutex(const Mutex &other);
			Mutex &operator=(const Mutex &other);

			CRITICAL_SECTION mCriticalSection;  ///< Owned Win32 critical section object.
		};


	} // namespace detail
} // namespace xlang


#endif // __XLANG_PRIVATE_THREADING_WIN32_MUTEX_H
