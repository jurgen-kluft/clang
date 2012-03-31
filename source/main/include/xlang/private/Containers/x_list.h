#ifndef __XLANG_PRIVATE_CONTAINERS_LIST_H
#define __XLANG_PRIVATE_CONTAINERS_LIST_H
#include "xbase\x_target.h"
#ifdef USE_PRAGMA_ONCE 
#pragma once 
#endif

#include "xbase\x_allocator.h"

#include "xlang\private\x_BasicTypes.h"
#include "xlang\private\Debug\x_Assert.h"

#include "xlang\x_AllocatorManager.h"
#include "xlang\x_Defines.h"


namespace xlang
{
	namespace detail
	{
		/// Class template implementing a generic unsorted list.
		template <class ItemType>
		class List
		{
		public:

			/// Defines the node type used within the list.
			/// Class template defining a generic node in a generic list.
			/// Wraps the list item type in a wrapper class with next and previous pointers.
			struct Node
			{
				ItemType	mItem;		///> The item stored in the node.
				Node		*mNext;		///> Pointer to the next node in the list.
				XCORE_CLASS_PLACEMENT_NEW_DELETE
			};

			/// Const iterator type.
			class ListConstIterator
			{
			public:

				/// Default constructor
				/// Constructs an invalid iterator which must be assigned before being used.
				XLANG_FORCEINLINE ListConstIterator() : mNode(0)
				{
				}

				/// Constructor
				/// \param node The list node that the iterator initially references.
				XLANG_FORCEINLINE explicit ListConstIterator(Node *const node) : mNode(node)
				{
				}

				/// Copy constructor.
				XLANG_FORCEINLINE ListConstIterator(const ListConstIterator &other) : mNode(other.mNode)
				{
				}

				/// Assignment operator.
				XLANG_FORCEINLINE ListConstIterator &operator=(const ListConstIterator &other)
				{
					mNode = other.mNode;
					return *this;
				}

				/// Dereference operator. Returns a reference to the list item that the iterator currently references.
				/// \return A const reference to the list item referenced by the iterator.
				/// \note If the iterator doesn't reference a valid item then the result is undefined.
				XLANG_FORCEINLINE const ItemType &operator*() const
				{
					XLANG_ASSERT(mNode);
					return mNode->mItem;
				}

				/// Pre-increment operator.
				/// Moves the iterator to reference the next node in the list.
				/// If the iterator currently references the last item in the list then the iterator
				/// beomes equal to End after incrementation.
				/// \note If the iterator is already equal to End then the result is undefined.
				XLANG_FORCEINLINE void operator++()
				{
					XLANG_ASSERT(mNode);
					mNode = mNode->mNext;
				}

				/// Equality operator. Compares two iterators for equality.
				/// \param other Another iterator whose value is to be compared to this one.
				/// \return True if the iterators reference the same item in the same list, otherwise false.
				XLANG_FORCEINLINE bool operator==(const ListConstIterator &other) const
				{
					return (mNode == other.mNode);
				}

				/// Inequality operator. Compares two iterators for inequality.
				/// \param other Another iterator whose value is to be compared to this one.
				/// \return True if the iterators reference different items or different lists, otherwise false.
				XLANG_FORCEINLINE bool operator!=(const ListConstIterator &other) const
				{
					return !operator==(other);
				}

			protected:

				Node *mNode;        ///< Pointer to the list node that the iterator references.
			};

			/// Iterator type.
			class ListIterator : public ListConstIterator
			{
			public:

				/// Default constructor
				/// Constructs an invalid iterator which must be assigned before being used.
				XLANG_FORCEINLINE ListIterator() : ListConstIterator(0)
				{
				}

				/// Constructor
				/// \param node The list node that the iterator initially references.
				XLANG_FORCEINLINE explicit ListIterator(Node *const node) : ListConstIterator(node)
				{
				}

				/// Copy constructor.
				XLANG_FORCEINLINE ListIterator(const ListConstIterator &other) : ListConstIterator::mNode(other.mNode)
				{
				}

				/// Assignment operator.
				XLANG_FORCEINLINE ListIterator &operator=(const ListConstIterator &other)
				{
					ListConstIterator::mNode = other.mNode;
					return *this;
				}

				/// Dereference operator. Returns a reference to the list item that the iterator currently references.
				/// \return A reference to the list item referenced by the iterator.
				/// \note If the iterator doesn't reference a valid item then the result is undefined.
				XLANG_FORCEINLINE ItemType &operator*()
				{
					XLANG_ASSERT(ListConstIterator::mNode);
					return ListConstIterator::mNode->mItem;
				}

				/// Equality operator. Compares two iterators for equality.
				/// \param other Another iterator whose value is to be compared to this one.
				/// \return True if the iterators reference the same item in the same list, otherwise false.
				XLANG_FORCEINLINE bool operator==(const ListConstIterator &other) const
				{
					return ListConstIterator::operator==(other);
				}

				/// Inequality operator. Compares two iterators for inequality.
				/// \param other Another iterator whose value is to be compared to this one.
				/// \return True if the iterators reference different items or different lists, otherwise false.
				XLANG_FORCEINLINE bool operator!=(const ListConstIterator &other) const
				{
					return ListConstIterator::operator!=(other);
				}
			};

			/// Defines the iterator type exposed by the list.
			typedef ListIterator iterator;

			/// Defines the const iterator type exposed by the list.
			typedef ListConstIterator const_iterator;

			/// Constructor
			inline List();

			/// Destructor
			inline ~List();

