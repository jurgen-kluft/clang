#define TESTS_TESTSUITES_FEATURETESTSUITE
#ifdef TESTS_TESTSUITES_FEATURETESTSUITE

#include <string>
#include <queue>

#include "clang\private\debug\x_assert.h"
#include "clang\x_actor.h"
#include "clang\x_actorref.h"
#include "clang\x_address.h"
#include "clang\x_framework.h"
#include "clang\x_receiver.h"
#include "clang\x_register.h"

#include "cunittest\cunittest.h"



template <class CountType>
class Sequencer : public clang::Actor
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

	inline void Receive(const CountType &message, const clang::Address /*from*/)
	{
		if (message != mNextValue++)
		{
			mStatus = BAD;
		}
	}

	inline void GetValue(const bool &/*message*/, const clang::Address from)
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

//typedef std::vector<clang::u32> IntVectorMessage;
struct IntVectorMessage
{
	clang::u32		n;
	clang::u32		a;
	clang::u32		b;
	clang::u32		c;
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

		class Trivial : public clang::Actor
		{
		};

		template <class MessageType>
		class Replier : public clang::Actor
		{
		public:

			inline Replier()
			{
				RegisterHandler(this, &Replier::Handler);
			}

		private:

			inline void Handler(const MessageType &message, const clang::Address from)
			{
				Send(message, from);
			}
		};

		template <class MessageType>
		class DefaultReplier : public clang::Actor
		{
		public:

			inline DefaultReplier()
			{
				RegisterHandler(this, &DefaultReplier::Handler);
				SetDefaultHandler(this, &DefaultReplier::DefaultHandler);
			}

		private:

			inline void Handler(const MessageType &message, const clang::Address from)
			{
				Send(message, from);
			}

			inline void DefaultHandler(const clang::Address from)
			{
				std::string hello("hello");
				Send(hello, from);
			}
		};

		class StringReplier : public Replier<const char *>
		{
		};

		class Signaler : public clang::Actor
		{
		public:

			inline Signaler()
			{
				RegisterHandler(this, &Signaler::Signal);
			}

		private:

			inline void Signal(const clang::Address &address, const clang::Address from)
			{
				// Send the 'from' address to the address received in the message.
				Send(from, address);
			}
		};

		class Poker : public clang::Actor
		{
		public:

			inline Poker()
			{
				RegisterHandler(this, &Poker::Poke);
			}

		private:

			inline void Poke(const clang::ActorRef &actor, const clang::Address from)
			{
				// Poke the actor referenced by the message, sending it the from address.
				Send(from, actor.GetAddress());
			}
		};

		class Switcher : public clang::Actor
		{
		public:

			inline Switcher()
			{
				RegisterHandler(this, &Switcher::SayHello);
			}

		private:

			inline void SayHello(const std::string &/*message*/, const clang::Address from)
			{
				DeregisterHandler(this, &Switcher::SayHello);
				RegisterHandler(this, &Switcher::SayGoodbye);
				Send(std::string("hello"), from);
			}

			inline void SayGoodbye(const std::string &/*message*/, const clang::Address from)
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

			inline Catcher() : mMessage(), mFrom(clang::Address::Null())
			{
			}

			inline void Catch(const MessageType &message, const clang::Address from)
			{
				mMessage = message;
				mFrom = from;
			}

			MessageType mMessage;
			clang::Address mFrom;
		};

		struct Holder
		{
			clang::ActorRef mRef;
		};

		class Counter : public clang::Actor
		{
		public:

			inline Counter() : mCount(0)
			{
				RegisterHandler(this, &Counter::Increment);
				RegisterHandler(this, &Counter::GetValue);
			}

		private:

			inline void Increment(const int &message, const clang::Address /*from*/)
			{
				mCount += message;
			}

			inline void GetValue(const bool &/*message*/, const clang::Address from)
			{
				Send(mCount, from);
			}

			int mCount;
		};

