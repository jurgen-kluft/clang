#ifndef __XLANG_PRIVATE_HANDLERS_RECEIVERHANDLER_H
#define __XLANG_PRIVATE_HANDLERS_RECEIVERHANDLER_H
#include "xbase/x_target.h"
#ifdef USE_PRAGMA_ONCE 
#pragma once 
#endif

#include "xbase/x_allocator.h"
#include "xlang/private/Debug/x_Assert.h"
#include "xlang/private/Handlers/x_IReceiverHandler.h"
#include "xlang/private/Messages/x_IMessage.h"
#include "xlang/private/Messages/x_Message.h"
#include "xlang/private/Messages/x_MessageCast.h"
#include "xlang/private/Messages/x_MessageTraits.h"

#include "xlang/x_Address.h"
#include "xlang/x_Defines.h"


namespace xlang
{
	namespace detail
	{
		/// Instantiable class template that remembers a message handler function and
		/// the type of message it accepts. It is responsible for checking whether
		/// incoming messages are of the type accepted by the handler, and executing the
		/// handler for messages that match.
		///
		/// Incoming messages are cast at runtime to the type of message handled by the
		/// stored handler, and the handler is executed only if the cast succeeds (returns
		/// a non-zero pointer). The dynamic cast operation is specialized to use either
		/// C++ dynamic_cast or a hand-rolled runtime type information (RTTI) system
		/// that avoids introducing the type information into types other than messages.
		/// 
		/// \tparam ObjectType The class on which the handler function is a method.
		/// \tparam ValueType The type of message handled by the message handler.
		template <class ObjectType, class ValueType>
		class ReceiverHandler : public IReceiverHandler
		{
		public:

			/// Pointer to a member function of the object type that can handle messages
			/// with the given value type.
			typedef void (ObjectType::*HandlerFunction)(const ValueType &message, const Address from);

			/// Constructor.
			inline ReceiverHandler(ObjectType *const object, HandlerFunction function)
				: mObject(object)
				, mHandlerFunction(function)
			{
			}

			/// Virtual destructor.
			inline virtual ~ReceiverHandler()
			{
			}

			/// Returns a pointer to the handler function registered by this instance.
			XLANG_FORCEINLINE HandlerFunction GetHandlerFunction() const
			{
				return mHandlerFunction;
			}

			/// Returns the unique name of the message type handled by this handler.
			inline virtual int GetMessageTypeId() const
			{
				return type2int<ValueType>::value();
			}

			/// Handles the given message, if it's of the type accepted by the handler.
			/// \return True, if the handler handled the message.
			/// \note The message is not consumed by the handler; just acted on or ignored.
			/// The message will be automatically destroyed when all handlers have seen it.
			inline virtual bool Handle(const IMessage *const message) const
			{
				typedef MessageCast<MessageTraits<ValueType>::HAS_TYPE_NAME> MessageCaster;

				XLANG_ASSERT(mObject);
				XLANG_ASSERT(mHandlerFunction);
				XLANG_ASSERT(message);

				// Try to convert the message, of unknown type, to message of the assumed type.
				const Message<ValueType> *const typedMessage = MessageCaster:: template CastMessage<ValueType>(message);
				if (typedMessage)
				{
					// Call the handler, passing it the message value and from address.
					(mObject->*mHandlerFunction)(typedMessage->Value(), typedMessage->From());
					return true;
				}

				return false;
			}

			XCORE_CLASS_PLACEMENT_NEW_DELETE
		private:

			ReceiverHandler(const ReceiverHandler &other);
			ReceiverHandler &operator=(const ReceiverHandler &other);

			ObjectType *mObject;                        ///< Pointer to the object owning the handler function.
			const HandlerFunction mHandlerFunction;     ///< Pointer to the handler member function on the owning object.
		};


	} // namespace detail
} // namespace xlang


#endif // __XLANG_PRIVATE_HANDLERS_RECEIVERHANDLER_H

