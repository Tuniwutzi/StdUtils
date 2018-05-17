#ifndef STDUTILS_HTTPRESPONSE_H
#define STDUTILS_HTTPRESPONSE_H

#include <StdUtils/Base.h>

#include <StdUtils/Networking/Http/HttpMessage.h>

namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{
			STDUTILS_START_ENUM(KnownStatusCodes)
			{
				OK = 200,
				BadRequest = 400,
				NotFound = 404,
				UpgradeRequired = 426,
				ServerError = 500
			}
			STDUTILS_END_ENUM(KnownStatusCodes);

			class HttpResponse : public HttpMessage
			{
			public:
				uint16_t statusCode;
				String statusDescription;

			public:
				explicit HttpResponse();
				~HttpResponse();

			public:
				uint16_t getStatusCode() const;
				void setStatusCode(uint16_t statusCode);

				String const& getStatusDescription() const;
				void setStatusDescription(String const& statusDescription);

				String getFirstLine() const;
				void setFirstLine(String const&);

				KnownStatusCodes::Value getKnownStatusCode() const;
				void setKnownStatusCode(KnownStatusCodes::Value code, bool updateDescription = true);

				String toString() const STDUTILS_OVERRIDE;
			};
		}
	}
}

#endif