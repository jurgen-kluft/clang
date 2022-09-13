#ifndef __XLANG_PRIVATE_HANDLERS_BLINDDEFAULTHANDLER_H
#define __XLANG_PRIVATE_HANDLERS_BLINDDEFAULTHANDLER_H

#include "clang/private/c_BasicTypes.h"
#include "clang/private/Debug/c_Assert.h"
#include "clang/private/Messages/c_IMessage.h"
#include "clang/private/Messages/c_Message.h"
#include "clang/private/Handlers/c_IDefaultHandler.h"

#include "clang/c_Address.h"
#include "clang/c_Defines.h"

namespace clang
{
	class Actor;

	namespace detail
	{
		/// Instantiable class template that remembers a 'blind' default handler function.
		/// A blind handler is one that takes the message as blind data: a void pointer and a size.
		template <class ActorType>
		class BlindDefaultHandler : public IDefaultHandler
		{
		public:

			/// Pointer to a member function of the actor type, which is the handler.
			typedef void (ActorType::*HandlerFunction)(const void *const data, const u32 size, const Address from);

			/// Constructor.
			XLANG_FORCEINLINE explicit BlindDefaultHandler(HandlerFunction function) : mHandlerFunction(function)
			{
			}

			/// Virtual destructor.
			inline virtual ~BlindDefaultHandler()
			{
			}

			/// Handles the given message.
			/// \note The message is not consumed by the handler; just acted on or ignored.
			/// The message will be automatically destroyed when all handlers have seen it.
			inline virtual void Handle(Actor *const actor, const IMessage *const message) const;

		private:

			BlindDefaultHandler(const BlindDefaultHandler &other);
			BlindDefaultHandler &operator=(const BlindDefaultHandler &other);

			const HandlerFunction mHandlerFunction;     ///< Pointer to a handler member function on the actor.
		};


		template <class ActorType>
		inline void BlindDefaultHandler<ActorType>::Handle(Actor *const actor, const IMessage *const message) const
		{
			XLANG_ASSERT(actor);
			XLANG_ASSERT(message);
			XLANG_ASSERT(mHandlerFunction);

			// Call the handler, passing it the from address and also the message as blind data.
			ActorType *const typedActor = static_cast<ActorType *>(actor);

			const void *const messageData(message->GetMessageData());
			const u32 messageSize(message->GetMessageValueSize());
			const clang::Address from(message->From());

			XLANG_ASSERT(messageData && messageSize);

			(typedActor->*mHandlerFunction)(messageData, messageSize, from);
		}


	} // namespace detail
} // namespace clang


#endif // __XLANG_PRIVATE_HANDLERS_BLINDDEFAULTHANDLER_H

