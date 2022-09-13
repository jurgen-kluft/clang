#ifndef __XLANG_PRIVATE_CORE_ACTORCREATOR_H
#define __XLANG_PRIVATE_CORE_ACTORCREATOR_H

#include "clang/private/c_BasicTypes.h"
#include "clang/private/Core/c_ActorAlignment.h"
#include "clang/private/Core/c_ActorCore.h"
#include "clang/private/Debug/c_Assert.h"
#include "clang/private/Directory/c_ActorDirectory.h"
#include "clang/private/Directory/c_Directory.h"
#include "clang/private/Threading/c_Lock.h"
#include "clang/private/Threading/c_Mutex.h"

#include "clang/c_Address.h"
#include "clang/c_AllocatorManager.h"
#include "clang/c_Defines.h"


namespace clang
{
	class Framework;

	namespace detail
	{
		/// Helper class that creates actors.
		/// When an actor is constructed, the Actor constructor fetches
		/// a pointer to its referenced actor core from this registry, where
		/// it has been set by the framework prior to construction. This hack
		/// allows us to pass actor core pointers to actor constructors without
		/// actually using a constructor parameter.
		class ActorCreator
		{
		public:

			friend class clang::Framework;

			/// Gets the remembered actor address.
			inline static Address GetAddress();

			/// Gets the remembered actor core pointer.
			inline static ActorCore *GetCoreAddress();

		private:
			ActorCreator();
			ActorCreator(const ActorCreator &other);
			ActorCreator &operator=(const ActorCreator &other);

			/// Creates an instance of an actor type via a provided constructor object.
			/// \note This method can only be called by the Framework.
			template <class ConstructorType>
			inline static typename ConstructorType::ActorType *CreateActor(const ConstructorType &constructor, Framework *const framework);

			/// Sets the remembered actor address.
			inline static void SetAddress(const Address &address);

			/// Sets the remembered actor core pointer.
			inline static void SetCoreAddress(ActorCore *const address);

			static Mutex smMutex;                   ///< Synchronizes access to the singleton instance.
			static Address smAddress;               ///< Remembered actor address.
			static ActorCore *smCoreAddress;        ///< Remembered actor core pointer.
		};


		XLANG_FORCEINLINE void ActorCreator::SetAddress(const Address &address)
		{
			smAddress = address;
		}


		XLANG_FORCEINLINE void ActorCreator::SetCoreAddress(ActorCore *const address)
		{
			smCoreAddress = address;
		}


		XLANG_FORCEINLINE Address ActorCreator::GetAddress()
		{
			return smAddress;
		}


		XLANG_FORCEINLINE ActorCore *ActorCreator::GetCoreAddress()
		{
			return smCoreAddress;
		}


		template <class ConstructorType>
		XLANG_FORCEINLINE typename ConstructorType::ActorType *ActorCreator::CreateActor(
			const ConstructorType &constructor,
			Framework *const framework)
		{
			typedef typename ConstructorType::ActorType ActorType;

			IAllocator *const allocator(AllocatorManager::Instance().GetAllocator());
			Address address(Address::Null());
			ActorCore *actorCore(0);

			// Allocate a separate, aligned memory block for the actor itself.
			const u32 size(sizeof(ActorType));
			const u32 alignment(ActorAlignment<ActorType>::ALIGNMENT);

			void *const actorMemory = allocator->AllocateAligned(size, alignment);
			if (actorMemory)
			{
				bool registered(false);

				{
					Lock lock(Directory::GetMutex());
					ActorDirectory &directory(ActorDirectory::Instance());

					// Register and construct the actor core, passing it the framework and referencing actor.
					// This basically only fails if we run out of memory.
					Actor *const tempActor = reinterpret_cast<Actor *>(actorMemory);
					address = directory.RegisterActor(framework, tempActor);

					if (address != Address::Null())
					{
						actorCore = directory.GetActor(address);
						registered = true;
					}
				}

				if (registered)
				{
					ActorType *actor(0);

					{
						// Lock the mutex and set the static actor core address for the actor core
						// constructor to read during construction. This is an awkward workaround for the
						// inability to pass it via the actor constructor parameters, which we don't control.
						Lock lock(smMutex);

						SetAddress(address);
						SetCoreAddress(actorCore);

						// Use the provided constructor helper to construct the actor within the provided
						// memory buffer. The derived actor class constructor may itself send messages or
						// construct other actors.
						actor = constructor(actorMemory);
					}

					return actor;
				}

				allocator->Free(actorMemory);
			}

			return 0;
		}


	} // namespace detail
} // namespace clang


#endif // __XLANG_PRIVATE_CORE_ACTORCREATOR_H

