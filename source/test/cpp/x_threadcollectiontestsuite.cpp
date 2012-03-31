#define TESTS_TESTSUITES_THREADCOLLECTIONTESTSUITE
#ifdef TESTS_TESTSUITES_THREADCOLLECTIONTESTSUITE

#include "xlang\private\x_BasicTypes.h"
#include "xlang\private\Containers\x_List.h"
#include "xlang\private\ThreadPool\x_ThreadCollection.h"
#include "xlang\private\Threading\x_Lock.h"
#include "xlang\private\Threading\x_Mutex.h"

#include "xunittest\xunittest.h"

// Placement new/delete
static inline void*	operator new(xcore::xsize_t num_bytes, void* mem)			{ return mem; }
static inline void	operator delete(void* mem, void* )							{ }


static xlang::detail::Mutex	sMutex;
static xlang::u32		sHitCount = 0;

struct Context
{
	inline Context() : mValue(0)
	{
	}

	inline Context(const xlang::u32 value) : mValue(value)
	{
	}

	xlang::u32 mValue;
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
		xlang::detail::Lock lock(sMutex);
		++sHitCount;
	}
}

UNITTEST_SUITE_BEGIN(TESTS_TESTSUITES_THREADCOLLECTIONTESTSUITE)
{
    UNITTEST_FIXTURE(main)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

		typedef xlang::detail::List<xlang::detail::Thread *> ThreadList;

		UNITTEST_TEST(TestConstruct)
		{
			xlang::detail::ThreadCollection threadCollection;
		}

		UNITTEST_TEST(TestOneThread)
		{
			xlang::detail::ThreadCollection threadCollection;

			sHitCount = 0;
			Context context(0);

			threadCollection.CreateThread(StaticEntryPoint, &context);
			threadCollection.DestroyThreads();

			CHECK_TRUE(sHitCount == 1);    // Thread function wasn't run");
			CHECK_TRUE(context.mValue == 1);    // Thread function wasn't run");
		}

		UNITTEST_TEST(TestTwoThreads)
		{
			xlang::detail::ThreadCollection threadCollection;

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
			xlang::detail::ThreadCollection threadCollection;
			const xlang::u32 numThreads = 128;

			sHitCount = 0;
			Context contexts[numThreads];

			// Create n threads, each referencing a different context.
			for (xlang::u32 index = 0; index < numThreads; ++index)
			{
				threadCollection.CreateThread(StaticEntryPoint, &contexts[index]);
			}

			// Destroy all the threads.
			threadCollection.DestroyThreads();

			CHECK_TRUE(sHitCount == numThreads);    // Thread function wasn't run");
			for (xlang::u32 index = 0; index < numThreads; ++index)
			{
				CHECK_TRUE(contexts[index].mValue == 1);    // Thread function wasn't run");
			}
		}

	};



} // namespace UnitTests
UNITTEST_SUITE_END

#endif // TESTS_TESTSUITES_THREADCOLLECTIONTESTSUITE

