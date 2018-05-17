#include <StdUtils/Networking/Http/WSMessageBodyWriter.h>


#include <private/Networking/Http/WSMessageBody.h>
#include <private/Networking/Http/WSMultipartMessageBody.h>


namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{
			WSMessageBodyWriter::WSMessageBodyWriter(SharedPointer<WSMessageBody> const& b)
				:ptr(b),
				bodyRaw(b.getRawPointer())
			{
			}

			WSMessageBodyWriter::WSMessageBodyWriter()
			{
			}

			void WSMessageBodyWriter::assertNotNull() const
			{
				if (this->isNull())
					throw Exception("This writer is null");
			}

			void WSMessageBodyWriter::write(char const* buf, uint32_t len)
			{
				this->assertNotNull();

				return this->bodyRaw->write(buf, len);
			}
			void WSMessageBodyWriter::write(char c)
			{
				this->assertNotNull();

				return this->bodyRaw->write(c);
			}

			void WSMessageBodyWriter::close()
			{
				this->assertNotNull();

				this->bodyRaw->close();
			}
			bool WSMessageBodyWriter::isClosed() const
			{
				this->assertNotNull();

				return this->bodyRaw->isClosed();
			}

			WSMessageTypes::Value WSMessageBodyWriter::getMessageType() const
			{
				this->assertNotNull();

				return this->bodyRaw->getMessageType();
			}

			bool WSMessageBodyWriter::knowsBodyLength() const
			{
				this->assertNotNull();

				return this->bodyRaw->knowsBodyLength();
			}
			uint64_t WSMessageBodyWriter::getBodyLength() const
			{
				this->assertNotNull();

				return this->bodyRaw->getBodyLength();
			}

			uint64_t WSMessageBodyWriter::getBytesToWrite() const
			{
				this->assertNotNull();

				return this->bodyRaw->getBytesToWrite();
			}
			bool WSMessageBodyWriter::isWriteCompleted() const
			{
				this->assertNotNull();

				return this->bodyRaw->isWriteCompleted();
			}

			bool WSMessageBodyWriter::isNull() const
			{
				return this->ptr.isNull();
			}
		}
	}
}