#ifndef __XLANG_PRIVATE_CONTAINERS_INTRUSIVEQUEUE_H
#define __XLANG_PRIVATE_CONTAINERS_INTRUSIVEQUEUE_H

#include "xlang/private/x_BasicTypes.h"
#include "xlang/private/Debug/x_Assert.h"

#include "xlang/x_Align.h"
#include "xlang/x_Defines.h"


namespace xlang
{
	namespace detail
	{
		/// Class template describing a generic queue.
		/// \note The item type is the node type and is expected to expose SetNext and GetNext methods.
		template <class ItemType>
		class IntrusiveQueue
		{
		public:

			/// Constructor
			inline IntrusiveQueue();

			/// Destructor
			inline ~IntrusiveQueue();

			/// Returns true if the queue contains no items.
			inline bool Empty() const;

			/// Pushes an item onto the queue.
			inline void Push(ItemType *const item);

			/// Removes and returns the item at the front of the queue.
			inline ItemType *Pop();

		private:

			IntrusiveQueue(const IntrusiveQueue &other);
			IntrusiveQueue &operator=(const IntrusiveQueue &other);

			ItemType *mFront;           ///< Pointer to the item at the front of the queue.
			ItemType *mBack;            ///< Pointer to the item at the back of the queue.
		};


		template <class ItemType>
		XLANG_FORCEINLINE IntrusiveQueue<ItemType>::IntrusiveQueue()
			: mFront(0)
			, mBack(0)
		{
		}


		template <class ItemType>
		XLANG_FORCEINLINE IntrusiveQueue<ItemType>::~IntrusiveQueue()
		{
			// If the queue hasn't been emptied by the caller we'll leak the nodes.
			XLANG_ASSERT(mFront == 0);
			XLANG_ASSERT(mBack == 0);
		}


		template <class ItemType>
		XLANG_FORCEINLINE bool IntrusiveQueue<ItemType>::Empty() const
		{
			return (mFront == 0);
		}


		template <class ItemType>
		XLANG_FORCEINLINE void IntrusiveQueue<ItemType>::Push(ItemType *const item)
		{
			XLANG_ASSERT(item);

			if (mBack)
			{
				XLANG_ASSERT(mFront);
				mBack->SetNext(item);
			}
			else
			{
				XLANG_ASSERT(mFront == 0);
				mFront = item;
			}

			item->SetNext(0);
			mBack = item;
		}


		template <class ItemType>
		XLANG_FORCEINLINE ItemType *IntrusiveQueue<ItemType>::Pop()
		{
			ItemType *item(0);

			if (mFront)
			{
				item = mFront;
				ItemType *const next(mFront->GetNext());

				mFront = next;
				if (!next)
				{
					mBack = 0;
				}
			}

			return item;
		}


	} // namespace detail
} // namespace xlang


#endif // __XLANG_PRIVATE_CONTAINERS_INTRUSIVEQUEUE_H

