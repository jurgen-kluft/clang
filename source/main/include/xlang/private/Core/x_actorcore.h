#ifndef __XLANG_PRIVATE_CORE_ACTORCORE_H
#define __XLANG_PRIVATE_CORE_ACTORCORE_H

#include "xlang/private/x_BasicTypes.h"
#include "xlang/private/Containers/x_IntrusiveList.h"
#include "xlang/private/Containers/x_IntrusiveQueue.h"
#include "xlang/private/Debug/x_Assert.h"
#include "xlang/private/Handlers/x_MessageHandler.h"
#include "xlang/private/Handlers/x_IMessageHandler.h"
#include "xlang/private/Handlers/x_MessageHandlerCast.h"
#include "xlang/private/Messages/x_IMessage.h"
#include "xlang/private/Messages/x_MessageTraits.h"

#include "xlang/x_Address.h"
#include "xlang/x_Defines.h"
#include "xlang/x_IAllocator.h"

namespace xlang
{
	class Actor;
	class Framework;

	namespace detail
	{
		/// Core functionality of an actor, not exposed to actor implementations.
		class ActorCore
		{
		public:

			friend class Actor;
			friend class Framework;

			/// Default constructor.
			/// \note Actor cores can't be constructed directly in user code.
			ActorCore();

			/// Constructor.
			/// \note Actor cores can't be constructed directly in user code.
			ActorCore(const u32 sequence, Framework *const framework, Actor *const actor);

			/// Destructor.
			~ActorCore();

			/// Sets the pointer to the next actor in a queue of actors.
			XLANG_FORCEINLINE void SetNext(ActorCore *const next)	{ mNext = next; }

			/// Gets the pointer to the next actor in a queue of actors.
			XLANG_FORCEINLINE ActorCore *GetNext() const			{ return mNext; }

			/// Sets the sequence number of the actor.
			XLANG_FORCEINLINE void SetSequence(const u32 sequence)	{ mSequence = sequence; }

			/// Gets the sequence number of the actor.
			XLANG_FORCEINLINE u32 GetSequence() const				{ return mSequence; }

			/// Pushes a message into the actor.
			XLANG_FORCEINLINE void Push(IMessage *const message)
			{
				XLANG_ASSERT(message);

				// Push the message onto the internal queue to await delivery.
				// The point of this is to maintain correct message delivery order within the actor.
				// The queue stores pointers to the IMessage interfaces for polymorphism.
				// We reuse the work queue lock to protect the per-actor message queues.
				mMessageQueue.Push(message);
				++mMessageCount;
			}

			/// Returns a pointer to the actor that contains this core.
			XLANG_FORCEINLINE Actor *GetParent() const				{ return mParent; }

			/// Gets the framework to which this actor belongs. Provides derived actor class
			/// implementations with access the owning framework.
			/// \return A pointer to the framework to which the actor belongs.
			XLANG_FORCEINLINE Framework *GetFramework() const		{ XLANG_ASSERT(mFramework); return mFramework; }

			/// Gets a reference to the mutex that protects the actor.
			Mutex &GetMutex() const;

			/// Returns true if the actor is marked as referenced by at least one ActorRef.
			XLANG_FORCEINLINE bool IsReferenced() const				{ return ((mState & STATE_REFERENCED) != 0); }

			/// Marks the actor as unreferenced and so ready for garbage collection.
			void Unreference();

			/// Returns true if the actor is being processed, or is scheduled for processing.
			XLANG_FORCEINLINE bool IsScheduled() const				{ return ((mState & STATE_BUSY) != 0); }

			/// Marks the actor as either being processed, or scheduled for processing.
			XLANG_FORCEINLINE void Schedule()						{ mState |= STATE_BUSY; }

			/// Marks the actor as neither being processed nor scheduled for processing.
			XLANG_FORCEINLINE void Unschedule()						{ mState &= STATE_BUSY; }

			/// Returns true if the actor is marked as in need of processing.
			XLANG_FORCEINLINE bool IsDirty() const					{ return ((mState & STATE_DIRTY) != 0); }

			/// Marks the actor as in need of processing, causing it to be scheduled.
			XLANG_FORCEINLINE void Dirty()							{ mState |= STATE_DIRTY; }

			/// Marks the actor as not in need of processing.
			XLANG_FORCEINLINE void Clean()							{ mState &= ~STATE_DIRTY; }

