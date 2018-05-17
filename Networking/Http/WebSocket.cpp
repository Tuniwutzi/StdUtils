#include <StdUtils/Networking/Http/WebSocket.h>


#include <StdUtils/Random.h>
#include <StdUtils/Exceptions.h>
#include <StdUtils/StringHelpers.h>
#include <StdUtils/Threading/Thread.h>
#include <StdUtils/HashGenerator.h>
#include <StdUtils/Encodings/Base64.h>
#include <StdUtils/Networking/DomainNameSystem.h>
#include <StdUtils/Networking/Http/WSAcceptException.h>
#include <private/Networking/Http/WSMultipartMessageBody.h>

#include <private/Networking/Http/WebSocketRFC6455.h>
#include <private/Networking/Http/WSRFC6455Connection.h>
#include <private/Networking/SocketStatus.h>


using namespace StdUtils::Threading;
using namespace StdUtils::Private::Networking;

using namespace std;


namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{
			WebSocket<WebSocketVersions::RFC6455>::Server::Server()
				:Socket(NULL)
			{
			}
			void WebSocket<WebSocketVersions::RFC6455>::Server::reset()
			{
				this->Socket = NULL;
				this->Endpoint.setNull();
			}


			WebSocket<WebSocketVersions::RFC6455>::WebSocket()
				:status(SocketStatus::Closed),
				connection(*new WSRFC6455Connection(*this))
			{
			}
			WebSocket<WebSocketVersions::RFC6455>::~WebSocket()
			{
				//TODO: Was, wenn ein Handle nicht geschlossen werden kann o.ae.?
				//close wirft exception aus destruktor -> Boese!
				this->close();
				delete &this->connection;
			}

			String WebSocket<WebSocketVersions::RFC6455>::digestKey(String const& key) const
			{
				static char const wsGuid[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
				String tmp(key + wsGuid);

				vector<char> sh(HashGenerator::SHA1().generate(tmp));

				return Encodings::Base64::Encode(sh);
			}

			void WebSocket<WebSocketVersions::RFC6455>::sendClose(char* reason, uint32_t reasonLength)
			{
				if (this->connection.CloseSent)
					throw Exception("Close message has already been sent");
				if (!this->connection.LastSent.isNull() && !this->connection.LastSent->isClosed())
					throw Exception("Cannot sent close message while a message is still being sent");

				WSMessageHeader h;
				h.Null = false;
				h.Last = true;
				h.Opcode = Opcodes::Close;
				h.Length = reasonLength;
				this->connection.sendHeaders(h);

				this->connection.LastSent->write(reason, reasonLength);
				this->connection.LastSent->close();

				this->connection.CloseSent = true;
			}

			void WebSocket<WebSocketVersions::RFC6455>::close()
			{
				if (this->connection.Stream)
				{
					if (this->connection.OwnsStream)
						delete this->connection.Stream;

					this->connection.Stream = NULL;
				}

				delete this->server.Socket;
				this->server.Socket = NULL;

				this->status = SocketStatus::Closed;
				this->server.reset();
				this->connection.reset();
			}

			void WebSocket<WebSocketVersions::RFC6455>::connect(HttpClientContext& ctx)
			{
				if (this->status != SocketStatus::Closed && this->status != SocketStatus::Disconnected)
					throw Exception("Socket must be closed or disconnected");

				if (ctx.getState() != HttpContextStates::Waiting)
					throw Exception("Illegal state for HttpClientContext");

				vector<char> rawKey;

				Random rdm;
				for (int i = 0; i < 4; i++)
				{
					int32_t tmp = rdm.next();
					for (int i1 = 0; i1 < sizeof(int32_t); i1++)
						rawKey.push_back(((char*)&tmp)[i1]);
				}

				String key(Encodings::Base64::Encode(rawKey));

				ctx.getRequest().getHeaders().set("Upgrade", "websocket");
				ctx.getRequest().getHeaders().set("Sec-WebSocket-Key", key);
				ctx.getRequest().getHeaders().set("Sec-WebSocket-Version", "13");

				ctx.sendRequest();

				HttpResponse const& rsp(ctx.receiveResponse());
				if (rsp.getStatusCode() == 101 && StringHelpers::ToLower(rsp.getStatusDescription()) == "switching protocols")
				{
					String acceptKey = rsp.getHeaders().get("Sec-WebSocket-Accept").getValue();
					if (StringHelpers::ToLower(rsp.getHeaders().get("Upgrade").getValue()) == "websocket"
						&& StringHelpers::ToLower(rsp.getHeaders().get("Connection").getValue()) == "upgrade")
					{
						if (acceptKey != this->digestKey(key))
							throw Exception("Invalid key sent by server");
					}
					else
						throw Exception("Expected headers missing or invalid");
				}
				else
					throw Exception("Invalid response status line");

				this->connection.Stream = ctx.getStream();
				this->connection.OwnsStream = ctx.ownsStream();
				ctx.disownStream();
				ctx.close();

				this->status = SocketStatus::Connected;
				this->connection.ServerSide = false;
			}
			void WebSocket<WebSocketVersions::RFC6455>::connect(WSEndpoint const& ep)
			{
				if (this->status != SocketStatus::Closed && this->status != SocketStatus::Disconnected)
					throw Exception("Socket must be closed or disconnected");

				if (!ep.getIPEndpoint().hasValue())
					throw Exception("WSEndpoint is missing connection data");

				Socket<SocketTypes::Tcp>* s = new Socket<SocketTypes::Tcp>();

				try
				{
					s->connect(ep.getIPEndpoint().getValue());
				}
				catch (...)
				{
					delete s;
					throw;
				}
				HttpClientContext ctx(s);
				this->connect(ctx);
			}
			void WebSocket<WebSocketVersions::RFC6455>::connect(HttpServerContext& ctx)
			{
				if (this->status != SocketStatus::Closed && this->status != SocketStatus::Disconnected)
					throw Exception("Socket must be closed or disconnected");

				if (!ctx.requestReceived())
					ctx.receiveRequest();

				HttpRequest const& req(ctx.getRequest());
				if (StringHelpers::ToLower(req.getHeaders().get("Upgrade").getValue()) != "websocket")
					throw Exception("Invalid value of upgrade header");

				String key = req.getHeaders().get("Sec-WebSocket-Key").getValue();
				ctx.getResponse().setStatusCode(101);
				ctx.getResponse().setStatusDescription("SWITCHING PROTOCOLS");
				ctx.getResponse().getHeaders().set("Connection", "upgrade");
				ctx.getResponse().getHeaders().set("Upgrade", "websocket");
				ctx.getResponse().getHeaders().set("Sec-WebSocket-Accept", digestKey(key));

				ctx.sendResponse();

				//TODO: Some better way to check if the client has accepted my handshake
				//If there is no way to tell but check for closedness, then at least use WaitForIO with a short timeout
				ctx.getStream()->refreshState();
				if (ctx.getStream()->isOpen())
				{
					this->connection.Stream = ctx.getStream();
					this->connection.OwnsStream = ctx.ownsStream();
					ctx.disownStream();
					ctx.close();

					this->status = SocketStatus::Connected;
					this->connection.ServerSide = true;
				}
				else
					throw Exception("Connection closed by client. Likely the handshake was not accepted");
			}

			void WebSocket<WebSocketVersions::RFC6455>::disconnect(bool preventDirty)
			{
				if (this->status == SocketStatus::Connected)
				{
					bool sending = !this->connection.LastSent.isNull() && !this->connection.LastSent->isClosed();
					bool receiving = !this->connection.LastReceived.isNull() && !this->connection.LastReceived->isClosed();

					if (preventDirty && (sending || receiving))
						throw Exception("There is an open body; WebSocket cannot be closed cleanly");

					if (sending && receiving)
					{
						this->connection.LastSent->markClosed();
						this->connection.LastReceived->markClosed();

						this->connection.Stream->close();
					}
					else if (sending)
					{
						this->connection.LastSent->markClosed();

						this->connection.Stream->close();
					}
					else if (receiving)
					{
						this->connection.LastReceived->markClosed();

						if (!this->connection.CloseSent && this->connection.Stream->isOpen())
							this->sendClose();

						this->connection.Stream->close();
					}
					else
					{
						if (preventDirty && !this->connection.CloseReceived)
							throw Exception("No disconnect request was received; WebSocket cannot be closed cleanly");

						if (!this->connection.CloseSent && this->connection.Stream->isOpen())
							this->sendClose();

						this->connection.Stream->close();
					}

					this->status = SocketStatus::Disconnected;
				}
				else if (this->status != SocketStatus::Disconnected)
					throw Exception("Socket cannot be disconnected; it is closed or a server socket");
			}

			void WebSocket<WebSocketVersions::RFC6455>::requestDisconnect(uint16_t statusCode, String const& reason)
			{
				if (this->status != SocketStatus::Connected)
					throw Exception("WebSocket is not connected");

				uint32_t len = (uint32_t)reason.size() + sizeof(uint16_t);
				char* buf = new char[len];
				u_short* sc = (u_short*)buf;
				*sc = htons(statusCode);
				memcpy(buf + 2, reason.data(), reason.size());

				this->sendClose(buf, len);

				delete[] buf;
			}
			void WebSocket<WebSocketVersions::RFC6455>::requestDisconnect()
			{
				if (this->status != SocketStatus::Connected)
					throw Exception("WebSocket is not connected");

				this->sendClose();
			}


			void WebSocket<WebSocketVersions::RFC6455>::bind(WSEndpoint const& ep)
			{
				if (!this->isClosed())
					throw Exception("WebSocket is not closed");

				if (!ep.getIPEndpoint().hasValue())
					throw Exception("Endpoint is missing connection data");

				if (ep.getHost().hasValue())
					throw Exception("Filter by host is not implemented");
				if (ep.getRessource().size() > 0 && ep.getRessource() != "/")
					throw Exception("Filter by ressource is not implemented");


				this->server.Socket = new TcpSocket();

				try
				{
					this->server.Socket->bind(ep.getIPEndpoint().getValue());
					this->server.Endpoint = ep;
					this->status = SocketStatus::Bound;
				}
				catch (...)
				{
					delete this->server.Socket;
					this->server.Socket = NULL;
					throw;
				}
			}
			void WebSocket<WebSocketVersions::RFC6455>::listen(int backlog)
			{
				if (!this->isBound())
					throw Exception("WebSocket is not bound");

				this->server.Socket->listen(backlog);
				this->status = SocketStatus::Listening;
			}

			void WebSocket<WebSocketVersions::RFC6455>::accept(WebSocket& buffer)
			{
				if (!this->isListening())
					throw Exception("WebSocket is not listening");

				if (!buffer.isClosed())
					throw Exception("Buffer must be a closed WebSocket");

				Socket<SocketTypes::Tcp>* s = this->server.Socket->accept();
#ifdef STDUTILS_COMPILE_SSL
				NetworkStream* ns = this->server.Endpoint.isSsl() ? new SslStream(s, *this->server.Endpoint.getSslCertificate(), true) : new TcpStream(s, true);
#else
				NetworkStream* ns = this->server.Endpoint.isSsl() ? throw Exception("Server is set to accept SSL encrypted connections, but STDUTILS_COMPILE_SSL is not defined") : new TcpStream(s, true);
#endif

				HttpServerContext* ctx(NULL);

				try
				{
					ctx = new HttpServerContext(ns, true);
				}
				catch (Exception& ex)
				{
					SharedPointer<HttpServerContext> ctxPointer;
					String msg(ex.what());
					throw WSAcceptException(WSAcceptException::ProtocolError, ctxPointer, msg);
				}

				//TODO: Filter by resource

				try
				{
					buffer.connect(*ctx);
					delete ctx;
				}
				catch (Exception& ex)
				{
					SharedPointer<HttpServerContext> ctxPointer(ctx);
					String msg(ex.what());
					throw WSAcceptException(WSAcceptException::HandshakeFailure, ctxPointer, msg);
				}
			}
			WebSocket<WebSocketVersions::RFC6455>* WebSocket<WebSocketVersions::RFC6455>::accept()
			{
				WebSocket* rv = new WebSocket();
				try
				{
					this->accept(*rv);
				}
				catch (...)
				{
					delete rv;
					throw;
				}
				return rv;
			}

			WSMessageBodyReader WebSocket<WebSocketVersions::RFC6455>::receive()
			{
				if (this->status != SocketStatus::Connected)
					throw Exception("WebSocket is not connected");
				if (this->disconnectRequestReceived())
					throw Exception("Disconnect request received, no further messages can be received");

				WSMessageHeader h(this->connection.receiveMessageHeader(false));
				while (this->connection.processIfControlFrame(h))
				{
					if (this->disconnectRequestReceived())
						throw Exception("Disconnect request received by peer");
					else
						h = this->connection.receiveMessageHeader(false);
				}

				this->connection.processContentFrame(h);

				return WSMessageBodyReader(this->connection.LastReceived);
			}
			bool WebSocket<WebSocketVersions::RFC6455>::tryReceive(WSMessageBodyReader* buf)
			{
				try
				{
					*buf = this->receive();
					return true;
				}
				catch (...)
				{
					return false;
				}
			}

			WSMessageBodyReader WebSocket<WebSocketVersions::RFC6455>::getCurrentReader()
			{
				if (this->status != SocketStatus::Connected && this->status != SocketStatus::Disconnected)
					throw Exception("This socket is closed or a server socket");

				//if (this->connection.LastReceived.isNull())
				//	throw Exception("No message has been received");
				//else
				return WSMessageBodyReader(this->connection.LastReceived);
			}

			WSMessageBodyWriter WebSocket<WebSocketVersions::RFC6455>::send(uint64_t bodyLength, WSMessageTypes::Value type)
			{
				if (this->status != SocketStatus::Connected)
					throw Exception("WebSocket is not connected");
				if (this->disconnectRequestSent())
					throw Exception("Cannot send message after disconnect request");

				WSMessageHeader h;
				h.Null = false;
				h.Last = true;
				h.Opcode = (type == WSMessageTypes::Text ? Opcodes::TextFrame : Opcodes::BinaryFrame);
				h.Length = bodyLength;

				this->connection.sendHeaders(h);
				return WSMessageBodyWriter(this->connection.LastSent);
			}
			WSMessageBodyWriter WebSocket<WebSocketVersions::RFC6455>::send(WSMessageTypes::Value type)
			{
				if (this->status != SocketStatus::Connected)
					throw Exception("WebSocket is not connected");
				if (this->disconnectRequestSent())
					throw Exception("Cannot send message after disconnect request");

				WSMessageHeader h;
				h.Null = false;
				h.Last = false;
				h.Opcode = (type == WSMessageTypes::Text ? Opcodes::TextFrame : Opcodes::BinaryFrame);
				h.Length = 0;

				this->connection.sendHeaders(h);
				return WSMessageBodyWriter(this->connection.LastSent);
			}
			bool WebSocket<WebSocketVersions::RFC6455>::trySend(uint64_t bodyLength, WSMessageTypes::Value type, WSMessageBodyWriter* buf)
			{
				try
				{
					*buf = this->send(bodyLength, type);
					return true;
				}
				catch (...)
				{
					return false;
				}
			}
			bool WebSocket<WebSocketVersions::RFC6455>::trySend(WSMessageTypes::Value type, WSMessageBodyWriter* buf)
			{
				try
				{
					*buf = this->send(type);
					return true;
				}
				catch (...)
				{
					return false;
				}
			}
			WSMessageBodyWriter WebSocket<WebSocketVersions::RFC6455>::getCurrentWriter()
			{
				if (this->status != SocketStatus::Connected && this->status != SocketStatus::Disconnected)
					throw Exception("This socket is closed or a server socket");


				//if (this->connection.LastSent.isNull())
				//	throw Exception("No message has been sent");
				//else
				return WSMessageBodyWriter(this->connection.LastSent);
			}



			bool WebSocket<WebSocketVersions::RFC6455>::isClosed() const
			{
				return this->status == SocketStatus::Closed;
			}
			bool WebSocket<WebSocketVersions::RFC6455>::isConnected() const
			{
				return this->status == SocketStatus::Connected;
			}
			bool WebSocket<WebSocketVersions::RFC6455>::isDisconnecting() const
			{
				if (this->isConnected())
				{
#ifdef _DEBUG
					if (this->disconnectRequestReceived() && this->disconnectRequestSent())
						throw Exception("DEBUG WebSocket<RFC6455>::isDisconnecting: disc request was sent AND received, but the last known status is still connected");
#endif
					if (this->disconnectRequestReceived() || this->disconnectRequestSent()) //Theoretisch xor, wenn die aber beide true sind muss isConnected() == false sein, sonst ist schon vorher was schiefgelaufen
						return true;
				}

				return false;
			}
			bool WebSocket<WebSocketVersions::RFC6455>::isDisconnected() const
			{
				return this->status == SocketStatus::Disconnected;
			}
			bool WebSocket<WebSocketVersions::RFC6455>::isBound() const
			{
				return this->status == SocketStatus::Bound || this->status == SocketStatus::Listening;
			}
			bool WebSocket<WebSocketVersions::RFC6455>::isListening() const
			{
				return this->status == SocketStatus::Listening;
			}

			bool WebSocket<WebSocketVersions::RFC6455>::isClean() const
			{
				if (this->isDisconnected())
					return this->disconnectRequestSent() && this->disconnectRequestReceived();

				return true;
			}

			bool WebSocket<WebSocketVersions::RFC6455>::disconnectRequestSent() const
			{
				return this->connection.CloseSent;
			}
			bool WebSocket<WebSocketVersions::RFC6455>::disconnectRequestReceived() const
			{
				return this->connection.CloseReceived;
			}

			void WebSocket<WebSocketVersions::RFC6455>::refreshState()
			{
				if (this->isConnected())
				{
					//Last known state is connected (only state in which anything can change without us knowing right away)
					if (this->connection.Stream->refreshOpenState())
					{
						//Socket is still connected
						if (this->connection.LastReceived.isNull() || this->connection.LastReceived->isClosed())
						{
							//No message is currently being received
							if (!this->connection.CloseReceived)
							{
								WSMessageHeader h;
								do
								{
									h = this->connection.receiveMessageHeader(true);
									if (!h.Null)
									{
										if (!this->connection.processIfControlFrame(h))
										{
											this->connection.bufferFrame(h);
											return;
										}
									}
								} while (!h.Null && !this->connection.CloseReceived);

								if (this->connection.CloseReceived && this->connection.CloseSent)
									this->disconnect(false);
							}
							else if (this->connection.CloseSent)
								this->disconnect(false);
						}
					}
					else
						this->disconnect(false);
				}
			}
			bool WebSocket<WebSocketVersions::RFC6455>::checkConnection()
			{
				this->refreshState();
				return this->isConnected();
			}

			bool WebSocket<WebSocketVersions::RFC6455>::dataAvailable()
			{
				if (!this->isConnected())
					throw Exception("WebSocket is not connected");

				if (this->disconnectRequestReceived())
					return false;

				if (this->connection.LastReceived.isNull() || this->connection.LastReceived->isClosed())
				{
					//Should work because: refreshStates receives all available messages until either:
					//-a CloseFrame has been received => dataAvailable should return false
					//-a ContentFrame has been received, which is now buffered in connection.NextReceived => dataAvailable should return true
					this->refreshState();
					return this->isConnected() && !this->connection.NextReceived.Null;
				}

				return false;
			}


			void WebSocket<WebSocketVersions::RFC6455>::ownStream()
			{
				this->connection.OwnsStream = true;
			}
			void WebSocket<WebSocketVersions::RFC6455>::disownStream()
			{
				this->connection.OwnsStream = false;
			}

			NetworkStream* WebSocket<WebSocketVersions::RFC6455>::getStream() const
			{
				if (this->isConnected() || this->isDisconnected())
					return this->connection.Stream;
				else
					throw Exception("WebSocket is closed or a server socket, cannot get stream");
			}
		}
	}
}
