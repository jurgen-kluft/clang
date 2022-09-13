#ifndef __XLANG_PRIVATE_HANDLERS_DEFAULTFALLBACKHANDLER_H
#define __XLANG_PRIVATE_HANDLERS_DEFAULTFALLBACKHANDLER_H

#if XLANG_ENABLE_UNHANDLED_MESSAGE_CHECKS
	#include <stdio.h>
#endif // XLANG_ENABLE_UNHANDLED_MESSAGE_CHECKS

#include "clang/private/c_BasicTypes.h"
#include "clang/private/Debug/c_Assert.h"

#include "clang/c_Defines.h"

#if XLANG_ENABLE_UNHANDLED_MESSAGE_CHECKS
	#define XLANG_FALLBACK_HANDLER_ARG(x) x
#else
	#define XLANG_FALLBACK_HANDLER_ARG(x)
#endif //XLANG_ENABLE_UNHANDLED_MESSAGE_CHECKS

namespace clang
{
	namespace detail
	{
		class DefaultFallbackHandler
		{
		public:

			inline void Handle(const void *const data, const u32 size, const Address from);
		};


		inline void DefaultFallbackHandler::Handle(
			const void *const XLANG_FALLBACK_HANDLER_ARG(data),
			const u32 XLANG_FALLBACK_HANDLER_ARG(size),
			const Address XLANG_FALLBACK_HANDLER_ARG(from))
		{
#if XLANG_ENABLE_UNHANDLED_MESSAGE_CHECKS
			fprintf(stderr, "Unhandled message of %d bytes sent from address %d:\n", size, from.AsInteger());

			// Dump the message data as hex words.
			if (data)
			{
				const char *const format("[%d] 0x%08x\n");

				const unsigned int *const begin(reinterpret_cast<const unsigned int *>(data));
				const unsigned int *const end(begin + size / sizeof(unsigned int));

				for (const unsigned int *word(begin); word != end; ++word)
				{
					fprintf(stderr, format, word - begin, *word);
				}
			}

			XLANG_FAIL();

#endif // XLANG_ENABLE_UNHANDLED_MESSAGE_CHECKS
		}


	} // namespace detail
} // namespace clang


#undef XLANG_FALLBACK_HANDLER_ARG


#endif // __XLANG_PRIVATE_HANDLERS_DEFAULTFALLBACKHANDLER_H

