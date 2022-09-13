#ifndef __XLANG_PRIVATE_HANDLERS_BLINDFALLBACKHANDLER_H
#define __XLANG_PRIVATE_HANDLERS_BLINDFALLBACKHANDLER_H
#include "cbase/c_target.h"
#ifdef USE_PRAGMA_ONCE 
#pragma once 
#endif

#include "cbase/c_allocator.h"

#include "clang/private/c_BasicTypes.h"
#include "clang/private/Debug/c_Assert.h"
#include "clang/private/Handlers/c_IFallbackHandler.h"
#include "clang/private/Messages/c_IMessage.h"

#include "clang/c_Address.h"
#include "clang/c_Defines.h"

namespace clang
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
} // namespace clang


#endif // __XLANG_PRIVATE_HANDLERS_BLINDFALLBACKHANDLER_H

