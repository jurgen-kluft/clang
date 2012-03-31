#ifndef __XLANG_PRIVATE_THREADING_WIN32_THREAD_H
#define __XLANG_PRIVATE_THREADING_WIN32_THREAD_H
#include "xbase\x_target.h"
#ifdef USE_PRAGMA_ONCE 
#pragma once 
#endif
#include "xbase\x_allocator.h"

#ifdef _MSC_VER
#pragma warning(push,0)
#endif //_MSC_VER

#include <windows.h>
#include <process.h>

#ifdef _MSC_VER
#pragma warning(pop)
#endif //_MSC_VER

#include "xlang\x_Defines.h"
#include "xlang\private\Debug\x_Assert.h"

namespace xlang
{
	namespace detail
	{
		/// A system thread, implemented using Win32 threads.
		class Thread
		{
		public:

			/// Defines a function that can serve as a thread entry point.
			/// \note Entry point functions must be static -- implying that they can't be class
			/// member functions.
			typedef void (*EntryPoint)(void *const context);

			/// Default constructor
			XLANG_FORCEINLINE Thread() 
				: mThread(0)
			{
			}

			/// Destructor
			XLANG_FORCEINLINE ~Thread()
			{
				XLANG_ASSERT(mThread == 0);
			}

			/// Starts the thread, executing the given entry point function.
			/// \param entryPoint The entry point function that the thread should execute.
			/// \param context Pointer to a context object providing the environment in which the thread runs.
			/// \return True, if the thread was started successfully.
			inline bool Start(EntryPoint entryPoint, void *const context);

			/// Waits for the thread to finish and return.
			/// The semantics are that Start and Join can be called repeatedly in pairs.
			inline void Join();

			/// Returns true if the thread is currently running.
			/// The thread is running if Start was called more recently than Join.
			inline bool Running() const;

			XCORE_CLASS_PLACEMENT_NEW_DELETE
		private:
			/// Struct that holds a pointer to a thread entry point function and some context data.
			struct ThreadData
			{
				EntryPoint	mEntryPoint;
				void		*mContext;
			};

			/// Thread entry point adapter function.
			/// Wraps a call to a standard xlang-style thread entry point in a Win32-style
			/// static thread entry point function signature.
			/// \param pData A pointer to a ThreadData structure containing an entry point
			/// function and a context pointer.
			/// \return Unused dummy return value.
			inline static u32 __stdcall ThreadStartProc(void *pData);

			Thread(const Thread &other);
			Thread &operator=(const Thread &other);

			HANDLE		mThread;		///< Handle of the internal Win32 thread.
			ThreadData	mThreadData;	///< Wrapper around the data passed to the thread on start.
		};


		XLANG_FORCEINLINE bool Thread::Start(EntryPoint entryPoint, void *const context)
		{
			XLANG_ASSERT(mThread == 0);

			// Create a data structure to wrap the data we need to pass to the entry function.
			mThreadData.mEntryPoint = entryPoint;
			mThreadData.mContext = context;

			// It's safer to use _beginthreadex() than calling CreateThread() directly.
			// http://stackoverflow.com/questions/331536/windows-threading-beginthread-vs-beginthreadex-vs-createthread-c
			mThread = (HANDLE)_beginthreadex(
				0,                                      // default security attributes
				0,                                      // use default stack size
				ThreadStartProc,                        // thread entry point function
				reinterpret_cast<void *>(&mThreadData), // pass the real entrypoint and context
				0,                                      // use default creation flags
				0);                                     // returns the thread identifier (unused)

			// Check the return value for success. 
			return (mThread != 0);
		}


		XLANG_FORCEINLINE void Thread::Join()
		{
			XLANG_ASSERT(mThread != 0);

			// Wait for the thread to terminate.
			// This treats the thread as an event, which is 'set' when it terminates.
			// Apparently this is common practice.
			WaitForSingleObject(
				mThread,                                // Event handle
				INFINITE);                              // Timeout

			// Destroy the thread object so it can be safely recreated next time.
			// For threads started with _beginthreadex(), CloseHandle() isn't called automatically.
			// This allows the caller time to synchronize with the thread ending.
			CloseHandle(mThread);
			mThread = 0;
		}


		XLANG_FORCEINLINE bool Thread::Running() const
		{
			return (mThread != 0);
		}


		inline u32 __stdcall Thread::ThreadStartProc(void *pData)
		{
			// Call the real entry point function, passing the provided context.
			ThreadData *threadData = reinterpret_cast<ThreadData *>(pData);
			threadData->mEntryPoint(threadData->mContext);

			// Terminating a thread with a call to _endthreadex helps to ensure proper
			// recovery of resources allocated for the thread.
			// http://msdn.microsoft.com/en-us/library/kdzttdcb(v=vs.80).aspx
			_endthreadex(0);

			return 0;
		}


	} // namespace detail
} // namespace xlang


#endif // __XLANG_PRIVATE_THREADING_WIN32_THREAD_H

