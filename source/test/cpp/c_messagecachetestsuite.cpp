#define TESTS_TESTSUITES_MESSAGECACHETESTSUITE
#ifdef TESTS_TESTSUITES_MESSAGECACHETESTSUITE

#include "clang\private\x_BasicTypes.h"
#include "clang\private\MessageCache\x_MessageCache.h"
#include "clang\x_Align.h"

#include "xunittest\xunittest.h"

// Placement new/delete
inline void*	operator new(xcore::xsize_t num_bytes, void* mem)			{ return mem; }
inline void	operator delete(void* mem, void* )							{ }

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
			clang::detail::MessageCache::Instance().Reference();
			clang::detail::MessageCache::Instance().Dereference();
		}

		UNITTEST_TEST(TestInstance)
		{
			clang::detail::MessageCache::Instance().Reference();
			clang::detail::MessageCache &freeList(clang::detail::MessageCache::Instance());

			CHECK_TRUE(&freeList != 0);    // Instance null");

			clang::detail::MessageCache::Instance().Dereference();
		}

		UNITTEST_TEST(TestAllocateFree)
		{
			clang::detail::MessageCache::Instance().Reference();
			clang::detail::MessageCache &freeList(clang::detail::MessageCache::Instance());

			void *const mem0(freeList.Allocate(sizeof(Item), XLANG_ALIGNOF(Item)));
			CHECK_TRUE(mem0 != 0);    // Allocate failed");
			freeList.Free(mem0, sizeof(Item));

			clang::detail::MessageCache::Instance().Dereference();
		}

		UNITTEST_TEST(TestAllocateAfterFree)
		{
			clang::detail::MessageCache::Instance().Reference();
			clang::detail::MessageCache &freeList(clang::detail::MessageCache::Instance());

			void *const mem0(freeList.Allocate(sizeof(Item), XLANG_ALIGNOF(Item)));
			CHECK_TRUE(mem0 != 0);    // Allocate failed");
			freeList.Free(mem0, sizeof(Item));

			void *const mem1(freeList.Allocate(sizeof(Item), XLANG_ALIGNOF(Item)));
			CHECK_TRUE(mem1 != 0);    // Allocate failed");
			freeList.Free(mem1, sizeof(Item));

			CHECK_TRUE(mem0 == mem1);    // Second allocate didn't reuse free block");

			clang::detail::MessageCache::Instance().Dereference();
		}

		UNITTEST_TEST(TestAllocateAfterFreeSmaller)
		{
			clang::detail::MessageCache::Instance().Reference();
			clang::detail::MessageCache &freeList(clang::detail::MessageCache::Instance());

			void *const mem0(freeList.Allocate(sizeof(Item) * 2, XLANG_ALIGNOF(Item)));
			CHECK_TRUE(mem0 != 0);    // Allocate failed");
			freeList.Free(mem0, sizeof(Item) * 2);

			void *const mem1(freeList.Allocate(sizeof(Item), XLANG_ALIGNOF(Item)));
			CHECK_TRUE(mem1 != 0);    // Allocate failed");
			freeList.Free(mem1, sizeof(Item));

			CHECK_TRUE(mem0 != mem1);    // Second allocate reuse free block of larger size");

			clang::detail::MessageCache::Instance().Dereference();
		}

		UNITTEST_TEST(TestAllocateAfterFreeLarger)
		{
			clang::detail::MessageCache::Instance().Reference();
			clang::detail::MessageCache &freeList(clang::detail::MessageCache::Instance());

			void *const mem0(freeList.Allocate(sizeof(Item), XLANG_ALIGNOF(Item)));
			CHECK_TRUE(mem0 != 0);    // Allocate failed");
			freeList.Free(mem0, sizeof(Item));

			void *const mem1(freeList.Allocate(sizeof(Item) * 2, XLANG_ALIGNOF(Item)));
			CHECK_TRUE(mem1 != 0);    // Allocate failed");
			freeList.Free(mem1, sizeof(Item) * 2);

			CHECK_TRUE(mem0 != mem1);    // Second allocate reuse free block of larger size");

			clang::detail::MessageCache::Instance().Dereference();
		}

		UNITTEST_TEST(TestAllocateAfterFreeLargerAlignment)
		{
			clang::detail::MessageCache::Instance().Reference();
			clang::detail::MessageCache &freeList(clang::detail::MessageCache::Instance());

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

			clang::detail::MessageCache::Instance().Dereference();
		}

		UNITTEST_TEST(TestAllocateAfterFreeSmallerAlignment)
		{
			clang::detail::MessageCache::Instance().Reference();
			clang::detail::MessageCache &freeList(clang::detail::MessageCache::Instance());

			void *const mem0(freeList.Allocate(sizeof(Item), XLANG_ALIGNOF(Item) * 2));
			CHECK_TRUE(mem0 != 0);    // Allocate failed");
			freeList.Free(mem0, sizeof(Item));

			void *const mem1(freeList.Allocate(sizeof(Item), XLANG_ALIGNOF(Item)));
			CHECK_TRUE(mem1 != 0);    // Allocate failed");
			freeList.Free(mem1, sizeof(Item));

			CHECK_TRUE(mem0 == mem1);    // Second allocate failed to reuse aligned free block");

			clang::detail::MessageCache::Instance().Dereference();
		}
	};
}
UNITTEST_SUITE_END


#endif // TESTS_TESTSUITES_MESSAGECACHETESTSUITE