		class TwoHandlerCounter : public clang::Actor
		{
		public:

			inline TwoHandlerCounter() : mCount(0)
			{
				RegisterHandler(this, &TwoHandlerCounter::IncrementOne);
				RegisterHandler(this, &TwoHandlerCounter::IncrementTwo);
				RegisterHandler(this, &TwoHandlerCounter::GetValue);
			}

		private:

			inline void IncrementOne(const int &message, const clang::Address /*from*/)
			{
				mCount += message;
			}

			inline void IncrementTwo(const float &/*message*/, const clang::Address /*from*/)
			{
				++mCount;
			}

			inline void GetValue(const bool &/*message*/, const clang::Address from)
			{
				Send(mCount, from);
			}

			int mCount;
		};

		class MultipleHandlerCounter : public clang::Actor
		{
		public:

			inline MultipleHandlerCounter() : mCount(0)
			{
				RegisterHandler(this, &MultipleHandlerCounter::IncrementOne);
				RegisterHandler(this, &MultipleHandlerCounter::IncrementTwo);
				RegisterHandler(this, &MultipleHandlerCounter::GetValue);
			}

		private:

			inline void IncrementOne(const int &message, const clang::Address /*from*/)
			{
				mCount += message;
			}

			inline void IncrementTwo(const int &/*message*/, const clang::Address /*from*/)
			{
				++mCount;
			}

			inline void GetValue(const bool &/*message*/, const clang::Address from)
			{
				Send(mCount, from);
			}

			int mCount;
		};


		class Parameterized : public clang::Actor
		{
		public:

			struct Parameters
			{
				clang::Address mAddress;
			};

			inline Parameterized(const Parameters &params) : mAddress(params.mAddress)
			{
				RegisterHandler(this, &Parameterized::Handler);
			}

			inline void Handler(const int &message, const clang::Address /*from*/)
			{
				// Send a message to the address provided on construction.
				Send(message, mAddress);
			}

		private:

			clang::Address mAddress;
		};

		class Recursor : public clang::Actor
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

