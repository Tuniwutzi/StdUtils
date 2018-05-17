#include <StdUtils/Base.h>

namespace StdUtils
{
	class STDUTILS_DLLCONTENT NumberFormat
	{
	public:
		static String ToHex(unsigned char c);
		static String ToHex(unsigned char const* c, int count, bool insertSpace = false);
		static String ToHex(signed char c);
		static String ToHex(signed char const* c, int count, bool insertSpace = true); //Insert space hier true, da negative hex-Zahlen sonst irgendwo dazwischen auftauchen
		static String ToHex(char c, bool sign = true);
		static String ToHex(char const* c, int count, bool sign = true, bool insertSpace = true); //Insert space hier true, da negative hex-Zahlen sonst irgendwo dazwischen auftauchen
		static String ToHex(uint64_t number);
		static String ToHex(int64_t number);
		static String ToHex(uint32_t number);
		static String ToHex(int32_t number);
	};
}