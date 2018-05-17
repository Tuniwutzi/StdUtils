#ifndef STDUTILS_NETWORKING_HTTP_WSMESSAGEBODYWRITER_H
#define STDUTILS_NETWORKING_HTTP_WSMESSAGEBODYWRITER_H


#include <StdUtils/Base.h>
#include <StdUtils/SharedPointer.h>
#include <StdUtils/Networking/Http/WebSocketVersions.h>
#include <StdUtils/Networking/Http/WSMessageTypes.h>


namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{
			class WSMessageBody;
			template<WebSocketVersions::Value>
			class WebSocket;

			class WSMessageBodyWriter
			{
				friend WebSocket<WebSocketVersions::RFC6455>;

			private:
				SharedPointer<WSMessageBody> ptr;
				WSMessageBody* bodyRaw; //Performance

			private:
				explicit WSMessageBodyWriter(SharedPointer<WSMessageBody> const&);

			public:
				WSMessageBodyWriter();

			private:
				void assertNotNull() const;

			public:
				void write(char const*, uint32_t);
				void write(char);

				void close();
				bool isClosed() const;

				WSMessageTypes::Value getMessageType() const;

				bool knowsBodyLength() const;
				uint64_t getBodyLength() const;

				uint64_t getBytesToWrite() const;
				bool isWriteCompleted() const;

				bool isNull() const;
			};
		}
	}
}

#endif