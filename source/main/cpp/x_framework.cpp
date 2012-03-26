#include "xlang\private\MessageCache\x_MessageCache.h"
#include "xlang\x_AllocatorManager.h"
#include "xlang\x_Framework.h"

namespace xlang
{
	Framework::Framework()
		: mThreadPool()
		, mFallbackMessageHandler(0)
		, mDefaultFallbackHandler()
	{
		Initialize(2);
	}

	Framework::Framework(const uint32_t numThreads)
		: mThreadPool()
		, mFallbackMessageHandler(0)
		, mDefaultFallbackHandler()
	{
		Initialize(numThreads);
	}


	Framework::~Framework()
	{
		mThreadPool.Stop();

		// Dereference the global free list to ensure it's destroyed.
		detail::MessageCache::Instance().Dereference();

		// Free the fallback handler object, if one is set.
		if (mFallbackMessageHandler)
		{
			AllocatorManager::Instance().GetAllocator()->Free(mFallbackMessageHandler);
		}
	}


} // namespace xlang

