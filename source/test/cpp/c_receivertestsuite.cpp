#define TESTS_TESTSUITES_RECEIVERTESTSUITE
#ifdef TESTS_TESTSUITES_RECEIVERTESTSUITE

#include "cbase\x_allocator.h"

#include "clang\private\x_BasicTypes.h"

#include "clang\private\Messages\x_IMessage.h"
#include "clang\private\Messages\x_Message.h"

#include "clang\x_Address.h"
#include "clang\x_AllocatorManager.h"
#include "clang\x_Framework.h"
#include "clang\x_Receiver.h"
#include "clang\x_Register.h"

#include "cunittest\cunittest.h"

// Placement new/delete
inline void*	operator new(ncore::xsize_t num_bytes, void* mem)			{ return mem; }
inline void	operator delete(void* mem, void* )							{ }

class MockMessage
{
public:

	inline explicit MockMessage(const clang::u32 value) : mValue(value)
	{
	}

	inline const clang::u32 &Value() const
	{
		return mValue;
	}

	inline clang::u32 &Value()
	{
		return mValue;
	}

	inline bool operator==(const MockMessage &other) const
	{
		return (mValue == other.mValue);
	}

	XCORE_CLASS_PLACEMENT_NEW_DELETE
private:
	clang::u32 mValue;
};


