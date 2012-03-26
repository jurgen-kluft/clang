#define TESTS_TESTSUITES_LISTTESTSUITE
#ifdef TESTS_TESTSUITES_LISTTESTSUITE

#include "xlang\private\Containers\x_List.h"

#include "xunittest\xunittest.h"

UNITTEST_SUITE_BEGIN(TESTS_TESTSUITES_LISTTESTSUITE)
{
    UNITTEST_FIXTURE(main)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

		class MockItem
		{
		public:

			inline MockItem() : mValue(0) { }
			inline explicit MockItem(const int value) : mValue(value) { }
			inline MockItem(const MockItem &other) : mValue(other.mValue) { }
			inline MockItem &operator=(const MockItem &other) { mValue = other.mValue; return *this; }
			inline bool operator==(const MockItem &other) const { return mValue == other.mValue; }
			inline bool operator!=(const MockItem &other) const { return mValue != other.mValue; }

		private:

			int mValue;
		};

		/// Unit test method.
		UNITTEST_TEST(TestInitialize)
		{
			xlang::detail::List<MockItem> list;
		}

		/// Unit test method.
		UNITTEST_TEST(TestInsert)
		{
			xlang::detail::List<MockItem> list;

			MockItem item(5);
			list.Insert(item);
		}

		/// Unit test method.
		UNITTEST_TEST(TestRemoveOnlyItem)
		{
			xlang::detail::List<MockItem> list;

			MockItem item(5);
			list.Insert(item);
			CHECK_TRUE(list.Remove(item));   // Failed to remove list item after inserting it");
		}

		/// Unit test method.
		UNITTEST_TEST(TestRemoveFrontItem)
		{
			xlang::detail::List<MockItem> list;

			MockItem itemOne(5);
			MockItem itemTwo(6);
			list.Insert(itemOne);
			list.Insert(itemTwo);
			CHECK_TRUE(list.Remove(itemOne));   // Failed to remove front list item");
		}

		/// Unit test method.
		UNITTEST_TEST(TestRemoveBackItem)
		{
			xlang::detail::List<MockItem> list;

			MockItem itemOne(5);
			MockItem itemTwo(6);
			list.Insert(itemOne);
			list.Insert(itemTwo);
			CHECK_TRUE(list.Remove(itemTwo));   // Failed to remove back list item");
		}

		/// Unit test method.
		UNITTEST_TEST(TestRemoveMiddleItem)
		{
			xlang::detail::List<MockItem> list;

			MockItem itemOne(5);
			MockItem itemTwo(6);
			MockItem itemThree(7);
			list.Insert(itemOne);
			list.Insert(itemTwo);
			list.Insert(itemThree);
			CHECK_TRUE(list.Remove(itemTwo));   // Failed to remove middle list item");
		}

		/// Unit test method.
		UNITTEST_TEST(TestContains)
		{
			xlang::detail::List<MockItem> list;

			MockItem item(5);
			list.Insert(item);
			CHECK_TRUE(list.Contains(item));   // Contains returned an incorrect result");
		}

		/// Unit test method.
		UNITTEST_TEST(TestContainsNegative)
		{
			xlang::detail::List<MockItem> list;

			MockItem item(5);
			list.Insert(item);
			MockItem otherItem(6);
			CHECK_TRUE(list.Contains(otherItem) == false);   // Contains returned an incorrect result");
		}

		/// Unit test method.
		UNITTEST_TEST(TestDuplicateItems)
		{
			xlang::detail::List<MockItem> list;

			MockItem item(6);
			list.Insert(item);
			list.Insert(item);

			CHECK_TRUE(list.Size() == 2);   // Duplicate items not handled correctly by Insert");
			CHECK_TRUE(list.Contains(item));   // Duplicate items not handled correctly by Contains");
			CHECK_TRUE(list.Remove(item));   // Duplicate items not handled correctly by Remove");
			CHECK_TRUE(list.Size() == 1);   // Duplicate items not handled correctly by Remove");
			CHECK_TRUE(list.Contains(item));   // Duplicate items not handled correctly by Contains");
			CHECK_TRUE(list.Remove(item));   // Duplicate items not handled correctly by Remove");
			CHECK_TRUE(list.Size() == 0);   // Duplicate items not handled correctly by Size");
			CHECK_TRUE(list.Contains(item) == false);   // Duplicate items not handled correctly by Contains");
		}

		/// Unit test method.
		UNITTEST_TEST(TestContainsAfterRemove)
		{
			xlang::detail::List<MockItem> list;

			MockItem itemOne(5);
			MockItem itemTwo(6);
			list.Insert(itemOne);
			list.Insert(itemTwo);
			list.Remove(itemOne);
			CHECK_TRUE(list.Contains(itemOne) == false);   // Contains confused by removing one of two items");
			CHECK_TRUE(list.Contains(itemTwo));   // Contains confused by removing one of two items");
		}

		/// Unit test method.
		UNITTEST_TEST(TestTemporarilyEmpty)
		{
			xlang::detail::List<MockItem> list;

			MockItem itemOne(5);
			MockItem itemTwo(6);
			list.Insert(itemOne);
			list.Remove(itemOne);
			list.Insert(itemTwo);

			CHECK_TRUE(list.Contains(itemOne) == false);   // Contains confused by temporarily empty list");
			CHECK_TRUE(list.Contains(itemTwo));   // Contains confused by temporarily empty list");
			CHECK_TRUE(list.Remove(itemOne) == false);   // Remove confused by temporarily empty list");
			CHECK_TRUE(list.Remove(itemTwo));   // Remove failed to remove item after list was temporarily empty");
		}

		/// Unit test method.
		UNITTEST_TEST(TestSizeInitially)
		{
			xlang::detail::List<MockItem> list;

			CHECK_TRUE(list.Size() == 0);   // Size not initially zero");
		}

		/// Unit test method.
		UNITTEST_TEST(TestSizeAfterInsert)
		{
			xlang::detail::List<MockItem> list;

			MockItem item(6);
			list.Insert(item);

			CHECK_TRUE(list.Size() == 1);   // Size not one after Insert");
		}

		/// Unit test method.
		UNITTEST_TEST(TestSizeAfterInsertRemove)
		{
			xlang::detail::List<MockItem> list;

			MockItem item(6);
			list.Insert(item);
			list.Remove(item);

			CHECK_TRUE(list.Size() == 0);   // Size not zero after Insert, Remove");
		}

		/// Unit test method.
		UNITTEST_TEST(TestSizeAfterInsertContains)
		{
			xlang::detail::List<MockItem> list;

			MockItem item(6);
			list.Insert(item);

			CHECK_TRUE(list.Contains(item) == true);   // Expected Contains to return true for inserted item");
			CHECK_TRUE(list.Size() == 1);   // Size not one after Insert, Contains");
		}

		/// Unit test method.
		UNITTEST_TEST(TestSizeAfterInsertInsert)
		{
			xlang::detail::List<MockItem> list;

			MockItem item(6);
			list.Insert(item);
			list.Insert(item);

			CHECK_TRUE(list.Size() == 2);   // Size not two after Insert, Insert");
		}
	};
}
UNITTEST_SUITE_END



#endif // TESTS_TESTSUITES_LISTTESTSUITE

