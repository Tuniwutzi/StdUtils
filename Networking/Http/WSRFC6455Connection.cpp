#include <private/Networking/Http/WSRFC6455Connection.h>


#include <private/Networking/SocketStatus.h>
#include <private/Networking/Http/WebSocketRFC6455.h>
#include <private/Networking/Http/WSMultipartMessageBody.h>



using namespace StdUtils::Private::Networking;


namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{
			WSRFC6455UnparsedMessage::WSRFC6455UnparsedMessage()
				:Length(0)
			{
			}
			void WSRFC6455UnparsedMessage::reset()
			{
				this->Length = 0;
			}

			WSRFC6455Connection::WSRFC6455Connection(WebSocket<WebSocketVersions::RFC6455>& socket)
				:Socket(socket),
				Stream(NULL),
				CloseReceived(false),
				CloseSent(false)
			{
			}

			void WSRFC6455Connection::failConnection()
			{
				if (this->Stream && this->Stream->isOpen())
				{
					try
					{
						this->Stream->close();
					}
					catch (...)
					{
					}
					this->Socket.refreshState();
				}
			}

			void WSRFC6455Connection::sendNextPart(WSMessageHeader const& wsm)
			{
#ifdef _DEBUG
				if (wsm.Opcode != Opcodes::Continuation)
					throw Exception("DEBUG WebSocket<RFC6455>::sendNextPart: Header is not a continuation frame");
#endif

				WSMessageHeader h(wsm);

				if (!this->ServerSide && !h.Masked)
					h.mask();

				unsigned char buffer[14];
				unsigned int headerLength = ProtocolContextRfc6455::assembleHeader(h.Last, 0, 0, 0, h.Opcode, (h.Masked ? (unsigned char*)h.Mask : NULL), h.Length, buffer);

				try
				{
					this->Stream->write((char*)buffer, headerLength);
				}
				catch (...)
				{
					this->Socket.refreshState();
					throw;
				}
			}

			void WSRFC6455Connection::reset()
			{
				this->ReceivedHeaderPart.reset();
				this->NextReceived.reset();

				if (!this->LastReceived.isNull())
				{
					this->LastReceived->markClosed();
					this->LastReceived.setNull();
				}
				if (!this->LastSent.isNull())
				{
					this->LastSent->markClosed();
					this->LastSent.setNull();
				}
				this->Stream = NULL;

				this->CloseReceived = false;
				this->CloseSent = false;
			}


			void WSRFC6455Connection::sendHeaders(WSMessageHeader& h)
			{
				if (this->CloseSent)
					throw Exception("Cannot send headers, close request was already sent");

				if (h.Null)
					return;
				if (!this->LastSent.isNull() && !this->LastSent->isClosed())
					throw Exception("Last sent message body was not closed. Use getCurrentWriter() to retrieve and close it");

				if (!this->ServerSide && !h.Masked)
					h.mask();

				unsigned char buffer[14];
				unsigned int headerLength = ProtocolContextRfc6455::assembleHeader(h.Last, 0, 0, 0, h.Opcode, (h.Masked ? (unsigned char*)h.Mask : NULL), h.Length, buffer);

				try
				{
					this->Stream->write((char*)buffer, headerLength);
				}
				catch (...)
				{
					this->Socket.refreshState();
					throw;
				}

				if (h.Opcode == Opcodes::TextFrame || h.Opcode == Opcodes::BinaryFrame || h.Opcode == Opcodes::Close)
				{
					DelegateMethod<WSRFC6455Connection, void, void> failConnectionMethod(*this, &WSRFC6455Connection::failConnection);
					if (h.Last)
					{
						this->LastSent = SharedPointer<WSMessageBody>(new WSMessageBody(*this->Stream, h, failConnectionMethod, true));
					}
					else
					{
						DelegateMethod<WSRFC6455Connection, void, WSMessageHeader const&> sendNextMethod(*this, &WSRFC6455Connection::sendNextPart);
						this->LastSent = SharedPointer<WSMessageBody>(new WSMultipartMessageBody(*this->Stream, h, failConnectionMethod, sendNextMethod));
					}
				}
			}


			WSMessageHeader WSRFC6455Connection::receiveMessageHeader(bool nonBlocking)
			{
				if (!this->LastReceived.isNull() && !this->LastReceived->isClosed())
					throw Exception("Cannot receive next message while the current one is not fully received");

				WSMessageHeader rv;

				if (this->NextReceived.Null)
				{
					while (!nonBlocking || this->Stream->dataAvailable())
					{
						try
						{
							this->ReceivedHeaderPart.Content[this->ReceivedHeaderPart.Length++] = this->Stream->read();
						}
						catch (...)
						{
							this->Socket.refreshState();
							throw;
						}

						if (ProtocolContextRfc6455::Parse(this->ReceivedHeaderPart.Content, this->ReceivedHeaderPart.Length, rv))
						{
							this->ReceivedHeaderPart.reset();
							//TODO: Eventuell auf unbekannten Opcode pruefen und in dem Fall die Verbindung schliessen
							return rv;
						}
					}
				}
				else
				{
					rv = this->NextReceived;
					this->NextReceived.reset();
				}

				return rv;
			}

			bool WSRFC6455Connection::processIfControlFrame(WSMessageHeader& h)
			{
				switch (h.Opcode)
				{
				case Opcodes::Close:
					this->CloseReceived = true;
					return true;
					break;
				case Opcodes::Ping:
				case Opcodes::Pong:
					throw Exception("Ping/Pong not implemented");
					break;
				default:
					return false;
					break;
				}
			}

			void WSRFC6455Connection::bufferFrame(WSMessageHeader& h)
			{
				this->NextReceived = h;
			}

			void WSRFC6455Connection::processContentFrame(WSMessageHeader& h)
			{
				DelegateMethod<WSRFC6455Connection, void, void> failConnectionMethod(*this, &WSRFC6455Connection::failConnection);
				if (h.Last)
				{
					this->LastReceived = SharedPointer<WSMessageBody>(new WSMessageBody(*this->Stream, h, failConnectionMethod, false));
				}
				else
				{
					DelegateMethod<WSRFC6455Connection, WSMessageHeader, bool> receivePart(*this, &WSRFC6455Connection::receiveMessageHeader);
					this->LastReceived = SharedPointer<WSMessageBody>(new WSMultipartMessageBody(*this->Stream, h, failConnectionMethod, receivePart));
				}
			}
		}
	}
}