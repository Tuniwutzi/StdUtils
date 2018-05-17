#ifndef STDUTILS_STRINGHELPERS_H
#define STDUTILS_STRINGHELPERS_H

#include <StdUtils/Base.h>
#include <StdUtils/Nullable.h>

#include <sstream>
#include <locale>
#include <vector>

namespace StdUtils
{
	class StringHelpers
	{
	public:
		template<typename T>
		static inline String ToString(T data)
		{
			std::stringstream ss;
			ss << data;
			return ss.str();
		}
		static inline String ToString(std::exception const& ex)
		{
			return ex.what();
		}
		template<typename T>
		static inline String ToString(Nullable<T> nb, String const& nullValue = "")
		{
			if (!nb.hasValue())
				return nullValue;
			else
				return StringHelpers::ToString<T>(nb.getValue());
		}
		static inline String ToString(char const* bytes, int length)
		{
			String rv;
			rv.reserve(length);

			char buf[512];
			int offset = 0;
			while (length > 0)
			{
				int i(0);
				for (;i < sizeof(buf) - 1 && i < length; i++)
					buf[i] = bytes[i + offset];
				buf[i] = 0;

				rv += buf;
				length -= i;
				offset += i;
			}

			return rv;
		}

		template<typename T>
		static inline T Parse(String const& str)
		{
			T out;

			std::istringstream ss(str);
			ss >> out;
			//ss.fail if no T, !ss.eof() if invalid chars after T
			if (ss.fail() || !ss.eof())
				throw Exception(str + String(" could not be parsed"));

			return out;
		}
		//C++11:
		//template<>
		//static inline double Parse<double>(String const& str)
		//{
		//	return stod(str);
		//}

		static inline String Trim(String const& str)
		{
			char const* start = str.data();
			char const* end = str.data() + str.length() - 1;
			while (start < end && start[0] == ' ')
				start++;
			while (start < end && end[0] == ' ')
				end--;

			return str.substr(start - str.data(), (end - start) + 1);
		}

		static inline bool Equals(String const& a, String const& b, bool caseSensitive = true)
		{
			if (caseSensitive)
				return a == b;
			else
			{
				if (a.length() == b.length())
				{
					char const* ad(a.data());
					char const* bd(b.data());

					for (String::size_type i = 0; i < a.length(); i++)
					{
						if (tolower(ad[i]) != tolower(bd[i]))
							return false;
					}
					return true;
				}
				else
					return false;
			}
		}

		static inline String ToLower(String const& str)
		{
			String rv;
			rv.reserve(str.length());

			char const* data(str.data());
			for (String::size_type i = 0; i < str.length(); i++)
				rv += tolower(data[i]);

			return rv;
		}

		static inline String Replace(String const& base, String const& toReplace, String const& with, bool replaceAll = true)
		{
			String::size_type offset = 0;
			String::size_type index;

			String rv;

			while ((index = base.find(toReplace, offset)) != base.npos)
			{
				rv += base.substr(offset, index - offset);
				rv += with;
				offset = index + toReplace.size();

				if (!replaceAll)
					break;
			}

			return rv + base.substr(offset);
		}

		static inline std::vector<String> Split(String const& base, String const& pattern, bool removeEmptyEntries = false)
		{
			std::vector<String> rv;

			bool keepEmpty = !removeEmptyEntries;

			String tmp(base);
			String::size_type idx;
			while ((idx = tmp.find(pattern)) != tmp.npos)
			{
				if (keepEmpty || idx != 0)
					rv.push_back(tmp.substr(0, idx));
				tmp = tmp.substr(idx + pattern.size());
			}
			if (keepEmpty || tmp.size() > 0)
				rv.push_back(tmp);

			return rv;
		}
	};
	template<>
	inline String StringHelpers::ToString<String const&>(String const& data)
	{
		return data;
	}
}

#endif