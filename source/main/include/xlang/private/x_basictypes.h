#ifndef __XLANG_PRIVATE_BASICTYPES_H
#define __XLANG_PRIVATE_BASICTYPES_H
#include "xbase\x_target.h"
#ifdef USE_PRAGMA_ONCE 
#pragma once 
#endif

// Included for size_t
#include <stddef.h>

#if !defined(_MSC_VER)
	// We need this header to define the uintptr_t type defined by C99.
	// The header is defined in C99, and some modern compilers supply it.
	// If yours doesn't provide it then just typedef uintptr_t as an unsigned
	// integer type the same width (in bits) as a pointer on your target platform.
	// For example, on 64-bit platforms:
	// typedef unsigned long long uintptr_t
	#include <inttypes.h>
#endif // !defined(_MSC_VER)


namespace xlang
{
	/// An 8-bit unsigned integer.
	typedef unsigned char uint8_t;

	/// A 32-bit unsigned integer.
	typedef unsigned int uint32_t;
}


#endif // __XLANG_PRIVATE_BASICTYPES_H

