#ifndef __XLANG_PRIVATE_HANDLERS_MESSAGEHANDLER_H
#define __XLANG_PRIVATE_HANDLERS_MESSAGEHANDLER_H
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE 
#pragma once 
#endif

#include "cbase/c_allocator.h"
#include "clang/private/Debug/c_Assert.h"
#include "clang/private/Handlers/c_IMessageHandler.h"
#include "clang/private/Messages/c_IMessage.h"
#include "clang/private/Messages/c_Message.h"
#include "clang/private/Messages/c_MessageCast.h"
#include "clang/private/Messages/c_MessageTraits.h"

#include "clang/c_Address.h"
#include "clang/c_Defines.h"

namespace clang
{
	class Actor;

	namespace detail
	{
		struct MessageHandler_t { MessageHandler_t() : _vfptr_(0), _marked_(0), _data_(0) {} void* _vfptr_; u64 _marked_; u64 _data_; };

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
		/// \tparam ActorType The type of actor whose message handlers are considered.
		/// \tparam ValueType The type of message handled by this message handler.
		template <class ActorType, class ValueType>
		class MessageHandler : public IMessageHandler
		{
		public:

			/// Pointer to a member function of the actor type that can handle messages
			/// with the given value type.
			typedef void (ActorType::*HandlerFunction)(const ValueType &message, const Address from);

			/// Constructor.
			inline explicit MessageHandler(HandlerFunction function) : mHandlerFunction(function)
			{
			}

			/// Virtual destructor.
			inline virtual ~MessageHandler()
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
			inline virtual bool Handle(Actor *const actor, const IMessage *const message) const
			{
				typedef MessageCast<MessageTraits<ValueType>::HAS_TYPE_NAME> MessageCaster;

				XLANG_ASSERT(actor);
				XLANG_ASSERT(mHandlerFunction);
				XLANG_ASSERT(message);

				// Try to convert the message, of unknown type, to message of the assumed type.
				const Message<ValueType> *const typedMessage = MessageCaster:: template CastMessage<ValueType>(message);
				if (typedMessage)
				{
					// Call the handler, passing it the message value and from address.
					ActorType *const typedActor = static_cast<ActorType *>(actor);
					(typedActor->*mHandlerFunction)(typedMessage->Value(), typedMessage->From());
					return true;
				}

				return false;
			}

			XCORE_CLASS_PLACEMENT_NEW_DELETE
		private:
			MessageHandler() : mHandlerFunction(NULL) {}
			MessageHandler(const MessageHandler &other);
			MessageHandler &operator=(const MessageHandler &other);

			friend class Actor;

			const HandlerFunction mHandlerFunction;     ///< Pointer to a handler member function on an actor.
		};

	} // namespace detail
} // namespace clang


#endif // __XLANG_PRIVATE_HANDLERS_MESSAGEHANDLER_H

