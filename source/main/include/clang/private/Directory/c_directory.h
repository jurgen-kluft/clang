#ifndef __XLANG_PRIVATE_DIRECTORY_DIRECTORY_H
#define __XLANG_PRIVATE_DIRECTORY_DIRECTORY_H

#include "clang/private/Threading/c_Mutex.h"
#include "clang/c_Defines.h"

namespace clang
{
	namespace detail
	{
		/// Provides thread synchronization for the actor pool and receiver registry.
		class Directory
		{
		public:

			/// Gets a reference to the mutex which locks the directory singleton for exclusive thread access.
			inline static Mutex &GetMutex();

		private:

			Directory();
			Directory(const Directory &other);
			Directory &operator=(const Directory &other);

			static Mutex smMutex;                   ///< Mutex that protects access to the instance.
		};


		XLANG_FORCEINLINE Mutex &Directory::GetMutex()
		{
			return smMutex;
		}


	} // namespace detail
} // namespace clang


#endif // __XLANG_PRIVATE_DIRECTORY_DIRECTORY_H

