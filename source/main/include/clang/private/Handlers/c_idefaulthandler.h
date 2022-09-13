#ifndef __XLANG_PRIVATE_HANDLERS_IDEFAULTHANDLER_H
#define __XLANG_PRIVATE_HANDLERS_IDEFAULTHANDLER_H

#include "clang/private/Messages/c_IMessage.h"

namespace clang
{
	class Actor;

	namespace detail
	{
		/// Interface that allows a default handler on an unknown actor to be referenced.
		class IDefaultHandler
		{
		public:

			/// Default constructor.
			inline IDefaultHandler()
			{
			}

			/// Virtual destructor.
			inline virtual ~IDefaultHandler()
			{
			}

			/// Handles the given message.
			virtual void Handle(Actor *const actor, const IMessage *const message) const = 0;
		};


	} // namespace detail
} // namespace clang


#endif // __XLANG_PRIVATE_HANDLERS_IDEFAULTHANDLER_H

