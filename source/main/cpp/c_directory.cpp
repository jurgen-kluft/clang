#include "clang/private/Directory/c_Directory.h"
#include "clang/private/Threading/c_Mutex.h"


namespace clang
{
	namespace detail
	{
		Mutex Directory::smMutex;

	} // namespace detail
} // namespace clang


