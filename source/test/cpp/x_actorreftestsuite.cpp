#define TESTS_TESTSUITES_ACTORREFTESTSUITE
#ifdef TESTS_TESTSUITES_ACTORREFTESTSUITE

#include "xlang\private\Directory\x_Directory.h"
#include "xlang\private\Directory\x_ActorDirectory.h"
#include "xlang\private\Threading\x_Lock.h"

#include "xlang\x_ActorRef.h"
#include "xlang\x_Framework.h"

#include "xunittest\xunittest.h"

// Placement new/delete
static inline void*	operator new(xcore::xsize_t num_bytes, void* mem)			{ return mem; }
static inline void	operator delete(void* mem, void* )							{ }


UNITTEST_SUITE_BEGIN(TESTS_TESTSUITES_ACTORREFTESTSUITE)
{
    UNITTEST_FIXTURE(main)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

		class SimpleActor : public xlang::Actor
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

		UNITTEST_TEST(TestConstruction)
		{
			xlang::Framework framework;
			xlang::ActorRef actor(framework.CreateActor<SimpleActor>());
		}

		UNITTEST_TEST(TestCopyConstruction)
		{
			xlang::Framework framework;
			xlang::ActorRef actor(framework.CreateActor<SimpleActor>());

			// Copy construct
			xlang::ActorRef copy(actor);

			xlang::u32 count(0);

			{
				xlang::detail::Lock lock(xlang::detail::Directory::GetMutex());
				count = xlang::detail::ActorDirectory::Instance().Count();
			}

			CHECK_TRUE(count == 1);   // Copy construction failed
		}

		UNITTEST_TEST(TestAssignment)
		{
			xlang::Framework framework;
			xlang::ActorRef actorOne(framework.CreateActor<SimpleActor>());
			xlang::ActorRef actorTwo(framework.CreateActor<SimpleActor>());

			// Assign
			actorOne = actorTwo;

			// Wait for actor one to be destroyed.
			xlang::u32 numEntities(2);
			while (numEntities != 1)
			{
				CHECK_TRUE(numEntities == 2);   // Copy construction failed

				xlang::detail::Lock lock(xlang::detail::Directory::GetMutex());
				numEntities = xlang::detail::ActorDirectory::Instance().Count();
			}
		}

		UNITTEST_TEST(TestScope)
		{
			xlang::Framework framework;
			xlang::u32 numEntities(0);

			{
				xlang::ActorRef actorOne(framework.CreateActor<SimpleActor>());
				(void) actorOne;

				numEntities = 0;
				while (numEntities != 1)
				{
					CHECK_TRUE(numEntities == 0);   // Incorrect entity count

					xlang::detail::Lock lock(xlang::detail::Directory::GetMutex());
					numEntities = xlang::detail::ActorDirectory::Instance().Count();
				}

				{
					xlang::ActorRef actorTwo(framework.CreateActor<SimpleActor>());
					(void) actorTwo;

					numEntities = 1;
					while (numEntities != 2)
					{
						CHECK_TRUE(numEntities == 1);   // Incorrect entity count

						xlang::detail::Lock lock(xlang::detail::Directory::GetMutex());
						numEntities = xlang::detail::ActorDirectory::Instance().Count();
					}
				}

				numEntities = 2;
				while (numEntities != 1)
				{
					CHECK_TRUE(numEntities == 2);   // Incorrect entity count

					xlang::detail::Lock lock(xlang::detail::Directory::GetMutex());
					numEntities = xlang::detail::ActorDirectory::Instance().Count();
				}
			}

			numEntities = 1;
			while (numEntities != 0)
			{
				CHECK_TRUE(numEntities == 1);   // Incorrect entity count

				xlang::detail::Lock lock(xlang::detail::Directory::GetMutex());
				numEntities = xlang::detail::ActorDirectory::Instance().Count();
			}
		}

		UNITTEST_TEST(TestTransfer)
		{
			xlang::Framework framework;
			xlang::u32 numEntities(0);

			xlang::ActorRef actorOne(framework.CreateActor<SimpleActor>());

			{
				xlang::ActorRef actorTwo(framework.CreateActor<SimpleActor>());

				numEntities = 1;
				while (numEntities != 2)
				{
					CHECK_TRUE(numEntities == 1);   // Incorrect entity count

					xlang::detail::Lock lock(xlang::detail::Directory::GetMutex());
					numEntities = xlang::detail::ActorDirectory::Instance().Count();
				}

				actorOne = actorTwo;
			}

			numEntities = 2;
			while (numEntities != 1)
			{
				CHECK_TRUE(numEntities == 2);   // Incorrect entity count

				xlang::detail::Lock lock(xlang::detail::Directory::GetMutex());
				numEntities = xlang::detail::ActorDirectory::Instance().Count();
			}
		}
	};
}
UNITTEST_SUITE_END


#endif // TESTS_TESTSUITES_ACTORREFTESTSUITE
