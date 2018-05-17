#ifndef STDUTILS_NETWORKING_HTTP_WSMESSAGEBODY_H
#define STDUTILS_NETWORKING_HTTP_WSMESSAGEBODY_H


#include <StdUtils/Delegates.h>
#include <StdUtils/Networking/NetworkStream.h>
#include <StdUtils/Networking/Http/WebSocketVersions.h>
#include <StdUtils/Networking/Http/WSMessageTypes.h>

#include <private/Networking/Http/WSMessageHeader.h>


namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{
			template<WebSocketVersions::Value PROTOCOL>
			class WebSocket;
			class WSRFC6455Connection;

			class WSMessageBody
			{
				friend WebSocket<WebSocketVersions::RFC6455>;
				friend WSRFC6455Connection;

				STDUTILS_NOCOPY(WSMessageBody);

			protected:
				typedef Delegate<void, void> FailConnectionMethod;
				typedef IDelegate<void, void> IFailConnectionMethod;

			protected:
				NetworkStream& stream;
				bool sending;
				bool closed;

				WSMessageTypes type;
				uint64_t bodyLength;
				uint64_t transferredBytes;

				bool masked;
				char mask[4];

				FailConnectionMethod failConnection;

			protected:
				WSMessageBody(NetworkStream&, WSMessageHeader const&, IFailConnectionMethod const&, bool);

			public:
				virtual ~WSMessageBody();

			protected:
				void assertOpen();

				void reinitialize(WSMessageHeader const&); //For use by inheriting classes (see WSMultipartBody)

				void doSend(char const*, uint32_t);
				uint32_t doReceive(char*, uint32_t);
				void markClosed();

			public:
				virtual void write(char const*, uint32_t);
				virtual void write(char);

				virtual uint32_t read(char*, uint32_t bufferSize);
				virtual char read();

				virtual void close();
				virtual void close(bool discardRemaining);
				virtual bool isClosed() const;

				WSMessageTypes::Value getMessageType() const;

				virtual bool knowsBodyLength() const;
				virtual uint64_t getBodyLength() const;

				virtual uint64_t getBytesToWrite() const;
				virtual bool isWriteCompleted() const;

				virtual uint64_t getBytesToRead() const;
				virtual bool isReadCompleted() const;
				
				virtual bool dataAvailable();
			};
		}
	}
}

#endif