			/// Marks the actor as being processed but not yet in need of further processing.
			XLANG_FORCEINLINE void CleanAndSchedule()
			{
				u32 state(mState);
				state &= ~STATE_DIRTY;
				state |= STATE_BUSY;
				mState = state;
			}

			/// Marks the actor as neither being processed nor in need of processing.
			XLANG_FORCEINLINE void CleanAndUnschedule()				{ mState &= ~(STATE_DIRTY | STATE_BUSY); }

			/// Returns true if the actor has been notified that its handlers need updating.
			XLANG_FORCEINLINE bool AreHandlersDirty() const			{ return ((mState & STATE_HANDLERS_DIRTY) != 0); }

			/// Marks that the actor's handler list has changed since it was last processed.
			XLANG_FORCEINLINE void DirtyHandlers()					{ mState |= STATE_HANDLERS_DIRTY; }

			/// Deregisters a previously registered handler.
			template <class ActorType, class ValueType>
			inline bool			DeregisterHandler(ActorType *const actor, void (ActorType::*handler)(const ValueType &message, const Address from));

			/// Checks whether the given message handler is registered.
			template <class ActorType, class ValueType>
			inline bool			IsHandlerRegistered(ActorType *const actor, void (ActorType::*handler)(const ValueType &message, const Address from));

			/// Removes any handlers marked for removal and adds any scheduled for adding.
			inline void			ValidateHandlers();

			/// Gets the number of messages queued at this actor, awaiting processing.
			inline u32			GetNumQueuedMessages() const;

			/// Checks whether the actor has any queued messages awaiting processing.
			inline bool			HasQueuedMessage() const;

			/// Gets the first message from the message queue, if any.
			inline IMessage*	GetQueuedMessage();

			/// Presents the actor with one of its queued messages, if any,
			/// and calls the associated handler.
			inline void			ProcessMessage(IMessage *const message);

		private:

			/// Flags describing the execution state of an actor.
			enum
			{ 
				STATE_BUSY = (1 << 0),								///< Being processed (in the work queue or being executed).
				STATE_DIRTY = (1 << 1),								///< In need of more processing after current execution.
				STATE_HANDLERS_DIRTY = (1 << 2),					///< One or more message handlers added or removed since last run.
				STATE_REFERENCED = (1 << 3),						///< Actor is referenced by one or more ActorRefs so can't be garbage collected.
				STATE_FORCESIZEINT = 0xFFFFFFFF						///< Ensures the enum is an integer.
			};

			typedef IntrusiveQueue<IMessage> MessageQueue;

							ActorCore(const ActorCore &other);
							ActorCore &operator=(const ActorCore &other);

			/// Updates the core's registered handler list with any changes from the actor.
			void			UpdateHandlers();

			/// Executes the core's default handler, if any, for an unhandled message.
			bool			ExecuteDefaultHandler(IMessage *const message);

			/// Executes the framework's fallback handler, if any, for an unhandled message.
			bool			ExecuteFallbackHandler(IMessage *const message);

			ActorCore					*mNext;						///< Pointer to the next actor in a queue of actors.
			Actor						*mParent;					///< Address of the actor instance containing this core.
			Framework					*mFramework;				///< The framework instance that owns this actor.
			u32							mSequence;					///< Sequence number of the actor (half of its unique address).
			u32							mMessageCount;				///< Number of messages in the message queue.
			MessageQueue				mMessageQueue;				///< Queue of messages awaiting processing.
			u32							mNumMessageHandlers;
			u32							mMaxMessageHandlers;
			detail::MessageHandler_t	mMessageHandlers[32];
			u32							mState;						///< Execution state (idle, busy, dirty).
		};


