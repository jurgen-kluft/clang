#define TESTS_TESTSUITES_POOLTESTSUITE
#ifdef TESTS_TESTSUITES_POOLTESTSUITE

#include "xlang\private\x_BasicTypes.h"
#include "xlang\private\MessageCache\x_Pool.h"
#include "xlang\x_Align.h"

#include "xunittest\xunittest.h"

// Placement new/delete
inline void*	operator new(xcore::xsize_t num_bytes, void* mem)			{ return mem; }
inline void	operator delete(void* mem, void* )							{ }

UNITTEST_SUITE_BEGIN(TESTS_TESTSUITES_POOLTESTSUITE)
{
    UNITTEST_FIXTURE(main)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

		struct Item
		{
			xlang::u32 a;
			xlang::u32 b;
		};

		UNITTEST_TEST(TestConstruct)
		{
			xlang::detail::Pool pool;
		}

		UNITTEST_TEST(TestAdd)
		{
			xlang::detail::Pool pool;

			Item item0;
			pool.Add(&item0);
		}

		UNITTEST_TEST(TestEmptyAfterAdd)
		{
			xlang::detail::Pool pool;

			Item item0;
			pool.Add(&item0);

			CHECK_TRUE(pool.FetchAligned(XLANG_ALIGNOF(Item)) == &item0);
		}

		UNITTEST_TEST(TestFetchWhileEmpty)
		{
			xlang::detail::Pool pool;
			CHECK_TRUE(pool.Fetch() == 0);	//Fetch should fail when empty
		}

		UNITTEST_TEST(TestFetchAfterAdd)
		{
			xlang::detail::Pool pool;

			Item item0;
			pool.Add(&item0);

			CHECK_TRUE(pool.Fetch() == &item0);	// Fetch failed
		}

		UNITTEST_TEST(TestEmptyAfterFetch)
		{
			xlang::detail::Pool pool;

			Item item0;
			pool.Add(&item0);

			CHECK_TRUE(pool.Fetch() == &item0);		// Fetch failed
			CHECK_TRUE(pool.Fetch() == 0);	// Pool should be empty after fetch
		}

		UNITTEST_TEST(TestFetchSizeWhileEmpty)
		{
			xlang::detail::Pool pool;
			CHECK_TRUE(pool.FetchAligned(XLANG_ALIGNOF(Item)) == 0); // Fetch should fail when empty
		}

		UNITTEST_TEST(TestFetchCorrectAlignment)
		{
			xlang::detail::Pool pool;

			Item item0;
			pool.Add(&item0);

			CHECK_TRUE(pool.FetchAligned(XLANG_ALIGNOF(Item)) == &item0); // Fetch failed
			CHECK_TRUE(pool.Fetch() == 0);	// Pool should be empty after fetch
		}

		UNITTEST_TEST(TestFetchWrongAlignment)
		{
			xlang::detail::Pool pool;

			Item item0;
			pool.Add(&item0);

			// The address may happen to be aligned.
			const xlang::u32 increasedAlignment(XLANG_ALIGNOF(Item) * 2);
			if (XLANG_ALIGNED(&item0, increasedAlignment))
			{
				CHECK_TRUE(pool.FetchAligned(increasedAlignment) == &item0); // Fetch failed
				CHECK_TRUE(pool.Fetch() == 0);	// Fetch shouldn't return anything
			}
			else
			{
				CHECK_TRUE(pool.FetchAligned(increasedAlignment) == 0); // Fetch should fail
				CHECK_TRUE(pool.Fetch() == &item0); // Fetch should return original item
			}
		}
	}
}
UNITTEST_SUITE_END


#endif // TESTS_TESTSUITES_POOLTESTSUITE

