#ifndef __XLANG_PRIVATE_MESSAGES_MESSAGESENDER_H
#define __XLANG_PRIVATE_MESSAGES_MESSAGESENDER_H
#include "xbase/x_target.h"
#ifdef USE_PRAGMA_ONCE 
#pragma once 
#endif

#include "xlang/private/Directory/x_Directory.h"
#include "xlang/private/Messages/x_IMessage.h"
#include "xlang/private/Messages/x_MessageCreator.h"
#include "xlang/private/Threading/x_Lock.h"

#include "xlang/x_Address.h"
#include "xlang/x_Defines.h"


namespace xlang
{
	class Framework;

	namespace detail
	{
		/// Helper class that knows how to send messages.
		/// The methods of this class represent non-inlined call points that break cyclic header
		/// dependencies and reduce code bloat from excessive inlining.
		class MessageSender
		{
		public:

			/// Sends the given value as a message from an address in the given framework
			/// to some other address. 
			template <class ValueType>
			inline static bool Send(const Framework *const framework, const ValueType &value, const Address &from, const Address &to);

			/// Sends the given value as a message from an address in the given framework
			/// to some other address, without waking a worker thread to process it.
			template <class ValueType>
			inline static bool TailSend(const Framework *const framework, const ValueType &value, const Address &from, const Address &to);

		private:

			/// Delivers the given message to the given address.
			/// This is a non-inlined called function to avoid code bloat.
			static bool Deliver(const Framework *const framework, IMessage *const message, const Address &address);

			/// Delivers the given message to the given address, without waking a worker thread to process it.
			/// This is a non-inlined called function to avoid code bloat.
			static bool TailDeliver(const Framework *const framework, IMessage *const message, const Address &address);
		};


		template <class ValueType>
		XLANG_FORCEINLINE bool MessageSender::Send(const Framework *const framework, const ValueType &value, const Address &from, const Address &to)
		{
			// The directory lock is used to protect the global free list.
			Lock lock(Directory::GetMutex());

			// Allocate a message. It'll be deleted by the target after it's been handled.
			IMessage *const message = MessageCreator::Create(value, from);
			if (message != 0)
			{
				// This call is non-inlined to reduce code bloat.
				if (Deliver(framework, message, to))
				{
					return true;
				}

				// If the message wasn't delivered we need to delete it ourselves.
				MessageCreator::Destroy(message);
			}

			return false;
		}


		template <class ValueType>
		XLANG_FORCEINLINE bool MessageSender::TailSend(const Framework *const framework, const ValueType &value, const Address &from, const Address &to)
		{
			// The directory lock is used to protect the global free list.
			Lock lock(Directory::GetMutex());

			// Allocate a message. It'll be deleted by the target after it's been handled.
			IMessage *const message = MessageCreator::Create(value, from);
			if (message != 0)
			{
				// This call is non-inlined to reduce code bloat.
				// This 'tail' call doesn't wake a worker thread to process the message.
				if (TailDeliver(framework, message, to))
				{
					return true;
				}

				// If the message wasn't delivered we need to delete it ourselves.
				MessageCreator::Destroy(message);
			}

			return false;
		}


	} // namespace detail
} // namespace xlang


#endif // __XLANG_PRIVATE_MESSAGES_MESSAGESENDER_H