		template <class ActorType, class ValueType>
		inline bool ActorCore::DeregisterHandler(ActorType *const /*actor*/, void (ActorType::*handler)(const ValueType &message, const Address from))
		{
			// If the message value type has a valid (non-zero) type name defined for it,
			// then we use explicit type names to match messages to handlers.
			// The default value of zero indicates that no type name has been defined,
			// in which case we rely on compiler-generated RTTI to identify message types.
			typedef MessageHandler<ActorType, ValueType> MessageHandlerType;
			typedef MessageHandlerCast<ActorType, MessageTraits<ValueType>::HAS_TYPE_NAME> HandlerCaster;

			// We don't need to lock this because only one thread can access it at a time.
			// Find the handler in the registered handler list.
			for (u32 i=0; i<mNumMessageHandlers; ++i)
			{
				IMessageHandler* messageHandler = (IMessageHandler*)&mMessageHandlers[i];
				// Try to convert this handler, of unknown type, to the target type.
				const MessageHandlerType *const typedHandler = HandlerCaster:: template CastHandler<ValueType>(messageHandler);
				if (typedHandler)
				{
					// Don't count the handler if it's already marked for deregistration.
					if (typedHandler->GetHandlerFunction() == handler && !typedHandler->IsMarked())
					{
						// Mark the handler for deregistration.
						// We defer the actual deregistration and deletion until
						// after all active message handlers have been executed, because
						// message handlers themselves can deregister handlers.
						messageHandler->Mark();

						// Mark the handlers as dirty so we update them before the next processing.
						DirtyHandlers();

						return true;
					}
				}
			}
			return false;
		}


		template <class ActorType, class ValueType>
		inline bool ActorCore::IsHandlerRegistered(ActorType *const /*actor*/, void (ActorType::*handler)(const ValueType &message, const Address from))
		{
			// If the message value type has a valid (non-zero) type name defined for it,
			// then we use explicit type names to match messages to handlers.
			// The default value of zero indicates that no type name has been defined,
			// in which case we rely on compiler-generated RTTI to identify message types.
			typedef MessageHandler<ActorType, ValueType> MessageHandlerType;
			typedef MessageHandlerCast<ActorType, MessageTraits<ValueType>::HAS_TYPE_NAME> HandlerCaster;

			// We don't need to lock this because only one thread can access it at a time.
			// Find the handler in the registered handler list.
			for (u32 i=0; i<mNumMessageHandlers; ++i)
			{
				IMessageHandler* messageHandler = (IMessageHandler*)&mMessageHandlers[i];
				// Try to convert this handler, of unknown type, to the target type.
				const MessageHandlerType *const typedHandler = HandlerCaster:: template CastHandler<ValueType>(messageHandler);
				if (typedHandler)
				{
					// Don't count the handler if it's already marked for deregistration.
					if (typedHandler->GetHandlerFunction() == handler && !typedHandler->IsMarked())
					{
						return true;
					}
				}
			}
			return false;

		}


		XLANG_FORCEINLINE u32 ActorCore::GetNumQueuedMessages() const
		{
			return mMessageCount;
		}


		XLANG_FORCEINLINE bool ActorCore::HasQueuedMessage() const
		{
			return (mMessageCount > 0);
		}


		XLANG_FORCEINLINE IMessage *ActorCore::GetQueuedMessage()
		{
			// This exploits the fact that bools are either 0 or 1 to avoid a branch.
			IMessage *const message(mMessageQueue.Pop());
			const bool gotMessage(message != 0);
			const u32 messageDecrement(static_cast<u32>(gotMessage));

			XLANG_ASSERT(messageDecrement <= 1);
			XLANG_ASSERT(mMessageCount >= messageDecrement);

			mMessageCount -= messageDecrement;
			return message;
		}


		XLANG_FORCEINLINE void ActorCore::ValidateHandlers()
		{
			if ((mState & STATE_HANDLERS_DIRTY) != 0)
			{
				mState &= (~STATE_HANDLERS_DIRTY);
				UpdateHandlers();
			}
		}


		XLANG_FORCEINLINE void ActorCore::ProcessMessage(IMessage *const message)
		{
			XLANG_ASSERT(message);

			// Give each registered handler a chance to handle this message.
			bool handled(false);

			// Use the message type name as the key
			int const msg_typeid = message->TypeId();

			// @TODO: Binary search!
			// Currently a linear search
			for (u32 i=0; i<mNumMessageHandlers; ++i)
			{
				IMessageHandler* handler = (IMessageHandler*)&mMessageHandlers[i];
				if (handler->GetMessageTypeId() == msg_typeid)
				{
					handled = handler->Handle(mParent, message);
					XLANG_ASSERT(handled);
				}
			}

			if (handled)
				return;

			// If no registered handler handled the message, execute the default handler instead.
			if (ExecuteDefaultHandler(message))
				return;

			// Finally if the actor has no default handler then run the framework's fallback handler.
			ExecuteFallbackHandler(message);
		}


	} // namespace detail
} // namespace xlang


#endif // __XLANG_PRIVATE_CORE_ACTORCORE_H

