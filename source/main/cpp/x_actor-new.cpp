#include "xlang/private/x_BasicTypes.h"
#include "xlang/private/Core/x_ActorCore.h"
#include "xlang/private/Debug/x_Assert.h"
#include "xlang/private/Directory/x_Directory.h"
#include "xlang/private/Messages/x_MessageCreator.h"
#include "xlang/private/Threading/x_Lock.h"

#include "xlang/x_Actor.h"
#include "xlang/x_AllocatorManager.h"
#include "xlang/x_Framework.h"


namespace xlang
{
	namespace detail
	{
		class xthread
		{

		};

		class xmessage
		{

		};

		class xactor_core
		{

		};

		class xworkqueue
		{
		public:

			bool		take(xthread* thread, xactor_core*& actor, xmessage*& msg, bool& add_back_to_workqueue)
			{
				// The boolean 'add_back_to_workqueue' is true when the message queue of 
				// 'actor' still has messages.

				// Acquire lock 'spin-lock'
				//     If the work-queue is empty then push the thread object onto the 'halted-threads stack'.
				//     We will return false and the thread will then call it's own wait() function.
				// Unlock 'spin-lock'
			}

			void		done(xactor_core* actor, xmessage* msg, bool add_back_to_workqueue)
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
				// Acquire 'spin-lock' and execute the below
				//    When the message queue size of the actor goes from 0 -> 1 add the actor to the work-queue.
				//    Pop a thread from the 'halted-threads stack', if we got one then call 'signal' on that thread. 
				// Unlock 'spin-lock'
			}

		};

	}
}