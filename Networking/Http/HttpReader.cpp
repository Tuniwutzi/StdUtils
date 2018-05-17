#include <private/Networking/Http/HttpReader.h>


#include <StdUtils/StringHelpers.h>


using namespace StdUtils;
using namespace StdUtils::Networking;
using namespace StdUtils::Networking::Http;


static String readLine(NetworkStream& s)
{
	static const uint64_t MaxLength = 4096;

	char line[MaxLength+1];
	int i = 0;
	bool r = false;

	while (i <= MaxLength)
	{
		char bt = s.read(); //TODO?: Effizienter. Koennte zumindest 2 Zeichen auslesen, solange das letzten kein \r war
		if (bt == '\r')
		{
			if (r)
				line[i++] = bt;
			else
				r = true;
		}
		else
		{
			r = false;

			if (bt == '\n')
				return StringHelpers::ToString(line, i);
			else
				line[i++] = bt;
		}
	}
	throw Exception(String("A single HttpHeader may not be bigger than ") + StringHelpers::ToString(MaxLength) + " Bytes");
}
//TODO: Klappt noch nicht ganz, begrenzt nicht auf 1MB (war eher eine 2-minuten-nicht-zu-viel-nachdenken-bitte-arbeit. Wahrscheinlich Logikfehler)
void HttpReader::ReadHeaders(NetworkStream& s, HttpHeaders& buffer)
{
	static uint64_t MaxLength = 1000000; //1MB

	uint64_t length = 0;
	bool end = false;
	while (!end)
	{
		String line(readLine(s));
		if (line.size() == 0)
			end = true;
		else if (length <= MaxLength)
		{
			length += line.size();
			buffer.add(line);
		}
		else
			throw Exception(String("HttpHeaders max not be bigger than ") + StringHelpers::ToString(MaxLength) + " Bytes");
	}
}
void HttpReader::ReadMessage(NetworkStream& s, HttpMessage& buffer)
{
	String line(readLine(s));
	buffer.setFirstLine(line);

	buffer.getHeaders().clear();
	HttpReader::ReadHeaders(s, buffer.getHeaders());
}