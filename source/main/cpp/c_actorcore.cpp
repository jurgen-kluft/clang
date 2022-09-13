#include "clang/private/c_BasicTypes.h"
#include "clang/private/Core/c_ActorCore.h"
#include "clang/private/Debug/c_Assert.h"
#include "clang/private/Directory/c_Directory.h"
#include "clang/private/Messages/c_MessageCreator.h"
#include "clang/private/Threading/c_Lock.h"

#include "clang/c_Actor.h"
#include "clang/c_AllocatorManager.h"
#include "clang/c_Framework.h"


namespace clang
{
	namespace detail
	{
		ActorCore::ActorCore()
			: mNext(0)
			, mParent(0)
			, mFramework(0)
			, mSequence(0)
			, mMessageCount(0)
			, mMessageQueue()
			, mNumMessageHandlers(0)
			, mMaxMessageHandlers(32)
			, mState(0)
		{
			// Actor cores shouldn't be default-constructed.
			XLANG_FAIL();
		}

		ActorCore::ActorCore(const u32 sequence, Framework *const framework, Actor *const actor)
			: mNext(0)
			, mParent(actor)
			, mFramework(framework)
			, mSequence(sequence)
			, mMessageCount(0)
			, mMessageQueue()
			, mNumMessageHandlers(0)
			, mMaxMessageHandlers(32)
			, mState(STATE_REFERENCED)
		{
			XLANG_ASSERT(GetSequence() != 0);
			XLANG_ASSERT(mFramework != 0);
		}

		ActorCore::~ActorCore()
		{
			// We don't need to lock this because only one thread can access it at a time.
			// Free all currently allocated handler objects.
			mNumMessageHandlers = 0;

			{
				// The directory lock is used to protect the global free list.
				Lock directoryLock(Directory::GetMutex());
				Lock frameworkLock(mFramework->GetMutex());

				// Free any left-over messages that haven't been processed.
				// This is undesirable but can happen if the actor is killed while
				// still processing messages.
				while (IMessage *const message = mMessageQueue.Pop())
				{
					MessageCreator::Destroy(message);
					--mMessageCount;
				}

				XLANG_ASSERT(mMessageCount == 0);
			}
		}

		Mutex &ActorCore::GetMutex() const
		{
			// We reuse the main message loop mutex to avoid locking an additional mutex.
			return mFramework->GetMutex();
		}

		void ActorCore::UpdateHandlers()
		{
			// Filter any handlers marked for deletion
			for (u32 i=0; i<mNumMessageHandlers; ++i)
			{
				IMessageHandler* handler = (IMessageHandler*)&mMessageHandlers[i];
				if (handler->IsMarked())
				{
					for (u32 j=i+1; j<mNumMessageHandlers; ++j)
						mMessageHandlers[i] = mMessageHandlers[j];
					--mNumMessageHandlers;
				}
			}

			if (mParent->mNewMessageHandlersNum!=0)
			{
				// Sorted-Insert of the new handlers
				for (u32 i=0; i<mParent->mNewMessageHandlersNum; ++i)
				{
					IMessageHandler* new_handler = (IMessageHandler*)&mParent->mNewMessageHandlers[i];
					if (new_handler->IsMarked())
						continue;

					// Search for a place
					s32 ins = 0;
					for (; ins<(s32)mNumMessageHandlers; ++ins)
					{
						IMessageHandler* handler = (IMessageHandler*)&mMessageHandlers[ins];
						if (new_handler->GetMessageTypeId() < handler->GetMessageTypeId())
							break;
					}

					// Insert
					// First, Move all entries up
					//for (u32 k=ins+1; k<mNumMessageHandlers; ++k)
					for (s32 k=mNumMessageHandlers-1; k>=ins; --k)
						mMessageHandlers[k+1] = mMessageHandlers[k];
					
					// Then, Data copy the new handler at the insert position
					mMessageHandlers[ins] = mParent->mNewMessageHandlers[i];
					mNumMessageHandlers++;
				}
				mParent->mNewMessageHandlersNum = 0;
			}
		}

		bool ActorCore::ExecuteDefaultHandler(IMessage *const message)
		{
			IDefaultHandler *const defaultHandler = mParent->GetDefaultHandler();
			if (defaultHandler)
			{
				defaultHandler->Handle(mParent, message);
				return true;
			}

			return false;
		}


		bool ActorCore::ExecuteFallbackHandler(IMessage *const message)
		{
			return mFramework->ExecuteFallbackHandler(message);
		}

		void ActorCore::Unreference()
		{
			// Schedule the actor core to make the threadpool garbage collect it.
			mState &= ~STATE_REFERENCED;
			mFramework->Schedule(this);
		}


	} // namespace detail
} // namespace clang


