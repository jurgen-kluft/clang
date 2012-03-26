#include "xlang\private\Core\x_ActorCreator.h"

namespace xlang
{
	namespace detail
	{
		Mutex ActorCreator::smMutex;
		Address ActorCreator::smAddress;
		ActorCore *ActorCreator::smCoreAddress = 0;

	} // namespace detail
} // namespace xlang

