#ifndef __XLANG_PRIVATE_THREADPOOL_THREADCOLLECTION_H
#define __XLANG_PRIVATE_THREADPOOL_THREADCOLLECTION_H
#include "xbase/x_target.h"
#ifdef USE_PRAGMA_ONCE 
#pragma once 
#endif

#include "xlang/private/x_BasicTypes.h"
#include "xlang/private/Containers/x_List.h"
#include "xlang/private/Threading/x_Thread.h"
#include "xlang/private/Threading/x_Mutex.h"

#include "xlang/x_Defines.h"

namespace xlang
{
	namespace detail
	{
		/// A collection of threads.
		class ThreadCollection
		{
		public:
			/// Default constructor.
			ThreadCollection();

			/// Entry point wrapper function run by each started thread.
			static void		StaticEntryPoint(void *const context);

			void			Reserve(u32 initialNumThreads);

			/// Creates a new worker thread.
			void			CreateThread(Thread::EntryPoint userEntryPoint, void *const userContext);

			/// Destroys all previously created worker threads.
			void			DestroyThreads();

			/// Returns the number of threads currently in the collection.
			/// \note Some of the threads may may be dormant and no longer running.
			inline u32		Size() const;

		private:

			struct ThreadInstance
			{
				XLANG_FORCEINLINE ThreadInstance()
					: mThreadCollection(NULL)
					, mThread(NULL)
					, mUserEntryPoint(NULL)
					, mUserContext(NULL)
					, mUsedNext(NULL)
					, mFreeNext(NULL)
				{
				}

				XLANG_FORCEINLINE ThreadInstance(ThreadCollection * const threadCollection, Thread * const thread, Thread::EntryPoint entryPoint, void *const userContext) 
					: mThreadCollection(threadCollection)
					, mThread(thread)
					, mUserEntryPoint(entryPoint)
					, mUserContext(userContext)
					, mUsedNext(NULL)
					, mFreeNext(NULL)
				{
					XLANG_ASSERT(mThreadCollection);
					XLANG_ASSERT(mThread);
					XLANG_ASSERT(mUserEntryPoint);
				}

				ThreadCollection	*mThreadCollection;
				Thread				*mThread;
				Thread::EntryPoint	mUserEntryPoint;
				void				*mUserContext;
				ThreadInstance		*mUsedNext;
				ThreadInstance		*mFreeNext;

				XCORE_CLASS_PLACEMENT_NEW_DELETE
			};

							ThreadCollection(const ThreadCollection &other);
							ThreadCollection &operator=(const ThreadCollection &other);

			/// Marks the given thread as finished, making it available for reuse.
			void			Finished(ThreadInstance *const threadData);

			Mutex			mMutex;									///< Ensures threadsafe access to owned lists.
			u32				mNumThreads;
			u32				mNumThreadsUsed;
			u32				mNumThreadsFree;
			ThreadInstance	*mThreads;								///< All the thread objects we've created.
			ThreadInstance	*mThreadsFree;
			ThreadInstance	*mThreadsFinished;						///< Threads which have terminated and need joining.
		};


		XLANG_FORCEINLINE u32 ThreadCollection::Size() const
		{
			return mNumThreadsUsed;
		}


	} // namespace detail
} // namespace xlang


#endif // __XLANG_PRIVATE_THREADPOOL_THREADCOLLECTION_H

