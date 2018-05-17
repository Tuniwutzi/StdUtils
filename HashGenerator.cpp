#include <StdUtils/HashGenerator.h>

#include <private/SHA1Generator.h>

namespace StdUtils
{
	HashGenerator& HashGenerator::SHA1()
	{
		static SHA1Generator gen;
		return gen;
	}
}