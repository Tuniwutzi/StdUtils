#ifndef STDUTILS_NETWORKING_HTTP_WEBSOCKET_H
#define STDUTILS_NETWORKING_HTTP_WEBSOCKET_H

#include <StdUtils/Networking/Http/HttpServerContext.h>
#include <StdUtils/Networking/Http/HttpClientContext.h>
#include <StdUtils/Networking/Http/WebSocketVersions.h>
#include <StdUtils/Networking/Http/WSMessageTypes.h>
#include <StdUtils/Networking/Http/WSEndpoint.h>
#include <StdUtils/Networking/Http/WSMessageBodyWriter.h>
#include <StdUtils/Networking/Http/WSMessageBodyReader.h>

#include <vector>


namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{
			class WSRFC6455Connection;

			template<>
			class WebSocket<WebSocketVersions::RFC6455>
			{
				STDUTILS_NOCOPY(WebSocket);
				struct Server
				{
					TcpSocket* Socket;
					WSEndpoint Endpoint;

					Server();
					void reset();
				};

			private:
				int status;

				//Connection connection;
				WSRFC6455Connection& connection;
				Server server;

			public:
				explicit WebSocket();
				~WebSocket();

			private:
				String digestKey(String const&) const;

				//checks and sets connection->CloseSent
				//checks for a sending WSMessageBody
				void sendClose(char* = NULL, uint32_t = 0);

			public:
				//Equals a complete reset
				//No exceptions, unless an underlying stream cannot be closed
				//Is affected by stream ownership
				void close();


				void connect(HttpClientContext&);
				void connect(WSEndpoint const&);
				void connect(HttpServerContext&); //TODO?: accept would be a more fitting name, but usually accept needs a socket that is bound and listening so for now I will keep connect

				//Throws, if WebSocket is bound, listening or closed
				//Throws an exception if preventDirty is true and a body is currently sending/receiving or a disconnect request has not been received
				//If connected:
				//Closes the underlying stream. Will try to send disconnect response first, if a disconnect request has already been received.
				//Is not affected by stream ownership
				void disconnect(bool preventDirty = true);

				//Throws, if the last known state is anything other than connected or if the request could not be sent (possibly due to a closed connection)
				//Throws, if a disconnect request has already been sent or received
				//Throws, if there is a sending WSMessageBody that is open
				//If the peer responds to the request, the connection is closed as soon as that response is received. No need to call disconnect yourself.
				//It is impossible to send any more data once you have requested a disconnect.
				//The peer can still send data to you, until he decides to respond to your disconnect request.
				void requestDisconnect(uint16_t statusCode, StdUtils::String const& reason);
				void requestDisconnect();


				void bind(WSEndpoint const&);
				void listen(int backlog = 50);

				void accept(WebSocket&);
				WebSocket* accept();


				WSMessageBodyReader receive();
				bool tryReceive(WSMessageBodyReader*);
				WSMessageBodyReader getCurrentReader();

				WSMessageBodyWriter send(uint64_t bodyLength, WSMessageTypes::Value type = WSMessageTypes::Text);
				WSMessageBodyWriter send(WSMessageTypes::Value type = WSMessageTypes::Text);
				bool trySend(WSMessageTypes::Value type, WSMessageBodyWriter*);
				bool trySend(uint64_t bodyLength, WSMessageTypes::Value type, WSMessageBodyWriter*);
				WSMessageBodyWriter getCurrentWriter();




				bool isClosed() const;
				bool isConnected() const;
				//Returns true only if the WebSocket is still connected, but a disconnect request has been sent or received
				bool isDisconnecting() const;
				bool isDisconnected() const;
				bool isBound() const;
				bool isListening() const;

				bool isClean() const;

				//Once the disconnect request has been sent, no more data can be sent from this instance
				bool disconnectRequestSent() const;
				//Once the disconnect request has been received, no more data can be received from this instance
				bool disconnectRequestReceived() const;

				//TODO?: Think about adding canReceive() const/canSend() const. They may add ambiguity and obfuscate the disconnect handshake though


				//Will try to receive the next message without blocking to see whether a close request/response has been received.
				void refreshState();
				//Calls refreshState and returns a boolean indicating wether or not the WebSocket is connected after refreshing
				bool checkConnection();

				//Throws if the last known status of the socket is anything other than connected
				//Other than that: does NOT throw, unless basic errors (unable to receive/invalid handle/socket errors/...) occur
				bool dataAvailable();


				//If the stream is disowned and close() is called, the stream will not be deleted by this socket
				//Retrieve the stream using getStream()
				void ownStream();
				void disownStream();

				NetworkStream* getStream() const;
			};
		}
	}
}

#endif