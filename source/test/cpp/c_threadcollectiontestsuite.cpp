#define TESTS_TESTSUITES_THREADCOLLECTIONTESTSUITE
#ifdef TESTS_TESTSUITES_THREADCOLLECTIONTESTSUITE

#include "clang\private\x_BasicTypes.h"
#include "clang\private\Containers\x_List.h"
#include "clang\private\ThreadPool\x_ThreadCollection.h"
#include "clang\private\Threading\x_Lock.h"
#include "clang\private\Threading\x_Mutex.h"

#include "xunittest\xunittest.h"

// Placement new/delete
inline void*	operator new(xcore::xsize_t num_bytes, void* mem)			{ return mem; }
inline void		operator delete(void* mem, void* )							{ }


static clang::detail::Mutex	sMutex;
static clang::u32		sHitCount = 0;

struct Context
{
	inline Context() : mValue(0)
	{
	}

	inline Context(const clang::u32 value) : mValue(value)
	{
	}

	clang::u32 mValue;
};

static void StaticEntryPoint(void *const context)
{
	Context *const threadContext(reinterpret_cast<Context *>(context));
	if (threadContext)
	{
		// This is per-thread so doesn't need locking.
		++threadContext->mValue;
	}

	{
		clang::detail::Lock lock(sMutex);
		++sHitCount;
	}
}

UNITTEST_SUITE_BEGIN(TESTS_TESTSUITES_THREADCOLLECTIONTESTSUITE)
{
    UNITTEST_FIXTURE(main)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

		typedef clang::detail::List<clang::detail::Thread *> ThreadList;

		UNITTEST_TEST(TestConstruct)
		{
			clang::detail::ThreadCollection threadCollection;
		}

		UNITTEST_TEST(TestOneThread)
		{
			clang::detail::ThreadCollection threadCollection;

			sHitCount = 0;
			Context context(0);

			threadCollection.CreateThread(StaticEntryPoint, &context);
			threadCollection.DestroyThreads();

			CHECK_TRUE(sHitCount == 1);    // Thread function wasn't run");
			CHECK_TRUE(context.mValue == 1);    // Thread function wasn't run");
		}

		UNITTEST_TEST(TestTwoThreads)
		{
			clang::detail::ThreadCollection threadCollection;

			sHitCount = 0;
			Context context0(0);
			Context context1(0);

			threadCollection.CreateThread(StaticEntryPoint, &context0);
			threadCollection.CreateThread(StaticEntryPoint, &context1);

			threadCollection.DestroyThreads();

			CHECK_TRUE(sHitCount == 2);    // Thread function wasn't run");
			CHECK_TRUE(context0.mValue == 1);    // Thread function wasn't run");
			CHECK_TRUE(context1.mValue == 1);    // Thread function wasn't run");
		}

		UNITTEST_TEST(TestThreadReuse)
		{
			clang::detail::ThreadCollection threadCollection;
			const clang::u32 numThreads = 128;

			sHitCount = 0;
			Context contexts[numThreads];

			// Create n threads, each referencing a different context.
			for (clang::u32 index = 0; index < numThreads; ++index)
			{
				threadCollection.CreateThread(StaticEntryPoint, &contexts[index]);
			}

			// Destroy all the threads.
			threadCollection.DestroyThreads();

			CHECK_TRUE(sHitCount == numThreads);    // Thread function wasn't run");
			for (clang::u32 index = 0; index < numThreads; ++index)
			{
				CHECK_TRUE(contexts[index].mValue == 1);    // Thread function wasn't run");
			}
		}

	};



} // namespace UnitTests
UNITTEST_SUITE_END

#endif // TESTS_TESTSUITES_THREADCOLLECTIONTESTSUITE

