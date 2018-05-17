#ifndef STDUTILS_PRIVATE_WEBSOCKETRFC6455_H
#define STDUTILS_PRIVATE_WEBSOCKETRFC6455_H


#include <vector>

#include <StdUtils/SharedPointer.h>
#include <StdUtils/Networking/Socket.h>
#include <StdUtils/Nullable.h>

#include <private/Networking/Http/WSMessageHeader.h>

//htonl
#ifdef _WINDOWS
#include <WinSock2.h>
#else
#include <netinet/in.h>
#endif


using namespace std;


namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{
			static uint64_t hton64(uint64_t value)
			{
				static int num(42);
				static bool sw(*(char *)&num == 42);
				if (sw)
					return ((uint64_t)htonl(value & 0xFFFFFFFF) << 32LL) | htonl(value >> 32);
				else
					return value;
			}

			static uint64_t ntoh64(uint64_t value)
			{
				static const int num(42);
				static const bool sw(*(char *)&num == 42);
				if (sw)
					return ((uint64_t)ntohl((value << 32LL) >> 32LL) << 32LL) | ntohl(value >> 32);
				else
					return value;
			}
			
			struct ProtocolContextRfc6455
			{
				static unsigned int const HEADER_BUFFER_LENGTH = 14;
				static uint64_t const MAX_CONTENT_LENGTH = 0x7FFFFFFFFFFFFFFF; //unsigned 64-Bit integer, most significant bit must be 0

				typedef WSMessageHeader::MediumPayloadLength MediumPayloadLength;
				typedef WSMessageHeader::BigPayloadLength BigPayloadLength;
				typedef BigPayloadLength PayloadLength;
				typedef union {unsigned char Raw[4]; uint32_t Integer;} Mask;
				struct RawHeader
				{
					unsigned char Raw[HEADER_BUFFER_LENGTH];
					unsigned char ReceivedHeaderBytes;

					bool fin()
					{
						return (*Raw & 0x80) != 0;
					}
					bool rsv1()
					{
						return (*Raw & 0x40) != 0;
					}
					bool rsv2()
					{
						return (*Raw & 0x20) != 0;
					}
					bool rsv3()
					{
						return (*Raw & 0x10) != 0;
					}
					Opcodes::Value opcode()
					{
						return (Opcodes::Value)(*Raw & 0x0f);
					}

					bool masked()
					{
						return (Raw[1] & 0x80) != 0;
					}

					PayloadLength payloadlength()
					{
						BigPayloadLength tmp = Raw[1] & 0x7f;
						if (tmp > 125)
						{
							if (tmp > 126)
							{
								tmp = ntoh64(*(BigPayloadLength*)(Raw + 2));
							}
							else
							{
								tmp = ntohs(*(MediumPayloadLength*)(Raw + 2));
							}
						}

						return tmp;
					}

					Mask& mask()
					{
						int offset = 0;

						BigPayloadLength tmp = Raw[1] & 0x7f;
						if (tmp > 125)
						{
							if (tmp > 126)
							{
								offset += sizeof(BigPayloadLength);
							}
							else
							{
								offset += sizeof(MediumPayloadLength);
							}
						}

						return *(Mask*)(Raw + 2 + offset);
					}

					char predictHeaderLength()
					{
						char length = -1;

						if (ReceivedHeaderBytes > 1)
						{
							length = 2;
							if (masked())
							{
								length += sizeof(Mask);
							}

							unsigned char tmp = Raw[1] & 0x7f;
							if (tmp > 125)
							{
								if (tmp > 126)
								{
									length += sizeof(BigPayloadLength);
								}
								else
								{
									length += sizeof(MediumPayloadLength);
								}
							}
						}

						return length;
					}

					RawHeader()
					{
						Reset();
					}
					void Reset()
					{
						memset(this, 0, sizeof(RawHeader));
					}
				};

				static unsigned int assembleHeader(bool isLastFragment, bool RSV1, bool RSV2, bool RSV3, unsigned char opcode, unsigned char* maskingKey, ProtocolContextRfc6455::PayloadLength payloadLength, unsigned char* buffer)
				{
					unsigned char* pos = (unsigned char*)buffer;

					*pos = isLastFragment ? 0x80 : 0x00;
					if (RSV1)
						*pos += 0x40;
					if (RSV2)
						*pos += 0x20;
					if (RSV3)
						*pos += 0x10;
					*pos += (opcode & 0x0F);

					pos++;

					*pos = maskingKey ? 0x80 : 0x00;
					if (payloadLength <= 125)
					{
						*pos |= (payloadLength & 0x7F);
						pos++;
					}
					else if (payloadLength <= 65535)
					{
						*pos |= 0x7e;

						pos++;


						ProtocolContextRfc6455::MediumPayloadLength* tmp = (ProtocolContextRfc6455::MediumPayloadLength*)pos;
						*tmp = htons((ProtocolContextRfc6455::MediumPayloadLength)payloadLength);

						pos += sizeof(ProtocolContextRfc6455::MediumPayloadLength);
					}
					else
					{
						*pos |= 0x7f;

						pos++;

						ProtocolContextRfc6455::BigPayloadLength* tmp = (ProtocolContextRfc6455::BigPayloadLength*)pos;
						*tmp = hton64(payloadLength);

						pos += sizeof(ProtocolContextRfc6455::BigPayloadLength);
					}

					if (maskingKey)
					{
						*pos++ = *maskingKey++;
						*pos++ = *maskingKey++;
						*pos++ = *maskingKey++;
						*pos++ = *maskingKey++;
					}

					return (unsigned int)(pos - buffer);
				}

				//static void SendWSMessage(WebSocketMessage const& message, bool fin, bool continuation, Socket<SocketTypes::Tcp>* socket)
				//{
				//	Opcodes::Value opcode;
				//	switch (message.getType())
				//	{
				//	case WebSocketMessageTypes::Text:
				//		opcode = Opcodes::TextFrame;
				//		break;
				//	case WebSocketMessageTypes::Binary:
				//		opcode = Opcodes::BinaryFrame;
				//		break;
				//	case WebSocketMessageTypes::Close:
				//		opcode = Opcodes::Close;
				//		break;
				//	default:
				//		throw Exception("Unknown WebSocketMessageType");
				//		break;
				//	}

				//	unsigned char buffer[ProtocolContextRfc6455::HEADER_BUFFER_LENGTH];
				//	socket->send((char*)buffer, assembleHeader(fin, 0,0,0, (continuation ? Opcodes::Continuation : (unsigned char)opcode), 0, message.getContentLength(), buffer));
				//	socket->send(message.getContent(), message.getContentLength());
				//}

				//static void add(RawHeader& header, vector<WebSocketMessage>& msgBuf, vector<WebSocketMessage>& parts, Nullable<WebSocketMessage>& closeMessage)
				//{
				//	bool partial = parts.size() > 0;
				//	WebSocketMessageTypes::Value partialType = WebSocketMessageTypes::Unknown;
				//	if (partial)
				//		partialType = parts.begin()->getType();

				//	WebSocketMessage message = header.message(partial, partialType);

				//	if (header.opcode() == Opcodes::Close)
				//	{
				//		closeMessage = message;
				//		return;
				//	}

				//	if (header.fin())
				//	{
				//		if (partial)
				//		{
				//			uint64_t totalSize = 0;
				//			for (vector<WebSocketMessage>::iterator it = parts.begin(); it != parts.end(); it++)
				//				totalSize += it->getContentLength();

				//			char* buf = new char[totalSize];

				//			uint64_t copied = 0;
				//			for (vector<WebSocketMessage>::iterator it = parts.begin(); it != parts.end(); it++)
				//			{
				//				memcpy(buf + copied, it->getContent(), it->getContentLength());
				//				copied += it->getContentLength();
				//			}

				//			parts.clear();
				//			msgBuf.insert(msgBuf.begin(), WebSocketMessage(buf, totalSize, partialType));
				//			delete[] buf;
				//		}
				//		else
				//		{
				//			msgBuf.insert(msgBuf.begin(), message);
				//		}
				//	}
				//	else
				//		parts.push_back(message);
				//}

				//static uint64_t doInterpret(char const* data, uint64_t dataLength, ProtocolContextRfc6455::RawHeader& header, vector<WebSocketMessage>& messageBuffer, vector<WebSocketMessage>& parts, Nullable<WebSocketMessage>& closeMessage)
				//{
				//	if (dataLength == 0)
				//		return 0;

				//	char const* pos = data;
				//	uint64_t length = dataLength;

				//	uint64_t parsedCurrent = 0;
				//	uint64_t parsedTotal = 0;

				//	while (length > 0)
				//	{
				//		if (header.Data == 0)
				//		{
				//			if (header.ReceivedHeaderBytes < 2)
				//			{
				//				if (length == 1)
				//				{
				//					header.Raw[header.ReceivedHeaderBytes] = *pos++;
				//					header.ReceivedHeaderBytes += 1;
				//				}
				//				else
				//				{
				//					memcpy(header.Raw + header.ReceivedHeaderBytes, pos, 2 - header.ReceivedHeaderBytes);
				//					pos += 2 - header.ReceivedHeaderBytes;
				//					parsedCurrent += 2 - header.ReceivedHeaderBytes;

				//					header.ReceivedHeaderBytes = 2;
				//				}
				//			}
				//			else
				//			{
				//				char awaitedBytes = header.predictHeaderLength() - header.ReceivedHeaderBytes;
				//				if (awaitedBytes > 0)
				//				{
				//					if ((unsigned char)awaitedBytes > length)
				//						awaitedBytes = (unsigned char)length;

				//					memcpy(header.Raw + header.ReceivedHeaderBytes, pos, awaitedBytes);

				//					pos += awaitedBytes;
				//					parsedCurrent += awaitedBytes;
				//					header.ReceivedHeaderBytes += awaitedBytes;
				//				}
				//				else if (header.payloadlength() > 0)
				//				{
				//					header.Data = new unsigned char[header.payloadlength()];
				//				}
				//			}

				//			if (header.ReceivedHeaderBytes == header.predictHeaderLength() && header.payloadlength() == 0)
				//			{
				//				add(header, messageBuffer, parts, closeMessage);
				//				//Nullable<WebSocketMessage> message = header.message();
				//				//if (message.hasValue())
				//				//	messageBuffer.insert(messageBuffer.begin(), message.getValue());

				//				header.Reset();
				//				parsedTotal += parsedCurrent;
				//				parsedCurrent = 0;
				//			}
				//		}
				//		else
				//		{
				//			ProtocolContextRfc6455::BigPayloadLength awaitedBytes = header.payloadlength() - header.ReceivedPayloadData;
				//			if (awaitedBytes > length)
				//				awaitedBytes = length;

				//			memcpy(header.Data + header.ReceivedPayloadData, pos, awaitedBytes);
				//			header.ReceivedPayloadData += awaitedBytes;

				//			if (header.ReceivedPayloadData == header.payloadlength())
				//			{
				//				add(header, messageBuffer, parts, closeMessage);
				//				//Nullable<WebSocketMessage> message = header.message();
				//				//if (message.hasValue())
				//				//	messageBuffer.insert(messageBuffer.begin(), message.getValue());

				//				header.Reset();
				//				parsedCurrent += awaitedBytes;
				//				parsedTotal += parsedCurrent;
				//				parsedCurrent = 0;
				//			}

				//			pos += awaitedBytes;
				//		}

				//		length = dataLength - (pos - data);
				//	}

				//	return parsedTotal;
				//}
				//static uint64_t InterpretReceivedWSData(char const* unparsed, uint64_t upLength, char const* data, uint64_t dataLength, vector<WebSocketMessage>& messageBuffer, vector<WebSocketMessage>& parts, Nullable<WebSocketMessage>& closeMessage)
				//{
				//	ProtocolContextRfc6455::RawHeader header;
				//	if (doInterpret(unparsed, upLength, header, messageBuffer, parts, closeMessage) != 0)
				//		throw Exception("Internal error interpreting WebSocket message");
				//	uint64_t parsed = doInterpret(data, dataLength, header, messageBuffer, parts, closeMessage);
				//	return parsed;
				//}





				static bool Parse(char const* data, uint8_t dataLength, WSMessageHeader& buffer)
				{
					if (dataLength > 14)
						throw Exception("WebSocket header too long or malformed");


					RawHeader rh;
					memcpy(rh.Raw, data, dataLength);
					rh.ReceivedHeaderBytes = dataLength;

					if (rh.predictHeaderLength() == dataLength)
					{
						switch (rh.opcode())
						{
						case Opcodes::BinaryFrame:
						case Opcodes::TextFrame:
						case Opcodes::Continuation:
						case Opcodes::Close:
							break;
						default:
							throw Exception("Unknown or unsupported Message type");
							break;
						}
						
						buffer.Null = false;

						buffer.Last = rh.fin();
						buffer.Opcode = rh.opcode();
						buffer.Length = rh.payloadlength();
						if (buffer.Masked = rh.masked()) //Intentional assignment
							memcpy(buffer.Mask, rh.mask().Raw, sizeof(buffer.Mask));

						return true;
					}
					return false;
				}
			};
		}
	}
}

#endif