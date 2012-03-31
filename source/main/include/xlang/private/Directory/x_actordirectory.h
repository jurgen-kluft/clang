#ifndef __XLANG_PRIVATE_DIRECTORY_ACTORDIRECTORY_H
#define __XLANG_PRIVATE_DIRECTORY_ACTORDIRECTORY_H

#include "xlang\private\x_BasicTypes.h"
#include "xlang\private\Core\x_ActorCore.h"
#include "xlang\private\Debug\x_Assert.h"
#include "xlang\private\PagedPool\x_PagedPool.h"

#include "xlang\x_Address.h"
#include "xlang\x_Defines.h"


namespace xlang
{
	class Framework;
	class Actor;

	namespace detail
	{
		/// A directory mapping unique addresses to actors.
		/// The directory is also a pool in which actor cores are allocated.
		class ActorDirectory
		{
		public:

			/// Gets a reference to the single directory object.
			inline static ActorDirectory &Instance();

			/// Default constructor.
			inline ActorDirectory();

			/// Returns the number of actors currently registered.
			inline u32 Count() const;

			/// Registers an actor and returns its unique address.
			Address RegisterActor(Framework *const framework, Actor *const actor);

			/// Deregisters the actor at the given address.
			/// \note The address can be the address of a currently registered actor.
			bool DeregisterActor(const Address &address);

			/// Gets a pointer to the actor core at the given address.
			/// \note The address can be the address of a currently registered actor.
			inline ActorCore *GetActor(const Address &address) const;

		private:
			typedef PagedPool<ActorCore, XLANG_MAX_ACTORS> ActorPool;

			ActorDirectory(const ActorDirectory &other);
			ActorDirectory &operator=(const ActorDirectory &other);

			static ActorDirectory smInstance;       ///< Single, static instance of the class.

			ActorPool mActorPool;                   ///< Pool of system-allocated actor cores.
		};


		XLANG_FORCEINLINE ActorDirectory &ActorDirectory::Instance()
		{
			return smInstance;
		}

		XLANG_FORCEINLINE ActorDirectory::ActorDirectory() : mActorPool()
		{
		}

		XLANG_FORCEINLINE u32 ActorDirectory::Count() const
		{
			return mActorPool.Count();
		}

		XLANG_FORCEINLINE ActorCore *ActorDirectory::GetActor(const Address &address) const
		{
			XLANG_ASSERT(Address::IsActorAddress(address));
			const u32 index(address.GetIndex());

			// The entry in the actor pool is memory for an ActorCore object.
			void *const entry(mActorPool.GetEntry(index));
			if (entry)
			{
				// Reject the actor core if its sequence number doesn't match.
				// This guards against new actor cores constructed at the same indices as old ones.
				ActorCore *const actorCore(reinterpret_cast<ActorCore *>(entry));
				if (actorCore->GetSequence() == address.GetSequence())
				{
					return actorCore;
				}
			}

			return 0;
		}


	} // namespace detail
} // namespace xlang


#endif // __XLANG_PRIVATE_DIRECTORY_ACTORDIRECTORY_H

