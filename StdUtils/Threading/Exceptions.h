#include <StdUtils/Exceptions.h>

namespace StdUtils
{
	namespace Threading
	{
		class STDUTILS_DLLCONTENT BasicThreadException : public OSApiException
		{
		public:
			BasicThreadException(String const& message)
				:OSApiException(message)
			{
			}
			BasicThreadException()
			{
			}
		};
		class STDUTILS_DLLCONTENT TimeoutException : public Exception
		{
		public:
			TimeoutException()
				:Exception("Timeout occurred")
			{
			}
		};
		class STDUTILS_DLLCONTENT BasicMutexException : public OSApiException
		{
		public:
			BasicMutexException(String const& message)
				:OSApiException(message)
			{
			}
			BasicMutexException()
			{
			}
		};
	}
}