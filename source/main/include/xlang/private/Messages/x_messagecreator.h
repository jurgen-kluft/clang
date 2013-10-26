#ifndef __XLANG_PRIVATE_MESSAGES_MESSAGECREATOR_H
#define __XLANG_PRIVATE_MESSAGES_MESSAGECREATOR_H
#include "xbase\x_target.h"
#ifdef USE_PRAGMA_ONCE 
#pragma once 
#endif

#include "xlang\private\x_basictypes.h"
#include "xlang\private\messagecache\x_messagecache.h"
#include "xlang\private\messages\x_imessage.h"
#include "xlang\private\messages\x_message.h"

#include "xlang\x_address.h"
#include "xlang\x_defines.h"

namespace xlang
{
	namespace detail
	{
		/// Helper class that constructs and destroys xlang's internal message objects.
		class MessageCreator
		{
		public:

			/// Allocates and constructs a message with the given value and from address.
			template <class ValueType>
			inline static Message<ValueType> *Create(const ValueType &value, const Address &from);

			/// Destructs and frees a message of unknown type referenced by an interface pointer.
			inline static void Destroy(IMessage *const message);
		};



		template <class ValueType>
		XLANG_FORCEINLINE Message<ValueType> *MessageCreator::Create(const ValueType &value, const Address &from)
		{
			typedef Message<ValueType> MessageType;

			const u32 blockSize(MessageType::GetSize());
			const u32 blockAlignment(MessageType::GetAlignment());

			// Allocate a message. It'll be deleted by the actor after it's been handled.
			// We allocate a block from the global free list for caching of common allocations.
			// The free list is thread-safe so we don't need to lock it ourselves.
			void *const block = MessageCache::Instance().Allocate(blockSize, blockAlignment);
			if (block)
			{
				return MessageType::Initialize(block, value, from);
			}

			return 0;
		}


		XLANG_FORCEINLINE void MessageCreator::Destroy(IMessage *const message)
		{
			// Call release on the message to give it chance to destruct its value type.
			message->Release();

			// Return the block to the global free list.
			MessageCache::Instance().Free(message->GetBlock(), message->GetBlockSize());
		}


	} // namespace detail
} // namespace xlang


#endif // __XLANG_PRIVATE_MESSAGES_MESSAGECREATOR_H

