#ifndef __XLANG_PRIVATE_CORE_ACTORDESTROYER_H
#define __XLANG_PRIVATE_CORE_ACTORDESTROYER_H

namespace clang
{
	namespace detail
	{
		class ActorCore;

		class ActorDestroyer
		{
		public:

			static void DestroyActor(ActorCore *const actorCore);
		};


	} // namespace detail
} // namespace clang


#endif // __XLANG_PRIVATE_CORE_ACTORDESTROYER_H

