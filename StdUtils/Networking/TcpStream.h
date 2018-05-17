#ifndef STDUTILS_NETWORKING_TCPSTREAM_H
#define STDUTILS_NETWORKING_TCPSTREAM_H

#include <StdUtils/Networking/SocketStream.h>

namespace StdUtils
{
	namespace Networking
	{
		class TcpStream : public SocketStream
		{
			STDUTILS_NOCOPY(TcpStream);

		protected:
			TcpSocket* socket;

		public:
			TcpStream(TcpSocket*, bool ownsSocket = true);
			virtual ~TcpStream();

		public:
			virtual void close() STDUTILS_OVERRIDE;

			virtual bool isOpen() const STDUTILS_OVERRIDE;
			virtual void refreshState() STDUTILS_OVERRIDE;

			virtual void write(char data) STDUTILS_OVERRIDE;
			virtual void write(char const* data, uint32_t length) STDUTILS_OVERRIDE;

			virtual char read() STDUTILS_OVERRIDE;
			virtual uint32_t read(char* buffer, uint32_t bufferSize) STDUTILS_OVERRIDE;

			virtual bool dataAvailable() STDUTILS_OVERRIDE;

			virtual TcpSocket* getSocket() const STDUTILS_OVERRIDE;
		};
	}
}

#endif