UNITTEST_SUITE_BEGIN(TESTS_TESTSUITES_RECEIVERTESTSUITE)
{
    UNITTEST_FIXTURE(main)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

		template <class ValueType>
		inline static clang::detail::IMessage *CreateMessage(const ValueType &value, const clang::Address &from)
		{
			typedef clang::detail::Message<ValueType> MessageType;

			clang::IAllocator &allocator(*clang::AllocatorManager::Instance().GetAllocator());

			const clang::u32 messageSize(MessageType::GetSize());
			const clang::u32 messageAlignment(MessageType::GetAlignment());

			void *const memory = allocator.AllocateAligned(messageSize, messageAlignment);
			clang::detail::IMessage *const message = MessageType::Initialize(memory, value, from);

			return message;
		}

		inline static void DestroyMessage(clang::detail::IMessage *const message)
		{
			clang::IAllocator &allocator(*clang::AllocatorManager::Instance().GetAllocator());
			allocator.Free(message->GetBlock());
		}


		class ResponderActor : public clang::Actor
		{
		public:

			inline ResponderActor()
			{
				RegisterHandler(this, &ResponderActor::Handler);
			}

			XCORE_CLASS_PLACEMENT_NEW_DELETE
		private:

			inline void Handler(const MockMessage &value, const clang::Address from)
			{
				Send(value, from);
			}
		};

		class Listener
		{
		public:

			inline Listener() 
				: mValue(0)
				, mFrom()
			{
			}

			inline explicit Listener(const MockMessage value)
				: mValue(value)
				, mFrom()
			{
			}

			inline MockMessage &Value()
			{
				return mValue;
			}

			inline clang::Address &From()
			{
				return mFrom;
			}

			inline void Handle(const MockMessage &message, const clang::Address from)
			{
				mValue = message;
				mFrom = from;
			}

		private:

			MockMessage mValue;
			clang::Address mFrom;
		};

		UNITTEST_TEST(TestConstruction)
		{
			clang::Receiver receiver;
		}

		UNITTEST_TEST(TestRegistration)
		{
			Listener listener;
			clang::Receiver receiver;
			receiver.RegisterHandler(&listener, &Listener::Handle);
		}

		UNITTEST_TEST(TestDeregistration)
		{
			Listener listener;
			clang::Receiver receiver;
			receiver.RegisterHandler(&listener, &Listener::Handle);
			receiver.DeregisterHandler(&listener, &Listener::Handle);
		}

		UNITTEST_TEST(TestPush)
		{
			Listener listener(MockMessage(1));
			clang::Receiver receiver;
			receiver.RegisterHandler(&listener, &Listener::Handle);

			const MockMessage value(2);
			clang::Address fromAddress;

			clang::detail::IMessage *const message = CreateMessage(value, fromAddress);
			CHECK_TRUE(message != 0);    // Failed to construct message");

			receiver.Push(message);
			CHECK_TRUE(listener.Value() == value);    // Registered handler not executed correctly");
			CHECK_TRUE(listener.From() == fromAddress);    // Registered handler not executed correctly");

			receiver.DeregisterHandler(&listener, &Listener::Handle);
		}

		UNITTEST_TEST(TestPushWithoutRegistration)
		{
			typedef clang::detail::Message<MockMessage> MessageType;

			Listener listener(MockMessage(1));
			clang::Receiver receiver;

			const MockMessage value(2);
			clang::Address fromAddress;

			clang::detail::IMessage *const message = CreateMessage(value, fromAddress);
			CHECK_TRUE(message != 0);    // Failed to construct message");

			receiver.Push(message);
			CHECK_TRUE(listener.Value() == MockMessage(1));    // Unregistered handler executed");
		}

		UNITTEST_TEST(TestPushAfterDeregistration)
		{
			typedef clang::detail::Message<MockMessage> MessageType;

			Listener listener(MockMessage(1));
			clang::Receiver receiver;

			receiver.RegisterHandler(&listener, &Listener::Handle);
			receiver.DeregisterHandler(&listener, &Listener::Handle);

			const MockMessage value(2);
			clang::Address fromAddress;

			clang::detail::IMessage *const message = CreateMessage(value, fromAddress);
			CHECK_TRUE(message != 0);    // Failed to construct message");

			receiver.Push(message);
			CHECK_TRUE(listener.Value() == MockMessage(1));    // Handler executed after deregistration");
		}

		UNITTEST_TEST(TestWait)
		{
			// Create a responder actor.
			clang::Framework framework;
			clang::ActorRef responder(framework.CreateActor<ResponderActor>());

			clang::Receiver receiver;

			// Push a test message to the responder actor, using the
			// address of the receiver as the from address.
			MockMessage value(1);
			responder.Push(value, receiver.GetAddress());

			// Wait for the return message to be received.
			receiver.Wait();
		}

		UNITTEST_TEST(TestMultipleWaits)
		{
			// Create a responder actor.
			clang::Framework framework;
			clang::ActorRef responder(framework.CreateActor<ResponderActor>());

			clang::Receiver receiver;

			// Push 5 messages to the responder actor, using the
			// address of the receiver as the from address.
			MockMessage value(1);
			responder.Push(value, receiver.GetAddress());
			responder.Push(value, receiver.GetAddress());
			responder.Push(value, receiver.GetAddress());
			responder.Push(value, receiver.GetAddress());
			responder.Push(value, receiver.GetAddress());

			// Wait for 5 return messages to be received.
			receiver.Wait();
			receiver.Wait();
			receiver.Wait();
			receiver.Wait();
			receiver.Wait();
		}

		UNITTEST_TEST(TestWaitFencing)
		{
			// Create a responder actor.
			clang::Framework framework;
			clang::ActorRef responder(framework.CreateActor<ResponderActor>());

			clang::Receiver receiver;

			// Push 5 messages to the responder actor, using the
			// address of the receiver as the from address.
			// After each send, wait for a return message.
			MockMessage value(1);

			responder.Push(value, receiver.GetAddress());
			receiver.Wait();

			responder.Push(value, receiver.GetAddress());
			receiver.Wait();

			responder.Push(value, receiver.GetAddress());
			receiver.Wait();

			responder.Push(value, receiver.GetAddress());
			receiver.Wait();

			responder.Push(value, receiver.GetAddress());
			receiver.Wait();
		}

		UNITTEST_TEST(TestCount)
		{
			// Create a responder actor.
			clang::Framework framework;
			clang::ActorRef responder(framework.CreateActor<ResponderActor>());

			clang::Receiver receiver;

			// Check initial count value.
			CHECK_TRUE(receiver.Count() == 0);    // Receiver::Count failed");

			// Push a test message to the responder actor, using the
			// address of the receiver as the from address.
			MockMessage value(1);
			responder.Push(value, receiver.GetAddress());

			// Busy-wait until the count becomes non-zero
			clang::u32 count = 0;
			while (count == 0)
			{
				count = receiver.Count();
			}

			// Check new count value.
			CHECK_TRUE(receiver.Count() == 1);    // Receiver::Count failed");

			// Wait for the received message.
			receiver.Wait();

			// Check count value after successful wait.
			CHECK_TRUE(receiver.Count() == 0);    // Receiver::Count failed");
		}

		UNITTEST_TEST(TestReset)
		{
			// Create a responder actor.
			clang::Framework framework;
			clang::ActorRef responder(framework.CreateActor<ResponderActor>());

			clang::Receiver receiver;

			// Check initial count value.
			CHECK_TRUE(receiver.Count() == 0);    // Receiver::Count failed");

			// Reset the count to test handling of trivial resets.
			receiver.Reset();

			// Check reset count value.
			CHECK_TRUE(receiver.Count() == 0);    // Receiver::Count failed");

			// Push a test message to the responder actor, using the
			// address of the receiver as the from address.
			MockMessage value(1);
			responder.Push(value, receiver.GetAddress());

			// Busy-wait until the count becomes non-zero
			clang::u32 count = 0;
			while (count == 0)
			{
				count = receiver.Count();
			}

			// Check new count value.
			CHECK_TRUE(receiver.Count() == 1);    // Receiver::Count failed");

			// Reset the count.
			receiver.Reset();

			// Check count value after reset
			CHECK_TRUE(receiver.Count() == 0);    // Receiver::Count failed");

			// Push a second test message.
			responder.Push(value, receiver.GetAddress());

			// Wait for the received message to check waiting still works.
			receiver.Wait();
		}

	};

}
UNITTEST_SUITE_END


#endif // TESTS_TESTSUITES_RECEIVERTESTSUITE

