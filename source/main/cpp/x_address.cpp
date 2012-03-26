#include "xlang\private\x_BasicTypes.h"
#include "xlang\private\Threading\x_Mutex.h"

#include "xlang\x_Address.h"


namespace xlang
{
	detail::Mutex Address::smMutex;
	uint32_t Address::smNextValue = 1;

} // namespace xlang


