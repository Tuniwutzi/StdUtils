#ifndef STDUTILS_EXCEPTIONS
#define STDUTILS_EXCEPTIONS

#include <StdUtils/Base.h>

#include <errno.h>
#include <exception>
#include <stdexcept>

#define STDUTILS_ASSERT_NOTNULL(variable, variableName) if (variable == NULL) throw NullPointerException(String("\"") + variableName + "\" may not be null.");
#define STDUTILS_ASSERT_NOTNULL_MESSAGE(variable, message) if (variable == NULL) throw NullPointerException(message);
#define STDUTILS_ASSERT_INBOUNDS(variable, min, max) if (variable < min || variable > max) { throw OutOfBoundsException(variable, min, max); }
#define STDUTILS_ASSERT_INBOUNDS_MSG(variable, min, max, message) if (variable < min || variable > max) { Util::String tmp(message); throw OutOfBoundsException(variable, min, max, tmp); }


namespace StdUtils
{
   class STDUTILS_DLLCONTENT Exception : public std::runtime_error
   {
   private:
      Exception const* innerException;

   public:
      Exception(String const& message)
          :std::runtime_error(message)
      {
      }
      virtual ~Exception() throw () //wird von gcc erzwungen
      {
      }
   };

	//class STDUTILS_DLLCONTENT StdException : public Exception
	//{
	//private:
	//	std::exception stdException;

	//public:
	//	StdException(std::exception const& ex)
	//		:Exception(ex.what())
	//	{
	//	}

	//	std::exception const& GetOriginal() const
	//	{
	//		return this->stdException;
	//	}
	//};

   //class STDUTILS_DLLCONTENT InvalidFormatException : public Exception
   //{
   //public:
   //   InvalidFormatException(String const& message)
   //      :Exception(message)
   //   {
   //   }

   //   InvalidFormatException(String value, int faultPosition)
   //       :Exception(String("Invalid format in ") + value + ", position " + StringHelpers::ToString(faultPosition))
   //   {
   //   }
   //};

   //class STDUTILS_DLLCONTENT ParseException : public Exception
   //{
   //public:
   //   ParseException(String const& message)
   //      :Exception(message)
   //   {
   //   }
   //   ParseException(String const& message, Exception const& innerException)
   //      :Exception(message, &innerException)
   //   {
   //   }
   //   ParseException(Exception const& innerException)
   //      :Exception("Parse failed, see inner exception for details", &innerException)
   //   {
   //   }

   //   virtual ~ParseException() throw ()
   //   {
   //   }
   //};

   //class STDUTILS_DLLCONTENT InvalidArgumentException : public Exception
   //{
   //private:
   //   String variableName;

   //public:
   //   InvalidArgumentException(String const& variableName, String const* message = NULL, Exception const* innerException = NULL)
   //      :Exception(message ? *message : String("Invalid argument: ") + variableName, innerException),
   //      variableName(variableName)
   //   {
   //   }

   //   virtual ~InvalidArgumentException() throw ()
   //   {
   //   }
   //};

   //class STDUTILS_DLLCONTENT InvalidOperationException : public Exception
   //{
   //public:
   //   InvalidOperationException(String const& message, Exception const* innerException = NULL)
   //      :Exception(message, innerException)
   //   {
   //   }
   //};

   //class STDUTILS_DLLCONTENT TimeoutException : public Exception
   //{
   //public:
   //   TimeoutException()
   //      :Exception("A timeout occurred")
   //   {
   //   }
   //};

   class STDUTILS_DLLCONTENT OSApiException : public Exception
   {
   public:
      typedef int ErrorCode;

   private:
      ErrorCode errorCode;

   public:
      OSApiException(String const& message, ErrorCode error = errno);

      OSApiException(ErrorCode error = errno);

      ErrorCode getErrorCode() const;
   };
}

#endif
