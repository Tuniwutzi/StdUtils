#ifndef STDUTILS_SERIALIZATION_FORMATTER_H
#define STDUTILS_SERIALIZATION_FORMATTER_H

#include <StdUtils/Dynamic.h>

#include <ostream>
#include <istream>

namespace StdUtils
{
	namespace Serialization
	{
		class Formatter
		{
		protected:
			Formatter() {}
			virtual ~Formatter() {}

		public:
			virtual void serialize(Dynamic const&, std::ostream&) const = 0;
			String serialize(Dynamic const&) const;
			template<typename T>
			void serialize(T const& val, std::ostream& str) const
			{
				this->serialize(Dynamic(val), str);
			}
			template<typename T>
			String serialize(T const& val) const
			{
				return this->serialize(Dynamic(val));
			}

			virtual Dynamic deserialize(std::istream&) const = 0;
			Dynamic deserialize(String const&) const;
			template<typename T>
			T deserialize(std::istream& str) const
			{
				return this->deserialize(str).getValue<T>();
			}
			template<typename T>
			T deserialize(String const& str) const
			{
				return this->deserialize(str).getValue<T>();
			}
		};
	}
}

#endif