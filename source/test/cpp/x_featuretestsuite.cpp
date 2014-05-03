#define TESTS_TESTSUITES_FEATURETESTSUITE
#ifdef TESTS_TESTSUITES_FEATURETESTSUITE

#include <string>
#include <queue>

#include "xlang\private\debug\x_assert.h"
#include "xlang\x_actor.h"
#include "xlang\x_actorref.h"
#include "xlang\x_address.h"
#include "xlang\x_framework.h"
#include "xlang\x_receiver.h"
#include "xlang\x_register.h"

#include "xunittest\xunittest.h"



template <class CountType>
class Sequencer : public xlang::Actor
{
public:

	static const char *GOOD;
	static const char *BAD;

	inline Sequencer() : mNextValue(0), mStatus(GOOD)
	{
		RegisterHandler(this, &Sequencer::Receive);
		RegisterHandler(this, &Sequencer::GetValue);
	}

private:

	inline void Receive(const CountType &message, const xlang::Address /*from*/)
	{
		if (message != mNextValue++)
		{
			mStatus = BAD;
		}
	}

	inline void GetValue(const bool &/*message*/, const xlang::Address from)
	{
		Send(mStatus, from);
	}

	CountType mNextValue;
	const char *mStatus;
};

template <class CountType>
const char *Sequencer<CountType>::GOOD = "good";

template <class CountType>
const char *Sequencer<CountType>::BAD = "bad";

//typedef std::vector<xlang::u32> IntVectorMessage;
struct IntVectorMessage
{
	xlang::u32		n;
	xlang::u32		a;
	xlang::u32		b;
	xlang::u32		c;
};
//XLANG_REGISTER_MESSAGE(IntVectorMessage);

struct CreateMessage { };
struct DestroyMessage { };
//XLANG_REGISTER_MESSAGE(CreateMessage);
//XLANG_REGISTER_MESSAGE(DestroyMessage);
//
//XLANG_REGISTER_MESSAGE(Address);
//XLANG_REGISTER_MESSAGE(ActorRef);
//
//XLANG_REGISTER_MESSAGE(bool);
//XLANG_REGISTER_MESSAGE(float);
//XLANG_REGISTER_MESSAGE(float*);
//XLANG_REGISTER_MESSAGE(float const*);
//XLANG_REGISTER_MESSAGE(char const*);
//XLANG_REGISTER_MESSAGE(int);
//XLANG_REGISTER_MESSAGE(unsigned int);
//typedef std::vector<int> VectorMessage;
//XLANG_REGISTER_MESSAGE(std::vector<int>);
//XLANG_REGISTER_MESSAGE(std::string);

