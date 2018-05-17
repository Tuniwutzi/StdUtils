#ifndef STDUTILS_NETWORKING_SOCKETSTREAM_H
#define STDUTILS_NETWORKING_SOCKETSTREAM_H

#include <StdUtils/Networking/NetworkStream.h>

namespace StdUtils
{
	namespace Networking
	{
		class SocketStream : public NetworkStream
		{
		protected:
			bool ownsSocket;

		protected:
			SocketStream(bool ownsSocket);
			virtual ~SocketStream();

		public:
			void ownSocket();
			void disownSocket();
		};
	}
}

#endif