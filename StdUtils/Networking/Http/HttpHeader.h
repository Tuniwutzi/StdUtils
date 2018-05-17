#ifndef STDUTILS_HTTPHEADER_H
#define STDUTILS_HTTPHEADER_H

#include <StdUtils/Base.h>
#include <StdUtils/Nullable.h>

namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{
			class HttpHeader
			{
			private:
				String name;
				Nullable<String> value;

			public:
				explicit HttpHeader(String const& headerString);
				explicit HttpHeader(String const& name, Nullable<String> const& value);

			public:
				String const& getName() const;
				Nullable<String> const& getValue() const;
				Nullable<String>& getValue();

				void setValue(Nullable<String> const& newValue);

				String toString() const;
			};
		}
	}
}

#endif