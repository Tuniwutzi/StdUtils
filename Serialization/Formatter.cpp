#include <StdUtils/Serialization/Formatter.h>


#include <sstream>


using namespace std;


namespace StdUtils
{
	namespace Serialization
	{
		Dynamic Formatter::deserialize(String const& str) const
		{
			std::stringstream ss;
			ss << str;
			return this->deserialize(ss);
		}

		String Formatter::serialize(Dynamic const& val) const
		{
			std::stringstream ss;
			this->serialize(val, ss);
			return ss.str();
		}
	}
}