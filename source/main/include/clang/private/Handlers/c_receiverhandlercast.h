#ifndef __XLANG_PRIVATE_HANDLERS_RECEIVERHANDLERCAST_H
#define __XLANG_PRIVATE_HANDLERS_RECEIVERHANDLERCAST_H

#include "xlang/private/Debug/x_Assert.h"
#include "xlang/private/Handlers/x_IReceiverHandler.h"
#include "xlang/private/Handlers/x_ReceiverHandler.h"
#include "xlang/private/Messages/x_MessageTraits.h"

#include "xlang/x_Defines.h"


namespace xlang
{
	namespace detail
	{


		/// \brief Dynamic cast utility for message handler pointers.
		template <class ObjectType, bool HAS_TYPE_NAME>
		class ReceiverHandlerCast
		{
		public:

			/// \brief Attempts to convert a given message handler, of unknown type, to one of a target type.
			template <class ValueType>
			XLANG_FORCEINLINE static const ReceiverHandler<ObjectType, ValueType> *CastHandler(const IReceiverHandler *const handler)
			{
				XLANG_ASSERT(handler);

				// If explicit type names are used then they must be defined for all message types.
				XLANG_ASSERT_MSG(handler->GetMessageTypeId()>=0, "Missing type name for message type");

				// Compare the handlers using type names.
				if (handler->GetMessageTypeId() != type2int<ValueType>::value())
				{
					return 0;
				}

				// Convert the given message handler to a handler for the known type.
				typedef ReceiverHandler<ObjectType, ValueType> HandlerType;
				return reinterpret_cast<const HandlerType *>(handler);
			}
		};

#ifndef XLANG_NO_DYNAMIC_CAST
		// Specialization of the ReceiverHandlerCast for the case where the message type has no type name.
		// This specialization uses C++ built-in RTTI instead of explicitly stored type names.
		template <class ObjectType>
		class ReceiverHandlerCast<ObjectType, false>
		{
		public:

			/// Attempts to convert the given message handler, of unknown message type, to a handler of the given type.
			/// \note Returns a null pointer if the unknown handler is of the wrong type.
			template <class ValueType>
			XLANG_FORCEINLINE static const ReceiverHandler<ObjectType, ValueType> *CastHandler(const IReceiverHandler *const handler)
			{
				XLANG_ASSERT(handler);

				// Explicit type names must be defined for all message types or none at all.
				XLANG_ASSERT_MSG(handler->GetMessageTypeId()>=0, "Type names specified for only some message types!");

				// Try to convert the given message handler to this type.
				typedef ReceiverHandler<ObjectType, ValueType> HandlerType;
				return dynamic_cast<const HandlerType *>(handler);
			}
		};
#endif

	} // namespace detail
} // namespace xlang


#endif // __XLANG_PRIVATE_HANDLERS_RECEIVERHANDLERCAST_H

