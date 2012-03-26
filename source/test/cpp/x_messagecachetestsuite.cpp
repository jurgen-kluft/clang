#define TESTS_TESTSUITES_MESSAGECACHETESTSUITE
#ifdef TESTS_TESTSUITES_MESSAGECACHETESTSUITE

#include "xlang\private\x_BasicTypes.h"
#include "xlang\private\MessageCache\x_MessageCache.h"
#include "xlang\x_Align.h"

#include "xunittest\xunittest.h"


UNITTEST_SUITE_BEGIN(TESTS_TESTSUITES_MESSAGECACHETESTSUITE)
{
    UNITTEST_FIXTURE(main)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

		struct Item
		{
			int a;
			int b;
		};

		UNITTEST_TEST(TestConstruct)
		{
			xlang::detail::MessageCache::Instance().Reference();
			xlang::detail::MessageCache::Instance().Dereference();
		}

		UNITTEST_TEST(TestInstance)
		{
			xlang::detail::MessageCache::Instance().Reference();
			xlang::detail::MessageCache &freeList(xlang::detail::MessageCache::Instance());

			CHECK_TRUE(&freeList != 0);    // Instance null");

			xlang::detail::MessageCache::Instance().Dereference();
		}

		UNITTEST_TEST(TestAllocateFree)
		{
			xlang::detail::MessageCache::Instance().Reference();
			xlang::detail::MessageCache &freeList(xlang::detail::MessageCache::Instance());

			void *const mem0(freeList.Allocate(sizeof(Item), XLANG_ALIGNOF(Item)));
			CHECK_TRUE(mem0 != 0);    // Allocate failed");
			freeList.Free(mem0, sizeof(Item));

			xlang::detail::MessageCache::Instance().Dereference();
		}

		UNITTEST_TEST(TestAllocateAfterFree)
		{
			xlang::detail::MessageCache::Instance().Reference();
			xlang::detail::MessageCache &freeList(xlang::detail::MessageCache::Instance());

			void *const mem0(freeList.Allocate(sizeof(Item), XLANG_ALIGNOF(Item)));
			CHECK_TRUE(mem0 != 0);    // Allocate failed");
			freeList.Free(mem0, sizeof(Item));

			void *const mem1(freeList.Allocate(sizeof(Item), XLANG_ALIGNOF(Item)));
			CHECK_TRUE(mem1 != 0);    // Allocate failed");
			freeList.Free(mem1, sizeof(Item));

			CHECK_TRUE(mem0 == mem1);    // Second allocate didn't reuse free block");

			xlang::detail::MessageCache::Instance().Dereference();
		}

		UNITTEST_TEST(TestAllocateAfterFreeSmaller)
		{
			xlang::detail::MessageCache::Instance().Reference();
			xlang::detail::MessageCache &freeList(xlang::detail::MessageCache::Instance());

			void *const mem0(freeList.Allocate(sizeof(Item) * 2, XLANG_ALIGNOF(Item)));
			CHECK_TRUE(mem0 != 0);    // Allocate failed");
			freeList.Free(mem0, sizeof(Item) * 2);

			void *const mem1(freeList.Allocate(sizeof(Item), XLANG_ALIGNOF(Item)));
			CHECK_TRUE(mem1 != 0);    // Allocate failed");
			freeList.Free(mem1, sizeof(Item));

			CHECK_TRUE(mem0 != mem1);    // Second allocate reuse free block of larger size");

			xlang::detail::MessageCache::Instance().Dereference();
		}

		UNITTEST_TEST(TestAllocateAfterFreeLarger)
		{
			xlang::detail::MessageCache::Instance().Reference();
			xlang::detail::MessageCache &freeList(xlang::detail::MessageCache::Instance());

			void *const mem0(freeList.Allocate(sizeof(Item), XLANG_ALIGNOF(Item)));
			CHECK_TRUE(mem0 != 0);    // Allocate failed");
			freeList.Free(mem0, sizeof(Item));

			void *const mem1(freeList.Allocate(sizeof(Item) * 2, XLANG_ALIGNOF(Item)));
			CHECK_TRUE(mem1 != 0);    // Allocate failed");
			freeList.Free(mem1, sizeof(Item) * 2);

			CHECK_TRUE(mem0 != mem1);    // Second allocate reuse free block of larger size");

			xlang::detail::MessageCache::Instance().Dereference();
		}

		UNITTEST_TEST(TestAllocateAfterFreeLargerAlignment)
		{
			xlang::detail::MessageCache::Instance().Reference();
			xlang::detail::MessageCache &freeList(xlang::detail::MessageCache::Instance());

			void *const mem0(freeList.Allocate(sizeof(Item), XLANG_ALIGNOF(Item)));
			CHECK_TRUE(mem0 != 0);    // Allocate failed");
			freeList.Free(mem0, sizeof(Item));

			void *const mem1(freeList.Allocate(sizeof(Item), XLANG_ALIGNOF(Item) * 2));
			CHECK_TRUE(mem1 != 0);    // Allocate failed");
			freeList.Free(mem1, sizeof(Item));

			// The first block may happen to be aligned anyway.
			if (XLANG_ALIGNED(mem0, XLANG_ALIGNOF(Item) * 2))
			{
				CHECK_TRUE(mem0 == mem1);    // Second allocate failed to reuse aligned free block");
			}
			else
			{
				CHECK_TRUE(mem0 != mem1);    // Second allocate reused non-aligned free block");
			}

			xlang::detail::MessageCache::Instance().Dereference();
		}

		UNITTEST_TEST(TestAllocateAfterFreeSmallerAlignment)
		{
			xlang::detail::MessageCache::Instance().Reference();
			xlang::detail::MessageCache &freeList(xlang::detail::MessageCache::Instance());

			void *const mem0(freeList.Allocate(sizeof(Item), XLANG_ALIGNOF(Item) * 2));
			CHECK_TRUE(mem0 != 0);    // Allocate failed");
			freeList.Free(mem0, sizeof(Item));

			void *const mem1(freeList.Allocate(sizeof(Item), XLANG_ALIGNOF(Item)));
			CHECK_TRUE(mem1 != 0);    // Allocate failed");
			freeList.Free(mem1, sizeof(Item));

			CHECK_TRUE(mem0 == mem1);    // Second allocate failed to reuse aligned free block");

			xlang::detail::MessageCache::Instance().Dereference();
		}
	};
}
UNITTEST_SUITE_END


#endif // TESTS_TESTSUITES_MESSAGECACHETESTSUITE

