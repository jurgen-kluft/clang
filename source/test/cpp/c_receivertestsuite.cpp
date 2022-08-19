#define TESTS_TESTSUITES_RECEIVERTESTSUITE
#ifdef TESTS_TESTSUITES_RECEIVERTESTSUITE

#include "xbase\x_allocator.h"

#include "xlang\private\x_BasicTypes.h"

#include "xlang\private\Messages\x_IMessage.h"
#include "xlang\private\Messages\x_Message.h"

#include "xlang\x_Address.h"
#include "xlang\x_AllocatorManager.h"
#include "xlang\x_Framework.h"
#include "xlang\x_Receiver.h"
#include "xlang\x_Register.h"

#include "xunittest\xunittest.h"

// Placement new/delete
inline void*	operator new(xcore::xsize_t num_bytes, void* mem)			{ return mem; }
inline void	operator delete(void* mem, void* )							{ }

class MockMessage
{
public:

	inline explicit MockMessage(const xlang::u32 value) : mValue(value)
	{
	}

	inline const xlang::u32 &Value() const
	{
		return mValue;
	}

	inline xlang::u32 &Value()
	{
		return mValue;
	}

	inline bool operator==(const MockMessage &other) const
	{
		return (mValue == other.mValue);
	}

	XCORE_CLASS_PLACEMENT_NEW_DELETE
private:
	xlang::u32 mValue;
};


UNITTEST_SUITE_BEGIN(TESTS_TESTSUITES_RECEIVERTESTSUITE)
{
    UNITTEST_FIXTURE(main)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

		template <class ValueType>
		inline static xlang::detail::IMessage *CreateMessage(const ValueType &value, const xlang::Address &from)
		{
			typedef xlang::detail::Message<ValueType> MessageType;

			xlang::IAllocator &allocator(*xlang::AllocatorManager::Instance().GetAllocator());

			const xlang::u32 messageSize(MessageType::GetSize());
			const xlang::u32 messageAlignment(MessageType::GetAlignment());

			void *const memory = allocator.AllocateAligned(messageSize, messageAlignment);
			xlang::detail::IMessage *const message = MessageType::Initialize(memory, value, from);

			return message;
		}

		inline static void DestroyMessage(xlang::detail::IMessage *const message)
		{
			xlang::IAllocator &allocator(*xlang::AllocatorManager::Instance().GetAllocator());
			allocator.Free(message->GetBlock());
		}


		class ResponderActor : public xlang::Actor
		{
		public:

			inline ResponderActor()
			{
				RegisterHandler(this, &ResponderActor::Handler);
			}

			XCORE_CLASS_PLACEMENT_NEW_DELETE
		private:

			inline void Handler(const MockMessage &value, const xlang::Address from)
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

			inline xlang::Address &From()
			{
				return mFrom;
			}

			inline void Handle(const MockMessage &message, const xlang::Address from)
			{
				mValue = message;
				mFrom = from;
			}

		private:

			MockMessage mValue;
			xlang::Address mFrom;
		};

		UNITTEST_TEST(TestConstruction)
		{
			xlang::Receiver receiver;
		}

		UNITTEST_TEST(TestRegistration)
		{
			Listener listener;
			xlang::Receiver receiver;
			receiver.RegisterHandler(&listener, &Listener::Handle);
		}

		UNITTEST_TEST(TestDeregistration)
		{
			Listener listener;
			xlang::Receiver receiver;
			receiver.RegisterHandler(&listener, &Listener::Handle);
			receiver.DeregisterHandler(&listener, &Listener::Handle);
		}

		UNITTEST_TEST(TestPush)
		{
			Listener listener(MockMessage(1));
			xlang::Receiver receiver;
			receiver.RegisterHandler(&listener, &Listener::Handle);

			const MockMessage value(2);
			xlang::Address fromAddress;

			xlang::detail::IMessage *const message = CreateMessage(value, fromAddress);
			CHECK_TRUE(message != 0);    // Failed to construct message");

			receiver.Push(message);
			CHECK_TRUE(listener.Value() == value);    // Registered handler not executed correctly");
			CHECK_TRUE(listener.From() == fromAddress);    // Registered handler not executed correctly");

			receiver.DeregisterHandler(&listener, &Listener::Handle);
		}

		UNITTEST_TEST(TestPushWithoutRegistration)
		{
			typedef xlang::detail::Message<MockMessage> MessageType;

			Listener listener(MockMessage(1));
			xlang::Receiver receiver;

			const MockMessage value(2);
			xlang::Address fromAddress;

			xlang::detail::IMessage *const message = CreateMessage(value, fromAddress);
			CHECK_TRUE(message != 0);    // Failed to construct message");

			receiver.Push(message);
			CHECK_TRUE(listener.Value() == MockMessage(1));    // Unregistered handler executed");
		}

		UNITTEST_TEST(TestPushAfterDeregistration)
		{
			typedef xlang::detail::Message<MockMessage> MessageType;

			Listener listener(MockMessage(1));
			xlang::Receiver receiver;

			receiver.RegisterHandler(&listener, &Listener::Handle);
			receiver.DeregisterHandler(&listener, &Listener::Handle);

			const MockMessage value(2);
			xlang::Address fromAddress;

			xlang::detail::IMessage *const message = CreateMessage(value, fromAddress);
			CHECK_TRUE(message != 0);    // Failed to construct message");

			receiver.Push(message);
			CHECK_TRUE(listener.Value() == MockMessage(1));    // Handler executed after deregistration");
		}

		UNITTEST_TEST(TestWait)
		{
			// Create a responder actor.
			xlang::Framework framework;
			xlang::ActorRef responder(framework.CreateActor<ResponderActor>());

			xlang::Receiver receiver;

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
			xlang::Framework framework;
			xlang::ActorRef responder(framework.CreateActor<ResponderActor>());

			xlang::Receiver receiver;

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
			xlang::Framework framework;
			xlang::ActorRef responder(framework.CreateActor<ResponderActor>());

			xlang::Receiver receiver;

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
			xlang::Framework framework;
			xlang::ActorRef responder(framework.CreateActor<ResponderActor>());

			xlang::Receiver receiver;

			// Check initial count value.
			CHECK_TRUE(receiver.Count() == 0);    // Receiver::Count failed");

			// Push a test message to the responder actor, using the
			// address of the receiver as the from address.
			MockMessage value(1);
			responder.Push(value, receiver.GetAddress());

			// Busy-wait until the count becomes non-zero
			xlang::u32 count = 0;
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
			xlang::Framework framework;
			xlang::ActorRef responder(framework.CreateActor<ResponderActor>());

			xlang::Receiver receiver;

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
			xlang::u32 count = 0;
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

