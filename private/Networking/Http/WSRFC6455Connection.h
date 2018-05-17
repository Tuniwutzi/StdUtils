#ifndef STDUTILS_PRIVATE_NETWORKING_HTTP_WSRFC6455CONNECTION_H
#define STDUTILS_PRIVATE_NETWORKING_HTTP_WSRFC6455CONNECTION_H

#include <StdUtils/Networking/NetworkStream.h>
#include <StdUtils/SharedPointer.h>
#include <StdUtils/Networking/Http/WebSocket.h>

#include <private/Networking/Http/WSMessageBody.h>
#include <private/Networking/Http/WSMessageHeader.h>


namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{
			struct WSRFC6455UnparsedMessage
			{
				char Content[14];
				uint8_t Length;

				WSRFC6455UnparsedMessage();
				void reset();
			};
			class WSRFC6455Connection
			{
				STDUTILS_NOCOPY(WSRFC6455Connection);

			public:
				WebSocket<WebSocketVersions::RFC6455>& Socket;

				StdUtils::Networking::NetworkStream* Stream;
				bool OwnsStream;
				bool ServerSide;

				bool CloseReceived;
				bool CloseSent;

				WSMessageHeader NextReceived;
				SharedPointer<StdUtils::Networking::Http::WSMessageBody> LastReceived;
				SharedPointer<StdUtils::Networking::Http::WSMessageBody> LastSent;

				WSRFC6455UnparsedMessage ReceivedHeaderPart;

			public:
				WSRFC6455Connection(WebSocket<WebSocketVersions::RFC6455>&);

			private:
				void failConnection();

				void sendNextPart(WSMessageHeader const&);

			public:
				void reset();


				void sendHeaders(WSMessageHeader& h);


				WSMessageHeader receiveMessageHeader(bool nonBlocking);

				bool processIfControlFrame(WSMessageHeader& h);

				void bufferFrame(WSMessageHeader& h);

				void processContentFrame(WSMessageHeader& h);
			};
		}
	}
}

#endif