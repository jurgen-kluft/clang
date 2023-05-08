#ifndef __XLANG_REGISTER_H
#define __XLANG_REGISTER_H
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE 
#pragma once 
#endif

#include "clang/private/Messages/c_MessageTraits.h"

#define XLANG_NO_REGISTER_MESSAGE
#ifndef XLANG_NO_REGISTER_MESSAGE

/**
\brief Message type registration macro.

Registers message types for use within clang.

Registration of message types is optional in clang. Registering the message
types used in an application causes clang to use hand-rolled Runtime Type
Information (RTTI) explicity stored with each message type, instead of the
built-in C++ RTTI. Doing so allows the built-in C++ RTTI to be turned off,
resulting in a memory storage saving in all types, not just message types.
This saving can be important in embedded and realtime applications such as
games, where for example a Vector class may need to be stored as just three
floats, with no additional runtime type information added silently by the
compiler.

An important limitation of the message type registration macro is that it
can only be used from within the global namespace. Furthermore the full
name of the message type must be given, including all namespace scoping.

\code
namespace MyNamespace
{

	class MyMessage
	{
	};

}

XLANG_REGISTER_MESSAGE(MyNamespace::MyMessage);
\endcode

(Unfortunately this means that it isn't generally possible to register messages
immediately after their declaration, as we'd often prefer).

\note If message types are registered, then \em all message types
must be registered, throughout the whole application, otherwise runtime
errors will occur. Therefore it is simpler and less error prone to not
register any message types at all. Only bother with message registration
if you specifically need to turn the built-in C++ RTTI system off.

\see <a href="http://www.theron-library.com/index.php?t=page&p=RegisteringMessages">Registering messages</a>
*/
#define XLANG_REGISTER_MESSAGE(MessageType)											\
namespace clang																		\
{																					\
	namespace detail																\
	{																				\
		template <typename T>														\
		struct MessageTraits;														\
		template <>																	\
		struct MessageTraits<MessageType>											\
		{																			\
			static const bool HAS_TYPE_NAME = true;									\
			static const char *const TYPE_NAME;										\
		};																			\
																					\
		const char *const MessageTraits<MessageType>::TYPE_NAME = #MessageType;		\
	}																				\
}

#endif // XLANG_REGISTER_MESSAGE


#endif // XLANG_REGISTER_H

