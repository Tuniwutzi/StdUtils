#ifndef STDUTILS_SERVICEEXCEPTION_H
#define STDUTILS_SERVICEEXCEPTION_H

#include <StdUtils/Exceptions.h>

namespace StdUtils
{
	namespace Application
	{
		class ServiceException : public Exception
		{
		public:
			STDUTILS_START_ENUM(ServiceExceptionTypes)
			{
				ArgumentError,
					APIError,
					CallError
			}
			STDUTILS_END_ENUM(ServiceExceptionTypes);

		private:
			ServiceExceptionTypes::Value type;

		public:
			ServiceException(String const& message, ServiceExceptionTypes::Value type = ServiceExceptionTypes::APIError);
			~ServiceException() throw ();

		public:
			ServiceExceptionTypes::Value getType() const;
		};
	}
}

#endif