#include <StdUtils/Networking/TcpSocket.h>

#include <StdUtils/Exceptions.h>

#include <private/Networking/Sockets.h>
#include <private/Networking/SocketStatus.h>
#include <private/Networking/IPInterface.h>

using namespace StdUtils;
using namespace StdUtils::Networking;
using namespace StdUtils::Private::Networking;

//Note: isBound returns true, if Socket::status is SocketStatus::Listening or SocketStatus::Bound
//Internally, these are to distinct states

Socket<SocketTypes::Tcp>::Socket()
	: ownsHandle(true),
	status(SocketStatus::Closed)
{
}

Socket<SocketTypes::Tcp>::~Socket()
{
	this->close();
}

static SocketHandle createTcpSocketHandle()
{
	SocketHandle rv = CREATESOCKET(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (SOCKETERROR(rv))
		throw OSApiException("Error creating socket handle", LASTSOCKETERROR);

	return rv;
}
//void Socket<SocketTypes::Tcp>::createHandle()
//{
//	this->createSocketHandle(&createTcpSocketHandle);
//	this->ownsHandle = true;
//}
static void closeTcpSocketHandle(SocketHandle h)
{
	CLOSESOCKET(h);
}
void Socket<SocketTypes::Tcp>::resetHandle()
{
	SocketHandle closeable = this->resetSocketHandle();
	if (closeable && this->ownsHandle) //TODO!: Portablere Loesung um auf handle == 0 zu kontrollieren
		closeTcpSocketHandle(closeable);
}

void Socket<SocketTypes::Tcp>::assertConnected() const
{
	if (this->status != SocketStatus::Connected)
		throw Exception("This socket is not connected");
}

void Socket<SocketTypes::Tcp>::close()
{
	this->resetHandle();

	this->status = SocketStatus::Closed;

	this->localEndpoint = Endpoint();
	this->remoteEndpoint = Endpoint();
}

void Socket<SocketTypes::Tcp>::connect(Endpoint const& ep)
{
	if (this->status != SocketStatus::Closed && this->status != SocketStatus::Disconnected)
		throw Exception("Socket must be closed or disconnected to connect.");

	SocketHandle h = createTcpSocketHandle();
	try
	{
		Endpoint lEndpoint;
		switch (ep.getAddress().getType())
		{
		case AddressTypes::IPv4:
			IPInterface<AddressTypes::IPv4>::connectToRemoteEndpoint(h, ep);
			lEndpoint = IPInterface<AddressTypes::IPv4>::getLocalEndpoint(h);
			break;
		default:
			throw Exception("Unknown AddressType");
			break;
		}

		this->setSocketHandle(h); //Possibly throws an exception, causing the connection to be established and closed right away, instead of tidily checking whether this instance is being monitored or not beforehand. Not horrible, because if this throws, it means a state-changing method was called while the socket was being actively monitored in a different thread. This is not allowed, see Socket.h
		this->remoteEndpoint = ep;
		this->localEndpoint = lEndpoint;
		this->status = SocketStatus::Connected;
	}
	catch (...)
	{
		closeTcpSocketHandle(h);
		throw;
	}
}
void Socket<SocketTypes::Tcp>::connect(IPAddress const& ip, unsigned short port)
{
	this->connect(Endpoint(ip, port));
}

void Socket<SocketTypes::Tcp>::disconnect()
{
	if (this->status == SocketStatus::Bound)
		throw Exception("This socket is bound. It may be closed but not disconnected");
	if (this->status == SocketStatus::Listening)
		throw Exception("This socket is listening. It may be closed but not disconnected");
	if (this->status == SocketStatus::Closed)
		throw Exception("This socket is closed.");

	if (this->status == SocketStatus::Connected)
		this->resetHandle(); //TODO!: Must ignore ownsHandle, otherwise the connection is not actually closed. If disconnect is explicitly called, the disconnect must happen regardless of handle ownership
	this->status = SocketStatus::Disconnected;
}

void Socket<SocketTypes::Tcp>::bind(Endpoint const& ep)
{
	if (this->status != SocketStatus::Closed)
		throw Exception("Socket must be closed to be bound");

	SocketHandle h = createTcpSocketHandle();
	try
	{
		this->bindWithHandle(ep, h);
	}
	catch (...)
	{
		closeTcpSocketHandle (h);
		throw;
	}
}
void Socket<SocketTypes::Tcp>::bind(unsigned short port, IPAddress const& ip)
{
	this->bind(Endpoint(ip, port));
}
void Socket<SocketTypes::Tcp>::bindWithHandle(Endpoint const& ep, SocketHandle h)
{
	if (this->status != SocketStatus::Closed)
		throw Exception("Socket must be closed to be bound");

	switch (ep.getAddress().getType())
	{
	case AddressTypes::IPv4:
		{
			IPInterface<AddressTypes::IPv4>::bindToLocalEndpoint(h, ep);
			//Refresh local endpoint - this could have changed if for example port 0 was specified => OS chooses a free port
			Endpoint local(IPInterface<AddressTypes::IPv4>::getLocalEndpoint(h));

			this->setSocketHandle(h); //Possibly throws an exception, causing the connection to be established and closed right away, instead of tidily checking whether this instance is being monitored or not beforehand. Not horrible, because if this throws, it means a state-changing method was called while the socket was being actively monitored in a different thread. This is not allowed, see Socket.h
			this->status = SocketStatus::Bound;
			this->localEndpoint = local;
		}
		break;
	default:
		throw Exception("Unknown AddressType");
	}
}

static bool doListen(SocketHandle handle, int bl)
{
	return !SOCKETERROR(listen(handle, bl));
}
void Socket<SocketTypes::Tcp>::listen(int backlog)
{
	if (this->status != SocketStatus::Bound)
		throw Exception("Socket must be bound to listen");

	if (!doListen(this->handle, backlog))
		throw OSApiException("Could not set socket to listening state", LASTSOCKETERROR);
	else
		this->status = SocketStatus::Listening;
}

void Socket<SocketTypes::Tcp>::accept(Socket<SocketTypes::Tcp>& buffer)
{
	if (this->status != SocketStatus::Listening)
		throw Exception("Socket must be listening to accept");

	if (buffer.status != SocketStatus::Closed)
		throw Exception("Buffer socket must be closed. Use a newly created instance or call close on it first.");

	Endpoint rEndpoint;
	SocketHandle newSocket;
	Endpoint lEndpoint;
	switch (this->localEndpoint.getAddress().getType())
	{
	case AddressTypes::IPv4:
		newSocket = IPInterface<AddressTypes::IPv4>::acceptOnLocalEndpoint(this->handle, this->localEndpoint, &rEndpoint);
		try
		{
			lEndpoint = IPInterface<AddressTypes::IPv4>::getLocalEndpoint(newSocket);
		}
		catch (...)
		{
			closeTcpSocketHandle(newSocket);
			throw;
		}
		break;
	default:
		throw Exception("Bound to unknown address type");
	}

	try
	{
		buffer.setSocketHandle(newSocket);
		buffer.status = SocketStatus::Connected;
		buffer.remoteEndpoint = rEndpoint;
		buffer.localEndpoint = lEndpoint;
	}
	catch (...)
	{
		//Extremely unlikely. Means, the closed buffer socket was actively monitored in a different thread (probably in an infinite loop, since a SocketMonitor::waitForUpdates-call returns immediately,
		//if it contains a closed socket. This, however, is not allowed, see Note in Socket.h, so it is valid to handle this error by closing the handle and throwing.
		closeTcpSocketHandle(newSocket);
		throw;
	}
}
Socket<SocketTypes::Tcp>* Socket<SocketTypes::Tcp>::accept()
{
	Socket<SocketTypes::Tcp>* rv = new Socket<SocketTypes::Tcp>();
	try
	{
		accept(*rv);
	}
	catch (...)
	{
		delete rv;
		throw;
	}
	return rv;
}

uint32_t Socket<SocketTypes::Tcp>::getReceivableBytes()
{
	this->assertConnected();

	u_long availableData = 0;

#ifdef _WINDOWS
	int returnvalue = ioctlsocket(this->handle, FIONREAD, &availableData);
#else
	int returnvalue = ioctl(this->handle, FIONREAD, &availableData);
#endif

	if (SOCKETERROR(returnvalue))
	{
		this->close();
		throw OSApiException("Error checking available bytes, socket closed", LASTSOCKETERROR);
	}

	return availableData;
}
bool Socket<SocketTypes::Tcp>::dataAvailable()
{
	return this->getReceivableBytes() > 0;
}

static int doSend(SocketHandle h, char* buf, uint32_t len, int flags = 0)
{
	if (len > INT32_MAX)
		throw Exception("Buffer sizes bigger than INT32_MAX not implemented"); //TODO: Implement
	return send(h, buf, (int) len, flags);
}
uint32_t Socket<SocketTypes::Tcp>::send(char const* data, uint32_t length)
{
	this->assertConnected();

	uint32_t totalSent = 0;
	while (totalSent < length)
	{
		int sent = doSend(this->handle, (char*) data + totalSent, length - totalSent, 0);
		if (SOCKETERROR(sent))
		{
			this->close();
			throw OSApiException("Could not send data", LASTSOCKETERROR);
		}
		else
			totalSent += sent;
	}

	return totalSent;
}

uint32_t Socket<SocketTypes::Tcp>::receive(char* buffer, uint32_t bufferSize)
{
	this->assertConnected();

	if (bufferSize > INT32_MAX)
		throw Exception("buffer sizes bigger than INT32_MAX not implemented"); //TODO: Implementieren!

	int amount = recv(this->handle, buffer, (int) bufferSize, 0);
	//Manche Linuxversionen geben einen Fehler zurueck mit errno 104 (Connection reset by peer), wenn die Verbindung geschlossen wurde#ifndef _WINDOWS	if (SOCKETERROR(amount) && LASTSOCKETERROR == 104)		amount	= 0;#endif
	if (SOCKETERROR(amount))
	{
		this->close();
		throw OSApiException("Error receiving from socket", LASTSOCKETERROR);
	}
	else if (amount == 0)
	{
		this->close();
		throw Exception("Remote host closed connection");
	}

	return amount;
}

bool Socket<SocketTypes::Tcp>::isClosed() const
{
	return this->status == SocketStatus::Closed;
}
bool Socket<SocketTypes::Tcp>::isConnected() const
{
	return this->status == SocketStatus::Connected;
}
bool Socket<SocketTypes::Tcp>::isDisconnected() const
{
	return this->status == SocketStatus::Disconnected;
}
bool Socket<SocketTypes::Tcp>::isBound() const
{
	return this->status == SocketStatus::Bound || this->status == SocketStatus::Listening;
}
bool Socket<SocketTypes::Tcp>::isListening() const
{
	return this->status == SocketStatus::Listening;
}

void Socket<SocketTypes::Tcp>::refreshState()
{
	if (this->isConnected())
	{
		fd_set chk;
		FD_ZERO(&chk);
		FD_SET(this->handle, &chk);

		timeval t;
		t.tv_sec = 0;
		t.tv_usec = 0;

		int result = select(((int) this->handle) + 1, &chk, NULL, &chk, &t);
		if (!SOCKETERROR(result))
		{
			if (result > 0 && FD_ISSET(this->handle, &chk))
			{
				if (!this->dataAvailable())
					this->close();
			}
			//else: result is 0 => Timelimit expired => Socket is still connected, but has not sent anything
		}
		else
		{
			this->close();
			throw OSApiException("Error refreshing socket state", LASTSOCKETERROR);
		}
	}
}
bool Socket<SocketTypes::Tcp>::checkConnection()
{
	this->refreshState();
	return this->isConnected();
}

Endpoint const& Socket<SocketTypes::Tcp>::getLocalEndpoint() const
{
	//TODO: Temporaer. Verbundene Socket hat auch einen Endpoint (vmtl. IP::Any, aber jfeden nen Port)
	//if (this->status == SocketStatus::Connected)
	//	throw Exception("Local endpoint for connected socket not implemented yet");

	//localEndpoint ist immer null, es sei denn die Socket ist bound
	return this->localEndpoint;
}
Endpoint const& Socket<SocketTypes::Tcp>::getRemoteEndpoint() const
{
	return this->remoteEndpoint;
}

void Socket<SocketTypes::Tcp>::ownHandle()
{
	this->ownsHandle = true;
}
void Socket<SocketTypes::Tcp>::disownHandle()
{
	this->ownsHandle = false;
}

SocketHandle Socket<SocketTypes::Tcp>::CreateHandle()
{
	return createTcpSocketHandle();
}