#include <StdUtils/Networking/NetworkStream.h>

namespace StdUtils
{
	namespace Networking
	{
		NetworkStream::~NetworkStream()
		{
		}

		bool NetworkStream::refreshOpenState()
		{
			this->refreshState();
			return this->isOpen();
		}
	}
}