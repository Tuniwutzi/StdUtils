#ifndef STDUTILS_NETWORKING_HTTP_WSMESSAGEBODYREADER_H
#define STDUTILS_NETWORKING_HTTP_WSMESSAGEBODYREADER_H


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

			class WSMessageBodyReader
			{
				friend WebSocket<WebSocketVersions::RFC6455>;

			private:
				SharedPointer<WSMessageBody> ptr;
				WSMessageBody* bodyRaw; //Performance

			private:
				explicit WSMessageBodyReader(SharedPointer<WSMessageBody> const&);

			public:
				WSMessageBodyReader();

			private:
				void assertNotNull() const;
				
			public:
				uint32_t read(char*, uint32_t bufferSize);
				char read();
				
				void close(bool discardUnreadBytes = false);
				bool isClosed() const;

				WSMessageTypes::Value getMessageType() const;

				bool knowsBodyLength() const;
				uint64_t getBodyLength() const;
				
				uint64_t getBytesToRead() const;
				bool isReadCompleted() const;

				bool dataAvailable();

				bool isNull() const;
			};
		}
	}
}

#endif