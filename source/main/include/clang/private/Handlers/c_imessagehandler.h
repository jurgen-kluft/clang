#ifndef __XLANG_PRIVATE_HANDLERS_IMESSAGEHANDLER_H
#define __XLANG_PRIVATE_HANDLERS_IMESSAGEHANDLER_H

#include "clang/private/c_BasicTypes.h"
#include "clang/private/Messages/c_IMessage.h"

#include "clang/c_Defines.h"

namespace clang
{
	class Actor;

	namespace detail
	{
		/// Baseclass that allows message handlers of various types to be stored in lists.
		class IMessageHandler
		{
		public:
			/// Default constructor.
			XLANG_FORCEINLINE IMessageHandler() : mMarked(0)
			{
			}

			/// Virtual destructor.
			inline virtual ~IMessageHandler()
			{
			}

			/// Marks the handler (eg. for deletion).    
			inline void Mark();

			/// Returns true if the handler is marked (eg. for deletion).    
			inline bool IsMarked() const;

			/// Returns the unique name of the message type handled by this handler.
			virtual int GetMessageTypeId() const = 0;

			/// Handles the given message, if it's of the type accepted by the handler.
			/// \return True, if the handler handled the message.
			virtual bool Handle(Actor *const actor, const IMessage *const message) const = 0;

		private:
			IMessageHandler(const IMessageHandler &other);
			IMessageHandler &operator=(const IMessageHandler &other);

			u64 mMarked;           ///< Flag used to mark the handler for deletion.
		};

		XLANG_FORCEINLINE void IMessageHandler::Mark()
		{
			mMarked |= 1;
		}

		XLANG_FORCEINLINE bool IMessageHandler::IsMarked() const
		{
			return (mMarked & 1) == 1;
		}


	} // namespace detail
} // namespace clang


#endif // __XLANG_PRIVATE_HANDLERS_IMESSAGEHANDLER_H

