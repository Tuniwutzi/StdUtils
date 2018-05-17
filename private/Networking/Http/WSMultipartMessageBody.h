#ifndef STDUTILS_NETWORKING_HTTP_WSMULTIPARTMESSAGEBODY_H
#define STDUTILS_NETWORKING_HTTP_WSMULTIPARTMESSAGEBODY_H


#include <private/Networking/Http/WSMessageBody.h>
#include <private/Networking/Http/WSRFC6455Connection.h>
#include <StdUtils/Networking/Http/WSMessageTypes.h>


namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{
			class WSMultipartMessageBody : public WSMessageBody
			{
				friend WSRFC6455Connection;

			protected:
				typedef Delegate<WSMessageHeader, bool> GetNextPartMethod;
				typedef IDelegate<WSMessageHeader, bool> IGetNextPartMethod;

				typedef Delegate<void, WSMessageHeader const&> SendNextPartMethod;
				typedef IDelegate<void, WSMessageHeader const&> ISendNextPartMethod;

			protected:
				GetNextPartMethod getNextPart;
				SendNextPartMethod sendNextPart;
				bool doMask; //only relevant if sending == true

				bool lastPart;

				uint64_t totalBodyLength;
				uint64_t totalProcessed;

			protected:
				explicit WSMultipartMessageBody(NetworkStream&, WSMessageHeader const&, IFailConnectionMethod const&, IGetNextPartMethod&);
				explicit WSMultipartMessageBody(NetworkStream&, WSMessageHeader const&, IFailConnectionMethod const&, ISendNextPartMethod&);

			public:
				virtual ~WSMultipartMessageBody();

			private:
				bool nextPart(bool);

			public:
				virtual void write(char const*, uint32_t);
				virtual void write(char);

				virtual uint32_t read(char*, uint32_t bufferSize);
				virtual char read();

				virtual void close();
				virtual void close(bool discardRemaining);

				virtual bool knowsBodyLength() const;
				virtual uint64_t getBodyLength() const;

				virtual uint64_t getBytesToWrite() const;
				virtual bool isWriteCompleted() const;

				virtual uint64_t getBytesToRead() const;
				virtual bool isReadCompleted() const;
				
				//virtual bool canReceive();
				virtual bool dataAvailable();
			};
		}
	}
}

#endif