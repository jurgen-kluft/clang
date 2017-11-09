#ifndef __XLANG_PRIVATE_HANDLERS_BLINDFALLBACKHANDLER_H
#define __XLANG_PRIVATE_HANDLERS_BLINDFALLBACKHANDLER_H
#include "xbase/x_target.h"
#ifdef USE_PRAGMA_ONCE 
#pragma once 
#endif

#include "xbase/x_allocator.h"

#include "xlang/private/x_BasicTypes.h"
#include "xlang/private/Debug/x_Assert.h"
#include "xlang/private/Handlers/x_IFallbackHandler.h"
#include "xlang/private/Messages/x_IMessage.h"

#include "xlang/x_Address.h"
#include "xlang/x_Defines.h"

namespace xlang
{
	namespace detail
	{
		/// Instantiable class template that remembers a 'blind' fallback message handler function
		/// registered with a framework and called for messages that are undelivered or unhandled.
		/// A blind handler is one that takes the message as blind data: a void pointer and a size.
		/// 
		/// \tparam ObjectType The class on which the handler function is a method.
		template <class ObjectType>
		class BlindFallbackHandler : public IFallbackHandler
		{
		public:

			/// Pointer to a member function of a handler object.
			typedef void (ObjectType::*HandlerFunction)(const void *const data, const u32 size, const Address from);

			/// Constructor.
			XLANG_FORCEINLINE BlindFallbackHandler(ObjectType *const object, HandlerFunction function)
				: mObject(object)
				, mHandlerFunction(function)
			{
			}

			/// Virtual destructor.
			inline virtual ~BlindFallbackHandler()
			{
			}

			/// Handles the given message.
			inline virtual void Handle(const IMessage *const message) const
			{
				XLANG_ASSERT(mObject);
				XLANG_ASSERT(mHandlerFunction);
				XLANG_ASSERT(message);

				// Call the handler, passing it the from address and also the message as blind data.
				const void *const messageData(message->GetMessageData());
				const u32 messageSize(message->GetMessageValueSize());
				const Address from(message->From());

				XLANG_ASSERT(messageData && messageSize);

				(mObject->*mHandlerFunction)(messageData, messageSize, from);
			}

			XCORE_CLASS_PLACEMENT_NEW_DELETE
		private:

			BlindFallbackHandler(const BlindFallbackHandler &other);
			BlindFallbackHandler &operator=(const BlindFallbackHandler &other);

			ObjectType *mObject;                        ///< Pointer to the object owning the handler function.
			const HandlerFunction mHandlerFunction;     ///< Pointer to the handler member function on the owning object.
		};


	} // namespace detail
} // namespace xlang


#endif // __XLANG_PRIVATE_HANDLERS_BLINDFALLBACKHANDLER_H

