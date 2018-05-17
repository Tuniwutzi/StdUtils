#ifndef UTIL_HASHGENERATOR_H
#define UTIL_HASHGENERATOR_H

#include <StdUtils/Base.h>

#include <vector>

namespace StdUtils
{
	class STDUTILS_DLLCONTENT HashGenerator
	{
	public:
		STDUTILS_START_ENUM(ChecksumFormats)
		{
			Hex
		}
		STDUTILS_END_ENUM(ChecksumFormats);

	protected:
		HashGenerator() {}

	public:
		virtual ~HashGenerator() {}

	public:
		virtual std::vector<char> generate(std::vector<char> const&) const = 0;
		virtual std::vector<char> generate(char const*, unsigned int) const = 0;
		virtual std::vector<char> generate(String const&) const = 0;

		virtual String generateChecksum(std::vector<char> const&, ChecksumFormats::Value type = ChecksumFormats::Hex) const = 0;
		virtual String generateChecksum(char const*, unsigned int, ChecksumFormats::Value type = ChecksumFormats::Hex) const = 0;
		virtual String generateChecksum(String const&, ChecksumFormats::Value type = ChecksumFormats::Hex) const = 0;

	public:
		static HashGenerator& SHA1();
	};
}

#endif