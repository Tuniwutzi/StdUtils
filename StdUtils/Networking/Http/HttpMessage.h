#ifndef STDUTILS_HTTPMESSAGE_H
#define STDUTILS_HTTPMESSAGE_H

#include <StdUtils/Base.h>

#include <StdUtils/Networking/Http/HttpHeaders.h>

namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{
			STDUTILS_START_ENUM(MessageTypes)
			{
				Request,
					Response
			}
			STDUTILS_END_ENUM(MessageTypes);

			class HttpMessage
			{
			private:
				MessageTypes::Value type;

				String protocolVersion;
				HttpHeaders headers;

			protected:
				explicit HttpMessage(MessageTypes::Value type);
			public:
				virtual ~HttpMessage() {}

			public:
				MessageTypes::Value getType() const;

				virtual String getFirstLine() const = 0;
				virtual void setFirstLine(String const&) = 0;

				String const& getProtocolVersion() const;
				void setProtocolVersion(String const& protocolVersion);

				HttpHeaders const& getHeaders() const;
				HttpHeaders& getHeaders();
				void setHeaders(HttpHeaders const& headers);

				void setContentType(String const& contentType);
				String const& getContentType() const;

				void setContentLength(unsigned int length);
				void removeContentLength();
				unsigned int getContentLength() const;

				void setTransferEncoding(String const& transferEncoding);
				String const& getTransferEncoding() const;

				virtual String toString() const = 0;
			};
		}
	}
}

#endif