#ifndef __XLANG_PRIVATE_DIRECTORY_RECEIVERDIRECTORY_H
#define __XLANG_PRIVATE_DIRECTORY_RECEIVERDIRECTORY_H

#include "clang/private/c_BasicTypes.h"
#include "clang/private/Debug/c_Assert.h"
#include "clang/private/PagedPool/c_PagedPool.h"
#include "clang/private/Threading/c_Mutex.h"

#include "clang/c_Address.h"
#include "clang/c_Defines.h"


namespace clang
{
	class Receiver;

	namespace detail
	{
		/// A directory mapping unique addresses to receivers.
		class ReceiverDirectory
		{
		public:

			/// Gets a reference to the single directory object.
			inline static ReceiverDirectory &Instance();

			/// Default constructor.
			inline ReceiverDirectory();

			/// Returns the number of receivers currently registered.
			inline u32 Count() const;

			/// Registers a receiver and returns its unique address.
			Address RegisterReceiver(Receiver *const receiver);

			/// Deregisters the receiver at the given address.
			/// \note The address can be the address of a currently registered receiver.
			bool DeregisterReceiver(const Address &address);

			/// Gets a pointer to the receiver at the given address.
			/// \note The address can be the address of a currently registered receiver.
			Receiver *GetReceiver(const Address &address) const;

		private:

			typedef PagedPool<Receiver *, XLANG_MAX_RECEIVERS> ReceiverPool;

			ReceiverDirectory(const ReceiverDirectory &other);
			ReceiverDirectory &operator=(const ReceiverDirectory &other);

			static ReceiverDirectory smInstance;    ///< Single, static instance of the class.

			ReceiverPool mReceiverPool;             ///< Pool of pointers to user-allocated receivers.
		};


		XLANG_FORCEINLINE ReceiverDirectory &ReceiverDirectory::Instance()
		{
			return smInstance;
		}


		XLANG_FORCEINLINE ReceiverDirectory::ReceiverDirectory() : mReceiverPool()
		{
		}


		XLANG_FORCEINLINE u32 ReceiverDirectory::Count() const
		{
			return mReceiverPool.Count();
		}


	} // namespace detail
} // namespace clang


#endif // __XLANG_PRIVATE_DIRECTORY_RECEIVERDIRECTORY_H

