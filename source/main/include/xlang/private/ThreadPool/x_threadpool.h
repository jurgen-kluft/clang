#ifndef __XLANG_PRIVATE_THREADPOOL_THREADPOOL_H
#define __XLANG_PRIVATE_THREADPOOL_THREADPOOL_H
#include "xbase\x_target.h"
#ifdef USE_PRAGMA_ONCE 
#pragma once 
#endif

#include "xlang\private\x_BasicTypes.h"
#include "xlang\private\Containers\x_IntrusiveQueue.h"
#include "xlang\private\Core\x_ActorCore.h"
#include "xlang\private\Core\x_ActorDestroyer.h"
#include "xlang\private\Debug\x_Assert.h"
#include "xlang\private\Directory\x_Directory.h"
#include "xlang\private\Messages\x_IMessage.h"
#include "xlang\private\Messages\x_MessageCreator.h"
#include "xlang\private\Threading\x_Lock.h"
#include "xlang\private\Threading\x_Thread.h"
#include "xlang\private\Threading\x_Monitor.h"
#include "xlang\private\ThreadPool\x_ThreadCollection.h"

#include "xlang\x_Align.h"
#include "xlang\x_AllocatorManager.h"
#include "xlang\x_Defines.h"


namespace xlang
{
	namespace detail
	{
		/// A pool of worker threads.
		class ThreadPool
		{
		public:

			/// Worker thread entry point function.
			/// Only global (static) functions can be used as thread entry points. Therefore this static method
			/// exists to wrap the non-static class method that is the real entry point.
			/// \param context Pointer to a context class that provides the context in which the thread is run.
			static void StaticWorkerThreadEntryPoint(void *const context);

			/// Manager thread entry point function.
			static void StaticManagerThreadEntryPoint(void *const context);

			/// Constructor.
			ThreadPool();

			/// Starts the pool, starting the given number of worker threads.
			void			Start(u32 count, u32 target_count);

			/// Stops the pool, terminating all worker threads.
			void			Stop();

			/// Requests that there be at most \ref count worker threads in the pool.
			/// \note If the current number is higher, threads are terminated until the maximum is reached.
			void			SetMaxThreads(const u32 count);

			/// Requests that there be at least \ref count worker threads in the pool.
			/// \note If the current number is lower, new threads are spawned until the maximum is reached.
			void			SetMinThreads(const u32 count);

			/// Returns the current maximum permitted number of worker threads in this pool.
			inline u32		GetMaxThreads() const;

			/// Returns the current minimum permitted number of worker threads in this pool.
			inline u32		GetMinThreads() const;

			/// Gets the actual number of worker threads currently in the pool.
			inline u32		GetNumThreads() const;

			/// Gets the peak number of worker threads ever in the pool.
			/// \note This includes any threads which were created but later terminated.
			inline u32		GetPeakThreads() const;

			/// Resets internal counters that track reported events for thread pool management.
			inline void		ResetCounters() const;

			/// Returns the number of messages processed within this pool.
			/// The count is incremented automatically and can be reset using ResetCounters.
			inline u32		GetNumMessagesProcessed() const;

			/// Returns the number of thread pulse events made in response to arriving messages.
			/// The count is incremented automatically and can be reset using ResetCounters.
			inline u32		GetNumThreadsPulsed() const;

			/// Returns the number of threads woken by pulse events in response to arriving messages.
			/// The count is incremented automatically and can be reset using ResetCounters.
			inline u32		GetNumThreadsWoken() const;

			/// Gets a reference to the core message processing mutex.
			inline Mutex	&GetMutex() const;

			/// Pushes an actor that has received a message onto the work queue for processing,
			/// and wakes up a worker thread to process it if one is available.
			inline void		Push(ActorCore *const actor);

			/// Pushes an actor that has received a message onto the work queue for processing,
			/// without waking up a worker thread. Instead the actor is processed by a running thread.
			inline void		TailPush(ActorCore *const actor);

