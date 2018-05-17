#include <private/Networking/Http/WSMultipartMessageBody.h>


#include <vector>

#include <StdUtils/Networking/Http/WSMessageBodyReader.h>
#include <private/Networking/Http/WSMessageHeader.h>


using namespace std;


namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{
			WSMultipartMessageBody::WSMultipartMessageBody(NetworkStream& ns, WSMessageHeader const& header, WSMessageBody::IFailConnectionMethod const& fc, IGetNextPartMethod& getNext)
				:WSMessageBody(ns, header, fc, false),
				getNextPart(getNext),
				lastPart(false),
				totalBodyLength(header.Length),
				totalProcessed(0)
			{
			}
			WSMultipartMessageBody::WSMultipartMessageBody(NetworkStream& ns, WSMessageHeader const& header, WSMessageBody::IFailConnectionMethod const& fc, WSMultipartMessageBody::ISendNextPartMethod& sendNext)
				:WSMessageBody(ns, header, fc, true),
				sendNextPart(sendNext),
				doMask(header.Masked),
				lastPart(false),
				totalBodyLength(header.Length),
				totalProcessed(0)
			{
			}

			WSMultipartMessageBody::~WSMultipartMessageBody()
			{
			}

			bool WSMultipartMessageBody::nextPart(bool nonBlocking)
			{
				WSMessageHeader wsm(this->getNextPart(nonBlocking));

				if (wsm.Null)
					return false;
				else
				{
					this->reinitialize(wsm);
					this->lastPart = wsm.Last;
					this->totalBodyLength += wsm.Length;

					return true;
				}
			}

			void WSMultipartMessageBody::write(char const* buf, uint32_t len)
			{
				this->assertOpen();

				WSMessageHeader wsh;

				wsh.Null = false;

				wsh.Last = false;
				wsh.Opcode = Opcodes::Continuation;
				wsh.Length = len;

				if (this->doMask)
					wsh.mask();

				this->totalBodyLength += len;
				this->sendNextPart(wsh);

				WSMessageBody::reinitialize(wsh);
				WSMessageBody::write(buf, len);

				this->totalProcessed += len;
			}
			void WSMultipartMessageBody::write(char c)
			{
				this->write(&c, 1);
			}

			uint32_t WSMultipartMessageBody::read(char* buffer, uint32_t bufferSize)
			{
				this->assertOpen();

				uint64_t bytesLeftInCurrentPart = WSMessageBody::getBytesToRead();
				if (bytesLeftInCurrentPart > 0)
				{
					int rv = WSMessageBody::read(buffer, bufferSize);
					this->totalProcessed += rv;
					return rv;
				}
				else if (!this->lastPart)
				{
					this->nextPart(false);

					if (this->lastPart && WSMessageBody::isReadCompleted())
						return 0; //Eine abschliessende Message ohne Inhalt ist eingetroffen - Exception nicht sinnvoll
					else
						return this->read(buffer, bufferSize);
				}
				else
					throw Exception("This body is already in EOF state");
			}
			char WSMultipartMessageBody::read()
			{
				char buf;

				this->read(&buf, 1);

				return buf;
			}

			//Only called for writing body
			void WSMultipartMessageBody::close()
			{
#ifdef _DEBUG
				if (!this->sending)
					throw Exception("DEBUG WSMultipartMessageBody::close(bool): Must not be called for a reading body");
#endif
				this->assertOpen();

				WSMessageHeader wsh;
				wsh.Null = false;

				wsh.Last = true;
				wsh.Opcode = Opcodes::Continuation;
				wsh.Length = 0;

				if (this->doMask)
					wsh.mask();

				this->sendNextPart(wsh);
				this->lastPart = true;
				this->closed = true;
			}
			//Only called for reading body
			void WSMultipartMessageBody::close(bool discardUnread)
			{
#ifdef _DEBUG
				if (this->sending)
					throw Exception("DEBUG WSMultipartMessageBody::close(bool): Must not be called for a writing body");
#endif
				this->totalProcessed += WSMessageBody::getBytesToRead();
				WSMessageBody::close(discardUnread);

				while (!this->lastPart)
				{
					this->closed = false;

					this->nextPart(false);
					this->totalProcessed += WSMessageBody::getBytesToRead();
					WSMessageBody::close(discardUnread);
				}
			}

			bool WSMultipartMessageBody::knowsBodyLength() const
			{
				return this->lastPart;
			}
			uint64_t WSMultipartMessageBody::getBodyLength() const
			{
				if (this->lastPart)
					return this->totalBodyLength;
				else
					throw Exception("This body does not know its body length");
			}

			uint64_t WSMultipartMessageBody::getBytesToWrite() const
			{
				if (this->lastPart)
					return this->totalBodyLength - this->totalProcessed;
				else
					throw Exception("This body does not know its body length");
			}
			bool WSMultipartMessageBody::isWriteCompleted() const
			{
				return this->lastPart && (this->getBytesToWrite() == 0);
			}

			uint64_t WSMultipartMessageBody::getBytesToRead() const
			{
				if (this->lastPart)
					return WSMessageBody::getBytesToRead();
				else
					throw Exception("This body does not know its body length");
			}
			bool WSMultipartMessageBody::isReadCompleted() const
			{
				return this->lastPart && WSMessageBody::isReadCompleted();
			}

			//bool WSMultipartMessageBody::canReceive()
			//{
			//	//Closed must be checked again after canReceive, because if the body turns out to be closed, the NetworkStream might have been deleted and must not be used
			//	return WSMessageBody::canReceive() || (!this->closed && !this->lastPart && this->stream.canReceive());
			//}
			bool WSMultipartMessageBody::dataAvailable()
			{
				if (!this->sending)
					return false;

				if (WSMessageBody::dataAvailable())
					return true;

				if (!this->closed && !this->lastPart)
				{
					//Try to receive next part without blocking and, if successfull, repeat this method (necessary, because the next message might have length 0)
					if (this->nextPart(true))
						return this->dataAvailable();
				}

				return false;
			}
		}
	}
}