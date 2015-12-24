#define TESTS_TESTSUITES_ACTORTESTSUITE
#ifdef TESTS_TESTSUITES_ACTORTESTSUITE

#include "xlang\private\x_BasicTypes.h"
#include "xlang\private\Threading\x_Mutex.h"

#include "xlang\x_Register.h"
#include "xlang\x_Framework.h"
#include "xlang\x_Actor.h"
#include "xlang\x_Receiver.h"
#include "xlang\private\Handlers\x_messagehandler.h"

#include "xunittest\xunittest.h"

// Placement new/delete
inline void*	operator new(xcore::xsize_t num_bytes, void* mem)			{ return mem; }
inline void	operator delete(void* mem, void* )							{ }

#define XHANDLER(ActorType, MessageType) xlang::detail::MessageHandler<ActorType, MessageType>
#define XHANDLER_ARRAY(size, name) xcore::xbyte name[size * xlang::SizeOfMessageHandler];

class ActorHandlerConstructor : public xlang::Actor
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

	inline void Handler(const int &message, const xlang::Address from)
	{
		Send(message, from);
	}

	XHANDLER_ARRAY(10, handlerArray);
};

class OneHandlerActor : public xlang::Actor
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

	inline void Handler(const Message &/*message*/, const xlang::Address /*from*/)
	{
	}
};

class TwoHandlerActor : public xlang::Actor
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

	inline void HandlerOne(const MessageOne &/*message*/, const xlang::Address /*from*/)
	{
	}

	inline void HandlerTwo(const MessageTwo &/*message*/, const xlang::Address /*from*/)
	{
	}
};

class ResponderActor : public xlang::Actor
{
public:

	inline ResponderActor()
	{
		RegisterHandler(this, &ResponderActor::Handler);
	}

private:

	inline void Handler(const int &value, const xlang::Address from)
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

			inline xlang::Address From()
			{
				mMutex.Lock();
				xlang::Address result(mFrom);
				mMutex.Unlock();

				return result;
			}

			inline void Handle(const int &value, const xlang::Address from)
			{
				mMutex.Lock();
				mValue = value;
				mFrom = from;
				mMutex.Unlock();
			}

		private:

			xlang::detail::Mutex mMutex;
			int mValue;
			xlang::Address mFrom;
		};

		UNITTEST_TEST(TestCreateActorWithOneHandler)
		{
			xlang::Framework framework;
			xlang::ActorRef actor(framework.CreateActor<OneHandlerActor>());
		}

		UNITTEST_TEST(TestCreateActorWithTwoHandlers)
		{
			xlang::Framework framework;
			xlang::ActorRef actor(framework.CreateActor<TwoHandlerActor>());
		}

		UNITTEST_TEST(TestExecuteOneHandler)
		{
			xlang::Framework framework;
			xlang::ActorRef actor(framework.CreateActor<OneHandlerActor>());

			xlang::Address here;
			OneHandlerActor::Message message;
			actor.Push(message, here);
		}

		UNITTEST_TEST(TestSendMessage)
		{
			// Create a responder actor.
			xlang::Framework framework;
			xlang::ActorRef responder(framework.CreateActor<ResponderActor>());

			// Create a listener to collect messages.
			Listener listener(0);

			// Create a receiver and register a handler method on the listener.
			xlang::Receiver receiver;
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
			xlang::Framework framework;
			xlang::ActorRef responder(framework.CreateActor<ResponderActor>());

			xlang::Receiver receiver;

			// Push 5 messages to the responder actor, using the
			// address of the receiver as the from address.
			// After each send, wait for a return message.

			const xlang::u32 NUM_MESSAGES = 1000;
			for (xlang::u32 count = 0; count < NUM_MESSAGES; ++count)
			{
				responder.Push(1, receiver.GetAddress());
			}

			for (xlang::u32 count = 0; count < NUM_MESSAGES; ++count)
			{
				receiver.Wait();
			}
		}

		// Tests that it's possible to register handlers in an actor constructor.
		UNITTEST_TEST(TestRegisterInConstructor)
		{
			xlang::Framework framework;

			xlang::ActorRef actor(framework.CreateActor<ActorHandlerConstructor>());

			xlang::Receiver receiver;
			actor.Push(5, receiver.GetAddress());

			receiver.Wait();
		}
	};

}
UNITTEST_SUITE_END


#endif // TESTS_TESTSUITES_ACTORTESTSUITE
