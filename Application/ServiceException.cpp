#include <StdUtils/Application/ServiceException.h>

namespace StdUtils
{
	namespace Application
	{
		ServiceException::ServiceException(String const& message, ServiceException::ServiceExceptionTypes::Value type)
			:Exception(message),
			type(type)
		{
		}
		ServiceException::~ServiceException() throw ()
		{
		}

		ServiceException::ServiceExceptionTypes::Value ServiceException::getType() const
		{
			return this->type;
		}
	}
}