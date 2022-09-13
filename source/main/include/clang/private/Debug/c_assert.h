#ifndef __XLANG_PRIVATE_DEBUG_ASSERT_H
#define __XLANG_PRIVATE_DEBUG_ASSERT_H
#include "cbase/c_target.h"
#ifdef USE_PRAGMA_ONCE 
#pragma once 
#endif

#include "cbase/c_debug.h"
#include "clang/c_Defines.h"

#if XLANG_ENABLE_ASSERTS

#ifndef XLANG_ASSERT
#define XLANG_ASSERT(condition)                ASSERT(condition)
#endif // XLANG_ASSERT

#ifndef XLANG_ASSERT_MSG
#define XLANG_ASSERT_MSG(condition, msg)       ASSERTS(condition, msg)
#endif // XLANG_ASSERT_MSG

#ifndef XLANG_FAIL
#define XLANG_FAIL()                           clang::detail::Fail(__FILE__, __LINE__)
#endif // XLANG_FAIL

#ifndef XLANG_FAIL_MSG
#define XLANG_FAIL_MSG(msg)                    clang::detail::Fail(__FILE__, __LINE__, msg)
#endif // XLANG_ASSERT

#else

#ifndef XLANG_ASSERT
#define XLANG_ASSERT(condition)
#endif // XLANG_ASSERT

#ifndef XLANG_ASSERT_MSG
#define XLANG_ASSERT_MSG(condition, msg)
#endif // XLANG_ASSERT_MSG

#ifndef XLANG_FAIL
#define XLANG_FAIL()
#endif // XLANG_FAIL

#ifndef XLANG_FAIL_MSG
#define XLANG_FAIL_MSG(msg)
#endif // XLANG_FAIL_MSG

#endif // XLANG_ENABLE_ASSERTS



namespace clang
{
	namespace detail
	{
#if XLANG_ENABLE_ASSERTS
		/**
		Reports an internal application or system failure.
		A message describing the failure is printed to stderr.
		\param file The name of the file in which the failure occurred.
		\param line The line number at which the failure occurred.
		\param message A message describing the failure.
		*/
		inline void Fail(const char *const file, const unsigned int line, const char *const message = 0)
		{
			//fprintf(stderr, "FAIL in %s (%d)", file, line);
			//if (message)
			//{
			//	fprintf(stderr, ": %s", message);
			//}

			//fprintf(stderr, "\n");
			ASSERT(false);
		}
#endif // XLANG_ENABLE_ASSERTS


	} // namespace detail
} // namespace clang


#endif // __XLANG_PRIVATE_DEBUG_ASSERT_H

