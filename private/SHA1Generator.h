#ifndef UTIL_SHA1GENERATOR_H
#define UTIL_SHA1GENERATOR_H

#include <StdUtils/HashGenerator.h>

namespace StdUtils
{
	class STDUTILS_DLLCONTENT SHA1Generator : public HashGenerator
	{
	public:
		SHA1Generator();

	public:
		std::vector<char> generate(std::vector<char> const&) const;
		std::vector<char> generate(char const*, unsigned int) const;
		std::vector<char> generate(String const&) const;

		String generateChecksum(std::vector<char> const&, ChecksumFormats::Value type = ChecksumFormats::Hex) const;
		String generateChecksum(char const*, unsigned int, ChecksumFormats::Value type = ChecksumFormats::Hex) const;
		String generateChecksum(String const&, ChecksumFormats::Value type = ChecksumFormats::Hex) const;
	};
}

#endif