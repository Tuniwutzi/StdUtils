#include <StdUtils/Networking/Http/WSMessageBodyReader.h>


#include <private/Networking/Http/WSMessageBody.h>


namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{
			WSMessageBodyReader::WSMessageBodyReader(SharedPointer<WSMessageBody> const& b)
				:ptr(b),
				bodyRaw(b.getRawPointer())
			{
			}

			WSMessageBodyReader::WSMessageBodyReader()
			{
			}

			void WSMessageBodyReader::assertNotNull() const
			{
				if (this->isNull())
					throw Exception("This reader is null");
			}

			uint32_t WSMessageBodyReader::read(char* buf, uint32_t bufferSize)
			{
				this->assertNotNull();

				return this->bodyRaw->read(buf, bufferSize);
			}
			char WSMessageBodyReader::read()
			{
				this->assertNotNull();

				return this->bodyRaw->read();
			}

			void WSMessageBodyReader::close(bool discard)
			{
				this->assertNotNull();

				this->bodyRaw->close(discard);
			}
			bool WSMessageBodyReader::isClosed() const
			{
				this->assertNotNull();

				return this->bodyRaw->isClosed();
			}

			WSMessageTypes::Value WSMessageBodyReader::getMessageType() const
			{
				this->assertNotNull();

				return this->bodyRaw->getMessageType();
			}

			bool WSMessageBodyReader::knowsBodyLength() const
			{
				this->assertNotNull();

				return this->bodyRaw->knowsBodyLength();
			}
			uint64_t WSMessageBodyReader::getBodyLength() const
			{
				this->assertNotNull();

				return this->bodyRaw->getBodyLength();
			}

			uint64_t WSMessageBodyReader::getBytesToRead() const
			{
				this->assertNotNull();

				return this->bodyRaw->getBytesToRead();
			}
			bool WSMessageBodyReader::isReadCompleted() const
			{
				this->assertNotNull();

				return this->bodyRaw->isReadCompleted();
			}

			bool WSMessageBodyReader::dataAvailable()
			{
				this->assertNotNull();

				return this->bodyRaw->dataAvailable();
			}

			bool WSMessageBodyReader::isNull() const
			{
				return this->ptr.isNull();
			}
		}
	}
}