#include <StdUtils/Serialization/Json/JsonFormatter.h>


#include <StdUtils/Dynamic.h>
#include <StdUtils/Exceptions.h>

#include <sstream>


using namespace std;


namespace StdUtils
{
	namespace Serialization
	{
		namespace Json
		{
			JsonFormatter::DeserializeState::DeserializeState()
				:ReadCommentCache('\0'),
				ReadCharCache('\0'),
				SkipComments(true),
				SkipEmpty(true)
			{
			}

			void JsonFormatter::DeserializeState::skipAll()
			{
				this->SkipComments = true;
				this->SkipEmpty = true;
			}
			void JsonFormatter::DeserializeState::skipNone()
			{
				this->SkipComments = false;
				this->SkipEmpty = false;
			}

			static String escape(String const& str)
			{
				return String("\"") + str + "\"";
			}

			void JsonFormatter::writeNull(std::ostream& to) const
			{
				to << "null";
			}
			void JsonFormatter::writeInt(intmax_t val, std::ostream& to) const
			{
				to << val;
			}
			void JsonFormatter::writeUInt(uintmax_t val, std::ostream& to) const
			{
				to << val;
			}
			void JsonFormatter::writeFP(fpmax val, std::ostream& to) const
			{
				to << val;
			}
			void JsonFormatter::writeBool(bool val, std::ostream& to) const
			{
				to << (val ? "true" : "false");
			}
			void JsonFormatter::writeString(String const& val, std::ostream& to) const
			{
				to << escape(val);
			}
			void JsonFormatter::writeList(DynamicList const& list, std::ostream& to) const
			{
				to << "[";
				bool first = true;
				for (DynamicList::const_iterator it = list.begin(); it != list.end(); it++)
				{
					if (first)
						first = false;
					else
						to << ",";
					this->serialize(**it, to);
				}
				to << "]";
			}
			void JsonFormatter::writeMap(DynamicMap const& map, std::ostream& to) const
			{
				to << "{";
				bool first = true;
				for (DynamicMap::const_iterator it = map.begin(); it != map.end(); it++)
				{
					if (first)
						first = false;
					else
						to << ",";
					to << escape(it->first);
					to << ":";
					this->serialize(*it->second, to);
				}
				to << "}";
			}
			void JsonFormatter::serialize(Dynamic const& obj, ostream& to) const
			{
				if (obj.isNull())
					this->writeNull(to);
				else if (obj.isNumeric() && obj.getType() != DynamicTypes::Char)
				{
					if (obj.isFloatingPoint())
						this->writeFP(obj.asDouble(), to);
					else if (obj.isSigned())
						this->writeInt(obj.asInteger(), to);
					else
						this->writeUInt(obj.asUnsignedInteger(), to);
				}
				else
				{
					switch (obj.getType())
					{
					case DynamicTypes::Char:
						this->writeString(String() + obj.getValue<char>(), to);
						break;
					case DynamicTypes::Boolean:
						this->writeBool(obj.getValue<bool>(), to);
						break;
					case DynamicTypes::String:
						this->writeString(obj.getValue<String>(), to);
						break;
					case DynamicTypes::List:
						this->writeList(obj.getValue<DynamicList>(), to);
						break;
					case DynamicTypes::Map:
						this->writeMap(obj.getValue<DynamicMap>(), to);
						break;
					default:
						throw Exception("Type can not be formatted in json");
						break;
					}
				}
			}

