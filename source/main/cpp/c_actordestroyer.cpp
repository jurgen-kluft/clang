#include "clang/private/Core/c_ActorCore.h"
#include "clang/private/Core/c_ActorDestroyer.h"
#include "clang/private/Debug/c_Assert.h"
#include "clang/private/Directory/c_ActorDirectory.h"
#include "clang/private/Directory/c_Directory.h"
#include "clang/private/Threading/c_Lock.h"

#include "clang/c_Actor.h"
#include "clang/c_AllocatorManager.h"


namespace clang
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
} // namespace clang

