#ifndef STDUTILS_NETWORKING_HTTP_WSMESSAGEHEADER_H
#define STDUTILS_NETWORKING_HTTP_WSMESSAGEHEADER_H


#include <StdUtils/Networking/Http/WSMessageTypes.h>
#include <StdUtils/Random.h>


namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{
			STDUTILS_START_ENUM(Opcodes)
			{
				Continuation = 0x00,
					TextFrame = 0x01,
					BinaryFrame = 0x02,
					Close = 0x08,
					Ping = 0x09,
					Pong = 0x0a
			}
			STDUTILS_END_ENUM(Opcodes);

			struct WSMessageHeader
			{
				typedef uint16_t MediumPayloadLength;
				typedef uint64_t BigPayloadLength;
				typedef BigPayloadLength PayloadLength;

				bool Null;

				//If not null:
				bool Last;
				Opcodes::Value Opcode;
				PayloadLength Length;
				bool Masked;
				char Mask[4];

				//WSMessageHeader()
				//	:Null(true),
				//	Masked(false)
				//{
				//}
				WSMessageHeader()
				{
					this->reset();
				}
				void reset()
				{
					this->Null = true;
					this->Masked = false;
				}
				void mask()
				{
					static Random rd;

					this->Masked = true;
					int* m = (int*)this->Mask;
					*m = rd.next();
				}
			};
		}
	}
}

#endif