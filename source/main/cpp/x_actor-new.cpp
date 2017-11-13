#include "xlang/private/x_BasicTypes.h"
#include "xlang/private/Core/x_ActorCore.h"
#include "xlang/private/Debug/x_Assert.h"
#include "xlang/private/Directory/x_Directory.h"
#include "xlang/private/Messages/x_MessageCreator.h"
#include "xlang/private/Threading/x_Lock.h"

#include "xlang/x_actor.h"
#include "xlang/x_allocatormanager.h"
#include "xlang/x_framework.h"


namespace xlang
{
	namespace detail
	{
		typedef		u32		msg_id_t;
		typedef		u32		actor_id_t;

		// For messages we can have one allocator per actor for sending messages.
		// This makes the actor be able to control/limit the messages that it
		// creates and sends.
		// The necessary information for a messages is where the message came
		// from so that the receiving actor can send a message back to the
		// sender.
		class xmessage
		{
		protected:
			msg_id_t	m_id;
			xactor*		m_dst;
			xactor*		m_src;
		};

		// Handling a message and figuring out what function to call on the
		// receiving actor.
		// An actor can register message handlers by first registering the
		// message struct

		class my_message : public xmessage
		{
		protected:
			XACTOR_DECLARE_MESSAGE(my_message);

			s32	m_my_data1;
			f32 m_my_data2;

		public:

			
		};

		// We base the receiving of messages on simple structs, messages are
		// always send back to the sender for garbage collection to simplify
		// creation, re-use and destruction of messages.

		class xactor
		{
		public:
			virtual void	process(xmessage* msg) = 0;
			virtual void	gc(xmessage* msg) = 0;
		};

		class xwork
		{
		public:
			virtual bool	take(xactor*& actor, xmessage*& msg, void*& tag) = 0;
			virtual void	done(xactor* actor, xmessage* msg, void* tag) = 0;
		};

		// There are a fixed number of worker-threads, initialized according to what
		// the user needs. The user can use the xsystem package to identify how
		// many physical and logical cores this machine has as well as how many
		// hardware threads.

		class xworker
		{
		public:
			virtual void	run(xwork* work) = 0;
		}

		class xengine
		{
		public:
			virtual void	start() = 0;
			virtual void	stop() = 0;
		}

		class xsystem
		{
		public:
			virtual void	join(actor_id_t id, xactor* actor) = 0;
			virtual void	send(actor_id_t to, xmessage* msg) = 0;

		};

		class xwork_imp : public xwork
		{
		public:
			void		init(s32 max_actors)
			{
				// Initialize a ring buffer that can hold 'max_actors' and will act as the
				// actual work queue.
			}

			bool		take(xthread* thread, xactor_core*& actor, xmessage*& msg, void*& arg)
			{
				// The boolean 'add_back_to_workqueue' is true when actor!=NULL and the message queue of 
				// the 'actor' still has messages.

				// If add_back_to_workqueue==false and the work-queue is empty then return 
				// false and the thread will go into the wait() function of the queue-semaphore.
			}

			void		done(xactor_core* actor, xmessage* msg, void* arg)
			{
				// If boolean 'add_back_to_workqueue' is true add the actor back to the work-queue
				// Deallocate the msg
			}


			// 
			// External calls
			// Send message to actor
			//

			/*
				A pre-allocated ring-buffer of MAX-ACTORS should be perfect for the work-queue.
			*/

			void		send(xactor_core* actor, xmessage* msg)
			{
				// This function can be called from multiple threads!
				//    When the message queue size of the actor goes from 0 -> 1 add the actor to the work-queue
				//    and call signal() on the semaphore.
			}

		};

	}
}