#define TESTS_TESTSUITES_DEFAULTALLOCATORTESTSUITE
#ifdef TESTS_TESTSUITES_DEFAULTALLOCATORTESTSUITE

#include "clang\private\x_BasicTypes.h"

#include "clang\x_Align.h"
#include "clang\x_DefaultAllocator.h"

#include "cunittest\cunittest.h"

// Placement new/delete
inline void*	operator new(ncore::xsize_t num_bytes, void* mem)			{ return mem; }
inline void	operator delete(void* mem, void* )							{ }

UNITTEST_SUITE_BEGIN(TESTS_TESTSUITES_DEFAULTALLOCATORTESTSUITE)
{
    UNITTEST_FIXTURE(main)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

		struct Item
		{
			inline Item(const clang::u32 a, const clang::u32 b, const clang::u32 c) 
				: mA(a)
				, mB(b)
				, mC(c)
			{
			}
			clang::u32 mA;
			clang::u32 mB;
			clang::u32 mC;
		};

		struct ItemTwo
		{
			inline ItemTwo(const clang::u32 a, const clang::u32 b) 
				: mA(a)
				, mB(b)
			{
			}

			clang::u32 mA;
			clang::u32 mB;
		};



		UNITTEST_TEST(TestConstruct)
		{
			clang::DefaultAllocator allocator;
		}

		UNITTEST_TEST(TestAllocate)
		{
			clang::DefaultAllocator allocator;

			void *const block(allocator.Allocate(sizeof(Item)));

			CHECK_TRUE(block != 0);    // Allocate return null block pointer");
			CHECK_TRUE(XLANG_ALIGNED(block, 4));    // Allocated block isn't aligned");

			allocator.Free(block);
		}

		UNITTEST_TEST(TestUseAllocatedBlock)
		{
			clang::DefaultAllocator allocator;

			void *const block(allocator.Allocate(sizeof(Item)));

			Item *const item(reinterpret_cast<Item *>(block));

			const Item val(0, 1, 2);
			*item = val;

			CHECK_TRUE(item->mA == val.mA);    // Allocated memory can't be used");
			CHECK_TRUE(item->mB == val.mB);    // Allocated memory can't be used");
			CHECK_TRUE(item->mC == val.mC);    // Allocated memory can't be used");

			allocator.Free(block);
		}

		UNITTEST_TEST(TestAllocateMultiple)
		{
			clang::DefaultAllocator allocator;

			void *const block0(allocator.Allocate(sizeof(Item)));
			void *const block1(allocator.Allocate(sizeof(Item)));

			CHECK_TRUE(block0 != 0);    // Allocate return null block pointer");
			CHECK_TRUE(block1 != 0);    // Allocate return null block pointer");

			CHECK_TRUE(XLANG_ALIGNED(block0, 4));    // Allocated block isn't aligned");
			CHECK_TRUE(XLANG_ALIGNED(block1, 4));    // Allocated block isn't aligned");

			clang::u8 *const bytes0(reinterpret_cast<clang::u8 *>(block0));
			clang::u8 *const bytes1(reinterpret_cast<clang::u8 *>(block1));

			CHECK_TRUE(bytes0 + sizeof(Item) <= bytes1 || bytes1 + sizeof(Item) <= bytes0);    // Allocate returned overlapping blocks");

			allocator.Free(block1);
			allocator.Free(block0);
		}

		UNITTEST_TEST(TestAllocateMultipleDifferentSizes)
		{
			clang::DefaultAllocator allocator;

			void *const block0(allocator.Allocate(sizeof(Item)));
			void *const block1(allocator.Allocate(sizeof(ItemTwo)));

			CHECK_TRUE(block0 != 0);    // Allocate return null block pointer");
			CHECK_TRUE(block1 != 0);    // Allocate return null block pointer");

			CHECK_TRUE(XLANG_ALIGNED(block0, 4));    // Allocated block isn't aligned");
			CHECK_TRUE(XLANG_ALIGNED(block1, 4));    // Allocated block isn't aligned");

			clang::u8 *const bytes0(reinterpret_cast<clang::u8 *>(block0));
			clang::u8 *const bytes1(reinterpret_cast<clang::u8 *>(block1));

			CHECK_TRUE(bytes0 + sizeof(Item) <= bytes1 || bytes1 + sizeof(ItemTwo) <= bytes0);    // Allocate returned overlapping blocks");

			allocator.Free(block1);
			allocator.Free(block0);
		}

		UNITTEST_TEST(TestFreeOutOfOrder)
		{
			clang::DefaultAllocator allocator;

			void *const block0(allocator.Allocate(sizeof(Item)));
			void *const block1(allocator.Allocate(sizeof(Item)));

			allocator.Free(block0);
			allocator.Free(block1);
		}

		UNITTEST_TEST(TestUseMultipleAllocatedBlocks)
		{
			clang::DefaultAllocator allocator;

			void *const block0(allocator.Allocate(sizeof(Item)));
			void *const block1(allocator.Allocate(sizeof(Item)));

			Item *const item0(reinterpret_cast<Item *>(block0));
			Item *const item1(reinterpret_cast<Item *>(block1));

			const Item val0(0, 1, 2);
			const Item val1(5000000, 4000000, 3000000);

			*item0 = val0;
			*item1 = val1;

			CHECK_TRUE(item0->mA == val0.mA);    // Allocated memory can't be used");
			CHECK_TRUE(item0->mB == val0.mB);    // Allocated memory can't be used");
			CHECK_TRUE(item0->mC == val0.mC);    // Allocated memory can't be used");

			CHECK_TRUE(item1->mA == val1.mA);    // Allocated memory can't be used");
			CHECK_TRUE(item1->mB == val1.mB);    // Allocated memory can't be used");
			CHECK_TRUE(item1->mC == val1.mC);    // Allocated memory can't be used");

			allocator.Free(block1);
			allocator.Free(block0);
		}

		UNITTEST_TEST(TestAllocateAligned4)
		{
			clang::DefaultAllocator allocator;

			void *const block(allocator.AllocateAligned(sizeof(Item), 4));
			CHECK_TRUE(XLANG_ALIGNED(block, 4));    // Allocated block isn't aligned");

			allocator.Free(block);
		}

		UNITTEST_TEST(TestAllocateAligned8)
		{
			clang::DefaultAllocator allocator;

			void *const block(allocator.AllocateAligned(sizeof(Item), 8));
			CHECK_TRUE(XLANG_ALIGNED(block, 8));    // Allocated block isn't aligned");

			allocator.Free(block);
		}

		UNITTEST_TEST(TestAllocateAligned16)
		{
			clang::DefaultAllocator allocator;

			void *const block(allocator.AllocateAligned(sizeof(Item), 16));
			CHECK_TRUE(XLANG_ALIGNED(block, 16));    // Allocated block isn't aligned");

			allocator.Free(block);
		}

		UNITTEST_TEST(TestAllocateAligned32)
		{
			clang::DefaultAllocator allocator;

			void *const block(allocator.AllocateAligned(sizeof(Item), 32));
			CHECK_TRUE(XLANG_ALIGNED(block, 32));    // Allocated block isn't aligned");

			allocator.Free(block);
		}

		UNITTEST_TEST(TestAllocateAligned64)
		{
			clang::DefaultAllocator allocator;

			void *const block(allocator.AllocateAligned(sizeof(Item), 64));
			CHECK_TRUE(XLANG_ALIGNED(block, 64));    // Allocated block isn't aligned");

			allocator.Free(block);
		}

		UNITTEST_TEST(TestAllocateAligned128)
		{
			clang::DefaultAllocator allocator;

			void *const block(allocator.AllocateAligned(sizeof(Item), 128));
			CHECK_TRUE(XLANG_ALIGNED(block, 128));    // Allocated block isn't aligned");

			allocator.Free(block);
		}

		UNITTEST_TEST(TestAllocateMultipleAligned)
		{
			clang::DefaultAllocator allocator;

			void *const block0(allocator.AllocateAligned(sizeof(Item), 4));
			void *const block1(allocator.AllocateAligned(sizeof(Item), 8));
			void *const block2(allocator.AllocateAligned(sizeof(Item), 16));
			void *const block3(allocator.AllocateAligned(sizeof(Item), 32));
			void *const block4(allocator.AllocateAligned(sizeof(Item), 64));
			void *const block5(allocator.AllocateAligned(sizeof(Item), 128));

			CHECK_TRUE(XLANG_ALIGNED(block0, 4));    // Allocated block isn't aligned");
			CHECK_TRUE(XLANG_ALIGNED(block1, 8));    // Allocated block isn't aligned");
			CHECK_TRUE(XLANG_ALIGNED(block2, 16));    // Allocated block isn't aligned");
			CHECK_TRUE(XLANG_ALIGNED(block3, 32));    // Allocated block isn't aligned");
			CHECK_TRUE(XLANG_ALIGNED(block4, 64));    // Allocated block isn't aligned");
			CHECK_TRUE(XLANG_ALIGNED(block5, 128));    // Allocated block isn't aligned");

			allocator.Free(block5);
			allocator.Free(block4);
			allocator.Free(block3);
			allocator.Free(block2);
			allocator.Free(block1);
			allocator.Free(block0);
		}

		UNITTEST_TEST(TestGetBytesAllocated)
		{
			clang::DefaultAllocator allocator;

			void *const block0(allocator.AllocateAligned(sizeof(Item), 4));
			void *const block1(allocator.AllocateAligned(sizeof(Item), 128));
			void *const block2(allocator.AllocateAligned(sizeof(Item), 4));
			void *const block3(allocator.AllocateAligned(sizeof(Item), 128));

#if XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS
			CHECK_TRUE(allocator.GetBytesAllocated() == 4 * sizeof(Item));    // Allocated byte count incorrect");
#else
			CHECK_TRUE(allocator.GetBytesAllocated() == 0);    // Allocated byte count incorrect");
#endif // XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS

			allocator.Free(block3);
			allocator.Free(block2);
			allocator.Free(block1);
			allocator.Free(block0);

			CHECK_TRUE(allocator.GetBytesAllocated() == 0);    // Allocated byte count incorrect");
		}

		UNITTEST_TEST(TestGetPeakBytesAllocated)
		{
			clang::DefaultAllocator allocator;

			void *const block0(allocator.AllocateAligned(sizeof(Item), 4));
			void *const block1(allocator.AllocateAligned(sizeof(Item), 128));
			void *const block2(allocator.AllocateAligned(sizeof(Item), 4));
			void *const block3(allocator.AllocateAligned(sizeof(Item), 128));

#if XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS
			CHECK_TRUE(allocator.GetPeakBytesAllocated() == 4 * sizeof(Item));    // Allocated byte count incorrect");
#else
			CHECK_TRUE(allocator.GetPeakBytesAllocated() == 0);    // Allocated byte count incorrect");
#endif // XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS

			allocator.Free(block1);
			allocator.Free(block0);

#if XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS
			CHECK_TRUE(allocator.GetPeakBytesAllocated() == 4 * sizeof(Item));    // Allocated byte count incorrect");
#else
			CHECK_TRUE(allocator.GetPeakBytesAllocated() == 0);    // Allocated byte count incorrect");
#endif // XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS

			void *const block4(allocator.Allocate(sizeof(Item)));
			void *const block5(allocator.AllocateAligned(sizeof(Item), 128));

#if XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS
			CHECK_TRUE(allocator.GetPeakBytesAllocated() == 4 * sizeof(Item));    // Allocated byte count incorrect");
#else
			CHECK_TRUE(allocator.GetPeakBytesAllocated() == 0);    // Allocated byte count incorrect");
#endif // XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS

			void *const block6(allocator.Allocate(sizeof(Item)));
			void *const block7(allocator.AllocateAligned(sizeof(Item), 128));

#if XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS
			CHECK_TRUE(allocator.GetPeakBytesAllocated() == 6 * sizeof(Item));    // Allocated byte count incorrect");
#else
			CHECK_TRUE(allocator.GetPeakBytesAllocated() == 0);    // Allocated byte count incorrect");
#endif // XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS

			allocator.Free(block3);
			allocator.Free(block2);
			allocator.Free(block4);
			allocator.Free(block5);
			allocator.Free(block6);
			allocator.Free(block7);

#if XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS
			CHECK_TRUE(allocator.GetPeakBytesAllocated() == 6 * sizeof(Item));    // Allocated byte count incorrect");
#else
			CHECK_TRUE(allocator.GetPeakBytesAllocated() == 0);    // Allocated byte count incorrect");
#endif // XLANG_ENABLE_DEFAULTALLOCATOR_CHECKS
		}

	};

} // namespace UnitTests
UNITTEST_SUITE_END


#endif	// TESTS_TESTSUITES_DEFAULTALLOCATORTESTSUITE