#ifndef __XLANG_PRIVATE_MESSAGES_MESSAGE_H
#define __XLANG_PRIVATE_MESSAGES_MESSAGE_H

#include <new>

#include "xlang\private\x_BasicTypes.h"
#include "xlang\private\Debug\x_Assert.h"
#include "xlang\private\Messages\x_IMessage.h"
#include "xlang\private\Messages\x_MessageTraits.h"
#include "xlang\private\Messages\x_MessageAlignment.h"

#include "xlang\x_AllocatorManager.h"
#include "xlang\x_Defines.h"


namespace xlang
{
	namespace detail
	{

		/// Message class, used for sending data between actors.
		template <class ValueType>
		class Message : public IMessage
		{
		public:

			typedef Message<ValueType> ThisType;

			/// Virtual destructor.
			XLANG_FORCEINLINE virtual ~Message()
			{
			}

			/// Returns the memory block size required to initialize a message of this type.
			XLANG_FORCEINLINE static uint32_t GetSize()
			{
				// We allocate an aligned buffer to hold the message and its copy of the value.
				// We lay the message and its value side by side in memory.
				// The value is first, since it's the value that needs the alignment.
				// The message object itself doesn't need special alignment.
				const uint32_t messageSize(sizeof(ThisType));
				uint32_t valueSize(sizeof(ValueType));

				// Empty structs passed as values have a size of one byte, which we don't like.
				// We expect every allocation to be a multiple of four bytes, so we round up.
				if (valueSize < 4)
				{
					valueSize = 4;
				}

				return valueSize + messageSize;
			}

			/// Returns the memory block alignment required to initialize a message of this type.
			XLANG_FORCEINLINE static uint32_t GetAlignment()
			{
				return MessageAlignment<ValueType>::ALIGNMENT;
			}

			/// Initializes a message of this type in the provided memory block.
			/// The block is allocated and freed by the caller.
			XLANG_FORCEINLINE static ThisType *Initialize(void *const block, const ValueType &value, const Address &from)
			{
				XLANG_ASSERT(block);

				// Instantiate a new instance of the value type in aligned position at the start of the buffer.
				// We assume that the message value type can be copy-constructed.
				// Messages are explicitly copied to avoid shared memory.
				ValueType *const pValue = new (block) ValueType(value);

				// Allocate the message object immediately after the value, passing it the value's address.
				void *const pObject(reinterpret_cast<void *>(pValue + 1));
				return new (pObject) ThisType(pValue, from);
			}

			/// Returns the name of the message type.
			/// This uniquely identifies the type of the message value.
			/// \note Unless explicitly specified to avoid C++ RTTI, message names are null.
			virtual const char *TypeName() const
			{
				return MessageTraits<ValueType>::TYPE_NAME;
			}

			/// Allows the message instance to destruct its constructed value object before being freed.
			virtual void Release()
			{
				// The referenced block owned by this message is blind data, but we know it holds
				// an instance of the value type, that needs to be explicitly destructed.
				// We have to call the destructor manually because we constructed the object in-place.
				Value().~ValueType();
			}

			/// Returns the size in bytes of the message value.
			virtual uint32_t GetMessageSize() const
			{
				// Calculate the size of the message value itself. There's no padding between the
				// message value block and the Message object that follows it.
				return GetBlockSize() - static_cast<uint32_t>(sizeof(ThisType));
			}

			/// Gets the value carried by the message.
			XLANG_FORCEINLINE const ValueType &Value() const
			{
				// The value is stored at the start of the memory block.
				return *reinterpret_cast<const ValueType *>(GetBlock());
			}

		private:

			/// Private constructor.
			XLANG_FORCEINLINE Message(void *const block, const Address &from) :
			   IMessage(from, block, ThisType::GetSize())
			   {
				   XLANG_ASSERT(block);
			   }

			   Message(const Message &other);
			   Message &operator=(const Message &other);
		};


	} // namespace detail
} // namespace xlang


#endif // __XLANG_PRIVATE_MESSAGES_MESSAGE_H

