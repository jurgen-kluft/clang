#include "clang/private/Core/c_ActorCreator.h"

namespace clang
{
	namespace detail
	{
		Mutex ActorCreator::smMutex;
		Address ActorCreator::smAddress;
		ActorCore *ActorCreator::smCoreAddress = 0;

	} // namespace detail
} // namespace clang

