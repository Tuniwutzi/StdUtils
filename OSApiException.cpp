#include <StdUtils/Exceptions.h>


#include <StdUtils/StringHelpers.h>





namespace StdUtils
{
	OSApiException::OSApiException(String const& message, ErrorCode error)
		:Exception(message + " (Error code: " + StringHelpers::ToString(error) + ")"),
		errorCode(error)
	{
	}

	OSApiException::OSApiException(ErrorCode error)
		:Exception(String("OS Api Errorcode: ") + StringHelpers::ToString(error)),
		errorCode(error)
	{
	}

	OSApiException::ErrorCode OSApiException::getErrorCode() const
	{
		return this->errorCode;
	}
}