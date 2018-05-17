#include <StdUtils/Networking/Http/WSAcceptException.h>

namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{
			WSAcceptException::WSAcceptException(WSAcceptException::FailReason reason, SharedPointer<HttpServerContext>& ctx, String const& msg)
				:Exception(msg),
				reason(reason),
				context(ctx)
			{
			}
		}
	}
}