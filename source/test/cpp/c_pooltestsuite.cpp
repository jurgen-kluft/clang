#define TESTS_TESTSUITES_POOLTESTSUITE
#ifdef TESTS_TESTSUITES_POOLTESTSUITE

#include "clang\private\x_BasicTypes.h"
#include "clang\private\MessageCache\x_Pool.h"
#include "clang\x_Align.h"

#include "cunittest\cunittest.h"

// Placement new/delete
inline void*	operator new(ncore::xsize_t num_bytes, void* mem)			{ return mem; }
inline void	operator delete(void* mem, void* )							{ }

UNITTEST_SUITE_BEGIN(TESTS_TESTSUITES_POOLTESTSUITE)
{
    UNITTEST_FIXTURE(main)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

		struct Item
		{
			clang::u32 a;
			clang::u32 b;
		};

		UNITTEST_TEST(TestConstruct)
		{
			clang::detail::Pool pool;
		}

		UNITTEST_TEST(TestAdd)
		{
			clang::detail::Pool pool;

			Item item0;
			pool.Add(&item0);
		}

		UNITTEST_TEST(TestEmptyAfterAdd)
		{
			clang::detail::Pool pool;

			Item item0;
			pool.Add(&item0);

			CHECK_TRUE(pool.FetchAligned(XLANG_ALIGNOF(Item)) == &item0);
		}

		UNITTEST_TEST(TestFetchWhileEmpty)
		{
			clang::detail::Pool pool;
			CHECK_TRUE(pool.Fetch() == 0);	//Fetch should fail when empty
		}

		UNITTEST_TEST(TestFetchAfterAdd)
		{
			clang::detail::Pool pool;

			Item item0;
			pool.Add(&item0);

			CHECK_TRUE(pool.Fetch() == &item0);	// Fetch failed
		}

		UNITTEST_TEST(TestEmptyAfterFetch)
		{
			clang::detail::Pool pool;

			Item item0;
			pool.Add(&item0);

			CHECK_TRUE(pool.Fetch() == &item0);		// Fetch failed
			CHECK_TRUE(pool.Fetch() == 0);	// Pool should be empty after fetch
		}

		UNITTEST_TEST(TestFetchSizeWhileEmpty)
		{
			clang::detail::Pool pool;
			CHECK_TRUE(pool.FetchAligned(XLANG_ALIGNOF(Item)) == 0); // Fetch should fail when empty
		}

		UNITTEST_TEST(TestFetchCorrectAlignment)
		{
			clang::detail::Pool pool;

			Item item0;
			pool.Add(&item0);

			CHECK_TRUE(pool.FetchAligned(XLANG_ALIGNOF(Item)) == &item0); // Fetch failed
			CHECK_TRUE(pool.Fetch() == 0);	// Pool should be empty after fetch
		}

		UNITTEST_TEST(TestFetchWrongAlignment)
		{
			clang::detail::Pool pool;

			Item item0;
			pool.Add(&item0);

			// The address may happen to be aligned.
			const clang::u32 increasedAlignment(XLANG_ALIGNOF(Item) * 2);
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

