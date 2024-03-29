//
// This sample shows how to register message handlers in an actor class.
//

#include "clang/c_Framework.h"
#include "clang/c_Receiver.h"
#include "clang/c_Actor.h"


// Placement new/delete
void*	operator new(ncore::xsize_t num_bytes, void* mem)			{ return mem; }
void	operator delete(void* mem, void* )							{ }


struct MessageIntValue
{
	inline MessageIntValue() : mValue(-1)
	{
	}

	inline MessageIntValue(const int value) : mValue(value)
    {
    }
    int mValue;
};


// An example actor with two different handlers for the same message type.
class ExampleActor : public clang::Actor
{
public:

    inline ExampleActor()
    {
        // Only handler one is registered initially.
        RegisterHandler(this, &ExampleActor::HandlerOne);
    }

private:

	inline void HandlerOne(const MessageIntValue &message, const clang::Address from)
    {
        printf("Handler ONE received message with value '%d'\n", message.mValue);
        
        // Switch to handler two
        DeregisterHandler(this, &ExampleActor::HandlerOne);
        RegisterHandler(this, &ExampleActor::HandlerTwo);
        
        Send(message, from);
    }

	inline void HandlerTwo(const MessageIntValue &message, const clang::Address from)
    {
        printf("Handler TWO received message with value '%d'\n", message.mValue);

        // Switch to handler one
        DeregisterHandler(this, &ExampleActor::HandlerTwo);
        RegisterHandler(this, &ExampleActor::HandlerOne);

        Send(message, from);
    }
};

class Catcher
{
public:

	inline void Catch(const MessageIntValue &message, const clang::Address from)
	{
		mMessage = message;
	}

	MessageIntValue mMessage;
};

int main()
{
    clang::Framework framework;
    clang::ActorRef exampleActor(framework.CreateActor<ExampleActor>());

    clang::Receiver receiver;
	Catcher catcher;
	receiver.RegisterHandler(&catcher, &Catcher::Catch);

    // Send a series of messages to the actor.
    // Each time it receives a message it switches handlers, so that
    // it reacts differently to odd and even messages.
    for (int count = 0; count < 10; ++count)
    {
		exampleActor.Push(MessageIntValue(count), receiver.GetAddress());
    }

    // Wait for the same number of response messages.
    for (int count = 0; count < 10; ++count)
    {
        receiver.Wait();
    }

    return 0;
}

