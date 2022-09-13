#include "clang/private/Core/c_ActorCore.h"
#include "clang/private/Directory/c_ActorDirectory.h"
#include "clang/private/Directory/c_ReceiverDirectory.h"
#include "clang/private/Messages/c_IMessage.h"
#include "clang/private/Messages/c_MessageSender.h"
#include "clang/private/Threading/c_Mutex.h"
#include "clang/private/Threading/c_Lock.h"

#include "clang/c_Address.h"
#include "clang/c_Framework.h"


namespace clang
{
	namespace detail
	{
		bool MessageSender::Deliver(const Framework *const framework, IMessage *const message, const Address &address)
		{
			if (Address::IsActorAddress(address))
			{
				ActorCore *const actorCore = ActorDirectory::Instance().GetActor(address);
				if (actorCore)
				{
					// We use a single mutex to protect access to the work queue and the actor message queues.
					// This turns out to be faster than using separate mutexes, perhaps because of the use of
					// expensive locks rather than lock-free primitives. Given this, our strategy is to reduce
					// the locks in the core actor processing loop down to just one, and instead make the code
					// within the lock as fast as possible.
					Lock lock(framework->GetMutex());

					// Push the message onto the actor's dedicated message queue.
					actorCore->Push(message);

					// Schedule the actor for processing and wake a worker thread to process it.
					framework->Schedule(actorCore);

					return true;
				}
			}
			else
			{
				Receiver *const receiver = ReceiverDirectory::Instance().GetReceiver(address);
				if (receiver)
				{
					receiver->Push(message);
					return true;
				}
			}

			// Call the framework's fallback handler, if one was provided.
			framework->ExecuteFallbackHandler(message);

			return false;
		}


		bool MessageSender::TailDeliver(const Framework *const framework, IMessage *const message, const Address &address)
		{
			if (Address::IsActorAddress(address))
			{
				ActorCore *const actorCore = ActorDirectory::Instance().GetActor(address);
				if (actorCore)
				{
					// We use a single mutex to protect access to the work queue and the actor message queues.
					// This turns out to be faster than using separate mutexes, perhaps because of the use of
					// expensive locks rather than lock-free primitives. Given this, our strategy is to reduce
					// the locks in the core actor processing loop down to just one, and instead make the code
					// within the lock as fast as possible.
					Lock lock(framework->GetMutex());

					// Push the message onto the actor's dedicated message queue.
					actorCore->Push(message);

					// Schedule the actor for processing without waking a worker thread.
					framework->TailSchedule(actorCore);

					return true;
				}
			}
			else
			{
				Receiver *const receiver = ReceiverDirectory::Instance().GetReceiver(address);
				if (receiver)
				{
					receiver->Push(message);
					return true;
				}
			}

			// Call the framework's fallback handler, if one was provided.
			framework->ExecuteFallbackHandler(message);

			return false;
		}


	} // namespace detail
} // namespace clang

