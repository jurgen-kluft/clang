#ifndef __XLANG_ADDRESS_H
#define __XLANG_ADDRESS_H

/**
\file Address.h
Address object, the unique name of an actor.
*/

#include "xlang\private\x_BasicTypes.h"
#include "xlang\private\Threading\x_Mutex.h"
#include "xlang\private\Threading\x_Lock.h"

#include "xlang\x_Defines.h"

namespace xlang
{
	namespace detail
	{
		class ActorDirectory;
		class ReceiverDirectory;
	}

	/**
	\brief The unique address of an entity that can send or receive messages.

	Addresses are the unique 'names' of entities that can receive messages in xlang.
	Knowing an entity's address is enough to be able to send it a message, and no other
	special access or permissions are required.

	The types of entities that have addresses, and therefore can receive messages, in
	xlang are \ref Actor "actors" and \ref Receiver "receivers". Both of these types
	of entities are assigned automatically-generated unique addresses on construction.

	Addresses can be copied and assigned, allowing the addresses of actors and receivers
	to be \ref Actor::Send "sent in messages" to other actors. The only way to generate
	a valid address is by querying and copying the address of an actor or receiver,
	ensuring that addresses remain unique.

	Default-constructed addresses are \em null and are equal to the unique null address
	returned by the static \ref Address::Null method, which is guaranteed not to be
	equal to the address of any addressable entity.

	Because knowing the address of an actor means being able to send it a message,
	addresses constitute the security and encapsulation mechanism within \ref Actor "actor"
	subsystems. If an actor creates a collection of 'child' actors that it owns and manages,
	then no code outside that actor can access the child actors or send them messages,
	unless the actor publicly exposes their addresses.

	\note The maximum numbers of actor and receiver addresses in xlang are limited
	by the \ref XLANG_MAX_ACTORS and \ref XLANG_MAX_RECEIVERS defines, which can
	both be overridden in user projects via project-level definitions.
	*/
	class Address
	{
	public:

		friend class detail::ActorDirectory;
		friend class detail::ReceiverDirectory;

		/**
		\brief Static method that returns the unique 'null' address.

		The null address is guaranteed not to be equal to the address of any actual entity.

		\code
		class Actor : public xlang::Actor
		{
		};

		xlang::Framework framework;
		xlang::ActorRef actor(framework.CreateActor<Actor>());

		assert(actor.GetAddress() != Address::Null(), "Created actor has null address!");
		\endcode

		\return The unique null address.
		*/
		inline static Address Null()
		{
			return Address();
		}

		/**
		\brief Returns true if the given address is an actor address, false if it is a receiver address.

		\note This refers purely to the 'kind' of the address and doesn't check whether the address
		actually references a valid actor or receiver.
		*/
		XLANG_FORCEINLINE static bool IsActorAddress(const Address &address)
		{
			// Receiver addresses are flagged with a bit flag in their index fields.
			return ((address.mIndex & RECEIVER_FLAG) == 0);
		}

		/**
		\brief Default constructor.

		Constructs a null address, equal to the address returned by Address::Null().
		*/
		XLANG_FORCEINLINE Address() : mSequence(0), mIndex(0)
		{
		}

		/**
		\brief Copy constructor.

		Addresses can be assigned. The new address is equal to the original and identifies the same entity.

		\code
		class Actor : public xlang::Actor
		{
		};

		xlang::Framework framework;
		xlang::ActorRef actor(framework.CreateActor<Actor>());
		xlang::Address address(actor.GetAddress());
		\endcode

		\param other The existing address to be copied.
		*/
		XLANG_FORCEINLINE Address(const Address &other) : mSequence(other.mSequence), mIndex(other.mIndex)
		{
		}

		/**
		\brief Assignment operator.

		Addresses can be assigned. The new address is equal to the original and identifies the same entity.

		\code
		class Actor : public xlang::Actor
		{
		};

		xlang::Framework framework;
		xlang::ActorRef actor(framework.CreateActor<Actor>());

		xlang::Address address;
		address = actor.GetAddress();
		\endcode

		\param other The existing address whose value is to be copied.
		*/
		XLANG_FORCEINLINE Address &operator=(const Address &other)
		{
			mSequence = other.mSequence;
			mIndex = other.mIndex;
			return *this;
		}

		/**
		\brief Gets the unique value of the address as an unsigned integer.

		\code
		class Actor : public xlang::Actor
		{
		};

		xlang::Framework framework;
		xlang::ActorRef actor(framework.CreateActor<Actor>());

		printf("Actor has address %d\n", actor.GetAddress().AsInteger());
		\endcode
		*/
		XLANG_FORCEINLINE uint32_t AsInteger() const
		{
			return mSequence;
		}

		/**
		\brief Equality operator.

		Returns true if two addresses are the same, otherwise false.

		\param other The address to be compared.
		\return True if the given address is the same as this one.
		*/
		XLANG_FORCEINLINE bool operator==(const Address &other) const
		{
			return (mSequence == other.mSequence);
		}

		/**
		\brief Inequality operator.

		Returns true if two addresses are different, otherwise false.

		\param other The address to be compared.
		\return True if the given address is different from this one.
		*/
		XLANG_FORCEINLINE bool operator!=(const Address &other) const
		{
			return (mSequence != other.mSequence);
		}

		/**
		\brief Less-than operator.

		This allows addresses to be sorted, for example in containers.

		\code
		class Actor : public xlang::Actor
		{
		};

		xlang::Framework framework;
		xlang::ActorRef actorOne(framework.CreateActor<Actor>());
		xlang::ActorRef actorTwo(framework.CreateActor<Actor>());

		std::set<xlang::Address> addresses;
		addresses.insert(actorOne.GetAddress());
		addresses.insert(actorTwo.GetAddress());
		\endcode
		*/
		XLANG_FORCEINLINE bool operator<(const Address &other) const
		{
			return (mSequence < other.mSequence);
		}

	private:

		static const uint32_t RECEIVER_FLAG = (1UL << 31);

		static detail::Mutex smMutex;       ///< Mutex that protects access to the static 'next value' member.
		static uint32_t smNextValue;        ///< Static member that remembers the next unique address value.

		/// \brief Returns the next unique address in sequence.
		inline static uint32_t GetNextSequenceNumber()
		{
			detail::Lock lock(smMutex);

			// Skip the special null value.
			if (smNextValue)
			{
				return smNextValue++;
			}

			smNextValue = 2;
			return 1;
		}

		XLANG_FORCEINLINE static Address MakeActorAddress(const uint32_t index)
		{
			const uint32_t sequence(GetNextSequenceNumber());
			return Address(sequence, index);
		}

		XLANG_FORCEINLINE static Address MakeReceiverAddress(const uint32_t index)
		{
			const uint32_t sequence(GetNextSequenceNumber());
			return Address(sequence, index | Address::RECEIVER_FLAG);
		}

		/// Constructor that accepts a specific value for the address.
		/// \param value The value for the newly constructed address.
		XLANG_FORCEINLINE Address(const uint32_t sequence, const uint32_t index) : mSequence(sequence), mIndex(index)
		{
		}

		XLANG_FORCEINLINE uint32_t GetSequence() const
		{
			return mSequence;
		}

		XLANG_FORCEINLINE uint32_t GetIndex() const
		{
			return (mIndex & (~RECEIVER_FLAG));
		}

		uint32_t mSequence;                 ///< Unique sequence number.
		uint32_t mIndex;                    ///< Pool index at which the addressed entity is registered.
	};


} // namespace xlang


#endif // THERON_ADDRESS_H

