#ifndef __XLANG_PRIVATE_HANDLERS_IRECEIVERHANDLER_H
#define __XLANG_PRIVATE_HANDLERS_IRECEIVERHANDLER_H

#include "clang/private/c_BasicTypes.h"
#include "clang/private/Messages/c_IMessage.h"

#include "clang/c_Defines.h"

namespace clang
{
	namespace detail
	{
		/// Baseclass that allows message handlers of various types to be stored in lists.
		class IReceiverHandler
		{
		public:

			/// Default constructor.
			XLANG_FORCEINLINE IReceiverHandler() : mNext(0)
			{
			}

			/// Virtual destructor.
			inline virtual ~IReceiverHandler()
			{
			}

			/// Sets the pointer to the next message handler in a list of handlers.
			inline void SetNext(IReceiverHandler *const next);

			/// Gets the pointer to the next message handler in a list of handlers.
			inline IReceiverHandler *GetNext() const;

			/// Returns the unique name of the message type handled by this handler.
			virtual int GetMessageTypeId() const = 0;

			/// Handles the given message, if it's of the type accepted by the handler.
			/// \return True, if the handler handled the message.
			virtual bool Handle(const IMessage *const message) const = 0;

		private:

			IReceiverHandler(const IReceiverHandler &other);
			IReceiverHandler &operator=(const IReceiverHandler &other);

			IReceiverHandler *mNext;    ///< Pointer to the next handler in a list of handlers.
		};


		XLANG_FORCEINLINE void IReceiverHandler::SetNext(IReceiverHandler *const next)
		{
			mNext = next;
		}


		XLANG_FORCEINLINE IReceiverHandler *IReceiverHandler::GetNext() const
		{
			return mNext;
		}


	} // namespace detail
} // namespace clang


#endif // __XLANG_PRIVATE_HANDLERS_IRECEIVERHANDLER_H

