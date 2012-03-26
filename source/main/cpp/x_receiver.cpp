#include "xlang\private\Debug\x_Assert.h"
#include "xlang\private\Directory\x_Directory.h"
#include "xlang\private\Directory\x_ReceiverDirectory.h"
#include "xlang\private\MessageCache\x_MessageCache.h"
#include "xlang\private\Messages\x_MessageCreator.h"
#include "xlang\private\Threading\x_Lock.h"

#include "xlang\x_Address.h"
#include "xlang\x_AllocatorManager.h"
#include "xlang\x_Defines.h"
#include "xlang\x_IAllocator.h"
#include "xlang\x_Receiver.h"


namespace xlang
{
	Receiver::Receiver() 
		: mAddress(Address::Null())
		, mMessageHandlers()
		, mMonitor()
		, mMessagesReceived(0)
	{
		// Reference the global free list to ensure it's created.
		detail::MessageCache::Instance().Reference();

		{
			detail::Lock lock(detail::Directory::GetMutex());

			// Register this receiver with the directory and get its unique address.
			mAddress = detail::ReceiverDirectory::Instance().RegisterReceiver(this);
			XLANG_ASSERT(mAddress != Address::Null());
		}
	}


	Receiver::~Receiver()
	{
		{
			detail::Lock lock(detail::Directory::GetMutex());
			detail::ReceiverDirectory::Instance().DeregisterReceiver(GetAddress());
		}

		{
			detail::Lock lock(mMonitor.GetMutex());

			// Free all currently allocated handler objects.
			while (detail::IReceiverHandler *const handler = mMessageHandlers.Front())
			{
				mMessageHandlers.Remove(handler);
				AllocatorManager::Instance().GetAllocator()->Free(handler);
			}
		}

		// Dereference the global free list to ensure it's destroyed.
		detail::MessageCache::Instance().Dereference();
	}


	void Receiver::Push(detail::IMessage *const message)
	{
		XLANG_ASSERT(message);

		{
			detail::Lock lock(mMonitor.GetMutex());

			MessageHandlerList::Iterator handlers(mMessageHandlers.Begin());
			const MessageHandlerList::Iterator handlersEnd(mMessageHandlers.End());

			while (handlers != handlersEnd)
			{
				// Execute the handler.
				// It does nothing if it can't handle the message type.
				detail::IReceiverHandler *const handler(*handlers);
				handler->Handle(message);

				++handlers;
			}

			// Wake up anyone who's waiting for a message to arrive.
			++mMessagesReceived;

			mMonitor.Pulse();
		}

		// Free the message, whether it was handled or not.
		// The directory lock is used to protect the global free list.
		detail::Lock lock(detail::Directory::GetMutex());
		detail::MessageCreator::Destroy(message);
	}


} // namespace xlang


