#ifndef __XLANG_PRIVATE_CORE_ACTORCONSTRUCTOR_H
#define __XLANG_PRIVATE_CORE_ACTORCONSTRUCTOR_H

#include "xlang/private/Debug/x_Assert.h"

#include "xlang/x_Defines.h"


namespace xlang
{
	namespace detail
	{
		/// Helper class template that constructs actors.
		/// The point of this class is really to allow reuse of the same creation code
		/// in ActorCreator. The template is partially specialized as a trick to avoid
		/// referencing a non-existant Parameters subclass on actor types that don't
		/// expose one.
		template <class TActor, bool withParameters>
		class ActorConstructor
		{
		public:

			typedef TActor ActorType;

			XLANG_FORCEINLINE ActorConstructor()
			{
			}

			/// Policy operator.
			XLANG_FORCEINLINE ActorType *operator()(void *const memory) const
			{
				XLANG_ASSERT(memory);
				return new (memory) ActorType();
			}

		private:

			ActorConstructor(const ActorConstructor &other);
			ActorConstructor &operator=(const ActorConstructor &other);
		};


		/// Specialization for true, where the actor class has a Parameters subtype.
		template <class TActor>
		class ActorConstructor<TActor, true>
		{
		public:

			typedef TActor ActorType;
			typedef typename ActorType::Parameters Parameters;

			XLANG_FORCEINLINE explicit ActorConstructor(const Parameters &params) : mParams(&params)
			{
			}

			/// Policy operator.
			XLANG_FORCEINLINE ActorType *operator()(void *const memory) const
			{
				XLANG_ASSERT(memory);
				return new (memory) ActorType(*mParams);
			}

		private:

			ActorConstructor(const ActorConstructor &other);
			ActorConstructor &operator=(const ActorConstructor &other);

			const Parameters *const mParams;
		};


	} // namespace detail
} // namespace xlang


#endif // __XLANG_PRIVATE_CORE_ACTORCONSTRUCTOR_H

