#include <StdUtils/Encodings/Base64.h>
#include <StdUtils/Exceptions.h>

using namespace std;

namespace StdUtils
{
	namespace Encodings
	{
		static const char base64_chars[] = 
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz"
			"0123456789+/";
		static char getCharValue(char b64Char)
		{
			for (char i = 0; i < sizeof(base64_chars) - 1; i++)
			{
				if (base64_chars[i] == b64Char)
					return i;
			}
			return -1;
		}
		static char getBase64Char(unsigned char currentChar, unsigned char lastChar, int overlapBits)
		{
			unsigned char c = currentChar >> (2 + overlapBits);

			c = c | (lastChar << (6 - overlapBits));
			c = c & 0x3F;

			return base64_chars[c];
		}
		static vector<char>::size_type getNeededBase64BufferLength(vector<char>::size_type datalength)
		{
			return (datalength + 2 - ((datalength + 2) % 3)) / 3 * 4;
		}


		String Base64::Encode(vector<char> const& in)
		{
			unsigned char const* toEncode = (unsigned char const*)in.data();
			vector<char>::size_type length = in.size();
			vector<char>::size_type size = getNeededBase64BufferLength(in.size());
			char* buffer = new char[size + 1];
			buffer[size] = 0;

			int overlapBits = 0;

			char* bufferPosition = buffer;
			for (unsigned char const* pos = toEncode; (pos - toEncode) < (decltype(pos-toEncode))length; pos++)
			{
				if (overlapBits == 6)
				{
					(*bufferPosition) = base64_chars[(*(pos - 1)) & 0x3F];
					bufferPosition++;
					overlapBits = 0;
				}

				char c = 0;
				if (pos > toEncode)
				{
					c = *(pos - 1);
				}
				(*bufferPosition) = getBase64Char(*pos, c, overlapBits);

				overlapBits = 8 - (6 - overlapBits);
				bufferPosition++;
			}

			unsigned char const * lastChar = toEncode + (length - 1);
			while (overlapBits > 0)
			{
				if (overlapBits == 6)
				{
					if (lastChar != 0)
					{
						(*bufferPosition) = base64_chars[(*lastChar) & 0x3F];
					}
					else
					{
						(*bufferPosition) = '=';
					}
					overlapBits = 0;
				}
				else
				{
					if (lastChar != 0)
					{
						(*bufferPosition) = getBase64Char(0, (*lastChar), overlapBits);
					}
					else
					{
						(*bufferPosition) = '=';
					}
					overlapBits = 8 - (6 - overlapBits);
				}
				lastChar = 0;
				bufferPosition++;
			}

			String returnvalue(buffer);
			delete[] buffer;
			return returnvalue;
		}

		vector<char> Base64::Decode(String const& in)
		{
			if ((in.size() % 4) == 0)
			{
				//leerzeichen und zeilenumbrücke ignorieren
				vector<char> out;

				char const* rawIn = in.data();
				for (String::size_type i = 0; i < in.size(); i += 4)
				{
					char in1 = rawIn[i + 0];
					char in2 = rawIn[i + 1];
					char in3 = rawIn[i + 2];
					char in4 = rawIn[i + 3];

					bool err = false;
					char out1 = 0;
					char out2 = 0;
					char out3 = 0;

					char in1Val = getCharValue(in1);
					if (in1Val > -1)
					{
						out1 += ((unsigned char)in1Val << 2);

						char in2Val = getCharValue(in2);
						if (in2Val > -1)
						{
							out1 += ((unsigned char)in2Val >> 4);
							out.push_back(out1);

							char in3Val = getCharValue(in3);
							if (in3Val > -1)
							{
								out2 += ((unsigned char)in2Val << 4);
								out2 += ((unsigned char)in3Val >> 2);
								out.push_back(out2);

								char in4Val = getCharValue(in4);
								if (in4Val > -1)
								{
									out3 += ((unsigned char)in3Val << 6);
									out3 += (in4Val);
									out.push_back(out3);
								}
								else if (in4 != '=')
									err = true;
							}
							else if (in3 != '=')
								err = true;
						}
					}

					if (err)
						throw Exception("Invalid Base64 string");
				}

				return out;
			}
			else
				throw Exception("Base64 string has invalid length");
		}
	}
}