#include <private/Networking/Http/HttpBodyChunked.h>


#include <StdUtils/Exceptions.h>
#include <StdUtils/NumberFormat.h>


namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{
			HttpBodyChunked::HttpBodyChunked(NetworkStream& s, bool receiving)
				:HttpBody(s, receiving)
			{
			}
			HttpBodyChunked::~HttpBodyChunked()
			{
				try
				{
					if (this->isOpen())
						this->close();
				}
				catch (...)
				{
				}
			}

			bool HttpBodyChunked::knowsWriteableBytes() const STDUTILS_OVERRIDE
			{
				this->assertWrite();
				return false;
			}
			uint64_t HttpBodyChunked::getWriteableBytes() const STDUTILS_OVERRIDE
			{
				this->assertWrite();
				throw Exception("Writeable bytes unknown");
			}
			void HttpBodyChunked::write(String const& str) STDUTILS_OVERRIDE
			{
				this->write(str.data(), str.size());
			}
			void HttpBodyChunked::write(char const* buf, uint64_t length) STDUTILS_OVERRIDE
			{
				this->assertWrite();

				if (length == 0)
					this->close();
				else
				{
					static String newline("\r\n");
					String size(NumberFormat::ToHex(length));

					this->socket.write(size.data(), (uint32_t)size.length());
					this->socket.write(newline.data(), (uint32_t)newline.size());
					this->socket.write(buf, length); //TODO!: No cast because length might be bigger than uint32_t -> Bug!
					this->socket.write(newline.data(), (uint32_t)newline.size());
				}
			}

			bool HttpBodyChunked::knowsReadableBytes() const STDUTILS_OVERRIDE
			{
				this->assertRead();
				return false;
			}
			uint64_t HttpBodyChunked::getReadableBytes() const STDUTILS_OVERRIDE
			{
				this->assertRead();
				throw Exception("Readable bytes unknown");
			}

			//Wichtig: Nicht unkontrolliert lesen, auf moegliche zu große Nachrichten u.s.w. achten, damit kein RAM ueberlauf passieren kann
			String HttpBodyChunked::read() STDUTILS_OVERRIDE
			{
				throw Exception("Reading side of HttpBodyChunked not implemented");
			}
			//Wichtig: Nicht unkontrolliert lesen, auf moegliche zu große Nachrichten u.s.w. achten, damit kein RAM ueberlauf passieren kann
			uint64_t HttpBodyChunked::read(char* buf, uint64_t bufferSize) STDUTILS_OVERRIDE
			{
				throw Exception("Reading side of HttpBodyChunked not implemented");
			}

			bool HttpBodyChunked::isFinished() const STDUTILS_OVERRIDE
			{
				if (this->isWritingBody())
					return !this->isOpen();
				else
					throw Exception("Reading side of HttpBodyChunked not implemented"); //Should be the same as it is for writing, but we will see when we implement read(...) methods

			}

			void HttpBodyChunked::close() STDUTILS_OVERRIDE
			{
				if (this->isOpen() && this->isWritingBody())
				{
					static String endstream("0\r\n\r\n");
					this->socket.write(endstream.data(), (uint32_t)endstream.size());
				}

				HttpBody::close();
			}
		}
	}
}