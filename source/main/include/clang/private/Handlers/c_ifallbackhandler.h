#ifndef __XLANG_PRIVATE_HANDLERS_IFALLBACKHANDLER_H
#define __XLANG_PRIVATE_HANDLERS_IFALLBACKHANDLER_H

#include "clang/private/c_BasicTypes.h"
#include "clang/private/Messages/c_IMessage.h"

#include "clang/c_Defines.h"


namespace clang
{
	namespace detail
	{
		/// Interface that allows a per-framework fallback message handler to be referenced.
		class IFallbackHandler
		{
		public:

			/// Default constructor.
			XLANG_FORCEINLINE IFallbackHandler()
			{
			}

			/// Virtual destructor.
			inline virtual ~IFallbackHandler()
			{
			}

			/// Handles the given message.
			virtual void Handle(const IMessage *const message) const = 0;

		private:

			IFallbackHandler(const IFallbackHandler &other);
			IFallbackHandler &operator=(const IFallbackHandler &other);
		};


	} // namespace detail
} // namespace clang


#endif // __XLANG_PRIVATE_HANDLERS_IFALLBACKHANDLER_H

