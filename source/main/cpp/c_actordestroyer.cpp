#include "xlang/private/Core/x_ActorCore.h"
#include "xlang/private/Core/x_ActorDestroyer.h"
#include "xlang/private/Debug/x_Assert.h"
#include "xlang/private/Directory/x_ActorDirectory.h"
#include "xlang/private/Directory/x_Directory.h"
#include "xlang/private/Threading/x_Lock.h"

#include "xlang/x_Actor.h"
#include "xlang/x_AllocatorManager.h"


namespace xlang
{
	namespace detail
	{
		void ActorDestroyer::DestroyActor(ActorCore *const actorCore)
		{
			// Get a pointer to the actor itself and take its address, before we delete it.
			Actor *const actor(actorCore->GetParent());
			const Address address(actor->GetAddress());

			// Lock the directory to make sure no one can send the actor a message.
			Lock lock(Directory::GetMutex());

			// This seems to actually call the derived actor class destructor, as we want.
			actor->~Actor();
			AllocatorManager::Instance().GetAllocator()->Free(actor);

			// Destroy the actor core and deregister it from its registered address.
			// We have to destroy the core after the actor, and not before, in case
			// the actor does something that depends on the core in its destructor,
			// for example if it sends a message or even creates another actor.
			if (!ActorDirectory::Instance().DeregisterActor(address))
			{
				// Failed to deregister actor core.
				XLANG_FAIL();
			}
		}


	} // namespace detail
} // namespace xlang

