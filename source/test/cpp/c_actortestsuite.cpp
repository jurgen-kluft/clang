#define TESTS_TESTSUITES_ACTORTESTSUITE
#ifdef TESTS_TESTSUITES_ACTORTESTSUITE

#include "clang\private\x_BasicTypes.h"
#include "clang\private\Threading\x_Mutex.h"

#include "clang\x_Register.h"
#include "clang\x_Framework.h"
#include "clang\x_Actor.h"
#include "clang\x_Receiver.h"
#include "clang\private\Handlers\x_messagehandler.h"

#include "cunittest\cunittest.h"

// Placement new/delete
inline void*	operator new(ncore::xsize_t num_bytes, void* mem)			{ return mem; }
inline void	operator delete(void* mem, void* )							{ }

#define XHANDLER(ActorType, MessageType) clang::detail::MessageHandler<ActorType, MessageType>
#define XHANDLER_ARRAY(size, name) ncore::xbyte name[size * clang::SizeOfMessageHandler];

class ActorHandlerConstructor : public clang::Actor
{
public:

	inline ActorHandlerConstructor()
	{
		//No memory allocations and we can sort the handlers
		//by the pointer returned by GetMessageTypeName() since
		//the pointer is unique. In this way we can do a binary
		//search when the handler needs to be invoked!
		//RegisterHandlerArray(handlerArray);
		//RegisterHandler(this, &ActorHandlerConstructor::Handler);
		RegisterHandler(this, &ActorHandlerConstructor::Handler);
	}

private:

	inline void Handler(const int &message, const clang::Address from)
	{
		Send(message, from);
	}

	XHANDLER_ARRAY(10, handlerArray);
};

class OneHandlerActor : public clang::Actor
{
public:

	struct Message
	{
	};

	inline OneHandlerActor()
	{
		RegisterHandler(this, &OneHandlerActor::Handler);
	}

private:

	inline void Handler(const Message &/*message*/, const clang::Address /*from*/)
	{
	}
};

class TwoHandlerActor : public clang::Actor
{
public:

	struct MessageOne
	{
	};

	struct MessageTwo
	{
	};

	inline TwoHandlerActor()
	{
		RegisterHandler(this, &TwoHandlerActor::HandlerOne);
		RegisterHandler(this, &TwoHandlerActor::HandlerTwo);
	}

private:

	inline void HandlerOne(const MessageOne &/*message*/, const clang::Address /*from*/)
	{
	}

	inline void HandlerTwo(const MessageTwo &/*message*/, const clang::Address /*from*/)
	{
	}
};

class ResponderActor : public clang::Actor
{
public:

	inline ResponderActor()
	{
		RegisterHandler(this, &ResponderActor::Handler);
	}

private:

	inline void Handler(const int &value, const clang::Address from)
	{
		Send(value, from);
	}
};


UNITTEST_SUITE_BEGIN(TESTS_TESTSUITES_ACTORTESTSUITE)
{
    UNITTEST_FIXTURE(main)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

		class Listener
		{
		public:

			inline Listener() 
				: mValue(0)
				, mFrom()
			{
			}

			inline explicit Listener(const int value)
				: mValue(value)
				, mFrom()
			{
			}

			inline int GetValue()
			{
				int result(0);

				mMutex.Lock();
				result = mValue;
				mMutex.Unlock();

				return result;
			}

			inline clang::Address From()
			{
				mMutex.Lock();
				clang::Address result(mFrom);
				mMutex.Unlock();

				return result;
			}

			inline void Handle(const int &value, const clang::Address from)
			{
				mMutex.Lock();
				mValue = value;
				mFrom = from;
				mMutex.Unlock();
			}

		private:

			clang::detail::Mutex mMutex;
			int mValue;
			clang::Address mFrom;
		};

		UNITTEST_TEST(TestCreateActorWithOneHandler)
		{
			clang::Framework framework;
			clang::ActorRef actor(framework.CreateActor<OneHandlerActor>());
		}

		UNITTEST_TEST(TestCreateActorWithTwoHandlers)
		{
			clang::Framework framework;
			clang::ActorRef actor(framework.CreateActor<TwoHandlerActor>());
		}

		UNITTEST_TEST(TestExecuteOneHandler)
		{
			clang::Framework framework;
			clang::ActorRef actor(framework.CreateActor<OneHandlerActor>());

			clang::Address here;
			OneHandlerActor::Message message;
			actor.Push(message, here);
		}

		UNITTEST_TEST(TestSendMessage)
		{
			// Create a responder actor.
			clang::Framework framework;
			clang::ActorRef responder(framework.CreateActor<ResponderActor>());

			// Create a listener to collect messages.
			Listener listener(0);

			// Create a receiver and register a handler method on the listener.
			clang::Receiver receiver;
			receiver.RegisterHandler(&listener, &Listener::Handle);

			// Push a test message to the responder actor, using the
			// address of the receiver as the from address.
			responder.Push(1, receiver.GetAddress());

			// When the ResponderActor receives a message, it sends a
			// message back to the sender in aknowledgement, using its
			// internal Send() method. The return message is addressed
			// to the receiver. On receiving it, the receiver will execute
			// the registered handler method of the listener, updating
			// the listener with the contents of the message.
			// We wait here to be alerted that the message has arrived
			// and the handler has been executed.
			receiver.Wait();

			// Check the message the listener received was from the ResponderActor.
			CHECK_TRUE(listener.From() == responder.GetAddress());    // Send failed
		}

		/// Tests that an actor swamped with messages will process all of them.
		UNITTEST_TEST(TestSwamping)
		{
			// Create a responder actor.
			clang::Framework framework;
			clang::ActorRef responder(framework.CreateActor<ResponderActor>());

			clang::Receiver receiver;

			// Push 5 messages to the responder actor, using the
			// address of the receiver as the from address.
			// After each send, wait for a return message.

			const clang::u32 NUM_MESSAGES = 1000;
			for (clang::u32 count = 0; count < NUM_MESSAGES; ++count)
			{
				responder.Push(1, receiver.GetAddress());
			}

			for (clang::u32 count = 0; count < NUM_MESSAGES; ++count)
			{
				receiver.Wait();
			}
		}

		// Tests that it's possible to register handlers in an actor constructor.
		UNITTEST_TEST(TestRegisterInConstructor)
		{
			clang::Framework framework;

			clang::ActorRef actor(framework.CreateActor<ActorHandlerConstructor>());

			clang::Receiver receiver;
			actor.Push(5, receiver.GetAddress());

			receiver.Wait();
		}
	};

}
UNITTEST_SUITE_END


#endif // TESTS_TESTSUITES_ACTORTESTSUITE
