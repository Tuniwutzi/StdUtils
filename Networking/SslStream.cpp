#ifdef STDUTILS_COMPILE_SSL
#include <StdUtils/Networking/SslStream.h>

#include <StdUtils/Exceptions.h>
#include <StdUtils/Threading/Mutex.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include <private/Networking/WaitForSockets.h>
#include <private/Networking/Sockets.h>


using namespace StdUtils::Threading;
using namespace StdUtils::Private;
using namespace StdUtils::Private::Networking;


namespace StdUtils
{
	namespace Networking
	{
		struct SSLInitializer
		{
			SSLInitializer()
			{
				SSL_library_init();
				SSL_load_error_strings();
				OpenSSL_add_all_algorithms();  //TODO: Eventuell schon in SSL_library_init gemacht, testen/nachlesen/probieren
			}
		};
		static SSLInitializer sslinit;

		SslStream::SslStream(TcpSocket* socket, bool ownsSocket)
			:TcpStream(socket, ownsSocket),
			ctx(NULL),
			ssl(NULL),
			server(false),
			closed(false)
		{
		}
		SslStream::SslStream(TcpSocket* socket, SslServerCertificate const& ssc, bool ownsSocket)
			:TcpStream(socket, ownsSocket),
			ctx(NULL),
			ssl(NULL),
			server(true),
			closed(false),
			certificate(ssc)
		{
		}

		SslStream::~SslStream()
		{
			this->close();
			if (this->ownsSocket)
			{
				delete this->socket;
				this->socket = NULL;
			}
		}

		void SslStream::loadCertificates()
		{
			SSL_CTX* c((SSL_CTX*)this->ctx);

			if (SSL_CTX_load_verify_locations(c, this->certificate.CertificateFile.data(), NULL) != 1)
				throw Exception(String("Error verifying key locations. ") + this->getErrorString());

			if (SSL_CTX_set_default_verify_paths(c) != 1)
				throw Exception(String("Error verifying key paths. ") + this->getErrorString());

			if (SSL_CTX_use_certificate_file(c, this->certificate.CertificateFile.data(), SSL_FILETYPE_PEM) != 1)
				throw Exception(String("Error loading certificate. ") + this->getErrorString());
			if (SSL_CTX_use_PrivateKey_file(c, this->certificate.PrivateKeyFile.data(), SSL_FILETYPE_PEM) != 1)
				throw Exception(String("Error loading private key. ") + this->getErrorString());
			if (SSL_CTX_check_private_key(c) != 1)
				throw Exception(String("Error checking private key. ") + this->getErrorString());
		}

		void SslStream::initialize()
		{
			if (this->closed)
				throw Exception("SslStream has already been closed");

			static Mutex lock;
			if (!this->ssl)
			{
				if (!this->socket->checkConnection())
					throw Exception("SslStream could not be initialized, the underlying socket is not connected");

				SocketHandle handle(this->socket->getHandle());

				//Set socket to non-blocking
				MakeSocketNonBlocking(handle);
				//#ifdef _WINDOWS
				//				unsigned long val = 1;
				//				int rv = ioctlsocket(handle, FIONBIO, &val);
				//				if (SOCKETERROR(rv))
				//					throw OSApiException("Error putting socket in non-blocking mode", LASTSOCKETERROR);
				//#else
				//				int flags = fcntl(this->handle, F_GETFL, 0); //Gesetzte flags lesen
				//				if (flags == -1)
				//					throw OSApiException("Error reading existing flags of tcp socket", errno);
				//				flags = flags | O_NONBLOCK; //nonblock flag setzen
				//				if (fcntl(this->handle, F_SETFL, flags) == -1)
				//					throw OSApiException("Error putting socket in non-blocking mode", errno);
				//#endif

				STDUTILS_SYNCHRONIZE(lock);


				static SSL_METHOD* serverMethod(TLSv1_server_method());
				static SSL_METHOD* clientMethod(TLSv1_client_method());

				this->ctx = SSL_CTX_new(this->server ? serverMethod : clientMethod);
				if (!this->ctx)
					throw Exception(String("SslStream could not be initialized, error creating ssl context: ") + this->getErrorString());

				//Seems to do nothing whith async sockets
				//SSL_CTX_set_mode((SSL_CTX*)this->ctx, SSL_MODE_AUTO_RETRY); //Make SSL_read and write automatically do any renegotiations of the connection if necessary

				try
				{
					if (this->server)
						this->loadCertificates();

					this->ssl = SSL_new((SSL_CTX*)this->ctx);
					if (!this->ssl)
						throw Exception(String("SslStream could not be initialized. Error creating ssl environment: ") + this->retrieveErrorFromQueue());

					if (SSL_set_fd((SSL*)this->ssl, (int)handle) != 1)
						throw Exception(String("SslStream could not be initialized. Error using the filedescriptor of the socket: ") + this->retrieveErrorFromQueue());

					this->doOpen();
				}
				catch (...)
				{
					//TODO: Laesst u.U. die Socket im Async-Modus zurueck auch wenn sie es vorher nicht war
					if (this->ssl)
					{
						SSL_free((SSL*)this->ssl);
						this->ssl = NULL;
					}

					SSL_CTX_free((SSL_CTX*)this->ctx);
					this->ctx = NULL;

					throw;
				}
			}
		}
		String SslStream::retrieveErrorFromQueue()
		{
			String rv;

			unsigned long e = ERR_get_error();
			if (e != 0)
			{
				char const* ptr = ERR_reason_error_string(e);
				if (ptr)
				{
					rv += " " + String(ptr) + " (library: ";

					ptr = ERR_lib_error_string(e);
					if (ptr)
						rv += ptr + String(")");
				}
			}

			return rv;
		}

