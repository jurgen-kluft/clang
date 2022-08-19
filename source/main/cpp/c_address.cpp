#include "xlang/private/x_BasicTypes.h"
#include "xlang/private/Threading/x_Mutex.h"

#include "xlang/x_Address.h"


namespace xlang
{
	detail::Mutex Address::smMutex;
	u32 Address::smNextValue = 1;

} // namespace xlang


