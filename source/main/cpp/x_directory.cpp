#include "xlang/private/Directory/x_Directory.h"
#include "xlang/private/Threading/x_Mutex.h"


namespace xlang
{
	namespace detail
	{
		Mutex Directory::smMutex;

	} // namespace detail
} // namespace xlang


