#ifndef __XLANG_PRIVATE_HANDLERS_MESSAGECAST_H
#define __XLANG_PRIVATE_HANDLERS_MESSAGECAST_H
#include "cbase/c_target.h"
#ifdef USE_PRAGMA_ONCE 
#pragma once 
#endif

#include "clang/private/Debug/c_Assert.h"
#include "clang/private/Messages/c_IMessage.h"
#include "clang/private/Messages/c_Message.h"
#include "clang/private/Messages/c_MessageTraits.h"

#include "clang/c_Defines.h"


namespace clang
{
	namespace detail
	{
		/// \brief Dynamic cast utility for message pointers.
		/// A cast utility that can be used to dynamically cast a message of unknown type
		/// to a message of a known type at runtime, using stored runtime type information.
		/// If the unknown message is of the target type then the cast succeeds and a pointer
		/// to the typecast message is returned, otherwise a null pointer is returned.
		///
		/// This utility roughly mimics the functionality of dynamic_cast, but includes
		/// two alternate implementations: one that uses dynamic_cast directly and another
		/// that rolls its own runtime type information only for message classes. The
		/// advantage of the second implementation is that the storage overhead of the extra
		/// runtime type information is not imposed on \em all classes, as with the C++ RTTI.
		/// If the second implementation is used consistently then dynamic_cast is not
		/// called at all, using a partial template specialization trick, and the C++ RTTI
		/// functionality can be turned off (usually by means of a compiler option).
		///
		/// \note Partial template specialization is used here as a device to avoid
		/// introducing a hardcoded dependency on C++ RTTI. The dependency can be avoided
		/// by defining non-zero type IDs for all message types used in the application.
		///
		/// \tparam HAS_TYPE_ID A flag indicating whether the message type has a name.
		template <bool HAS_TYPE_ID>
		class MessageCast
		{
		public:

			/// \brief Attempts to convert a given message, of unknown type, to one of a target type.
			/// Returns a null pointer if the message is of the wrong type.
			/// \tparam ValueType The value type of the target message.
			/// \param message A pointer to the message of unknown type.
			/// \return A pointer to the converted message, or null if the types don't match.
			template <class ValueType>
			XLANG_FORCEINLINE static const Message<ValueType> *CastMessage(const IMessage *const message)
			{
				XLANG_ASSERT(message);

				// If explicit type names are used then they must be defined for all message types.
				XLANG_ASSERT_MSG(message->TypeId()>=0, "Message type has null type name");

				// Check the type of the message using the type name it carries, which was set on creation.
				if (message->TypeId() == type2int<ValueType>::value())
				{
					// Hard-convert the given message to the indicated type.
					return reinterpret_cast<const Message<ValueType> *>(message);
				}

				return 0;
			}
		};


	} // namespace detail
} // namespace clang


#endif // __XLANG_PRIVATE_HANDLERS_MESSAGECAST_H