			static char getChar(istream& in, JsonFormatter::DeserializeState& st)
			{
				char read;
				if (st.ReadCharCache != '\0')
				{
					read = st.ReadCharCache;
					st.ReadCharCache = '\0';
				}
				else if (st.ReadCommentCache != '\0')
				{
					read = st.ReadCommentCache;
					st.ReadCommentCache = '\0';
				}
				else
				{
					read = in.get();
					if (in.eof())
						throw Exception("end of stream reached");
				}
				return read;
			}
			static char readChar(istream& in, JsonFormatter::DeserializeState& st)
			{
				char read = getChar(in, st);

				if (st.SkipComments && read == '/')
				{
					char second;
					try
					{
						second = getChar(in, st);
					}
					catch (...)
					{
						return read; //I cannot be sure if this is supposed to be a comment. maybe it is a valid end to the json data. therefore, just return the char that could be read
					}

					if (second == '/')
					{
						while (in.get() != '\n') {}
						return readChar(in, st);
					}
					else if (second == '*')
					{
						bool inComment = true;
						while (inComment)
						{
							while (in.get() != '*') {}
							if (in.get() == '/')
								inComment = false;
						}
						return readChar(in, st);
					}
					else
						st.ReadCommentCache = second;
				}
				else if (st.SkipEmpty && (read == ' ' || read == '\t' || read == '\r' || read == '\n'))
				{
					return readChar(in, st);
				}

				return read;
			}
			static bool isDigit(char c)
			{
				return c >= '0' && c <= '9';
			}
			template<>
			uintmax_t JsonFormatter::parse(istream& in, DeserializeState& st) const
			{
				stringstream ss;

				st.skipAll();

				char c = readChar(in, st);
				if (!isDigit(c))
					throw Exception("Invalid format for ulong");

				ss << c;
				while (isDigit((c = readChar(in, st))))
					ss << c;
				st.ReadCharCache = c;

				uint64_t rv;
				ss >> rv;
				return rv;
			}
			template<>
			intmax_t JsonFormatter::parse(istream& in, DeserializeState& st) const
			{
				stringstream ss;

				st.skipAll();

				char c = readChar(in, st);
				bool neg = c == '-';
				if (!neg)
					st.ReadCharCache = c;
				return parse<uint64_t>(in, st) * (neg ? -1 : 1);
			}
			template<>
			fpmax JsonFormatter::parse(istream& in, DeserializeState& st) const
			{
				stringstream ss;

				st.skipAll();

				char c = readChar(in, st);
				st.skipNone();

				bool neg = c == '-';
				if (neg || c == '+')
					c = readChar(in, st);

				if (!isDigit(c) && c != '.')
					throw Exception("Invalid format for double");
				else if (isDigit(c))
				{
					ss << c;
					while (isDigit((c = readChar(in, st))))
						ss << c;
				}

				if (c == '.')
				{
					ss << '.';
					while (isDigit((c = readChar(in, st))))
						ss << c;
				}
				st.ReadCharCache = c;

				double rv;
				ss >> rv;
				return rv * (neg ? -1 : 1);
			}
			template<>
			bool JsonFormatter::parse(istream& in, DeserializeState& st) const
			{
				st.skipAll();

				switch (readChar(in, st))
				{
				case 't':
					st.skipNone();
					if (readChar(in, st) == 'r' && readChar(in, st) == 'u' && readChar(in, st) == 'e')
						return true;
					break;
				case 'f':
					st.skipNone();
					if (readChar(in, st) == 'a' && readChar(in, st) == 'l' && readChar(in, st) == 's' && readChar(in, st) == 'e')
						return false;
					break;
				}
				throw Exception("Invalid format for boolean");
			}
			template<>
			String JsonFormatter::parse(istream& in, DeserializeState& st) const
			{
				st.skipAll();
				char c = readChar(in, st);
				if (c == '\"' || c == '\'')
				{
					char delim = c;
					st.skipNone();
					String value;

					bool escaped = false;

					while ((c = readChar(in, st)) != delim || escaped)
					{
						if (escaped)
						{
							switch (c)
							{
							case 'n':
								value += '\n';
								break;
							case 'r':
								value += '\r';
								break;
							case 't':
								value += '\t';
								break;
							case '\\':
								value += '\\';
								break;
							case '\'':
								value += '\'';
								break;
							case '\"':
								value += '\"';
								break;
							default:
								throw Exception("Unknown escape sequence in string");
								break;
							}
							escaped = false;
						}
						else if (c == '\\')
							escaped = true;
						else
							value += c;
					}

					return value;
				}
				else
					throw Exception("Invalid format for string");
			}
			template<>
			DynamicList JsonFormatter::parse(istream& in, DeserializeState& st) const
			{
				st.skipAll();

				if (readChar(in, st) == '[')
				{
					DynamicList rv;

					char c;
					do
					{
						if ((c = readChar(in, st)) == ']' && rv.size() == 0)
							return rv;

						st.ReadCharCache = c;
						rv.add(parseDynamic(in, st));

						st.skipAll();
					} while ((c = readChar(in, st)) == ',');

					if (c == ']')
						return rv;
					else
						throw Exception("Invalid format for json array");
				}
				else
					throw Exception("Invalid format for json array");
			}
			template<>
			DynamicMap JsonFormatter::parse(istream& in, DeserializeState& st) const
			{
				st.skipAll();

				if (readChar(in, st) == '{')
				{
					DynamicMap rv;

					char c;
					do
					{
					    char nameQuoteChar = '\0';
					    if ((c = readChar(in, st)) == '\"' || c == '\'')
                        {
                            nameQuoteChar = c;
                            c = readChar(in, st);
                        }

						if (c == '}' && rv.size() == 0) //rv.size() == 0, da sonst diese Schleife schonmal gelaufen ist, der letzte c also ',' war. Wenn nun c == '}' sieht der string also so aus: ",}"
							return rv;
						if ((c < 'a' || c > 'z') && (c < 'A' || c > 'Z'))
							throw Exception(String("Invalid first char for field name: ") + c);

						String name(1, c);

						st.skipNone();
						bool nameEnd = false;
						while ((c = readChar(in, st)) != ':' || nameQuoteChar != '\0')
						{
						    if (nameEnd)
                                throw Exception(String("Invalid char after end of name, expected ':': ") + c);
							else if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
								name += c;
                            else if (nameQuoteChar != '\0' && c == nameQuoteChar)
                            {
                                nameEnd = true;
                                nameQuoteChar = '\0';
                                st.skipAll();
                            }
							else
								throw Exception(String("Unexpected char in field name: ") + c);
						}

						if (name.size() <= 0)
							throw Exception("Expected non-empty field name");

						rv.set(name, parseDynamic(in, st));

						st.skipAll();
					} while ((c = readChar(in, st)) == ',');

					if (c == '}')
						return rv;
					else
						throw Exception(String("Expected ',' or '}': ") + c);
				}
				else
					throw Exception("Invalid format for json object");
			}
			template<>
			Dynamic JsonFormatter::parse(istream& in, DeserializeState& st) const
			{
				st.skipAll();
				char c = readChar(in, st);
				st.ReadCharCache = c;
				switch (c)
				{
				case '{':
					return Dynamic(parse<DynamicMap>(in, st));
					break;
				case '[':
					return Dynamic(parse<DynamicList>(in, st));
					break;
				case '\"':
				case '\'':
					return Dynamic(parse<String>(in, st));
					break;
				case 't':
				case 'f':
					return Dynamic(parse<bool>(in, st));
					break;
				case 'n':
					{
						st.skipNone();

						if (readChar(in, st) == 'n' && readChar(in, st) == 'u' && readChar(in, st) == 'l' && readChar(in, st) == 'l')
							return Dynamic();
						else
							throw Exception("Invalid format");
					}
					break;
				case '-':
				case '+':
					return Dynamic(parse<fpmax>(in, st));
					break;
				default:
					if (c >= '0' && c <= '9')
						return Dynamic(parse<fpmax>(in, st));
					break;
				}

				throw Exception(String("Invalid char '") + c + "'");
			}
			Dynamic JsonFormatter::parseDynamic(istream& in, DeserializeState& st) const
			{
				return this->parse<Dynamic>(in, st);
			}

			//Must be below templated methods, because deserialize<Dynamic>(istream&) must be defined first
			Dynamic JsonFormatter::deserialize(istream& in) const
			{
				DeserializeState st;
				return this->parse<Dynamic>(in, st);
			}
		}
	}
}