					clang::Framework &framework(GetFramework());
					clang::ActorRef child(framework.CreateActor<Recursor>(childParams));
				}
			}
		};

		class TailRecursor : public clang::Actor
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

					clang::Framework &framework(GetFramework());
					clang::ActorRef child(framework.CreateActor<TailRecursor>(childParams));
				}
			}

		private:

			int mCount;
		};

		class AutoSender : public clang::Actor
		{
		public:

			typedef clang::Address Parameters;

			inline AutoSender(const Parameters &address)
			{
				// Send a message in the actor constructor.
				Send(0, address);

				// Send using TailSend to check that works too.
				TailSend(1, address);
			}
		};

		class TailSender : public clang::Actor
		{
		public:

			typedef clang::Address Parameters;

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

			clang::Address mAddress;
		};

		class AutoDeregistrar : public clang::Actor
		{
		public:

			inline AutoDeregistrar()
			{
				RegisterHandler(this, &AutoDeregistrar::HandlerOne);
				RegisterHandler(this, &AutoDeregistrar::HandlerTwo);
				DeregisterHandler(this, &AutoDeregistrar::HandlerOne);
			}

			inline void HandlerOne(const int &/*message*/, const clang::Address from)
			{
				Send(1, from);
			}

			inline void HandlerTwo(const int &/*message*/, const clang::Address from)
			{
				Send(2, from);
			}
		};

		class TailDeregistrar : public clang::Actor
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

			inline void Handler(const int &/*message*/, const clang::Address from)
			{
				Send(0, from);
			}
		};

		class MessageQueueCounter : public clang::Actor
		{
		public:

			inline MessageQueueCounter()
			{
				RegisterHandler(this, &MessageQueueCounter::Handler);
			}

			inline void Handler(const int &/*message*/, const clang::Address from)
			{
				Send(GetNumQueuedMessages(), from);
			}
		};

		class BlindActor : public clang::Actor
		{
		public:

			inline BlindActor()
			{
				SetDefaultHandler(this, &BlindActor::BlindDefaultHandler);
			}

		private:

			inline void BlindDefaultHandler(const void *const data, const clang::u32 size, const clang::Address from)
			{
				// We know the message is a u32.
				const clang::u32 *const p(reinterpret_cast<const clang::u32 *>(data));
				const clang::u32 value(*p);

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

			inline void Catch(const MessageType &message, const clang::Address /*from*/)
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

		class HandlerChecker : public clang::Actor
		{
		public:

			inline HandlerChecker()
			{
				RegisterHandler(this, &HandlerChecker::Check2);
			}

		private:

			inline void Check2(const int & /*message*/, const clang::Address from)
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

			inline void Dummy(const int & message, const clang::Address from)
			{
				// We do this just so that Dummy and Unregistered differ, so the compiler won't merge them!
				Send(message, from);
			}

			inline void Unregistered(const int & /*message*/, const clang::Address /*from*/)
			{
			}
		};

		class Nestor : public clang::Actor
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

			inline void Create(const CreateMessage & /*message*/, const clang::Address /*from*/)
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

			inline void Destroy(const DestroyMessage & /*message*/, const clang::Address from)
			{
				mCaller = from;
				if (mReplies[0] && mReplies[1] && mReplies[2])
				{
					mChildren.clear();
					Send(true, mCaller);
				}
			}

			inline void Receive(const int & message, const clang::Address /*from*/)
			{
				mReplies[message] = true;

				if (mCaller != clang::Address::Null() && mReplies[0] && mReplies[1] && mReplies[2])
				{
					mChildren.clear();
					Send(true, mCaller);
				}
			}

			std::vector<clang::ActorRef> mChildren;
			std::vector<bool> mReplies;
			clang::Address mCaller;
		};

		class FallbackHandler
		{
		public:

			inline void Handle(const clang::Address from)
			{
				mAddress = from;
			}

			clang::Address mAddress;
		};

		class BlindFallbackHandler
		{
		public:

			BlindFallbackHandler() : mData(0), mValue(0), mSize(0)
			{
			}

			inline void Handle(const void *const data, const clang::u32 size, const clang::Address from)
			{
				mData = data;
				mValue = *reinterpret_cast<const clang::u32 *>(data);
				mSize = size;
				mAddress = from;
			}

			const void *mData;
			clang::u32 mValue;
			clang::u32 mSize;
			clang::Address mAddress;
		};

		class LastGasp : public clang::Actor
		{
		public:

			typedef clang::Address Parameters;

			inline explicit LastGasp(const clang::Address address) : mAddress(address)
			{
			}

			inline ~LastGasp()
			{
				Send(0, mAddress);
			}

		private:

			clang::Address mAddress;
		};


		UNITTEST_TEST(NullActorReference)
		{
			clang::ActorRef nullReference;
			CHECK_TRUE(nullReference == clang::ActorRef::Null());    // Default-constructed reference isn't null");
			CHECK_TRUE((nullReference != clang::ActorRef::Null()) == false);    // Default-constructed reference isn't null");
		}

		UNITTEST_TEST(CreateActorInFunction)
		{
			clang::Framework framework;
			clang::ActorRef actor(framework.CreateActor<Trivial>());
		}

		UNITTEST_TEST(CreateActorWithParamsInFunction)
		{
			clang::Framework framework;
			clang::Receiver receiver;

			Parameterized::Parameters params;
			params.mAddress = receiver.GetAddress();
			clang::ActorRef actor(framework.CreateActor<Parameterized>(params));

			framework.Send(0, clang::Address::Null(), actor.GetAddress());

			receiver.Wait();
		}

		UNITTEST_TEST(SendMessageToReceiverInFunction)
		{
			clang::Framework framework;
			clang::Receiver receiver;
			framework.Send(0.0f, receiver.GetAddress(), receiver.GetAddress());
		}

		UNITTEST_TEST(SendMessageFromNullAddressInFunction)
		{
			clang::Framework framework;
			clang::Receiver receiver;

			framework.Send(0, clang::Address::Null(), receiver.GetAddress());
			receiver.Wait();
		}

		UNITTEST_TEST(SendMessageToActorFromNullAddressInFunction)
		{
			clang::Framework framework;
			clang::Receiver receiver;
			clang::ActorRef signaler(framework.CreateActor<Signaler>());

			framework.Send(receiver.GetAddress(), clang::Address::Null(), signaler.GetAddress());
			receiver.Wait();
		}

		UNITTEST_TEST(SendMessageToActorFromReceiverInFunction)
		{
			clang::Framework framework;
			clang::Receiver receiver;
			clang::ActorRef signaler(framework.CreateActor<Signaler>());

			framework.Send(receiver.GetAddress(), receiver.GetAddress(), signaler.GetAddress());
			receiver.Wait();
		}

		UNITTEST_TEST(PushMessageToActorFromNullAddressInFunction)
		{
			clang::Framework framework;
			clang::Receiver receiver;
			clang::ActorRef signaler(framework.CreateActor<Signaler>());

			signaler.Push(receiver.GetAddress(), clang::Address::Null());
			receiver.Wait();
		}

		UNITTEST_TEST(PushMessageToActorFromReceiverInFunction)
		{
			clang::Framework framework;
			clang::Receiver receiver;
			clang::ActorRef signaler(framework.CreateActor<Signaler>());

			signaler.Push(receiver.GetAddress(), receiver.GetAddress());
			receiver.Wait();
		}

		UNITTEST_TEST(ReceiveReplyInFunction)
		{
			typedef Replier<float> FloatReplier;

			clang::Framework framework;
			clang::Receiver receiver;
			clang::ActorRef actor(framework.CreateActor<FloatReplier>());

			framework.Send(5.0f, receiver.GetAddress(), actor.GetAddress());

			receiver.Wait();
		}

		UNITTEST_TEST(CatchReplyInFunction)
		{
			typedef Replier<float> FloatReplier;
			typedef Catcher<float> FloatCatcher;

			clang::Framework framework;
			clang::ActorRef actor(framework.CreateActor<FloatReplier>());

			clang::Receiver receiver;
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

			clang::Framework framework;
			clang::Receiver receiver;

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

			clang::Framework framework;
			clang::ActorRef actor(framework.CreateActor<VectorReplier>());

			clang::Receiver receiver;
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

			clang::Framework framework;
			clang::ActorRef actor(framework.CreateActor<PointerReplier>());

			clang::Receiver receiver;
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

			clang::Framework framework;
			clang::ActorRef actor(framework.CreateActor<PointerReplier>());

			clang::Receiver receiver;
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
			clang::Framework framework;
			clang::ActorRef actor(framework.CreateActor<StringReplier>());
		}

		UNITTEST_TEST(SendMessageToDerivedActor)
		{
			typedef const char * StringMessage;
			typedef Catcher<StringMessage> StringCatcher;

			clang::Framework framework;
			clang::ActorRef actor(framework.CreateActor<StringReplier>());

			clang::Receiver receiver;
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

			clang::Framework framework;
			clang::ActorRef actor(framework.CreateActor<Counter>());

			clang::Receiver receiver;
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

			clang::Framework framework;
			clang::ActorRef actor(framework.CreateActor<IntReplier>());

			clang::Receiver receiver;
			framework.Send(10, receiver.GetAddress(), actor.GetAddress());

			receiver.Wait();
		}

		UNITTEST_TEST(OneHandlerAtATime)
		{
			typedef Catcher<int> CountCatcher;

			clang::Framework framework;
			clang::ActorRef actor(framework.CreateActor<TwoHandlerCounter>());

			clang::Receiver receiver;
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

			clang::Framework framework;
			clang::ActorRef actor(framework.CreateActor<MultipleHandlerCounter>());

			clang::Receiver receiver;
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

			clang::Framework framework;
			clang::ActorRef actor(framework.CreateActor<IntSequencer>());

			clang::Receiver receiver;
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
			typedef Catcher<clang::Address> AddressCatcher;

			clang::Framework framework;
			clang::ActorRef actorA(framework.CreateActor<Signaler>());
			clang::ActorRef actorB(framework.CreateActor<Signaler>());

			clang::Receiver receiver;
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
			typedef Catcher<clang::Address> AddressCatcher;

			clang::Framework framework;
			clang::ActorRef actorA(framework.CreateActor<Poker>());
			clang::ActorRef actorB(framework.CreateActor<Signaler>());

			clang::Receiver receiver;
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

			clang::Framework framework;
			clang::ActorRef actor(framework.CreateActor<FloatReplier>());

			StringCatcher catcher;
			clang::Receiver receiver;
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

			clang::Framework framework;
			clang::ActorRef actor(framework.CreateActor<Switcher>());

			StringCatcher catcher;
			clang::Receiver receiver;
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
			clang::Framework framework;

			Recursor::Parameters params;
			params.mCount = 10;
			clang::ActorRef actor(framework.CreateActor<Recursor>(params));
		}

		UNITTEST_TEST(SendMessageInConstructor)
		{
			clang::Framework framework;
			clang::Receiver receiver;

			// Pass the address of the receiver ton the actor constructor.
			AutoSender::Parameters params(receiver.GetAddress());
			clang::ActorRef actor(framework.CreateActor<AutoSender>(params));

			// Wait for the messages sent by the actor on construction.
			receiver.Wait();
			receiver.Wait();
		}

		UNITTEST_TEST(DeregisterHandlerInConstructor)
		{
			typedef Catcher<int> IntCatcher;

			clang::Framework framework;

			clang::Receiver receiver;
			IntCatcher catcher;
			receiver.RegisterHandler(&catcher, &IntCatcher::Catch);

			clang::ActorRef actor(framework.CreateActor<AutoDeregistrar>());

			// Send the actor a message and check that the first handler doesn't send us a reply.
			framework.Send(0, receiver.GetAddress(), actor.GetAddress());

			receiver.Wait();
			CHECK_TRUE(catcher.mMessage == 2);    // Received wrong message");
			CHECK_TRUE(receiver.Count() == 0);    // Received too many messages");
		}


		UNITTEST_TEST(CreateActorInDestructor)
		{
			clang::Framework framework;

			TailRecursor::Parameters params;
			params.mCount = 10;
			clang::ActorRef actor(framework.CreateActor<TailRecursor>(params));
		}

		UNITTEST_TEST(SendMessageInDestructor)
		{
			clang::Framework framework;
			clang::Receiver receiver;

			{
				// Pass the address of the receiver ton the actor constructor.
				TailSender::Parameters params(receiver.GetAddress());
				clang::ActorRef actor(framework.CreateActor<TailSender>(params));
			}

			// Wait for the messages sent by the actor on construction.
			receiver.Wait();
			receiver.Wait();
		}

		UNITTEST_TEST(DeregisterHandlerInDestructor)
		{
			// We check that it's safe to deregister a handler in an actor destructor,
			// but since it can't handle messages after destruction, there's little effect.
			clang::Framework framework;
			clang::ActorRef actor(framework.CreateActor<TailDeregistrar>());
		}

		UNITTEST_TEST(CreateActorInHandler)
		{
			clang::Framework framework;
			clang::Receiver receiver;
			clang::ActorRef actor(framework.CreateActor<Nestor>());

			framework.Send(CreateMessage(), receiver.GetAddress(), actor.GetAddress());
			framework.Send(DestroyMessage(), receiver.GetAddress(), actor.GetAddress());

			receiver.Wait();
		}

		UNITTEST_TEST(GetNumQueuedMessagesInHandler)
		{
			typedef Catcher<clang::u32> CountCatcher;

			clang::Framework framework;
			clang::Receiver receiver;

			CountCatcher catcher;
			receiver.RegisterHandler(&catcher, &CountCatcher::Catch);

			clang::ActorRef actor(framework.CreateActor<MessageQueueCounter>());

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
			typedef Catcher<clang::u32> CountCatcher;

			clang::Framework framework;
			clang::Receiver receiver;

			clang::ActorRef actor(framework.CreateActor<StringReplier>());

			// Send the actor two messages.
			StringMessage stringMessage("hello");
			framework.Send(stringMessage, receiver.GetAddress(), actor.GetAddress());
			framework.Send(stringMessage, receiver.GetAddress(), actor.GetAddress());

			// Race conditions decide how many messages are in the queue when we ask.
			clang::u32 numMessages(actor.GetNumQueuedMessages());
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
			typedef Accumulator<clang::u32> UIntAccumulator;

			clang::Framework framework;
			clang::Receiver receiver;

			UIntAccumulator accumulator;
			receiver.RegisterHandler(&accumulator, &UIntAccumulator::Catch);

			clang::ActorRef actor(framework.CreateActor<BlindActor>());

			// Send the actor a u32 message, which is the type it secretly expects.
			framework.Send(clang::u32(75), receiver.GetAddress(), actor.GetAddress());

			// The actor sends back the value of the message data and the size.
			receiver.Wait();
			receiver.Wait();

			CHECK_TRUE(accumulator.Pop() == 75);    // Bad blind data");
			CHECK_TRUE(accumulator.Pop() == 4);    // Bad blind data size");
		}

		UNITTEST_TEST(IsHandlerRegisteredInHandler)
		{
			typedef Accumulator<bool> BoolAccumulator;

			clang::Framework framework;
			clang::Receiver receiver;

			BoolAccumulator accumulator;
			receiver.RegisterHandler(&accumulator, &BoolAccumulator::Catch);

			clang::ActorRef actor(framework.CreateActor<HandlerChecker>());
			framework.Send(int(0), receiver.GetAddress(), actor.GetAddress());

			clang::u32 count(21);
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
			typedef Accumulator<clang::u32> UIntAccumulator;
			typedef Replier<clang::u32> UIntReplier;

			clang::Framework framework;
			clang::Receiver receiver;

			UIntAccumulator accumulator;
			receiver.RegisterHandler(&accumulator, &UIntAccumulator::Catch);

			{
				clang::ActorRef actor(framework.CreateActor<UIntReplier>());

				// Send the actor a series of messages.
				framework.Send(clang::u32(0), receiver.GetAddress(), actor.GetAddress());
				framework.Send(clang::u32(1), receiver.GetAddress(), actor.GetAddress());
				framework.Send(clang::u32(2), receiver.GetAddress(), actor.GetAddress());
				framework.Send(clang::u32(3), receiver.GetAddress(), actor.GetAddress());
				framework.Send(clang::u32(4), receiver.GetAddress(), actor.GetAddress());
				framework.Send(clang::u32(5), receiver.GetAddress(), actor.GetAddress());
				framework.Send(clang::u32(6), receiver.GetAddress(), actor.GetAddress());
				framework.Send(clang::u32(7), receiver.GetAddress(), actor.GetAddress());
			}

			// Check that we received the replies even though the actor became unreferenced.

			clang::u32 count(8);
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
			clang::Framework framework;
			FallbackHandler fallbackHandler;

			CHECK_TRUE(framework.SetFallbackHandler(&fallbackHandler, &FallbackHandler::Handle));    // Register failed");
		}

		UNITTEST_TEST(HandleUndeliveredMessageSentInFunction)
		{
			clang::Framework framework;
			clang::Receiver receiver;

			FallbackHandler fallbackHandler;
			framework.SetFallbackHandler(&fallbackHandler, &FallbackHandler::Handle);

			// Create an actor and let it die but remember its address.
			clang::Address lastGaspAddress;

			{
				clang::ActorRef actor(framework.CreateActor<LastGasp>(receiver.GetAddress()));
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
			clang::Address signalerAddress;

			{
				clang::Framework framework;
				clang::Receiver receiver;

				framework.SetFallbackHandler(&fallbackHandler, &FallbackHandler::Handle);

				// Create an actor and let it die but remember its address.
				clang::Address lastGaspAddress;

				{
					clang::ActorRef actor(framework.CreateActor<LastGasp>(receiver.GetAddress()));
					lastGaspAddress = actor.GetAddress();
				}

				// Wait for a signal message indicating the actor is being destructed.
				receiver.Wait();

				// Create a second actor to send a message to the stale address of the first.
				// The Signaler sends a message to the address it is sent.
				clang::ActorRef signaler(framework.CreateActor<Signaler>());
				signalerAddress = signaler.GetAddress();
				framework.Send(lastGaspAddress, receiver.GetAddress(), signalerAddress);

				// Destruct the framework to ensure all messages have been processed.
			}

			// Check that the undelivered message was handled by the registered fallback handler.
			CHECK_TRUE(fallbackHandler.mAddress == signalerAddress);    // Fallback handler failed");
		}

		UNITTEST_TEST(HandleUnhandledMessageSentInFunction)
		{
			typedef Replier<clang::u32> UIntReplier;

			clang::Framework framework;
			clang::Receiver receiver;

			FallbackHandler fallbackHandler;
			framework.SetFallbackHandler(&fallbackHandler, &FallbackHandler::Handle);

			// Create a replier that handles only ints, then send it a float.
			clang::ActorRef replier(framework.CreateActor<UIntReplier>());
			framework.Send(5.0f, receiver.GetAddress(), replier.GetAddress());

			// Send the replier an int and wait for the reply so we know both messages
			// have been processed.
			framework.Send(clang::u32(5), receiver.GetAddress(), replier.GetAddress());
			receiver.Wait();

			// Check that the unhandled message was handled by the registered fallback handler.
			CHECK_TRUE(fallbackHandler.mAddress == receiver.GetAddress());    // Fallback handler failed");
		}

		UNITTEST_TEST(HandleUndeliveredBlindMessageSentInFunction)
		{
			clang::Framework framework;
			clang::Receiver receiver;

			BlindFallbackHandler fallbackHandler;
			framework.SetFallbackHandler(&fallbackHandler, &BlindFallbackHandler::Handle);

			// Create an actor and let it die but remember its address.
			clang::Address lastGaspAddress;

			{
				clang::ActorRef actor(framework.CreateActor<LastGasp>(receiver.GetAddress()));
				lastGaspAddress = actor.GetAddress();
			}

			// Wait for a signal message indicating the actor is being destructed.
			receiver.Wait();

			// Send a message to the stale address.
			framework.Send(clang::u32(42), receiver.GetAddress(), lastGaspAddress);

			// Check that the undelivered message was handled by the registered fallback handler.
			CHECK_TRUE(fallbackHandler.mData != 0);    // Blind fallback handler failed");
			CHECK_TRUE(fallbackHandler.mValue == 42);    // Blind fallback handler failed");
			CHECK_TRUE(fallbackHandler.mSize == sizeof(clang::u32));    // Blind fallback handler failed");
			CHECK_TRUE(fallbackHandler.mAddress == receiver.GetAddress());    // Blind fallback handler failed");
		}

		UNITTEST_TEST(SendRegisteredMessage)
		{
			typedef Replier<IntVectorMessage> IntVectorReplier;
			typedef Catcher<IntVectorMessage> IntVectorCatcher;

			clang::Framework framework;
			clang::Receiver receiver;
			IntVectorCatcher catcher;

			receiver.RegisterHandler(&catcher, &IntVectorCatcher::Catch);
			clang::ActorRef replier(framework.CreateActor<IntVectorReplier>());

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
