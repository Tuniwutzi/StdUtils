#ifndef STDUTILS_BASE64_H
#define STDUTILS_BASE64_H

#include <StdUtils/Base.h>
#include <vector>

namespace StdUtils
{
	namespace Encodings
	{
		class Base64
		{
		public:
			static String Encode(std::vector<char> const& in);
			static std::vector<char> Decode(String const& in);
		};
	}
}

#endif