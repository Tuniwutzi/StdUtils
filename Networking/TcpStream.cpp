#include <StdUtils/Networking/TcpStream.h>

namespace StdUtils
{
	namespace Networking
	{
		TcpStream::TcpStream(TcpSocket* s, bool ownsSocket)
			:SocketStream(ownsSocket),
			socket(s)
		{
		}
		TcpStream::~TcpStream()
		{
			if (this->ownsSocket)
				delete this->socket;
		}

		void TcpStream::close() STDUTILS_OVERRIDE
		{
			this->socket->close();
		}

		bool TcpStream::isOpen() const STDUTILS_OVERRIDE
		{
			return this->socket->isConnected();
		}
		void TcpStream::refreshState() STDUTILS_OVERRIDE
		{
			this->socket->refreshState();
		}

		void TcpStream::write(char data) STDUTILS_OVERRIDE
		{
			this->socket->send(&data, 1);
		}
		void TcpStream::write(char const* data, uint32_t length) STDUTILS_OVERRIDE
		{
			this->socket->send(data, length);
		}

		char TcpStream::read() STDUTILS_OVERRIDE
		{
			char rv;
			this->socket->receive(&rv, 1);
			return rv;
		}
		uint32_t TcpStream::read(char* buffer, uint32_t bufferSize) STDUTILS_OVERRIDE
		{
			return this->socket->receive(buffer, bufferSize);
		}

		bool TcpStream::dataAvailable() STDUTILS_OVERRIDE
		{
			return this->socket->dataAvailable();
		}

		TcpSocket* TcpStream::getSocket() const STDUTILS_OVERRIDE
		{
			return this->socket;
		}
	}
}