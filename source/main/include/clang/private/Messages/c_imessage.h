#ifndef __XLANG_PRIVATE_MESSAGES_IMESSAGE_H
#define __XLANG_PRIVATE_MESSAGES_IMESSAGE_H
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE 
#pragma once 
#endif

#include "clang/private/c_BasicTypes.h"
#include "clang/c_Address.h"
#include "clang/c_Defines.h"

namespace clang
{
	namespace detail
	{
		class type2intgeneric
		{
		protected:
			static const ncore::s32 next()
			{
				static ncore::s32 id = 1;
				return id++;
			}
		};

		template <typename T>
		struct type2int : public type2intgeneric
		{
			static const ncore::s32 value() 
			{
				static const ncore::s32 id = next();
				return id;
			}
		};

		/// Interface describing the generic API of the message class template.
		class IMessage
		{
		public:

			/// Sets the pointer to the next message in a queue of messages.
			XLANG_FORCEINLINE void SetNext(IMessage *const next)
			{
				mNext = next;
			}

			/// Gets the pointer to the next message in a queue of messages.
			XLANG_FORCEINLINE IMessage *GetNext() const
			{
				return mNext;
			}

			/// Gets the address from which the message was sent.
			XLANG_FORCEINLINE Address From() const
			{
				return mFrom;
			}

			/// Returns the memory block in which this message was allocated.
			XLANG_FORCEINLINE void *GetBlock() const
			{
				XLANG_ASSERT(mBlock);
				return mBlock;
			}

			/// Returns the size in bytes of the memory block in which this message was allocated.
			XLANG_FORCEINLINE u32 GetBlockSize() const
			{
				XLANG_ASSERT(mBlockSize);
				return mBlockSize;
			}

			/// Returns the message value as blind data.
			XLANG_FORCEINLINE const void *GetMessageData() const
			{
				XLANG_ASSERT(mBlock);
				return mBlock;
			}

			/// Returns the size in bytes of the message data.
			virtual u32 GetMessageValueSize() const = 0;

			/// Returns the name of the message type.
			/// This uniquely identifies the type of the message value.
			/// \note Unless explicitly specified to avoid C++ RTTI, message names are null.
			virtual int TypeId() const = 0;

			/// Allows the message instance to destruct its constructed value object before being freed.
			virtual void Release() = 0;

		protected:

			/// Constructs an IMessage.
			/// \param from The address from which the message was sent.
			/// \param block The memory block containing the message.
			/// \param blockSize The size of the memory block containing the message.
			/// \param typeName String identifier uniquely identifying the type of the message value.
			XLANG_FORCEINLINE IMessage(const Address &from, void *const block, const u32 blockSize) 
				: mNext(0)
				, mFrom(from)
				, mBlock(block)
				, mBlockSize(blockSize)
			{
			}

			/// Virtual destructor.
			XLANG_FORCEINLINE virtual ~IMessage()
			{
			}

		private:
							IMessage(const IMessage &other);
							IMessage &operator=(const IMessage &other);

			IMessage		*mNext;			///< Pointer to the next message in a message queue.
			const Address	mFrom;			///< The address from which the message was sent.
			void *const		mBlock;			///< Pointer to the memory block containing the message.
			const u32		mBlockSize;		///< Total size of the message memory block in bytes.
		};


	} // namespace detail
} // namespace clang


#endif // __XLANG_PRIVATE_MESSAGES_IMESSAGE_H