UNITTEST_SUITE_BEGIN(TESTS_TESTSUITES_FEATURETESTSUITE)
{
    UNITTEST_FIXTURE(main)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

		class Trivial : public xlang::Actor
		{
		};

		template <class MessageType>
		class Replier : public xlang::Actor
		{
		public:

			inline Replier()
			{
				RegisterHandler(this, &Replier::Handler);
			}

		private:

			inline void Handler(const MessageType &message, const xlang::Address from)
			{
				Send(message, from);
			}
		};

		template <class MessageType>
		class DefaultReplier : public xlang::Actor
		{
		public:

			inline DefaultReplier()
			{
				RegisterHandler(this, &DefaultReplier::Handler);
				SetDefaultHandler(this, &DefaultReplier::DefaultHandler);
			}

		private:

			inline void Handler(const MessageType &message, const xlang::Address from)
			{
				Send(message, from);
			}

			inline void DefaultHandler(const xlang::Address from)
			{
				std::string hello("hello");
				Send(hello, from);
			}
		};

		class StringReplier : public Replier<const char *>
		{
		};

		class Signaler : public xlang::Actor
		{
		public:

			inline Signaler()
			{
				RegisterHandler(this, &Signaler::Signal);
			}

		private:

			inline void Signal(const xlang::Address &address, const xlang::Address from)
			{
				// Send the 'from' address to the address received in the message.
				Send(from, address);
			}
		};

		class Poker : public xlang::Actor
		{
		public:

			inline Poker()
			{
				RegisterHandler(this, &Poker::Poke);
			}

		private:

			inline void Poke(const xlang::ActorRef &actor, const xlang::Address from)
			{
				// Poke the actor referenced by the message, sending it the from address.
				Send(from, actor.GetAddress());
			}
		};

		class Switcher : public xlang::Actor
		{
		public:

			inline Switcher()
			{
				RegisterHandler(this, &Switcher::SayHello);
			}

		private:

			inline void SayHello(const std::string &/*message*/, const xlang::Address from)
			{
				DeregisterHandler(this, &Switcher::SayHello);
				RegisterHandler(this, &Switcher::SayGoodbye);
				Send(std::string("hello"), from);
			}

			inline void SayGoodbye(const std::string &/*message*/, const xlang::Address from)
			{
				DeregisterHandler(this, &Switcher::SayGoodbye);
				RegisterHandler(this, &Switcher::SayHello);
				Send(std::string("goodbye"), from);
			}
		};

		template <class MessageType>
		class Catcher
		{
		public:

			inline Catcher() : mMessage(), mFrom(xlang::Address::Null())
			{
			}

			inline void Catch(const MessageType &message, const xlang::Address from)
			{
				mMessage = message;
				mFrom = from;
			}

			MessageType mMessage;
			xlang::Address mFrom;
		};

		struct Holder
		{
			xlang::ActorRef mRef;
		};

		class Counter : public xlang::Actor
		{
		public:

			inline Counter() : mCount(0)
			{
				RegisterHandler(this, &Counter::Increment);
				RegisterHandler(this, &Counter::GetValue);
			}

		private:

			inline void Increment(const int &message, const xlang::Address /*from*/)
			{
				mCount += message;
			}

			inline void GetValue(const bool &/*message*/, const xlang::Address from)
			{
				Send(mCount, from);
			}

			int mCount;
		};

		class TwoHandlerCounter : public xlang::Actor
		{
		public:

			inline TwoHandlerCounter() : mCount(0)
			{
				RegisterHandler(this, &TwoHandlerCounter::IncrementOne);
				RegisterHandler(this, &TwoHandlerCounter::IncrementTwo);
				RegisterHandler(this, &TwoHandlerCounter::GetValue);
			}

		private:

			inline void IncrementOne(const int &message, const xlang::Address /*from*/)
			{
				mCount += message;
			}

			inline void IncrementTwo(const float &/*message*/, const xlang::Address /*from*/)
			{
				++mCount;
			}

			inline void GetValue(const bool &/*message*/, const xlang::Address from)
			{
				Send(mCount, from);
			}

			int mCount;
		};

		class MultipleHandlerCounter : public xlang::Actor
		{
		public:

			inline MultipleHandlerCounter() : mCount(0)
			{
				RegisterHandler(this, &MultipleHandlerCounter::IncrementOne);
				RegisterHandler(this, &MultipleHandlerCounter::IncrementTwo);
				RegisterHandler(this, &MultipleHandlerCounter::GetValue);
			}

		private:

			inline void IncrementOne(const int &message, const xlang::Address /*from*/)
			{
				mCount += message;
			}

			inline void IncrementTwo(const int &/*message*/, const xlang::Address /*from*/)
			{
				++mCount;
			}

			inline void GetValue(const bool &/*message*/, const xlang::Address from)
			{
				Send(mCount, from);
			}

			int mCount;
		};


		class Parameterized : public xlang::Actor
		{
		public:

			struct Parameters
			{
				xlang::Address mAddress;
			};

			inline Parameterized(const Parameters &params) : mAddress(params.mAddress)
			{
				RegisterHandler(this, &Parameterized::Handler);
			}

			inline void Handler(const int &message, const xlang::Address /*from*/)
			{
				// Send a message to the address provided on construction.
				Send(message, mAddress);
			}

		private:

			xlang::Address mAddress;
		};

		class Recursor : public xlang::Actor
		{
		public:

			struct Parameters
			{
				int mCount;
			};

			inline Recursor(const Parameters &params)
			{
				// Recursively create a child actor within the constructor.
				if (params.mCount > 0)
				{
					Parameters childParams;
					childParams.mCount = params.mCount - 1;

					xlang::Framework &framework(GetFramework());
					xlang::ActorRef child(framework.CreateActor<Recursor>(childParams));
				}
			}
		};

		class TailRecursor : public xlang::Actor
		{
		public:

			struct Parameters
			{
				int mCount;
			};

			inline TailRecursor(const Parameters &params) : mCount(params.mCount)
			{
			}

			inline ~TailRecursor()
			{
				// Recursively create a child actor within the destructor.
				if (mCount > 0)
				{
					Parameters childParams;
					childParams.mCount = mCount - 1;

					xlang::Framework &framework(GetFramework());
					xlang::ActorRef child(framework.CreateActor<TailRecursor>(childParams));
				}
			}

		private:

			int mCount;
		};

		class AutoSender : public xlang::Actor
		{
		public:

			typedef xlang::Address Parameters;

			inline AutoSender(const Parameters &address)
			{
				// Send a message in the actor constructor.
				Send(0, address);

				// Send using TailSend to check that works too.
				TailSend(1, address);
			}
		};

		class TailSender : public xlang::Actor
		{
		public:

			typedef xlang::Address Parameters;

			inline TailSender(const Parameters &address) : mAddress(address)
			{
			}

			inline ~TailSender()
			{
				// Send a message in the actor destructor.
				Send(0, mAddress);

				// Send using TailSend to check that works too.
				TailSend(1, mAddress);
			}

			xlang::Address mAddress;
		};

		class AutoDeregistrar : public xlang::Actor
		{
		public:

			inline AutoDeregistrar()
			{
				RegisterHandler(this, &AutoDeregistrar::HandlerOne);
				RegisterHandler(this, &AutoDeregistrar::HandlerTwo);
				DeregisterHandler(this, &AutoDeregistrar::HandlerOne);
			}

			inline void HandlerOne(const int &/*message*/, const xlang::Address from)
			{
				Send(1, from);
			}

			inline void HandlerTwo(const int &/*message*/, const xlang::Address from)
			{
				Send(2, from);
			}
		};

		class TailDeregistrar : public xlang::Actor
		{
		public:

			inline TailDeregistrar()
			{
				RegisterHandler(this, &TailDeregistrar::Handler);
			}

			inline ~TailDeregistrar()
			{
				DeregisterHandler(this, &TailDeregistrar::Handler);
			}

			inline void Handler(const int &/*message*/, const xlang::Address from)
			{
				Send(0, from);
			}
		};

		class MessageQueueCounter : public xlang::Actor
		{
		public:

			inline MessageQueueCounter()
			{
				RegisterHandler(this, &MessageQueueCounter::Handler);
			}

			inline void Handler(const int &/*message*/, const xlang::Address from)
			{
				Send(GetNumQueuedMessages(), from);
			}
		};

		class BlindActor : public xlang::Actor
		{
		public:

			inline BlindActor()
			{
				SetDefaultHandler(this, &BlindActor::BlindDefaultHandler);
			}

		private:

			inline void BlindDefaultHandler(const void *const data, const xlang::u32 size, const xlang::Address from)
			{
				// We know the message is a u32.
				const xlang::u32 *const p(reinterpret_cast<const xlang::u32 *>(data));
				const xlang::u32 value(*p);

				Send(value, from);
				Send(size, from);
			}
		};

		template <class MessageType>
		class Accumulator
		{
		public:

			inline Accumulator() : mMessages()
			{
			}

			inline void Catch(const MessageType &message, const xlang::Address /*from*/)
			{
				mMessages.push(message);
			}

			MessageType Pop()
			{
				XLANG_ASSERT(mMessages.empty() == false);
				MessageType message(mMessages.front());
				mMessages.pop();
				return message;
			}

		private:

			std::queue<MessageType> mMessages;
		};

		class HandlerChecker : public xlang::Actor
		{
		public:

			inline HandlerChecker()
			{
				RegisterHandler(this, &HandlerChecker::Check2);
			}

		private:

			inline void Check2(const int & /*message*/, const xlang::Address from)
			{
				Send(IsHandlerRegistered(this, &HandlerChecker::Check2), from);
				Send(IsHandlerRegistered(this, &HandlerChecker::Dummy), from);
				Send(IsHandlerRegistered(this, &HandlerChecker::Unregistered), from);

				RegisterHandler(this, &HandlerChecker::Dummy);

				Send(IsHandlerRegistered(this, &HandlerChecker::Check2), from);
				Send(IsHandlerRegistered(this, &HandlerChecker::Dummy), from);
				Send(IsHandlerRegistered(this, &HandlerChecker::Unregistered), from);

				DeregisterHandler(this, &HandlerChecker::Dummy);

				Send(IsHandlerRegistered(this, &HandlerChecker::Check2), from);
				Send(IsHandlerRegistered(this, &HandlerChecker::Dummy), from);
				Send(IsHandlerRegistered(this, &HandlerChecker::Unregistered), from);

				DeregisterHandler(this, &HandlerChecker::Check2);

				Send(IsHandlerRegistered(this, &HandlerChecker::Check2), from);
				Send(IsHandlerRegistered(this, &HandlerChecker::Dummy), from);
				Send(IsHandlerRegistered(this, &HandlerChecker::Unregistered), from);

				RegisterHandler(this, &HandlerChecker::Dummy);
				RegisterHandler(this, &HandlerChecker::Check2);
				RegisterHandler(this, &HandlerChecker::Check2);

				Send(IsHandlerRegistered(this, &HandlerChecker::Check2), from);
				Send(IsHandlerRegistered(this, &HandlerChecker::Dummy), from);
				Send(IsHandlerRegistered(this, &HandlerChecker::Unregistered), from);

				DeregisterHandler(this, &HandlerChecker::Check2);

				Send(IsHandlerRegistered(this, &HandlerChecker::Check2), from);
				Send(IsHandlerRegistered(this, &HandlerChecker::Dummy), from);
				Send(IsHandlerRegistered(this, &HandlerChecker::Unregistered), from);

				DeregisterHandler(this, &HandlerChecker::Check2);

				Send(IsHandlerRegistered(this, &HandlerChecker::Check2), from);
				Send(IsHandlerRegistered(this, &HandlerChecker::Dummy), from);
				Send(IsHandlerRegistered(this, &HandlerChecker::Unregistered), from);
			}

			inline void Dummy(const int & message, const xlang::Address from)
			{
				// We do this just so that Dummy and Unregistered differ, so the compiler won't merge them!
				Send(message, from);
			}

			inline void Unregistered(const int & /*message*/, const xlang::Address /*from*/)
			{
			}
		};

		class Nestor : public xlang::Actor
		{
		public:

			inline Nestor()
			{
				RegisterHandler(this, &Nestor::Create);
				RegisterHandler(this, &Nestor::Destroy);
				RegisterHandler(this, &Nestor::Receive);
			}

		private:

			typedef Replier<int> ChildActor;

			inline void Create(const CreateMessage & /*message*/, const xlang::Address /*from*/)
			{
				mChildren.push_back(GetFramework().CreateActor<ChildActor>());
				mChildren.push_back(GetFramework().CreateActor<ChildActor>());
				mChildren.push_back(GetFramework().CreateActor<ChildActor>());

				mReplies.push_back(false);
				mReplies.push_back(false);
				mReplies.push_back(false);

				Send(0, mChildren[0].GetAddress());
				Send(1, mChildren[1].GetAddress());
				Send(2, mChildren[2].GetAddress());
			}

			inline void Destroy(const DestroyMessage & /*message*/, const xlang::Address from)
			{
				mCaller = from;
				if (mReplies[0] && mReplies[1] && mReplies[2])
				{
					mChildren.clear();
					Send(true, mCaller);
				}
			}

			inline void Receive(const int & message, const xlang::Address /*from*/)
			{
				mReplies[message] = true;

				if (mCaller != xlang::Address::Null() && mReplies[0] && mReplies[1] && mReplies[2])
				{
					mChildren.clear();
					Send(true, mCaller);
				}
			}

			std::vector<xlang::ActorRef> mChildren;
			std::vector<bool> mReplies;
			xlang::Address mCaller;
		};

		class FallbackHandler
		{
		public:

			inline void Handle(const xlang::Address from)
			{
				mAddress = from;
			}

			xlang::Address mAddress;
		};

		class BlindFallbackHandler
		{
		public:

			BlindFallbackHandler() : mData(0), mValue(0), mSize(0)
			{
			}

			inline void Handle(const void *const data, const xlang::u32 size, const xlang::Address from)
			{
				mData = data;
				mValue = *reinterpret_cast<const xlang::u32 *>(data);
				mSize = size;
				mAddress = from;
			}

			const void *mData;
			xlang::u32 mValue;
			xlang::u32 mSize;
			xlang::Address mAddress;
		};

		class LastGasp : public xlang::Actor
		{
		public:

			typedef xlang::Address Parameters;

			inline explicit LastGasp(const xlang::Address address) : mAddress(address)
			{
			}

			inline ~LastGasp()
			{
				Send(0, mAddress);
			}

		private:

			xlang::Address mAddress;
		};


		UNITTEST_TEST(NullActorReference)
		{
			xlang::ActorRef nullReference;
			CHECK_TRUE(nullReference == xlang::ActorRef::Null());    // Default-constructed reference isn't null");
			CHECK_TRUE((nullReference != xlang::ActorRef::Null()) == false);    // Default-constructed reference isn't null");
		}

		UNITTEST_TEST(CreateActorInFunction)
		{
			xlang::Framework framework;
			xlang::ActorRef actor(framework.CreateActor<Trivial>());
		}

		UNITTEST_TEST(CreateActorWithParamsInFunction)
		{
			xlang::Framework framework;
			xlang::Receiver receiver;

			Parameterized::Parameters params;
			params.mAddress = receiver.GetAddress();
			xlang::ActorRef actor(framework.CreateActor<Parameterized>(params));

			framework.Send(0, xlang::Address::Null(), actor.GetAddress());

			receiver.Wait();
		}

		UNITTEST_TEST(SendMessageToReceiverInFunction)
		{
			xlang::Framework framework;
			xlang::Receiver receiver;
			framework.Send(0.0f, receiver.GetAddress(), receiver.GetAddress());
		}

		UNITTEST_TEST(SendMessageFromNullAddressInFunction)
		{
			xlang::Framework framework;
			xlang::Receiver receiver;

			framework.Send(0, xlang::Address::Null(), receiver.GetAddress());
			receiver.Wait();
		}

		UNITTEST_TEST(SendMessageToActorFromNullAddressInFunction)
		{
			xlang::Framework framework;
			xlang::Receiver receiver;
			xlang::ActorRef signaler(framework.CreateActor<Signaler>());

			framework.Send(receiver.GetAddress(), xlang::Address::Null(), signaler.GetAddress());
			receiver.Wait();
		}

		UNITTEST_TEST(SendMessageToActorFromReceiverInFunction)
		{
			xlang::Framework framework;
			xlang::Receiver receiver;
			xlang::ActorRef signaler(framework.CreateActor<Signaler>());

			framework.Send(receiver.GetAddress(), receiver.GetAddress(), signaler.GetAddress());
			receiver.Wait();
		}

		UNITTEST_TEST(PushMessageToActorFromNullAddressInFunction)
		{
			xlang::Framework framework;
			xlang::Receiver receiver;
			xlang::ActorRef signaler(framework.CreateActor<Signaler>());

			signaler.Push(receiver.GetAddress(), xlang::Address::Null());
			receiver.Wait();
		}

		UNITTEST_TEST(PushMessageToActorFromReceiverInFunction)
		{
			xlang::Framework framework;
			xlang::Receiver receiver;
			xlang::ActorRef signaler(framework.CreateActor<Signaler>());

			signaler.Push(receiver.GetAddress(), receiver.GetAddress());
			receiver.Wait();
		}

		UNITTEST_TEST(ReceiveReplyInFunction)
		{
			typedef Replier<float> FloatReplier;

			xlang::Framework framework;
			xlang::Receiver receiver;
			xlang::ActorRef actor(framework.CreateActor<FloatReplier>());

			framework.Send(5.0f, receiver.GetAddress(), actor.GetAddress());

			receiver.Wait();
		}

		UNITTEST_TEST(CatchReplyInFunction)
		{
			typedef Replier<float> FloatReplier;
			typedef Catcher<float> FloatCatcher;

			xlang::Framework framework;
			xlang::ActorRef actor(framework.CreateActor<FloatReplier>());

			xlang::Receiver receiver;
			FloatCatcher catcher;
			receiver.RegisterHandler(&catcher, &FloatCatcher::Catch);

			framework.Send(5.0f, receiver.GetAddress(), actor.GetAddress());

			receiver.Wait();

			CHECK_TRUE(catcher.mMessage == 5.0f);    // Caught message value wrong");
			CHECK_TRUE(catcher.mFrom == actor.GetAddress());    // Caught from address wrong");
		}

		UNITTEST_TEST(StoreActorRefAsMemberInFunction)
		{
			typedef Replier<float> FloatReplier;

			xlang::Framework framework;
			xlang::Receiver receiver;

			Holder object;
			object.mRef = framework.CreateActor<FloatReplier>();

			framework.Send(5.0f, receiver.GetAddress(), object.mRef.GetAddress());

			receiver.Wait();
		}

		UNITTEST_TEST(SendNonPODMessageInFunction)
		{
			typedef std::vector<int> VectorMessage;
			typedef Replier<VectorMessage> VectorReplier;
			typedef Catcher<VectorMessage> VectorCatcher;

			xlang::Framework framework;
			xlang::ActorRef actor(framework.CreateActor<VectorReplier>());

			xlang::Receiver receiver;
			VectorCatcher catcher;
			receiver.RegisterHandler(&catcher, &VectorCatcher::Catch);

			VectorMessage vectorMessage;
			vectorMessage.push_back(0);
			vectorMessage.push_back(1);
			vectorMessage.push_back(2);
			framework.Send(vectorMessage, receiver.GetAddress(), actor.GetAddress());

			receiver.Wait();

			CHECK_TRUE(catcher.mMessage == vectorMessage);    // Reply message is wrong");
		}

		UNITTEST_TEST(SendPointerMessageInFunction)
		{
			typedef float * PointerMessage;
			typedef Replier<PointerMessage> PointerReplier;
			typedef Catcher<PointerMessage> PointerCatcher;

			xlang::Framework framework;
			xlang::ActorRef actor(framework.CreateActor<PointerReplier>());

			xlang::Receiver receiver;
			PointerCatcher catcher;
			receiver.RegisterHandler(&catcher, &PointerCatcher::Catch);

			float a(0.0f);
			PointerMessage pointerMessage(&a);
			framework.Send(pointerMessage, receiver.GetAddress(), actor.GetAddress());

			receiver.Wait();

			CHECK_TRUE(catcher.mMessage == &a);    // Reply message is wrong");
		}

		UNITTEST_TEST(SendConstPointerMessageInFunction)
		{
			typedef const float * PointerMessage;
			typedef Replier<PointerMessage> PointerReplier;
			typedef Catcher<PointerMessage> PointerCatcher;

			xlang::Framework framework;
			xlang::ActorRef actor(framework.CreateActor<PointerReplier>());

			xlang::Receiver receiver;
			PointerCatcher catcher;
			receiver.RegisterHandler(&catcher, &PointerCatcher::Catch);

			float a(0.0f);
			PointerMessage pointerMessage(&a);
			framework.Send(pointerMessage, receiver.GetAddress(), actor.GetAddress());

			receiver.Wait();

			CHECK_TRUE(catcher.mMessage == &a);    // Reply message is wrong");
		}

		UNITTEST_TEST(CreateDerivedActor)
		{
			xlang::Framework framework;
			xlang::ActorRef actor(framework.CreateActor<StringReplier>());
		}

		UNITTEST_TEST(SendMessageToDerivedActor)
		{
			typedef const char * StringMessage;
			typedef Catcher<StringMessage> StringCatcher;

			xlang::Framework framework;
			xlang::ActorRef actor(framework.CreateActor<StringReplier>());

			xlang::Receiver receiver;
			StringCatcher catcher;
			receiver.RegisterHandler(&catcher, &StringCatcher::Catch);

			const char *testString = "hello";
			StringMessage stringMessage(testString);
			framework.Send(stringMessage, receiver.GetAddress(), actor.GetAddress());

			receiver.Wait();

			CHECK_TRUE(catcher.mMessage == stringMessage);    // Reply message is wrong");
		}

		UNITTEST_TEST(IncrementCounter)
		{
			typedef Catcher<int> CountCatcher;

			xlang::Framework framework;
			xlang::ActorRef actor(framework.CreateActor<Counter>());

			xlang::Receiver receiver;
			CountCatcher catcher;
			receiver.RegisterHandler(&catcher, &CountCatcher::Catch);

			framework.Send(1, receiver.GetAddress(), actor.GetAddress());
			framework.Send(2, receiver.GetAddress(), actor.GetAddress());
			framework.Send(3, receiver.GetAddress(), actor.GetAddress());
			framework.Send(4, receiver.GetAddress(), actor.GetAddress());
			framework.Send(5, receiver.GetAddress(), actor.GetAddress());
			framework.Send(6, receiver.GetAddress(), actor.GetAddress());

			framework.Send(true, receiver.GetAddress(), actor.GetAddress());

			receiver.Wait();

			CHECK_TRUE(catcher.mMessage == 21);    // Count is wrong");
		}

		UNITTEST_TEST(ActorTemplate)
		{
			typedef Replier<int> IntReplier;

			xlang::Framework framework;
			xlang::ActorRef actor(framework.CreateActor<IntReplier>());

			xlang::Receiver receiver;
			framework.Send(10, receiver.GetAddress(), actor.GetAddress());

			receiver.Wait();
		}

		UNITTEST_TEST(OneHandlerAtATime)
		{
			typedef Catcher<int> CountCatcher;

			xlang::Framework framework;
			xlang::ActorRef actor(framework.CreateActor<TwoHandlerCounter>());

			xlang::Receiver receiver;
			CountCatcher catcher;
			receiver.RegisterHandler(&catcher, &CountCatcher::Catch);

			framework.Send(2, receiver.GetAddress(), actor.GetAddress());
			framework.Send(0.0f, receiver.GetAddress(), actor.GetAddress());
			framework.Send(2, receiver.GetAddress(), actor.GetAddress());
			framework.Send(0.0f, receiver.GetAddress(), actor.GetAddress());
			framework.Send(2, receiver.GetAddress(), actor.GetAddress());
			framework.Send(0.0f, receiver.GetAddress(), actor.GetAddress());

			// Get the counter value.
			framework.Send(true, receiver.GetAddress(), actor.GetAddress());

			receiver.Wait();

			CHECK_TRUE(catcher.mMessage == 9);    // Count is wrong");
		}

		UNITTEST_TEST(MultipleHandlersForMessageType)
		{
			typedef Catcher<int> CountCatcher;

			xlang::Framework framework;
			xlang::ActorRef actor(framework.CreateActor<MultipleHandlerCounter>());

			xlang::Receiver receiver;
			CountCatcher catcher;
			receiver.RegisterHandler(&catcher, &CountCatcher::Catch);

			framework.Send(2, receiver.GetAddress(), actor.GetAddress());
			framework.Send(2, receiver.GetAddress(), actor.GetAddress());
			framework.Send(2, receiver.GetAddress(), actor.GetAddress());

			// Get the counter value.
			framework.Send(true, receiver.GetAddress(), actor.GetAddress());

			receiver.Wait();

			CHECK_TRUE(catcher.mMessage == 9);    // Count is wrong");
		}

		UNITTEST_TEST(MessageArrivalOrder)
		{
			typedef Catcher<const char *> StringCatcher;
			typedef Sequencer<int> IntSequencer;

			xlang::Framework framework;
			xlang::ActorRef actor(framework.CreateActor<IntSequencer>());

			xlang::Receiver receiver;
			StringCatcher catcher;
			receiver.RegisterHandler(&catcher, &StringCatcher::Catch);

			framework.Send(0, receiver.GetAddress(), actor.GetAddress());
			framework.Send(1, receiver.GetAddress(), actor.GetAddress());
			framework.Send(2, receiver.GetAddress(), actor.GetAddress());
			framework.Send(3, receiver.GetAddress(), actor.GetAddress());
			framework.Send(4, receiver.GetAddress(), actor.GetAddress());
			framework.Send(5, receiver.GetAddress(), actor.GetAddress());
			framework.Send(6, receiver.GetAddress(), actor.GetAddress());
			framework.Send(7, receiver.GetAddress(), actor.GetAddress());

			// Get the validity value.
			framework.Send(true, receiver.GetAddress(), actor.GetAddress());

			receiver.Wait();
			CHECK_TRUE(catcher.mMessage == IntSequencer::GOOD);    // Sequencer status is wrong");

			framework.Send(9, receiver.GetAddress(), actor.GetAddress());

			// Get the validity value.
			framework.Send(true, receiver.GetAddress(), actor.GetAddress());

			receiver.Wait();
			CHECK_TRUE(catcher.mMessage == IntSequencer::BAD);    // Sequencer status is wrong");
		}

		UNITTEST_TEST(SendAddressAsMessage)
		{
			typedef Catcher<xlang::Address> AddressCatcher;

			xlang::Framework framework;
			xlang::ActorRef actorA(framework.CreateActor<Signaler>());
			xlang::ActorRef actorB(framework.CreateActor<Signaler>());

			xlang::Receiver receiver;
			AddressCatcher catcher;
			receiver.RegisterHandler(&catcher, &AddressCatcher::Catch);

			// Send A a message telling it to signal B.
			// A sends the receiver address to B as the signal,
			// causing B to send A's address to the receiver in turn.
			framework.Send(actorB.GetAddress(), receiver.GetAddress(), actorA.GetAddress());

			receiver.Wait();

			CHECK_TRUE(catcher.mMessage == actorA.GetAddress());    // Wrong address");
		}

		UNITTEST_TEST(SendActorRefAsMessage)
		{
			typedef Catcher<xlang::Address> AddressCatcher;

			xlang::Framework framework;
			xlang::ActorRef actorA(framework.CreateActor<Poker>());
			xlang::ActorRef actorB(framework.CreateActor<Signaler>());

			xlang::Receiver receiver;
			AddressCatcher catcher;
			receiver.RegisterHandler(&catcher, &AddressCatcher::Catch);

			// Send A a reference to B, telling it to poke B.
			// A sends the receiver address to B as the poke,
			// causing B to send A's address to the receiver in turn.
			framework.Send(actorB, receiver.GetAddress(), actorA.GetAddress());

			receiver.Wait();

			CHECK_TRUE(catcher.mMessage == actorA.GetAddress());    // Wrong address");
		}

#if 1
		UNITTEST_TEST(SendMessageToDefaultHandlerInFunction)
		{
			typedef DefaultReplier<float> FloatReplier;
			typedef Catcher<std::string> StringCatcher;

			xlang::Framework framework;
			xlang::ActorRef actor(framework.CreateActor<FloatReplier>());

			StringCatcher catcher;
			xlang::Receiver receiver;
			receiver.RegisterHandler(&catcher, &StringCatcher::Catch);

			// Send an int to the replier, which expects floats but has a default handler.
			framework.Send(52, receiver.GetAddress(), actor.GetAddress());

			receiver.Wait();

			CHECK_TRUE(catcher.mMessage == "hello");    // Default handler not executed");
		}
#endif
#if 1
		UNITTEST_TEST(RegisterHandlerFromHandler)
		{
			typedef Catcher<std::string> StringCatcher;

			xlang::Framework framework;
			xlang::ActorRef actor(framework.CreateActor<Switcher>());

			StringCatcher catcher;
			xlang::Receiver receiver;
			receiver.RegisterHandler(&catcher, &StringCatcher::Catch);

			framework.Send(std::string("hello"), receiver.GetAddress(), actor.GetAddress());
			receiver.Wait();
			CHECK_TRUE(catcher.mMessage == "hello");    // Handler not executed");

			framework.Send(std::string("hello"), receiver.GetAddress(), actor.GetAddress());
			receiver.Wait();
			CHECK_TRUE(catcher.mMessage == "goodbye");    // Handler not executed");

			framework.Send(std::string("hello"), receiver.GetAddress(), actor.GetAddress());
			receiver.Wait();
			CHECK_TRUE(catcher.mMessage == "hello");    // Handler not executed");

			framework.Send(std::string("hello"), receiver.GetAddress(), actor.GetAddress());
			receiver.Wait();
			CHECK_TRUE(catcher.mMessage == "goodbye");    // Handler not executed");
		}

		UNITTEST_TEST(CreateActorInConstructor)
		{
			xlang::Framework framework;

			Recursor::Parameters params;
			params.mCount = 10;
			xlang::ActorRef actor(framework.CreateActor<Recursor>(params));
		}

		UNITTEST_TEST(SendMessageInConstructor)
		{
			xlang::Framework framework;
			xlang::Receiver receiver;

			// Pass the address of the receiver ton the actor constructor.
			AutoSender::Parameters params(receiver.GetAddress());
			xlang::ActorRef actor(framework.CreateActor<AutoSender>(params));

			// Wait for the messages sent by the actor on construction.
			receiver.Wait();
			receiver.Wait();
		}

		UNITTEST_TEST(DeregisterHandlerInConstructor)
		{
			typedef Catcher<int> IntCatcher;

			xlang::Framework framework;

			xlang::Receiver receiver;
			IntCatcher catcher;
			receiver.RegisterHandler(&catcher, &IntCatcher::Catch);

			xlang::ActorRef actor(framework.CreateActor<AutoDeregistrar>());

			// Send the actor a message and check that the first handler doesn't send us a reply.
			framework.Send(0, receiver.GetAddress(), actor.GetAddress());

			receiver.Wait();
			CHECK_TRUE(catcher.mMessage == 2);    // Received wrong message");
			CHECK_TRUE(receiver.Count() == 0);    // Received too many messages");
		}


		UNITTEST_TEST(CreateActorInDestructor)
		{
			xlang::Framework framework;

			TailRecursor::Parameters params;
			params.mCount = 10;
			xlang::ActorRef actor(framework.CreateActor<TailRecursor>(params));
		}

		UNITTEST_TEST(SendMessageInDestructor)
		{
			xlang::Framework framework;
			xlang::Receiver receiver;

			{
				// Pass the address of the receiver ton the actor constructor.
				TailSender::Parameters params(receiver.GetAddress());
				xlang::ActorRef actor(framework.CreateActor<TailSender>(params));
			}

			// Wait for the messages sent by the actor on construction.
			receiver.Wait();
			receiver.Wait();
		}

		UNITTEST_TEST(DeregisterHandlerInDestructor)
		{
			// We check that it's safe to deregister a handler in an actor destructor,
			// but since it can't handle messages after destruction, there's little effect.
			xlang::Framework framework;
			xlang::ActorRef actor(framework.CreateActor<TailDeregistrar>());
		}

		UNITTEST_TEST(CreateActorInHandler)
		{
			xlang::Framework framework;
			xlang::Receiver receiver;
			xlang::ActorRef actor(framework.CreateActor<Nestor>());

			framework.Send(CreateMessage(), receiver.GetAddress(), actor.GetAddress());
			framework.Send(DestroyMessage(), receiver.GetAddress(), actor.GetAddress());

			receiver.Wait();
		}

		UNITTEST_TEST(GetNumQueuedMessagesInHandler)
		{
			typedef Catcher<xlang::u32> CountCatcher;

			xlang::Framework framework;
			xlang::Receiver receiver;

			CountCatcher catcher;
			receiver.RegisterHandler(&catcher, &CountCatcher::Catch);

			xlang::ActorRef actor(framework.CreateActor<MessageQueueCounter>());

			// Send the actor two messages.
			framework.Send(0, receiver.GetAddress(), actor.GetAddress());
			framework.Send(0, receiver.GetAddress(), actor.GetAddress());

			// Wait for and check both replies.
			// Race condition decides whether the second message has already arrived.
			receiver.Wait();
			CHECK_TRUE(catcher.mMessage == 0 || catcher.mMessage == 1);    // Bad count");

			receiver.Wait();
			CHECK_TRUE(catcher.mMessage == 0);    // Bad count");
		}

		UNITTEST_TEST(GetNumQueuedMessagesInFunction)
		{
			typedef const char * StringMessage;
			typedef Catcher<xlang::u32> CountCatcher;

			xlang::Framework framework;
			xlang::Receiver receiver;

			xlang::ActorRef actor(framework.CreateActor<StringReplier>());

			// Send the actor two messages.
			StringMessage stringMessage("hello");
			framework.Send(stringMessage, receiver.GetAddress(), actor.GetAddress());
			framework.Send(stringMessage, receiver.GetAddress(), actor.GetAddress());

			// Race conditions decide how many messages are in the queue when we ask.
			xlang::u32 numMessages(actor.GetNumQueuedMessages());
			CHECK_TRUE(numMessages < 3);    // Bad count");

			receiver.Wait();

			numMessages = actor.GetNumQueuedMessages();
			CHECK_TRUE(numMessages < 2);    // Bad count");

			receiver.Wait();

			numMessages = actor.GetNumQueuedMessages();
			CHECK_TRUE(numMessages == 0);    // Bad count");
		}

		UNITTEST_TEST(UseBlindDefaultHandler)
		{
			typedef Accumulator<xlang::u32> UIntAccumulator;

			xlang::Framework framework;
			xlang::Receiver receiver;

			UIntAccumulator accumulator;
			receiver.RegisterHandler(&accumulator, &UIntAccumulator::Catch);

			xlang::ActorRef actor(framework.CreateActor<BlindActor>());

			// Send the actor a u32 message, which is the type it secretly expects.
			framework.Send(xlang::u32(75), receiver.GetAddress(), actor.GetAddress());

			// The actor sends back the value of the message data and the size.
			receiver.Wait();
			receiver.Wait();

			CHECK_TRUE(accumulator.Pop() == 75);    // Bad blind data");
			CHECK_TRUE(accumulator.Pop() == 4);    // Bad blind data size");
		}

		UNITTEST_TEST(IsHandlerRegisteredInHandler)
		{
			typedef Accumulator<bool> BoolAccumulator;

			xlang::Framework framework;
			xlang::Receiver receiver;

			BoolAccumulator accumulator;
			receiver.RegisterHandler(&accumulator, &BoolAccumulator::Catch);

			xlang::ActorRef actor(framework.CreateActor<HandlerChecker>());
			framework.Send(int(0), receiver.GetAddress(), actor.GetAddress());

			xlang::u32 count(21);
			while (count)
			{
				count -= receiver.Wait(count);
			}

			CHECK_TRUE(accumulator.Pop() == true);    // Bad registration check result 0");
			CHECK_TRUE(accumulator.Pop() == false);    // Bad registration check result 1");
			CHECK_TRUE(accumulator.Pop() == false);    // Bad registration check result 2");

			//RegisterHandler(this, &HandlerChecker::Dummy);
			CHECK_TRUE(accumulator.Pop() == true);    // Bad registration check result 3");
			CHECK_TRUE(accumulator.Pop() == true);    // Bad registration check result 4");
			CHECK_TRUE(accumulator.Pop() == false);    // Bad registration check result 5");

			// DeregisterHandler(this, &HandlerChecker::Dummy);
			CHECK_TRUE(accumulator.Pop() == true);    // Bad registration check result 6");
			CHECK_TRUE(accumulator.Pop() == false);    // Bad registration check result 7");
			CHECK_TRUE(accumulator.Pop() == false);    // Bad registration check result 8");

			// DeregisterHandler(this, &HandlerChecker::Check);
			CHECK_TRUE(accumulator.Pop() == false);    // Bad registration check result 9");
			CHECK_TRUE(accumulator.Pop() == false);    // Bad registration check result 10");
			CHECK_TRUE(accumulator.Pop() == false);    // Bad registration check result 11");

			// RegisterHandler(this, &HandlerChecker::Dummy);
			// RegisterHandler(this, &HandlerChecker::Check);
			// RegisterHandler(this, &HandlerChecker::Check);
			CHECK_TRUE(accumulator.Pop() == true);    // Bad registration check result 12");
			CHECK_TRUE(accumulator.Pop() == true);    // Bad registration check result 13");
			CHECK_TRUE(accumulator.Pop() == false);    // Bad registration check result 14");

			// DeregisterHandler(this, &HandlerChecker::Check);
			CHECK_TRUE(accumulator.Pop() == true);    // Bad registration check result 15");
			CHECK_TRUE(accumulator.Pop() == true);    // Bad registration check result 16");
			CHECK_TRUE(accumulator.Pop() == false);    // Bad registration check result 17");

			// DeregisterHandler(this, &HandlerChecker::Check);
			CHECK_TRUE(accumulator.Pop() == false);    // Bad registration check result 18");
			CHECK_TRUE(accumulator.Pop() == true);    // Bad registration check result 19");
			CHECK_TRUE(accumulator.Pop() == false);    // Bad registration check result 20");
		}

		UNITTEST_TEST(SendMessagesToShortLivedActor)
		{
			typedef Accumulator<xlang::u32> UIntAccumulator;
			typedef Replier<xlang::u32> UIntReplier;

			xlang::Framework framework;
			xlang::Receiver receiver;

			UIntAccumulator accumulator;
			receiver.RegisterHandler(&accumulator, &UIntAccumulator::Catch);

			{
				xlang::ActorRef actor(framework.CreateActor<UIntReplier>());

				// Send the actor a series of messages.
				framework.Send(xlang::u32(0), receiver.GetAddress(), actor.GetAddress());
				framework.Send(xlang::u32(1), receiver.GetAddress(), actor.GetAddress());
				framework.Send(xlang::u32(2), receiver.GetAddress(), actor.GetAddress());
				framework.Send(xlang::u32(3), receiver.GetAddress(), actor.GetAddress());
				framework.Send(xlang::u32(4), receiver.GetAddress(), actor.GetAddress());
				framework.Send(xlang::u32(5), receiver.GetAddress(), actor.GetAddress());
				framework.Send(xlang::u32(6), receiver.GetAddress(), actor.GetAddress());
				framework.Send(xlang::u32(7), receiver.GetAddress(), actor.GetAddress());
			}

			// Check that we received the replies even though the actor became unreferenced.

			xlang::u32 count(8);
			while (count)
			{
				count -= receiver.Wait(8);
			}

			CHECK_TRUE(accumulator.Pop() == 0);    // Bad returned message");
			CHECK_TRUE(accumulator.Pop() == 1);    // Bad returned message");
			CHECK_TRUE(accumulator.Pop() == 2);    // Bad returned message");
			CHECK_TRUE(accumulator.Pop() == 3);    // Bad returned message");
			CHECK_TRUE(accumulator.Pop() == 4);    // Bad returned message");
			CHECK_TRUE(accumulator.Pop() == 5);    // Bad returned message");
			CHECK_TRUE(accumulator.Pop() == 6);    // Bad returned message");
			CHECK_TRUE(accumulator.Pop() == 7);    // Bad returned message");
		}

		UNITTEST_TEST(SetFallbackHandler)
		{
			xlang::Framework framework;
			FallbackHandler fallbackHandler;

			CHECK_TRUE(framework.SetFallbackHandler(&fallbackHandler, &FallbackHandler::Handle));    // Register failed");
		}

		UNITTEST_TEST(HandleUndeliveredMessageSentInFunction)
		{
			xlang::Framework framework;
			xlang::Receiver receiver;

			FallbackHandler fallbackHandler;
			framework.SetFallbackHandler(&fallbackHandler, &FallbackHandler::Handle);

			// Create an actor and let it die but remember its address.
			xlang::Address lastGaspAddress;

			{
				xlang::ActorRef actor(framework.CreateActor<LastGasp>(receiver.GetAddress()));
				lastGaspAddress = actor.GetAddress();
			}

			// Wait for a signal message indicating the actor is being destructed.
			receiver.Wait();

			// Send a message to the stale address.
			framework.Send(0, receiver.GetAddress(), lastGaspAddress);

			// Check that the undelivered message was handled by the registered fallback handler.
			CHECK_TRUE(fallbackHandler.mAddress == receiver.GetAddress());    // Fallback handler failed");
		}

		UNITTEST_TEST(HandleUndeliveredMessageSentInHandler)
		{
			FallbackHandler fallbackHandler;
			xlang::Address signalerAddress;

			{
				xlang::Framework framework;
				xlang::Receiver receiver;

				framework.SetFallbackHandler(&fallbackHandler, &FallbackHandler::Handle);

				// Create an actor and let it die but remember its address.
				xlang::Address lastGaspAddress;

				{
					xlang::ActorRef actor(framework.CreateActor<LastGasp>(receiver.GetAddress()));
					lastGaspAddress = actor.GetAddress();
				}

				// Wait for a signal message indicating the actor is being destructed.
				receiver.Wait();

				// Create a second actor to send a message to the stale address of the first.
				// The Signaler sends a message to the address it is sent.
				xlang::ActorRef signaler(framework.CreateActor<Signaler>());
				signalerAddress = signaler.GetAddress();
				framework.Send(lastGaspAddress, receiver.GetAddress(), signalerAddress);

				// Destruct the framework to ensure all messages have been processed.
			}

			// Check that the undelivered message was handled by the registered fallback handler.
			CHECK_TRUE(fallbackHandler.mAddress == signalerAddress);    // Fallback handler failed");
		}

		UNITTEST_TEST(HandleUnhandledMessageSentInFunction)
		{
			typedef Replier<xlang::u32> UIntReplier;

			xlang::Framework framework;
			xlang::Receiver receiver;

			FallbackHandler fallbackHandler;
			framework.SetFallbackHandler(&fallbackHandler, &FallbackHandler::Handle);

			// Create a replier that handles only ints, then send it a float.
			xlang::ActorRef replier(framework.CreateActor<UIntReplier>());
			framework.Send(5.0f, receiver.GetAddress(), replier.GetAddress());

			// Send the replier an int and wait for the reply so we know both messages
			// have been processed.
			framework.Send(xlang::u32(5), receiver.GetAddress(), replier.GetAddress());
			receiver.Wait();

			// Check that the unhandled message was handled by the registered fallback handler.
			CHECK_TRUE(fallbackHandler.mAddress == receiver.GetAddress());    // Fallback handler failed");
		}

		UNITTEST_TEST(HandleUndeliveredBlindMessageSentInFunction)
		{
			xlang::Framework framework;
			xlang::Receiver receiver;

			BlindFallbackHandler fallbackHandler;
			framework.SetFallbackHandler(&fallbackHandler, &BlindFallbackHandler::Handle);

			// Create an actor and let it die but remember its address.
			xlang::Address lastGaspAddress;

			{
				xlang::ActorRef actor(framework.CreateActor<LastGasp>(receiver.GetAddress()));
				lastGaspAddress = actor.GetAddress();
			}

			// Wait for a signal message indicating the actor is being destructed.
			receiver.Wait();

			// Send a message to the stale address.
			framework.Send(xlang::u32(42), receiver.GetAddress(), lastGaspAddress);

			// Check that the undelivered message was handled by the registered fallback handler.
			CHECK_TRUE(fallbackHandler.mData != 0);    // Blind fallback handler failed");
			CHECK_TRUE(fallbackHandler.mValue == 42);    // Blind fallback handler failed");
			CHECK_TRUE(fallbackHandler.mSize == sizeof(xlang::u32));    // Blind fallback handler failed");
			CHECK_TRUE(fallbackHandler.mAddress == receiver.GetAddress());    // Blind fallback handler failed");
		}

		UNITTEST_TEST(SendRegisteredMessage)
		{
			typedef Replier<IntVectorMessage> IntVectorReplier;
			typedef Catcher<IntVectorMessage> IntVectorCatcher;

			xlang::Framework framework;
			xlang::Receiver receiver;
			IntVectorCatcher catcher;

			receiver.RegisterHandler(&catcher, &IntVectorCatcher::Catch);
			xlang::ActorRef replier(framework.CreateActor<IntVectorReplier>());

			IntVectorMessage message;
			message.a = (0);
			message.b = (1);
			message.c = (2);
			message.n = (3);

			framework.Send(message, receiver.GetAddress(), replier.GetAddress());
			receiver.Wait();

			CHECK_TRUE(catcher.mMessage.n == 3);    // Bad reply message");
			CHECK_TRUE(catcher.mMessage.a == 0);    // Bad reply message");
			CHECK_TRUE(catcher.mMessage.b == 1);    // Bad reply message");
			CHECK_TRUE(catcher.mMessage.c == 2);    // Bad reply message");
		}

#endif
	};
}
UNITTEST_SUITE_END



#endif	// TESTS_TESTSUITES_FEATURETESTSUITE