			/// Returns an iterator referencing the first item in the list.
			/// \return An iterator referencing the first item in the list, or End if the list is empty.
			inline iterator Begin();

			/// Returns an iterator referencing the end of the list.
			/// Incrementing an iterator that references the last item in the list results in the
			/// iterator being equal to the iterator returned by End.
			/// \return An iterator referencing the end of the list.
			inline iterator End();

			/// Returns an iterator referencing the first item in the list.
			/// \return An iterator referencing the first item in the list, or End if the list is empty.
			inline const_iterator Begin() const;

			/// Returns an iterator referencing the end of the list.
			/// Incrementing an iterator that references the last item in the list results in the
			/// iterator being equal to the iterator returned by End.
			/// \return An iterator referencing the end of the list.
			inline const_iterator End() const;

			/// Returns the number of items currently in the list.
			/// \return The number of items in the list.
			inline u32 Size() const;

			/// Returns true if there are no items currently in the list.
			inline bool Empty() const;

			/// Empties the list, removing all previously inserted items.
			inline void Clear();

			/// Adds an item to the list. The new item is copied by reference and added at the head.
			/// No checking for duplicates is performed.
			/// \param item A const reference to the item to be added to the list.
			inline void Insert(const ItemType &item);

			/// Returns a bool result indicating whether the list contains an item equal to the
			/// given item.
			/// \param item The item to be searched for.
			/// \return True, if the list contains an equal item, otherwise false.
			inline bool Contains(const ItemType &item) const;

			/// Removes the first occurance of an item from the list.
			/// The list is searched for an item equal to the provided item. If one is found
			/// then the first such item found is removed from the list.
			/// \note If the list contains duplicate copies of the removed item then the
			/// duplicates will remain and should be removed with subsequent calls to Remove.
			/// \param item The item to be removed.
			/// \return True if the item was found and removed, otherwise false.
			inline bool Remove(const ItemType &item);

			/// Returns a const reference to the first item in the list.
			inline const ItemType &Front() const;

		private:

			List(const List &other);
			List &operator=(const List &other);

			Node *mHead;                ///< Pointer to the node at the head of the list.
		};


		template <class ItemType>
		XLANG_FORCEINLINE List<ItemType>::List() :
		mHead(0)
		{
		}


		template <class ItemType>
		XLANG_FORCEINLINE List<ItemType>::~List()
		{
			// Free all the nodes currently allocated for the list.
			Clear();
		}


		template <class ItemType>
		XLANG_FORCEINLINE typename List<ItemType>::iterator List<ItemType>::Begin()
		{
			return iterator(mHead);
		}


		template <class ItemType>
		XLANG_FORCEINLINE typename List<ItemType>::iterator List<ItemType>::End()
		{
			return iterator(0);
		}


		template <class ItemType>
		XLANG_FORCEINLINE typename List<ItemType>::const_iterator List<ItemType>::Begin() const
		{
			return const_iterator(mHead);
		}


		template <class ItemType>
		XLANG_FORCEINLINE typename List<ItemType>::const_iterator List<ItemType>::End() const
		{
			return const_iterator(0);
		}


		template <class ItemType>
		XLANG_FORCEINLINE u32 List<ItemType>::Size() const
		{
			u32 count(0);
			Node *node(mHead);

			while (node)
			{
				++count;
				node = node->mNext;
			}

			return count;
		}


		template <class ItemType>
		XLANG_FORCEINLINE bool List<ItemType>::Empty() const
		{
			return (mHead == 0);
		}


		template <class ItemType>
		XLANG_FORCEINLINE void List<ItemType>::Clear()
		{
			// Free all the nodes currently allocated for the list.
			Node *node(mHead);
			while (node)
			{
				Node *const next = node->mNext;
				AllocatorManager::Instance().GetAllocator()->Free(node);
				node = next;
			}

			mHead = 0;
		}


		template <class ItemType>
		XLANG_FORCEINLINE void List<ItemType>::Insert(const ItemType &item)
		{
			void *const memory = AllocatorManager::Instance().GetAllocator()->Allocate(sizeof(Node));
			XLANG_ASSERT(memory);

			Node *const newNode = new (memory) Node;
			XLANG_ASSERT(newNode);

			newNode->mItem = item;
			newNode->mNext = mHead;
			mHead = newNode;
		}


		template <class ItemType>
		XLANG_FORCEINLINE bool List<ItemType>::Contains(const ItemType &item) const
		{
			const Node *node(mHead);
			while (node)
			{
				if (node->mItem == item)
				{
					return true;
				}

				node = node->mNext;
			}

			return false;
		}


		template <class ItemType>
		inline bool List<ItemType>::Remove(const ItemType &item)
		{
			// Find the node and the one before it, if any
			Node *previous(0);
			Node *node(mHead);

			while (node)
			{
				if (node->mItem == item)
				{
					break;
				}

				previous = node;
				node = node->mNext;
			}

			// Node in list?
			if (node)
			{
				// Node at head?
				if (previous == 0)
				{
					mHead = node->mNext;
				}
				else
				{
					previous->mNext = node->mNext;
				}

				AllocatorManager::Instance().GetAllocator()->Free(node);
				return true;
			}

			return false;
		}


		template <class ItemType>
		XLANG_FORCEINLINE const ItemType &List<ItemType>::Front() const
		{
			XLANG_ASSERT(mHead);
			return mHead->mItem;
		}


	} // namespace detail
} // namespace xlang


#endif // __XLANG_PRIVATE_CONTAINERS_LIST_H

