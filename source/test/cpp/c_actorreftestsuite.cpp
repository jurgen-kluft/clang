#define TESTS_TESTSUITES_ACTORREFTESTSUITE
#ifdef TESTS_TESTSUITES_ACTORREFTESTSUITE

#include "clang\private\Directory\x_Directory.h"
#include "clang\private\Directory\x_ActorDirectory.h"
#include "clang\private\Threading\x_Lock.h"

#include "clang\x_ActorRef.h"
#include "clang\x_Framework.h"

#include "xunittest\xunittest.h"

// Placement new/delete
inline void*	operator new(xcore::xsize_t num_bytes, void* mem)			{ return mem; }
inline void	operator delete(void* mem, void* )							{ }


UNITTEST_SUITE_BEGIN(TESTS_TESTSUITES_ACTORREFTESTSUITE)
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

		UNITTEST_TEST(TestConstruction)
		{
			clang::Framework framework;
			clang::ActorRef actor(framework.CreateActor<SimpleActor>());
		}

		UNITTEST_TEST(TestCopyConstruction)
		{
			clang::Framework framework;
			clang::ActorRef actor(framework.CreateActor<SimpleActor>());

			// Copy construct
			clang::ActorRef copy(actor);

			clang::u32 count(0);

			{
				clang::detail::Lock lock(clang::detail::Directory::GetMutex());
				count = clang::detail::ActorDirectory::Instance().Count();
			}

			CHECK_TRUE(count == 1);   // Copy construction failed
		}

		UNITTEST_TEST(TestAssignment)
		{
			clang::Framework framework;
			clang::ActorRef actorOne(framework.CreateActor<SimpleActor>());
			clang::ActorRef actorTwo(framework.CreateActor<SimpleActor>());

			// Assign
			actorOne = actorTwo;

			// Wait for actor one to be destroyed.
			clang::u32 numEntities(2);
			while (numEntities != 1)
			{
				CHECK_TRUE(numEntities == 2);   // Copy construction failed

				clang::detail::Lock lock(clang::detail::Directory::GetMutex());
				numEntities = clang::detail::ActorDirectory::Instance().Count();
			}
		}

		UNITTEST_TEST(TestScope)
		{
			clang::Framework framework;
			clang::u32 numEntities(0);

			{
				clang::ActorRef actorOne(framework.CreateActor<SimpleActor>());
				(void) actorOne;

				numEntities = 0;
				while (numEntities != 1)
				{
					CHECK_TRUE(numEntities == 0);   // Incorrect entity count

					clang::detail::Lock lock(clang::detail::Directory::GetMutex());
					numEntities = clang::detail::ActorDirectory::Instance().Count();
				}

				{
					clang::ActorRef actorTwo(framework.CreateActor<SimpleActor>());
					(void) actorTwo;

					numEntities = 1;
					while (numEntities != 2)
					{
						CHECK_TRUE(numEntities == 1);   // Incorrect entity count

						clang::detail::Lock lock(clang::detail::Directory::GetMutex());
						numEntities = clang::detail::ActorDirectory::Instance().Count();
					}
				}

				numEntities = 2;
				while (numEntities != 1)
				{
					CHECK_TRUE(numEntities == 2);   // Incorrect entity count

					clang::detail::Lock lock(clang::detail::Directory::GetMutex());
					numEntities = clang::detail::ActorDirectory::Instance().Count();
				}
			}

			numEntities = 1;
			while (numEntities != 0)
			{
				CHECK_TRUE(numEntities == 1);   // Incorrect entity count

				clang::detail::Lock lock(clang::detail::Directory::GetMutex());
				numEntities = clang::detail::ActorDirectory::Instance().Count();
			}
		}

		UNITTEST_TEST(TestTransfer)
		{
			clang::Framework framework;
			clang::u32 numEntities(0);

			clang::ActorRef actorOne(framework.CreateActor<SimpleActor>());

			{
				clang::ActorRef actorTwo(framework.CreateActor<SimpleActor>());

				numEntities = 1;
				while (numEntities != 2)
				{
					CHECK_TRUE(numEntities == 1);   // Incorrect entity count

					clang::detail::Lock lock(clang::detail::Directory::GetMutex());
					numEntities = clang::detail::ActorDirectory::Instance().Count();
				}

				actorOne = actorTwo;
			}

			numEntities = 2;
			while (numEntities != 1)
			{
				CHECK_TRUE(numEntities == 2);   // Incorrect entity count

				clang::detail::Lock lock(clang::detail::Directory::GetMutex());
				numEntities = clang::detail::ActorDirectory::Instance().Count();
			}
		}
	};
}
UNITTEST_SUITE_END


#endif // TESTS_TESTSUITES_ACTORREFTESTSUITE