		String SslStream::resolveSslError(int sslError, int returnvalue)
		{
			String rv;
			bool checkQueue = false;

			switch (sslError)
			{
			case SSL_ERROR_NONE:
				rv = "No error";
				break;
			case SSL_ERROR_ZERO_RETURN:
				rv = "TLS/SSL connection has been closed.";
				break;
			case SSL_ERROR_WANT_READ:
			case SSL_ERROR_WANT_WRITE:
			case SSL_ERROR_WANT_CONNECT:
			case SSL_ERROR_WANT_ACCEPT:
				rv = "Non-blocking operation could not be completed. This should have been handled elsewhere, likely an error in StdUtils";
				break;
			case SSL_ERROR_WANT_X509_LOOKUP:
				rv = "Unexpected error code, likely an error in StdUtils";
				break;
			case SSL_ERROR_SYSCALL:
				rv = "IO error occurred.";
				checkQueue = true;
				break;
			case SSL_ERROR_SSL:
				rv = "Failure in SSL library, likely a protocol issue.";
				checkQueue = true;
				break;
			default:
				rv = "Unknown error occurred.";
				break;
			}

			if (checkQueue)
			{
				String queuedError = this->retrieveErrorFromQueue();
				if (queuedError.size() == 0)
				{
					if (sslError == SSL_ERROR_SYSCALL)
					{
						if (returnvalue == 0)
							rv += " Protocol violated by EOF";
						else if (returnvalue == -1)
							rv += " Unterlying BIO reported IO error"; //TODO: Doku sagt 'For Socket I/O on Unix systems consult errno for details'
					}
				}
				else
					rv += " " + queuedError;
			}

			return rv;
		}
		String SslStream::getErrorString(int errval)
		{
			if (this->ssl)
				return this->resolveSslError(SSL_get_error((SSL*)this->ssl, errval), errval);
			else
				return this->retrieveErrorFromQueue();
		}

		bool SslStream::resolveWants(int error)
		{
			SocketWaitData node;
			node.H = this->socket->getHandle();
			node.Input = error == SSL_ERROR_WANT_READ;
			node.Output = error == SSL_ERROR_WANT_WRITE;

			if (node.Input || node.Output)
			{
#ifdef _DEBUG
				if (WaitForSockets(&node, 1, 0) == 0)
					throw Exception("DEBUG: SslStream::doWrite: WaitForIO had unexpected timeout");
#else
				WaitForSockets(&node, 1, 0);
#endif

				return true;
			}

			return false;
		}

