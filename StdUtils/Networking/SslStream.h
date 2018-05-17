#ifndef STDUTILS_NETWORKING_SSLSTREAM_H
#define STDUTILS_NETWORKING_SSLSTREAM_H

#include <StdUtils/Networking/TcpStream.h>

#ifndef STDUTILS_COMPILE_SSL
#ifndef _WINDOWS
#warning Define STDUTILS_COMPILE_SSL to use SslStream
#else
#pragma message("Define STDUTILS_COMPILE_SSL to use SslStream")
#endif
#endif

namespace StdUtils
{
	namespace Networking
	{
		struct SslServerCertificate
		{
			String CertificateFile;
			String PrivateKeyFile;
		};
		class SslStream : public TcpStream
		{
			STDUTILS_NOCOPY(SslStream);

		private:
			void* ctx;
			void* ssl;

			bool server;
			SslServerCertificate certificate;

			bool closed;

		public:
			//TODO: Hier wird nicht eindeutig klar, dass ein Konstruktor exklusiv fuer Clients ist, der andere exklusiv fuer Server
			//Eventuell stattdessen statische Methoden ::Client(...), ::Server(...)?
			SslStream(TcpSocket*, bool ownsSocket = true); //Client
			SslStream(TcpSocket*, SslServerCertificate const&, bool ownsSocket = true); //Server
			virtual ~SslStream();

		private:
			void loadCertificates();
			void initialize();
			String retrieveErrorFromQueue();
			String resolveSslError(int, int);
			String getErrorString(int = 1);

			bool resolveWants(int);

			void doOpen();
			int doWrite(char const*, int);
			int doReceive(char*, int, bool peek, bool mayBlock, bool throwWhenDisconnected);

		public:
			virtual void close();

			virtual void write(char data);
			virtual void write(char const* data, uint32_t length);

			virtual char read();
			virtual uint32_t read(char* buffer, uint32_t bufferSize);

			virtual bool dataAvailable() STDUTILS_OVERRIDE;
		};
	}
}

#endif
