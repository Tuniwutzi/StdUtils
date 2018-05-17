#ifndef STDUTILS_TCPSOCKET_H
#define STDUTILS_TCPSOCKET_H


#include <StdUtils/Networking/Socket.h>


namespace StdUtils
{
	namespace Networking
	{
		template<>
		class Socket<SocketTypes::Tcp> : public Socket<SocketTypes::Any>
		{
			STDUTILS_NOCOPY(Socket);

		private:
			int status;
			bool ownsHandle;

			Endpoint localEndpoint;
			Endpoint remoteEndpoint;

		public:
			explicit Socket();
			~Socket();

		private:
			void createHandle();
			void resetHandle();

			void assertConnected() const;

		public:
			//Equals a complete reset
			//No exceptions unless OS-functions return completely unexpected values
			//ie a handle cannot be destroyed
			//Is affected by handle ownership
			void close();


			void connect(Endpoint const& ep);
			void connect(IPAddress const& ip, unsigned short port);
			void connectWithHandle(Endpoint const&, SocketHandle);

			//Throws, if Socket is bound, listening or closed
			//Closes the handle
			//Is not affected by handle ownership
			void disconnect();


			void bind(Endpoint const& ep);
			void bind(unsigned short port = Endpoint::AnyPort, IPAddress const& ip = IPAddress::Any());
			void bindWithHandle(Endpoint const&, SocketHandle handle);

			void listen(int backlog = 50);

			void accept(Socket& buffer);
			Socket* accept();


			//Throws, if receive fails
			uint32_t receive(char* buffer, uint32_t bufferSize);
			//Throws, if send fails
			uint32_t send(char const* buffer, uint32_t bufferSize);
			//No methods for single chars. Not compatible with non-blocking sockets

			//Only throw if the Socket is closed, bound or listening. As in: if the call was clearly intended for a different object.
			//Otherwise just returns false on failure
			bool tryReceive(char* buffer, uint32_t bufferSize, uint32_t* receivedBytes);
			bool trySend(char const* buffer, uint32_t bufferSize, uint32_t* sentBytes);


			bool isClosed() const;
			bool isConnected() const;
			bool isDisconnected() const;
			//Returns true, if the socket is bound or listening
			bool isBound() const;
			bool isListening() const;

			void refreshState();
			bool checkConnection();


			//Throws only if the last known status of the socket is not connected
			//In other words: Throws only on invalid call by library user
			uint32_t getReceivableBytes();
			//Throws only if the last known status of the socket is not connected
			//In other words: Throws only on invalid call by library user
			bool dataAvailable();

			Endpoint const& getRemoteEndpoint() const;
			Endpoint const& getLocalEndpoint() const;

			//If the handle is disowned, it will not be closed by any cleanup performed in this socket (ie close())
			//An explicit request to disconnect will still close the handle, as it is the only way to close the connection
			void ownHandle();
			void disownHandle();

		public:
			static SocketHandle CreateHandle();
		};

		typedef Socket<SocketTypes::Tcp> TcpSocket;
	}
}

#endif