		void SslStream::doOpen()
		{
			int rv;
			if ((rv = (this->server ? SSL_accept((SSL*)this->ssl) : SSL_connect((SSL*)this->ssl))) != 1)
			{
				int error = SSL_get_error((SSL*)this->ssl, rv);

				if (this->resolveWants(error))
					this->doOpen();
				else
					throw Exception(String("SslStream could not be initialized. Error establishing ssl connection: ") + this->resolveSslError(error, rv));
			}
		}
		int SslStream::doWrite(char const* b, int l)
		{
			//TODO: rekursion entfernen

			int rv = SSL_write((SSL*)this->ssl, b, l);

			if (rv > 0)
			{
#ifdef _DEBUG
				if (rv != l)
					throw Exception("DEBUG SslStream::write: SSL_write was successfull, but did not write all bytes in buffer. The documentation stated that it would and SslStream::write depends on it");
#endif
				return rv;
			}
			else if (rv == 0)
			{
				this->close();
				throw Exception(String("Error writing, connection was closed. ") + this->getErrorString(rv));
			}
			else
			{
				int error = SSL_get_error((SSL*)this->ssl, rv);

				if (this->resolveWants(error))
				{
					//At this point, either the ssl connection has been closed or is ready for writing/reading
					//If it has been closed, the doWrite-call will get a return value of 0, causing it to throw
					return this->doWrite(b, l);
				}
				else
					throw Exception(String("Error writing. ") + this->resolveSslError(error, rv));
			}
		}
		int SslStream::doReceive(char* buffer, int length, bool peek, bool mayBlock, bool throwWhenDisconnected)
		{
			int rv = (peek ? SSL_peek((SSL*)this->ssl, buffer, length) : SSL_read((SSL*)this->ssl, buffer, length));
			if (rv > 0)
				return rv;
			else
			{
				if (rv == 0) //=> Connection closed. Either cleanly (SSL_get_error(rv) == SSL_ERROR_ZERO_RETURN) or not
				{
					this->close();

					if (throwWhenDisconnected)
						throw Exception(String("Connection closed. ") + this->getErrorString(rv));
					//TODO: evtl werfen, wenn nicht sauber geschlossen? oder irgendwie loggen? dunno...
				}
				else if (mayBlock)
				{
					int error = SSL_get_error((SSL*)this->ssl, rv);
					if (this->resolveWants(error))
					{
						//At this point, either the ssl connection has been closed or is ready for writing/reading
						//If it has been closed, the doRead-call will get a return value of 0, causing it to throw
						return doReceive(buffer, length, peek, mayBlock, throwWhenDisconnected);
					}
					else
						throw Exception(String("Error receiving data. ") + this->getErrorString(rv));
				}
				else
					return 0;
			}
		}

		void SslStream::close()
		{
			//TODO?: Exception, wenn er gerade in WaitForIO ist (Vielleicht eher nicht; Wenn ich das schliesse, waehrend ich woanders noch sende/empfange habe ich doch eh mist gebaut?)
			if (this->ssl)
			{
				if (this->socket->checkConnection())
				{
					int shutdownState = SSL_get_shutdown((SSL*)this->ssl);
					if ((shutdownState & SSL_SENT_SHUTDOWN) == 0)
					{
						//regarding the following lines; doc says: 0 => shutdown not finished, call again if bidirectional shutdown shall be performed. SSL_get_error is useless (paraphrased)
						//dafuq? just try thrice and give up
						int tries = 3;
						while (tries-- > 0 && SSL_shutdown((SSL*)this->ssl) == 0);
					}
				}

				SSL_free((SSL*)this->ssl);
				SSL_CTX_free((SSL_CTX*)this->ctx);

				this->ssl = NULL;
				this->ctx = NULL;

				this->socket->close();

				this->closed = true;
			}
		}

		void SslStream::write(char data)
		{
			this->write(&data, 1);
		}
		void SslStream::write(char const* data, uint32_t length)
		{
			this->initialize();

			uint32_t written = 0;
			while ((length - written) > INT32_MAX)
				written += doWrite(data + written, INT32_MAX);

			this->doWrite(data + written, (int)(length - written));
		}

		char SslStream::read()
		{
			char buf;

			this->read(&buf, 1);

			return buf;
		}
		uint32_t SslStream::read(char* buffer, uint32_t length)
		{
			this->initialize();

			if (length <= INT32_MAX)
				return this->doReceive(buffer, length, false, true, true);
			else
			{
				uint32_t haveRead = this->doReceive(buffer, INT32_MAX, false, true, true);
				if (haveRead == INT32_MAX)
					haveRead += this->doReceive(buffer + INT32_MAX, length - INT32_MAX, false, false, true);

				return haveRead;
			}
		}

		bool SslStream::dataAvailable()
		{
			this->initialize();

			int pending = SSL_pending((SSL*)this->ssl);
			if (pending <= 0)
			{
				static char buf[1];
				pending = doReceive(buf, sizeof(buf), true, false, false);
			}

			return pending > 0;
		}
	}
}
#endif
