#define TESTS_TESTSUITES_FRAMEWORKTESTSUITE
#ifdef TESTS_TESTSUITES_FRAMEWORKTESTSUITE

#include "clang\x_Framework.h"
#include "clang\x_register.h"

#include "cunittest\cunittest.h"

// Placement new/delete
inline void*	operator new(ncore::xsize_t num_bytes, void* mem)			{ return mem; }
inline void	operator delete(void* mem, void* )							{ }

class IntMessage
{
public:

	inline explicit IntMessage(const clang::u32 value) : mValue(value)
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

	inline bool operator==(const IntMessage &other) const
	{
		return (mValue == other.mValue);
	}

private:

	clang::u32 mValue;
};


UNITTEST_SUITE_BEGIN(TESTS_TESTSUITES_FRAMEWORKTESTSUITE)
{
    UNITTEST_FIXTURE(main)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

		class SimpleActor : public clang::Actor
		{
		public:

			struct Parameters
			{
			};

			inline SimpleActor()
			{
			}

			inline explicit SimpleActor(const Parameters &/*params*/)
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

			inline void Handler(const IntMessage &value, const clang::Address from)
			{
				Send(value, from);
			}
		};

		class ThreadCountActor : public clang::Actor
		{
		public:

			inline ThreadCountActor()
			{
				RegisterHandler(this, &ThreadCountActor::SetNumThreads);
			}

		private:

			inline void SetNumThreads(const IntMessage &numThreads, const clang::Address from)
			{
				GetFramework().SetMinThreads(numThreads.Value());
				GetFramework().SetMaxThreads(numThreads.Value());

				// Send the message back to synchronize termination.
				Send(numThreads, from);
			}
		};

		UNITTEST_TEST(TestDefaultConstruction)
		{
			clang::Framework framework;
		}

		UNITTEST_TEST(TestExplicitConstruction)
		{
			clang::Framework framework(2);
		}

		UNITTEST_TEST(TestCreateActorNoParams)
		{
			clang::Framework framework;
			clang::ActorRef actorRef(framework.CreateActor<SimpleActor>());
		}

		UNITTEST_TEST(TestCreateActorWithParams)
		{
			clang::Framework framework;
			SimpleActor::Parameters params;
			clang::ActorRef actorRef(framework.CreateActor<SimpleActor>(params));
		}

		UNITTEST_TEST(TestGetNumThreadsInitial)
		{
			clang::Framework framework(1);
			CHECK_TRUE(framework.GetNumThreads() == 1);    // Too many threads created");
		}

		UNITTEST_TEST(TestGetNumThreadsAfterSetMinThreads)
		{
			clang::Framework framework(1);
			framework.SetMinThreads(3);

			// Thread count should eventually reach the requested limit.
			// But we don't want to busy-wait in the test so we just check the bound.
			CHECK_TRUE(framework.GetNumThreads() >= 1);    // Too few threads");
		}

		UNITTEST_TEST(TestGetNumThreadsAfterSetMaxThreads)
		{
			clang::Framework framework(3);
			clang::Receiver receiver;
			clang::ActorRef actorRef(framework.CreateActor<SimpleActor>());

			// Thread count should eventually reach the requested limit.
			// But we don't want to busy-wait in the test so we just check the bound.
			CHECK_TRUE(framework.GetNumThreads() >= 1);    // Too few threads");

			framework.SetMaxThreads(1);
			CHECK_TRUE(framework.GetNumThreads() <= 3);    // Too many threads");
		}

		UNITTEST_TEST(TestGetPeakThreadsInitial)
		{
			clang::Framework framework(3);

			// Thread count should eventually reach the requested limit.
			// But we don't want to busy-wait in the test so we just check the bound.
			CHECK_TRUE(framework.GetPeakThreads() <= 3);    // Too many threads created");
		}

		UNITTEST_TEST(TestGetPeakThreadsAfterSetMinThreads)
		{
			clang::Framework framework(1);
			framework.SetMinThreads(3);

			// Thread count should eventually reach the requested limit.
			// But we don't want to busy-wait in the test so we just check the bound.
			CHECK_TRUE(framework.GetPeakThreads() >= 1);    // Too few threads");
		}

		UNITTEST_TEST(TestGetPeakThreadsAfterSetMaxThreads)
		{
			clang::Framework framework(3);
			clang::Receiver receiver;
			clang::ActorRef actorRef(framework.CreateActor<SimpleActor>());

			// Thread count should eventually reach the requested limit.
			// But we don't want to busy-wait in the test so we just check the bound.
			CHECK_TRUE(framework.GetPeakThreads() <= 3);    // Peak thread count incorrect");

			framework.SetMaxThreads(1);
			CHECK_TRUE(framework.GetPeakThreads() <= 3);    // Peak thread count incorrect");
		}

		UNITTEST_TEST(TestGetMaxThreadsDefault)
		{
			clang::Framework framework;
			CHECK_TRUE(framework.GetMaxThreads() >= 2);    // Bad default max thread limit");
		}

		UNITTEST_TEST(TestGetMaxThreadsInitial)
		{
			clang::Framework framework(5);
			CHECK_TRUE(framework.GetMaxThreads() >= 5);    // Bad initial max thread limit");
		}

		UNITTEST_TEST(TestGetMaxThreadsAfterSetMaxThreads)
		{
			// Setting a higher maximum may have no effect but shouldn't reduce it.
			clang::Framework framework(2);
			framework.SetMaxThreads(5);
			CHECK_TRUE(framework.GetMaxThreads() >= 2);    // Bad max thread limit");
			CHECK_TRUE(framework.GetMaxThreads() <= 5);    // Bad max thread limit");
		}

		UNITTEST_TEST(TestGetMaxThreadsAfterSetMinThreads)
		{
			clang::Framework framework(5);

			// Setting a lower minimum may or may not cause the maximum to drop.
			framework.SetMinThreads(2);
			CHECK_TRUE(framework.GetMaxThreads() >= 2);    // Max thread limit too low");
			CHECK_TRUE(framework.GetMaxThreads() <= 5);    // Max thread limit too high");

			// Setting a higher minimum should cause the maximum to rise.
			framework.SetMinThreads(7);
			CHECK_TRUE(framework.GetMaxThreads() >= 7);    // Max thread limit too low");
		}

		UNITTEST_TEST(TestGetMinThreadsDefault)
		{
			clang::Framework framework;
			CHECK_TRUE(framework.GetMinThreads() <= 2);    // Bad default min thread limit");
		}

		UNITTEST_TEST(TestGetMinThreadsInitial)
		{
			clang::Framework framework(5);
			CHECK_TRUE(framework.GetMinThreads() <= 5);    // Bad initial min thread limit");
		}

		UNITTEST_TEST(TestGetMinThreadsAfterSetMinThreads)
		{
			// Setting a lower minimum may have no effect but shouldn't reduce it.
			clang::Framework framework(5);
			framework.SetMinThreads(2);
			CHECK_TRUE(framework.GetMinThreads() >= 2);    // Bad min thread limit");
			CHECK_TRUE(framework.GetMinThreads() <= 5);    // Bad min thread limit");
		}

		UNITTEST_TEST(TestGetMinThreadsAfterSetMaxThreads)
		{
			clang::Framework framework(3);

			// Setting a higher maximum may or may not cause the maximum to rise.
			framework.SetMaxThreads(5);
			CHECK_TRUE(framework.GetMinThreads() <= 5);    // Min thread limit too high");

			// Setting a lower maximum should cause the minimum to drop.
			framework.SetMaxThreads(2);
			CHECK_TRUE(framework.GetMinThreads() <= 2);    // Min thread limit too high");
		}

		UNITTEST_TEST(TestGetNumMessagesProcessed)
		{
			clang::Framework framework(2);
			clang::Receiver receiver;

			// Local scope to check that actor destruction isn't counted as message processing.
			{
				// Create two responders that simply return integers sent to them.
				// We want two in parallel to check thread-safety.
				clang::ActorRef actorOne(framework.CreateActor<ResponderActor>());
				clang::ActorRef actorTwo(framework.CreateActor<ResponderActor>());

				// Send n messages to each responder.
				for (int count = 0; count < 100; ++count)
				{
					IntMessage msg(count);
					framework.Send(msg, receiver.GetAddress(), actorOne.GetAddress());
					framework.Send(msg, receiver.GetAddress(), actorTwo.GetAddress());
				}

				// Wait for all the replies.
				for (int count = 0; count < 100; ++count)
				{
					receiver.Wait();
					receiver.Wait();
				}
			}

			CHECK_TRUE(framework.GetCounterValue(clang::Framework::COUNTER_MESSAGES_PROCESSED) == 200);    // Processed message count incorrect");
		}

		UNITTEST_TEST(TestGetNumThreadPulses)
		{
			clang::Framework framework(2);
			clang::Receiver receiver;

			{
				// Create a responder that simply returns integers sent to it.
				clang::ActorRef actor(framework.CreateActor<ResponderActor>());

				// Send n messages to each responder.
				for (int count = 0; count < 100; ++count)
				{
					IntMessage msg(count);
					framework.Send(msg, receiver.GetAddress(), actor.GetAddress());
				}

				// Wait for all the replies.
				for (int count = 0; count < 100; ++count)
				{
					receiver.Wait();
				}
			}

			// We expect many of the messages to arrive while the actor is being processed.
			// Such messages shouldn't cause the threadpool to be pulsed, so aren't counted.
			// But it's non-deterministic, so we can't say much.
			CHECK_TRUE(framework.GetCounterValue(clang::Framework::COUNTER_THREADS_PULSED) <= 100);    // Processed message count incorrect");
		}

		UNITTEST_TEST(TestGetNumWokenThreadsSerial)
		{
			clang::Framework framework(2);
			clang::Receiver receiver;

			{
				// Create a responder that simply returns integers sent to it.
				clang::ActorRef actor(framework.CreateActor<ResponderActor>());

				// Send n messages to each responder.
				for (int count = 0; count < 100; ++count)
				{
					IntMessage msg(count);
					framework.Send(msg, receiver.GetAddress(), actor.GetAddress());
				}

				// Wait for all the replies.
				for (int count = 0; count < 100; ++count)
				{
					receiver.Wait();
				}
			}

			// We expect many of the messages to arrive while the actor is being processed.
			// Such messages shouldn't cause the threadpool to be pulsed, so don't wake any threads.
			// But it's non-deterministic, so we can't say much.
			CHECK_TRUE(framework.GetCounterValue(clang::Framework::COUNTER_THREADS_WOKEN) <= 100);    // Woken thread count incorrect");
		}

		UNITTEST_TEST(TestGetNumWokenThreadsParallel)
		{
			clang::Framework framework(5);
			clang::Receiver receiver;

			{
				// Create 5 responders that simply return integers sent to them.
				// We need them all to active in parallel to ensure the threads are woken.
				clang::ActorRef actors[5] =
				{
					framework.CreateActor<ResponderActor>(),
					framework.CreateActor<ResponderActor>(),
					framework.CreateActor<ResponderActor>(),
					framework.CreateActor<ResponderActor>(),
					framework.CreateActor<ResponderActor>()
				};

				// Send n messages to each responder.
				for (int count = 0; count < 100; ++count)
				{
					IntMessage msg(count);
					framework.Send(msg, receiver.GetAddress(), actors[0].GetAddress());
					framework.Send(msg, receiver.GetAddress(), actors[1].GetAddress());
					framework.Send(msg, receiver.GetAddress(), actors[2].GetAddress());
					framework.Send(msg, receiver.GetAddress(), actors[3].GetAddress());
					framework.Send(msg, receiver.GetAddress(), actors[4].GetAddress());
				}

				// Wait for all the replies.
				for (int count = 0; count < 100; ++count)
				{
					receiver.Wait();
					receiver.Wait();
					receiver.Wait();
					receiver.Wait();
					receiver.Wait();
				}
			}

			// We expect many of the messages to arrive while other actors are being processed, keeping the threads busy.
			// Such messages should cause the threadpool to be pulsed, waking the other threads.
			// Due to the non-deterministic nature it's possible that some threads go to sleep and are woken again.
			// It's also possible that some or all of the threads never slept at all so never needed to be woken.
			// Therefore it's not possible to test much here.
			CHECK_TRUE(framework.GetCounterValue(clang::Framework::COUNTER_THREADS_WOKEN) <= 500);    // Woken thread count incorrect");
		}

		UNITTEST_TEST(TestResetCounters)
		{
			clang::Framework framework(2);
			clang::Receiver receiver;

			{
				// Create two responders that simply return integers sent to them.
				// We want two in parallel to check thread-safety.
				clang::ActorRef actorOne(framework.CreateActor<ResponderActor>());
				clang::ActorRef actorTwo(framework.CreateActor<ResponderActor>());

				// Send n messages to each responder.
				for (int count = 0; count < 100; ++count)
				{
					IntMessage msg(count);
					framework.Send(msg, receiver.GetAddress(), actorOne.GetAddress());
					framework.Send(msg, receiver.GetAddress(), actorTwo.GetAddress());
				}

				// Wait for all the replies.
				for (int count = 0; count < 100; ++count)
				{
					receiver.Wait();
					receiver.Wait();
				}
			}

			// Reset the counters.
			framework.ResetCounters();

			// The worker threads may still be woken to garbage collect the actors.
			CHECK_TRUE(framework.GetCounterValue(clang::Framework::COUNTER_MESSAGES_PROCESSED) == 0);    // Message processing count not reset");
			CHECK_TRUE(framework.GetCounterValue(clang::Framework::COUNTER_THREADS_PULSED) == 0);    // Thread pulse count not reset");
			CHECK_TRUE(framework.GetCounterValue(clang::Framework::COUNTER_THREADS_WOKEN) <= 2);    // Woken thread count not reset");
		}

		UNITTEST_TEST(TestThreadPoolThreadsafety)
		{
			clang::Framework framework;
			clang::Receiver receiver;

			// Create two actors that each set the number of framework threads in response to messages.
			clang::ActorRef actorOne(framework.CreateActor<ThreadCountActor>());
			clang::ActorRef actorTwo(framework.CreateActor<ThreadCountActor>());

			// Send a large number of messages, causing both actors to swamp the shared framework
			// with requests to change the number of worker threads.
			for (int count = 0; count < 100; ++count)
			{
				framework.Send(IntMessage( 5), receiver.GetAddress(), actorOne.GetAddress());
				framework.Send(IntMessage(10), receiver.GetAddress(), actorTwo.GetAddress());
				framework.Send(IntMessage(12), receiver.GetAddress(), actorOne.GetAddress());
				framework.Send(IntMessage( 7), receiver.GetAddress(), actorTwo.GetAddress());
			}

			// Wait for all replies before terminating.
			for (int count = 0; count < 100; ++count)
			{
				receiver.Wait();
				receiver.Wait();
				receiver.Wait();
				receiver.Wait();
			}
		}
	};
}
UNITTEST_SUITE_END


#endif	// TESTS_TESTSUITES_FRAMEWORKTESTSUITE