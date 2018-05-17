#ifndef STDUTILS_WSACCEPTEXCEPTION_H
#define STDUTILS_WSACCEPTEXCEPTION_H

#include <StdUtils/Exceptions.h>
#include <StdUtils/Networking/Http/HttpServerContext.h>
#include <StdUtils/SharedPointer.h>

namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{
			class WSAcceptException : public Exception
			{
			public:
				enum FailReason
				{
					Unknown,
					EndpointMismatch,
					HandshakeFailure,
					ProtocolError
				};

			private:
				FailReason reason;
				SharedPointer<HttpServerContext> context;

			public:
				WSAcceptException(FailReason, SharedPointer<HttpServerContext>&, String const&);
				virtual ~WSAcceptException() throw ()
				{
				}

			public:
				FailReason getReason() const;
				SharedPointer<HttpServerContext>& getContext();
				SharedPointer<HttpServerContext> const& getContext() const;
			};
		}
	}
}

#endif
