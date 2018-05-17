#define NOMINMAX

#include <private/Networking/Http/WSMessageBody.h>


#include <StdUtils/Networking/Http/WSMessageBodyReader.h>
#include <private/Networking/Http/WSMessageHeader.h>


#include <algorithm>
using namespace std;


namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{
			WSMessageBody::WSMessageBody(NetworkStream& ns, WSMessageHeader const& header, WSMessageBody::IFailConnectionMethod const& fc, bool sending)
				:stream(ns),
				sending(sending),
				failConnection(fc),
				closed(false)
			{
				switch (header.Opcode)
				{
				case Opcodes::TextFrame:
					this->type = WSMessageTypes::Text;
					break;
				case Opcodes::BinaryFrame:
					this->type = WSMessageTypes::Binary;
					break;
					//TODO: Close; Was tun? AKtuell egal, wird eh direkt geschlossen, wichtig wenn Close-Reasons mit hilfe von Reader/Writer implementiert werden
				default:
					//TODO: Sollte in WebSocket abgefangen sein, nochmal kontrollieren
					break;
				}
				this->reinitialize(header);
			}

			WSMessageBody::~WSMessageBody()
			{
			}

			void WSMessageBody::assertOpen()
			{
				if (this->closed)
					throw Exception("Message body is closed");
			}

			void WSMessageBody::reinitialize(WSMessageHeader const& header)
			{
				this->assertOpen();

				this->bodyLength = header.Length;
				this->transferredBytes = 0;

				if ((this->masked = header.Masked)) //Intentional assignment
					memcpy(this->mask, header.Mask, sizeof(this->mask));
			}

			void WSMessageBody::doSend(char const* buf, uint32_t l)
			{
				try
				{
					this->stream.write(buf, l);
					this->transferredBytes += l;
				}
				catch (...)
				{
					if (!this->stream.refreshOpenState())
					{
						this->closed = true;
						this->failConnection();
					}
					throw;
				}
			}
			uint32_t WSMessageBody::doReceive(char* buf, uint32_t l)
			{
				try
				{
					uint32_t rv(this->stream.read(buf, l));
					this->transferredBytes += rv;
					return rv;
				}
				catch (...)
				{
					if (!this->stream.refreshOpenState())
					{
						this->closed = true;
						this->failConnection();
					}
					throw;
				}
			}
			void WSMessageBody::markClosed()
			{
				this->closed = true;
			}

			void WSMessageBody::write(char const* buf, uint32_t len)
			{
				this->assertOpen();

				if (len == 0)
					return;

				uint64_t bytesLeft = WSMessageBody::getBytesToWrite();
				if (bytesLeft >= len)
				{
					if (this->masked)
					{
						char* writeBuffer = new char[len];
						for (uint64_t i = this->transferredBytes; i < (len + this->transferredBytes); i++)
							writeBuffer[i - this->transferredBytes] = buf[i] ^ this->mask[i % 4];
						this->doSend(writeBuffer, len);
						delete[] writeBuffer;
					}
					else
						this->doSend(buf, len);
				}
				else
					throw Exception("Cannot write more bytes into the body than are expected");
			}
			void WSMessageBody::write(char c)
			{
				this->write(&c, 1);
			}

			uint32_t WSMessageBody::read(char* buf, uint32_t bufferSize)
			{
				this->assertOpen();
				
				uint64_t rv;

				uint64_t bytesLeft = WSMessageBody::getBytesToRead();
				if (bytesLeft > 0)
				{
					uint64_t read = this->doReceive(buf, min((uint64_t)bufferSize, bytesLeft)); //siehe TODO bei Returnvalue
					rv = read;
				}
				else
					throw Exception("This message body is already at EOF");

				if (this->masked)
				{
					uint64_t maskOffset = this->transferredBytes - rv;
					for (uint64_t i = 0; i < rv; i++)
						buf[i] = buf[i] ^ this->mask[(i + maskOffset)%4];
				}
				
				//TODO!: Inkonsistenz mit Rueckgabewerten - muss im Algorithmus abgefangen werden, sodass max. die 32-bit bufferSize ausgelesen wird, auch wenn bytesLeft groesser ist
				return rv;
			}
			char WSMessageBody::read()
			{
				char buf;

				this->read(&buf, 1);

				return buf;
			}

			//Will only be called on a writing body
			void WSMessageBody::close()
			{
				this->assertOpen();

				if (this->getBytesToWrite() > 0)
					throw Exception(String("Can not close message body until it is fully written"));

				this->closed = true;
			}
			//Will only be called on a reading body
			void WSMessageBody::close(bool discardUnread)
			{
				this->assertOpen();

				uint64_t bytesLeft = WSMessageBody::getBytesToRead();
				if (bytesLeft > 0)
				{
					if (discardUnread)
					{
						static char discardbuffer[1024];

						while (bytesLeft > 0)
						{
							//Cast of uint64_t to uint32_t valid, because the top reachable value is sizeof(discardbuffer)
							bytesLeft -= this->doReceive(discardbuffer, (uint32_t)std::min((uint64_t)sizeof(discardbuffer), bytesLeft));
						}
					}
					else
						throw Exception("Message body was not completely read before closing and discardUnreadBytes was set to false.");
				}

				this->closed = true;
			}

			bool WSMessageBody::isClosed() const
			{
				return this->closed;
			}

			WSMessageTypes::Value WSMessageBody::getMessageType() const
			{
				return this->type;
			}

			bool WSMessageBody::knowsBodyLength() const
			{
				return true;
			}
			uint64_t WSMessageBody::getBodyLength() const
			{
				return this->bodyLength;
			}

			uint64_t WSMessageBody::getBytesToWrite() const
			{
				return this->bodyLength - this->transferredBytes;
			}
			bool WSMessageBody::isWriteCompleted() const
			{
				return this->getBytesToWrite() <= 0;
			}

			uint64_t WSMessageBody::getBytesToRead() const
			{
				return this->bodyLength - this->transferredBytes;
			}
			bool WSMessageBody::isReadCompleted() const
			{
				return this->getBytesToRead() <= 0;
			}

			//bool WSMessageBody::canReceive()
			//{
			//	return !this->sending && !this->closed && this->getBytesToRead() > 0 && this->stream.canReceive();
			//}
			bool WSMessageBody::dataAvailable()
			{
				return !this->sending && !this->closed && this->getBytesToRead() > 0 && this->stream.dataAvailable();
			}
		}
	}
}