		private:

			typedef IntrusiveQueue<ActorCore> WorkQueue;

			/// Clamps a given thread count to a legal range.
			inline static u32 ClampThreadCount(const u32 count);

			ThreadPool(const ThreadPool &other);
			ThreadPool &operator=(const ThreadPool &other);

			/// Worker thread function.
			void			WorkerThreadProc();

			/// Manager thread function.
			void			ManagerThreadProc();

			/// Processes an actor core entry retrieved from the work queue.
			inline void		ProcessActorCore(Lock &lock, ActorCore *const actorCore);

			// Accessed in the main loop.
			u32				mNumThreads;							///< Counts the number of threads running.
			u32				mTargetThreads;							///< The number of threads currently desired.
			WorkQueue		mWorkQueue;								///< Threadsafe queue of actors waiting to be processed.
			mutable Monitor	mWorkQueueMonitor;						///< Synchronizes access to the work queue.
			mutable Monitor	mManagerMonitor;						///< Locking event that wakes the manager thread.
			mutable u32		mNumMessagesProcessed;					///< Counter used to count processed messages.
			mutable u32		mNumThreadsPulsed;						///< Counts the number of times we signaled a worker thread to wake.
			mutable u32		mNumThreadsWoken;						///< Counter used to count woken threads.

			// Accessed infrequently.
			ThreadCollection mWorkerThreads;						///< Owned collection of worker threads.
			Thread			mManagerThread;							///< Dynamically creates and destroys the worker threads.
		};


		XLANG_FORCEINLINE u32 ThreadPool::GetMaxThreads() const
		{
			u32 count(0);

			{
				Lock lock(mManagerMonitor.GetMutex());
				count = mTargetThreads;
			}

			return count;
		}


		XLANG_FORCEINLINE u32 ThreadPool::GetMinThreads() const
		{
			u32 count(0);

			{
				Lock lock(mManagerMonitor.GetMutex());
				count = mTargetThreads;
			}

			return count;
		}


		XLANG_FORCEINLINE u32 ThreadPool::GetNumThreads() const
		{
			u32 count(0);

			{
				Lock lock(mManagerMonitor.GetMutex());
				count = mNumThreads;
			}

			return count;
		}


		XLANG_FORCEINLINE u32 ThreadPool::GetPeakThreads() const
		{
			u32 count(0);

			{
				Lock lock(mManagerMonitor.GetMutex());
				count = mWorkerThreads.Size();
			}

			return count;
		}


		XLANG_FORCEINLINE void ThreadPool::ResetCounters() const
		{
			Lock lock(mWorkQueueMonitor.GetMutex());

			mNumMessagesProcessed = 0;
			mNumThreadsPulsed = 0;
			mNumThreadsWoken = 0;
		}


		XLANG_FORCEINLINE u32 ThreadPool::GetNumMessagesProcessed() const
		{
			u32 count(0);

			{
				Lock lock(mWorkQueueMonitor.GetMutex());
				count = mNumMessagesProcessed;
			}

			return count;
		}


		XLANG_FORCEINLINE u32 ThreadPool::GetNumThreadsPulsed() const
		{
			u32 count(0);

			{
				Lock lock(mWorkQueueMonitor.GetMutex());
				count = mNumThreadsPulsed;
			}

			return count;
		}


		XLANG_FORCEINLINE u32 ThreadPool::GetNumThreadsWoken() const
		{
			u32 count(0);

			{
				Lock lock(mWorkQueueMonitor.GetMutex());
				count = mNumThreadsWoken;
			}

			return count;
		}


		XLANG_FORCEINLINE u32 ThreadPool::ClampThreadCount(const u32 count)
		{
			if (count == 0)
			{
				return 1;
			}

			if (count > XLANG_MAX_THREADS_PER_FRAMEWORK)
			{
				return XLANG_MAX_THREADS_PER_FRAMEWORK;
			}

			return count;
		}


