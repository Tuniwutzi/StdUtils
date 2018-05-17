#ifndef STDUTILS_JSONFORMATTER_H
#define STDUTILS_JSONFORMATTER_H

#include <StdUtils/Serialization/Formatter.h>

#include <StdUtils/DynamicList.h>
#include <StdUtils/DynamicMap.h>

namespace StdUtils
{
	namespace Serialization
	{
		namespace Json
		{
			class JsonFormatter : public StdUtils::Serialization::Formatter
			{
			public:
				struct DeserializeState
				{
					char ReadCommentCache; //Stores a char, if readChar has read a /, checked for a subsequent / or * to start a comment, but found an arbitrary char. This arbitrary char is stored in ReadCommentCache and returned, when getChar is next called
					char ReadCharCache; //Stores the first byte from the stream, when it has been read to determine the type
					bool SkipComments;
					bool SkipEmpty;

					DeserializeState();

					void skipAll();
					void skipNone();
				};
			private:
				//Wird benoetigt um Templates verspaetet zu instanzieren
				Dynamic parseDynamic(std::istream& in, DeserializeState& st) const;

				//Typen orientieren sich nicht an dynamic, sondern an Typen die in Json unterstuetzt sind
				void writeNull(std::ostream&) const;
				void writeInt(intmax_t, std::ostream&) const;
				void writeUInt(uintmax_t, std::ostream&) const;
				void writeFP(fpmax, std::ostream&) const;
				void writeBool(bool, std::ostream&) const;
				void writeString(String const&, std::ostream&) const;
				void writeList(DynamicList const&, std::ostream&) const;
				void writeMap(DynamicMap const&, std::ostream&) const;

				template<typename T>
				T parse(std::istream&, DeserializeState&) const;



			public:
				void serialize(Dynamic const&, std::ostream&) const STDUTILS_OVERRIDE;
				using Formatter::serialize;

				Dynamic deserialize(std::istream&) const STDUTILS_OVERRIDE;
				using Formatter::deserialize;
			};
		}
	}
}

#endif