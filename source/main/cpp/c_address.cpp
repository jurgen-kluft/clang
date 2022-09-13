#include "clang/private/c_BasicTypes.h"
#include "clang/private/Threading/c_Mutex.h"

#include "clang/c_Address.h"


namespace clang
{
	detail::Mutex Address::smMutex;
	u32 Address::smNextValue = 1;

} // namespace clang


