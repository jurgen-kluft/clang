#include "clang/private/Directory/c_ActorDirectory.h"
#include "clang/private/Debug/c_Assert.h"

#include "clang/c_Actor.h"
#include "clang/c_Framework.h"

// Placement new/delete
inline void*	operator new(xcore::xsize_t num_bytes, void* mem)			{ return mem; }
inline void	operator delete(void* mem, void* )							{ }

namespace clang
{
	namespace detail
	{
		ActorDirectory ActorDirectory::smInstance;

		Address ActorDirectory::RegisterActor(Framework *const framework, Actor *const actor)
		{
			u32 index(0);
			if (mActorPool.Allocate(index))
			{
				const Address address(Address::MakeActorAddress(index));

				// The entry in the actor pool is memory for an ActorCore object.
				void *const entry(mActorPool.GetEntry(index));
				XLANG_ASSERT(entry);

				// Construct the actor core, initializing its sequence number.
				// The sequence number is used to authenticate it later.
				new (entry) ActorCore(address.GetSequence(), framework, actor);

				return address;
			}

			return Address::Null();
		}


		bool ActorDirectory::DeregisterActor(const Address &address)
		{
			XLANG_ASSERT(Address::IsActorAddress(address));
			const u32 index(address.GetIndex());

			// The entry in the actor pool is memory for an ActorCore object.
			void *const entry(mActorPool.GetEntry(index));
			if (entry)
			{
				// Reject the actor core if its address (ie. sequence number) doesn't match.
				// This guards against new actor cores constructed at the same indices as old ones.
				ActorCore *const actorCore(reinterpret_cast<ActorCore *>(entry));
				if (actorCore->GetSequence() == address.GetSequence())
				{
					// Null the sequence number of the actor core so it doesn't match any future queries.
					// Zero is an invalid sequence number which isn't assigned to any actor.
					actorCore->SetSequence(0);

					// Destruct the actor core manually since it's buffer-allocated so we can't call delete.
					actorCore->~ActorCore();
					if (mActorPool.Free(index))
					{
						return true;
					}
				}
			}

			return false;
		}


	} // namespace detail
} // namespace clang


