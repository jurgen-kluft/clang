#include "xlang\private\Debug\x_Assert.h"
#include "xlang\private\Threading\x_Lock.h"
#include "xlang\private\ThreadPool\x_ThreadCollection.h"

#include "xlang\x_AllocatorManager.h"
#include "xlang\x_Defines.h"


namespace xlang
{
	namespace detail
	{
		void ThreadCollection::StaticEntryPoint(void *const context)
		{
			ThreadInstance *const threadData(reinterpret_cast<ThreadInstance *>(context));
			XLANG_ASSERT(threadData);

			ThreadCollection *const threadCollection(threadData->mThreadCollection);
			Thread::EntryPoint userEntryPoint(threadData->mUserEntryPoint);
			void *const userContext(threadData->mUserContext);

			XLANG_ASSERT(threadCollection);
			XLANG_ASSERT(userEntryPoint);

			// Call the wrapped user-level entry point function, passing it the user-level context data.
			userEntryPoint(userContext);

			// On return from the user-level entry point function, add the thread to the finished list.
			// This allows the calling thread (which is not this thread) to call Join() on it later.
			threadCollection->Finished(threadData);
		}

		ThreadCollection::ThreadCollection() 
			: mMutex()
			, mNumThreads(0)
			, mNumThreadsUsed(0)
			, mNumThreadsFree(0)
			, mThreads(NULL)
			, mThreadsFree(NULL)
		{
		}

		void	ThreadCollection::Reserve(u32 initialNumThreads) 
		{
			for (u32 i=0; i<initialNumThreads; ++i)
			{
				void *const dataMemory = AllocatorManager::Instance().GetAllocator()->Allocate(sizeof(ThreadInstance));
				ThreadInstance* threadData = new (dataMemory) ThreadInstance();
				void *const threadMemory = AllocatorManager::Instance().GetAllocator()->Allocate(sizeof(Thread));
				Thread* thread = new (threadMemory) Thread();
				threadData->mThread = thread;
				threadData->mFreeNext = mThreadsFree;
				mThreadsFree = threadData;
				mNumThreadsFree++;
				threadData->mUsedNext = mThreads;
				mThreads = threadData;
				mNumThreads++;
			}
		}


		void ThreadCollection::CreateThread(Thread::EntryPoint userEntryPoint, void *const userContext)
		{
			XLANG_ASSERT(userEntryPoint);

			Thread			*thread(0);
			ThreadInstance	*threadData(0);

			// Reuse one of the previously created threads that has finished, if available.
			{
				Lock lock(mMutex);

				if (mThreadsFree != NULL)
				{
					threadData = mThreadsFree;
					mThreadsFree = mThreadsFree->mFreeNext;
					threadData->mThreadCollection = this;
					threadData->mFreeNext = NULL;
					--mNumThreadsFree;
				}
			}

			if (threadData)
			{
				thread = threadData->mThread;

				// Join the finished thread and wait for it to terminate.
				// Join should be called from the same thread that called Start,
				// so we create and destroy all worker threads with the same manager thread.
				if (thread->Running())
					thread->Join();

				// Update the thread data with the new user entry point and user context.
				threadData->mUserEntryPoint = userEntryPoint;
				threadData->mUserContext = userContext;
			}
			else
			{
				// Allocate a new thread.
				void *const threadMemory = AllocatorManager::Instance().GetAllocator()->Allocate(sizeof(Thread));
				if (threadMemory == 0)
				{
					return;
				}

				// Allocate a thread data block.
				void *const dataMemory = AllocatorManager::Instance().GetAllocator()->Allocate(sizeof(ThreadInstance));
				if (dataMemory == 0)
				{
					AllocatorManager::Instance().GetAllocator()->Free(threadMemory);
					return;
				}

				// Construct the thread and its data, setting up the data with a pointer to the thread and its context.
				// The thread pointer allows the thread function to be provided with a pointer to its thread object.
				thread = new (threadMemory) Thread();
				threadData = new (dataMemory) ThreadInstance(this,thread,userEntryPoint,userContext);

				// Remember the added threads
				threadData->mUsedNext = mThreads;
				mThreads = threadData;
				++mNumThreads;
			}

			// Start the thread, running it via our wrapper entry point.
			// We call our own wrapper entry point, passing it data including the user entry point and context.
			++mNumThreadsUsed;
			thread->Start(StaticEntryPoint, threadData);
		}


		void ThreadCollection::DestroyThreads()
		{
			// Wait for the worker threads to stop.
			{
				ThreadInstance* it = mThreads;
				while(it != NULL)
				{
					ThreadInstance * threadData = it;
					it = it->mUsedNext;

					Thread *const thread(threadData->mThread);

					// Call Join on this thread; it shouldn't have been called yet.
					// This waits until the thread finishes.
					if(thread->Running())
						thread->Join();

					// Explicitly call the destructor, since we allocated using placement new.
					thread->~Thread();

					// Free the memory for the thread and the context block.
					AllocatorManager::Instance().GetAllocator()->Free(threadData);
					AllocatorManager::Instance().GetAllocator()->Free(thread);
				}
			}

			mThreads = NULL;
			mNumThreads = 0;
			mThreadsFree = NULL;
			mNumThreadsFree = 0;
		}


		void ThreadCollection::Finished(ThreadInstance *const threadData)
		{
			Lock lock(mMutex);
			threadData->mFreeNext = mThreadsFree;
			mThreadsFree = threadData;
			++mNumThreadsFree;
			--mNumThreadsUsed;
		}


	} // namespace detail
} // namespace xlang

