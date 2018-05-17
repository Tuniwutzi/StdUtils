#include <StdUtils/Networking/SocketStream.h>

namespace StdUtils
{
	namespace Networking
	{	
		SocketStream::SocketStream(bool ownsSocket)
			:ownsSocket(ownsSocket)
		{
		}
		SocketStream::~SocketStream()
		{
		}

		void SocketStream::ownSocket()
		{
			this->ownsSocket = true;
		}
		void SocketStream::disownSocket()
		{
			this->ownsSocket = false;
		}
	}
}