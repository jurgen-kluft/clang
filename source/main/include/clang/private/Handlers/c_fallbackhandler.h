#ifndef __XLANG_PRIVATE_HANDLERS_FALLBACKHANDLER_H
#define __XLANG_PRIVATE_HANDLERS_FALLBACKHANDLER_H

#include "clang/private/Debug/c_Assert.h"
#include "clang/private/Handlers/c_IFallbackHandler.h"
#include "clang/private/Messages/c_IMessage.h"

#include "clang/c_Address.h"
#include "clang/c_Defines.h"


namespace clang
{
	namespace detail
	{
		/// Instantiable class template that remembers a fallback message handler function
		/// registered with a framework and called for messages that are undelivered or unhandled.
		/// 
		/// \tparam ObjectType The class on which the handler function is a method.
		template <class ObjectType>
		class FallbackHandler : public IFallbackHandler
		{
		public:

			/// Pointer to a member function of a handler object.
			typedef void (ObjectType::*HandlerFunction)(const Address from);

			/// Constructor.
			XLANG_FORCEINLINE FallbackHandler(ObjectType *const object, HandlerFunction function) 
				: mObject(object)
				, mHandlerFunction(function)
			{
			}

			/// Virtual destructor.
			inline virtual ~FallbackHandler()
			{
			}

			/// Handles the given message.
			inline virtual void Handle(const IMessage *const message) const
			{
				XLANG_ASSERT(mObject);
				XLANG_ASSERT(mHandlerFunction);
				XLANG_ASSERT(message);

				// Call the handler, passing it the from address.
				(mObject->*mHandlerFunction)(message->From());
			}

		private:

			FallbackHandler(const FallbackHandler &other);
			FallbackHandler &operator=(const FallbackHandler &other);

			ObjectType *mObject;                        ///< Pointer to the object owning the handler function.
			const HandlerFunction mHandlerFunction;     ///< Pointer to the handler member function on the owning object.
		};


	} // namespace detail
} // namespace clang


#endif // __XLANG_PRIVATE_HANDLERS_FALLBACKHANDLER_H

