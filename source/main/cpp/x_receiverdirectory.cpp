#include "xlang\private\Directory\x_ReceiverDirectory.h"
#include "xlang\private\Debug\x_Assert.h"

#include "xlang\x_Receiver.h"


namespace xlang
{
	namespace detail
	{
		ReceiverDirectory ReceiverDirectory::smInstance;

		Address ReceiverDirectory::RegisterReceiver(Receiver *const receiver)
		{
			u32 index(0);
			if (mReceiverPool.Allocate(index))
			{
				// Fill in the allocated entry with the pointer to the receiver.
				void *const entry(mReceiverPool.GetEntry(index));
				if (entry)
				{
					Receiver **const p = reinterpret_cast<Receiver **>(entry);
					*p = receiver;

					return Address::MakeReceiverAddress(index);
				}
			}

			return Address::Null();
		}


		bool ReceiverDirectory::DeregisterReceiver(const Address &address)
		{
			// Receiver addresses are flagged with a bit flag in the index fields.
			XLANG_ASSERT(!Address::IsActorAddress(address));
			const u32 index(address.GetIndex());

			if (mReceiverPool.Free(index))
			{
				return true;
			}

			return false;
		}


		Receiver *ReceiverDirectory::GetReceiver(const Address &address) const
		{
			// Receiver addresses are flagged with a bit flag in the index fields.
			XLANG_ASSERT(!Address::IsActorAddress(address));
			const u32 index(address.GetIndex());

			// The entry in the receiver pool is memory for a receiver pointer.
			void *const entry(mReceiverPool.GetEntry(index));
			if (entry)
			{
				Receiver **const p = reinterpret_cast<Receiver **>(entry);
				Receiver *const receiver(*p);

				// Reject the receiver if its address (ie. sequence number) doesn't match.
				// This guards against new receivers constructed at the same indices as old ones.
				if (receiver->GetAddress() == address)
				{
					return receiver;
				}
			}

			return 0;
		}


	} // namespace detail
} // namespace xlang


