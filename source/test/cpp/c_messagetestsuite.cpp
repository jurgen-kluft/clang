#define TESTS_TESTSUITES_MESSAGETESTSUITE
#ifdef TESTS_TESTSUITES_MESSAGETESTSUITE

#include "clang\private\Messages\x_IMessage.h"
#include "clang\private\Messages\x_Message.h"

#include "clang\x_Address.h"
#include "clang\x_AllocatorManager.h"
#include "clang\x_Register.h"

#include "xunittest\xunittest.h"

// Placement new/delete
inline void*	operator new(xcore::xsize_t num_bytes, void* mem)			{ return mem; }
inline void	operator delete(void* mem, void* )							{ }

struct MessageValue
{
	inline MessageValue() : a(0), b(0)
	{
	}

	int a;
	int b;
};

struct NamedMessageValue
{
	inline NamedMessageValue() : a(0), b(0)
	{
	}

	int a;
	int b;
};


UNITTEST_SUITE_BEGIN(TESTS_TESTSUITES_MESSAGETESTSUITE)
{
    UNITTEST_FIXTURE(main)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}
		
		UNITTEST_TEST(TestGetSize)
		{
			typedef clang::detail::Message<MessageValue> MessageType;
			const clang::u32 messageSize(MessageType::GetSize());
			CHECK_TRUE(messageSize >= 4);    // Message::GetSize() returned invalid size");
		}

		UNITTEST_TEST(TestGetAlignment)
		{
			typedef clang::detail::Message<MessageValue> MessageType;
			const clang::u32 messageAlignment(MessageType::GetAlignment());
			CHECK_TRUE(messageAlignment >= 4);    // Message::TestGetAlignment() returned alignment less than 4");
			CHECK_TRUE((messageAlignment % 4) == 0);    // Message::TestGetAlignment() returned non-power-of-two");
		}

		UNITTEST_TEST(TestConstruction)
		{
			typedef clang::detail::Message<MessageValue> MessageType;

			clang::Address here;
			clang::IAllocator &allocator(*clang::AllocatorManager::Instance().GetAllocator());

			const clang::u32 messageSize(MessageType::GetSize());
			const clang::u32 messageAlignment(MessageType::GetAlignment());

			void *const memory = allocator.AllocateAligned(messageSize, messageAlignment);

			MessageValue value;
			MessageType *const message = MessageType::Initialize(memory, value, here);

			CHECK_TRUE(message != 0);    // Failed to initialize message");

			allocator.Free(memory);
		}

		UNITTEST_TEST(TestValue)
		{
			typedef clang::detail::Message<MessageValue> MessageType;

			clang::Address here;
			clang::IAllocator &allocator(*clang::AllocatorManager::Instance().GetAllocator());

			const clang::u32 messageSize(MessageType::GetSize());
			const clang::u32 messageAlignment(MessageType::GetAlignment());

			void *const memory = allocator.AllocateAligned(messageSize, messageAlignment);

			MessageValue value;
			value.a = 3;
			value.b = 7;

			MessageType *const message = MessageType::Initialize(memory, value, here);

			CHECK_TRUE(message->Value().a == 3);    // Message value incorrect");
			CHECK_TRUE(message->Value().b == 7);    // Message value incorrect");

			allocator.Free(memory);
		}

		UNITTEST_TEST(TestIMessage)
		{
			typedef clang::detail::Message<MessageValue> MessageType;

			clang::Address here;
			clang::IAllocator &allocator(*clang::AllocatorManager::Instance().GetAllocator());

			const clang::u32 messageSize(MessageType::GetSize());
			const clang::u32 messageAlignment(MessageType::GetAlignment());

			void *const memory = allocator.AllocateAligned(messageSize, messageAlignment);

			MessageValue value;
			MessageType *const message = MessageType::Initialize(memory, value, here);

			//CHECK_TRUE(dynamic_cast<clang::detail::IMessage *>(message) != 0);    // Failed to cast message to IMessage");

			allocator.Free(memory);
		}

		UNITTEST_TEST(TestFrom)
		{
			typedef clang::detail::Message<MessageValue> MessageType;

			clang::Address here;
			clang::IAllocator &allocator(*clang::AllocatorManager::Instance().GetAllocator());

			const clang::u32 messageSize(MessageType::GetSize());
			const clang::u32 messageAlignment(MessageType::GetAlignment());

			void *const memory = allocator.AllocateAligned(messageSize, messageAlignment);

			MessageValue value;
			clang::detail::IMessage *const message = MessageType::Initialize(memory, value, here);

			CHECK_TRUE(message->From() == here);    // Message from address incorrect");

			allocator.Free(memory);
		}

		UNITTEST_TEST(TestGetBlock)
		{
			typedef clang::detail::Message<MessageValue> MessageType;

			clang::Address here;
			clang::IAllocator &allocator(*clang::AllocatorManager::Instance().GetAllocator());

			const clang::u32 messageSize(MessageType::GetSize());
			const clang::u32 messageAlignment(MessageType::GetAlignment());

			void *const memory = allocator.AllocateAligned(messageSize, messageAlignment);

			MessageValue value;
			clang::detail::IMessage *const message = MessageType::Initialize(memory, value, here);

			CHECK_TRUE(message->GetBlock() == memory);    // Message block address incorrect");

			allocator.Free(message->GetBlock());
		}

		UNITTEST_TEST(TestGetBlockSize)
		{
			typedef clang::detail::Message<MessageValue> MessageType;

			clang::Address here;
			clang::IAllocator &allocator(*clang::AllocatorManager::Instance().GetAllocator());

			const clang::u32 messageSize(MessageType::GetSize());
			const clang::u32 messageAlignment(MessageType::GetAlignment());

			void *const memory = allocator.AllocateAligned(messageSize, messageAlignment);

			MessageValue value;
			clang::detail::IMessage *const message = MessageType::Initialize(memory, value, here);

			CHECK_TRUE(message->GetBlockSize() == messageSize);    // Message block size incorrect");

			allocator.Free(memory);
		}

		UNITTEST_TEST(TestTypeNameUnregistered)
		{
			typedef clang::detail::Message<MessageValue> MessageType;

			clang::Address here;
			clang::IAllocator &allocator(*clang::AllocatorManager::Instance().GetAllocator());

			const clang::u32 messageSize(MessageType::GetSize());
			const clang::u32 messageAlignment(MessageType::GetAlignment());

			void *const memory = allocator.AllocateAligned(messageSize, messageAlignment);

			MessageValue value;
			clang::detail::IMessage *const message = MessageType::Initialize(memory, value, here);

			CHECK_TRUE(message->TypeId() >= 0);    // Unregistered message type has non-zero type name");

			allocator.Free(memory);
		}

		UNITTEST_TEST(TestTypeNameRegistered)
		{
			typedef clang::detail::Message<NamedMessageValue> MessageType;

			clang::Address here;
			clang::IAllocator &allocator(*clang::AllocatorManager::Instance().GetAllocator());

			const clang::u32 messageSize(MessageType::GetSize());
			const clang::u32 messageAlignment(MessageType::GetAlignment());

			void *const memory = allocator.AllocateAligned(messageSize, messageAlignment);

			NamedMessageValue value;
			clang::detail::IMessage *const message = MessageType::Initialize(memory, value, here);

			CHECK_TRUE(message->TypeId() >= 0);    // Registered message type has zero type name");

			allocator.Free(memory);
		}
	};

}
UNITTEST_SUITE_END


#endif // TESTS_TESTSUITES_MESSAGETESTSUITE

