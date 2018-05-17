#ifndef STDUTILS_NETWORKING_NETWORKSTREAM_H
#define STDUTILS_NETWORKING_NETWORKSTREAM_H

#include <StdUtils/Networking/TcpSocket.h>

namespace StdUtils
{
	namespace Networking
	{
		class NetworkStream
		{
		public:
			virtual ~NetworkStream();

		public:
			virtual void close() = 0;

			virtual bool isOpen() const = 0;
			virtual void refreshState() = 0;
			virtual bool refreshOpenState();

			virtual void write(char data) = 0;
			virtual void write(char const* data, uint32_t length) = 0;

			virtual char read() = 0;
			virtual uint32_t read(char* buffer, uint32_t bufferSize) = 0;

			//Throws only if the stream is not open to begin or basic IO errors occur
			//Otherwise it just returns false, ie on a closed on a closed connection or an otherwise changed status, that would have made calling this method wrong if known beforehand
			virtual bool dataAvailable() = 0;

			virtual Socket<SocketTypes::Any>* getSocket() const = 0;
		};
	}
}

#endif