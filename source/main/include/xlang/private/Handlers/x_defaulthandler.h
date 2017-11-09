#ifndef __XLANG_PRIVATE_HANDLERS_DEFAULTHANDLER_H
#define __XLANG_PRIVATE_HANDLERS_DEFAULTHANDLER_H

#include "xlang/private/Debug/x_Assert.h"
#include "xlang/private/Messages/x_IMessage.h"
#include "xlang/private/Handlers/x_IDefaultHandler.h"

#include "xlang/x_Address.h"
#include "xlang/x_Defines.h"

namespace xlang
{
	class Actor;

	namespace detail
	{
		/// Instantiable class template that remembers a default handler function.
		template <class ActorType>
		class DefaultHandler : public IDefaultHandler
		{
		public:

			/// Pointer to a member function of the actor type, which is the handler.
			typedef void (ActorType::*HandlerFunction)(const Address from);

			/// Constructor.
			XLANG_FORCEINLINE explicit DefaultHandler(HandlerFunction function) : mHandlerFunction(function)
			{
			}

			/// Virtual destructor.
			inline virtual ~DefaultHandler()
			{
			}

			/// Handles the given message.
			/// \note The message is not consumed by the handler; just acted on or ignored.
			/// The message will be automatically destroyed when all handlers have seen it.
			inline virtual void Handle(Actor *const actor, const IMessage *const message) const;

		private:

			DefaultHandler(const DefaultHandler &other);
			DefaultHandler &operator=(const DefaultHandler &other);

			const HandlerFunction mHandlerFunction;     ///< Pointer to a handler member function on the actor.
		};


		template <class ActorType>
		inline void DefaultHandler<ActorType>::Handle(Actor *const actor, const IMessage *const message) const
		{
			XLANG_ASSERT(actor);
			XLANG_ASSERT(message);
			XLANG_ASSERT(mHandlerFunction);

			// Call the handler, passing it the from address.
			// We can't pass the value because we don't even know the type.
			ActorType *const typedActor = static_cast<ActorType *>(actor);
			(typedActor->*mHandlerFunction)(message->From());
		}


	} // namespace detail
} // namespace xlang


#endif // __XLANG_PRIVATE_HANDLERS_DEFAULTHANDLER_H