		XLANG_FORCEINLINE Mutex &ThreadPool::GetMutex() const
		{
			return mWorkQueueMonitor.GetMutex();
		}


		XLANG_FORCEINLINE void ThreadPool::Push(ActorCore *const actorCore)
		{
			// Don't schedule ourselves if the actor is already scheduled or running.
			// Actors which need further processing at the end of their current
			// processing, are marked dirty, and will be rescheduled when executed.
			if (actorCore->IsScheduled())
			{
				// Flag the actor as dirty so it will be re-scheduled after execution.
				actorCore->Dirty();
				return;
			}

			// Mark the actor as busy.
			actorCore->Schedule();

			// Push the actor onto the work queue and wake a worker thread.
			mWorkQueue.Push(actorCore);

			// Wake up a worker thread.
			mWorkQueueMonitor.Pulse();
			++mNumThreadsPulsed;
		}


		XLANG_FORCEINLINE void ThreadPool::TailPush(ActorCore *const actorCore)
		{
			// Don't schedule ourselves if the actor is already scheduled or running.
			// Actors which need further processing at the end of their current
			// processing, are marked dirty, and will be rescheduled when executed.
			if (actorCore->IsScheduled())
			{
				// Flag the actor as dirty so it will be re-scheduled after execution.
				actorCore->Dirty();
				return;
			}

			// Mark the actor as busy.
			actorCore->Schedule();

			// Push the actor onto the work queue without waking a worker thread.
			mWorkQueue.Push(actorCore);
		}


		XLANG_FORCEINLINE void ThreadPool::ProcessActorCore(Lock &lock, ActorCore *const actorCore)
		{
			// Read an unprocessed message from the actor's message queue.
			// If there are no queued messages the returned pointer is null.
			IMessage *const message(actorCore->GetQueuedMessage());

			// We have to hold the lock while we check the referenced state, to make sure a
			// dereferencing ActorRef that decremented the reference count has finished accessing
			// it before we free it, in the case where the actor has become unreferenced.
			const bool referenced(actorCore->IsReferenced());

			// Increment the message processing counter if we'll process a message.
			// We do this while still holding the lock to ensure the counter can't be cleared
			// just before we increment it. We exploit the fact that bools are 0 or 1 to avoid branches.
			const u32 messageValid(message != 0);
			const u32 actorReferenced(referenced);
			mNumMessagesProcessed += (messageValid & actorReferenced);

			lock.Unlock();

			// An actor is still 'live' if it has unprocessed messages or is still referenced.
			const bool live((message != 0) | referenced);
			if (live)
			{
				// If the actor has a waiting message then process the message, even if the
				// actor is no longer referenced. This ensures messages send to actors just
				// before they become unreferenced are correctly processed.
				if (message)
				{
					// Update the actor's message handlers and handle the message.
					actorCore->ValidateHandlers();
					actorCore->ProcessMessage(message);

					// Destroy the message now it's been read.
					// The directory lock is used to protect the global free list.
					Lock directoryLock(Directory::GetMutex());
					MessageCreator::Destroy(message);
				}
			}
			else
			{
				// Garbage collect the unreferenced actor.
				// This also frees any messages still in its queue.
				ActorDestroyer::DestroyActor(actorCore);
			}

			lock.Relock();

			if (live)
			{
				// Re-add the actor to the work queue if it still needs more processing,
				// including if it's unreferenced and we haven't destroyed it yet.
				if (actorCore->IsDirty() | actorCore->HasQueuedMessage() | !referenced)
				{
					actorCore->CleanAndSchedule();
					mWorkQueue.Push(actorCore);
					return;
				}
				else
				{
					actorCore->CleanAndUnschedule();
				}
			}
		}


	} // namespace detail
} // namespace xlang


#endif // __XLANG_PRIVATE_THREADPOOL_THREADPOOL_H

