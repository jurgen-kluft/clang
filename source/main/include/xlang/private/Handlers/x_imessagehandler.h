#ifndef __XLANG_PRIVATE_HANDLERS_IMESSAGEHANDLER_H
#define __XLANG_PRIVATE_HANDLERS_IMESSAGEHANDLER_H

#include "xlang\private\x_BasicTypes.h"
#include "xlang\private\Messages\x_IMessage.h"

#include "xlang\x_Defines.h"

namespace xlang
{
	class Actor;

	namespace detail
	{
		/// Baseclass that allows message handlers of various types to be stored in lists.
		class IMessageHandler
		{
		public:

			/// Default constructor.
			XLANG_FORCEINLINE IMessageHandler() : mNext(0), mMarked(false)
			{
			}

			/// Virtual destructor.
			inline virtual ~IMessageHandler()
			{
			}

			/// Sets the pointer to the next message handler in a list of handlers.
			inline void SetNext(IMessageHandler *const next);

			/// Gets the pointer to the next message handler in a list of handlers.
			inline IMessageHandler *GetNext() const;

			/// Marks the handler (eg. for deletion).    
			inline void Mark();

			/// Returns true if the handler is marked (eg. for deletion).    
			inline bool IsMarked() const;

			/// Returns the unique name of the message type handled by this handler.
			virtual const char *GetMessageTypeName() const = 0;

			/// Handles the given message, if it's of the type accepted by the handler.
			/// \return True, if the handler handled the message.
			virtual bool Handle(Actor *const actor, const IMessage *const message) const = 0;

		private:

			IMessageHandler(const IMessageHandler &other);
			IMessageHandler &operator=(const IMessageHandler &other);

			IMessageHandler *mNext; ///< Pointer to the next handler in a list of handlers.
			bool mMarked;           ///< Flag used to mark the handler for deletion.
		};


		XLANG_FORCEINLINE void IMessageHandler::SetNext(IMessageHandler *const next)
		{
			mNext = next;
		}


		XLANG_FORCEINLINE IMessageHandler *IMessageHandler::GetNext() const
		{
			return mNext;
		}


		XLANG_FORCEINLINE void IMessageHandler::Mark()
		{
			mMarked = true;
		}


		XLANG_FORCEINLINE bool IMessageHandler::IsMarked() const
		{
			return mMarked;
		}


	} // namespace detail
} // namespace xlang


#endif // __XLANG_PRIVATE_HANDLERS_IMESSAGEHANDLER_H
