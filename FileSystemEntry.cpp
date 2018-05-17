#include <StdUtils/FileSystem/FileSystemEntry.h>


#include <stdlib.h>

#include <StdUtils/Exceptions.h>
#include <StdUtils/FileSystem/Path.h>
#include <StdUtils/FileSystem/Directory.h>

#ifdef _WINDOWS
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif


namespace StdUtils
{
	namespace FileSystem
	{
#ifdef _WINDOWS
#else
	bool getStats(char const* path, struct stat* fi)
    {
		static struct stat fiBuf;
		if (!fi)
			fi = &fiBuf;
    	int rv = lstat(path, fi);
    	if (rv == 0)
    		return true;
    	else if (errno == ENOENT)
    		return false;
    	else
    		throw OSApiException("Error getting file info", errno);
    }
#endif

		FileSystemEntry::FileSystemEntry(Type type, String const& path)
			:type(type), path(path)
		{
		}

        bool FileSystemEntry::operator==(FileSystemEntry const& other) const
        {
            if (this->type != other.type)
                return false;
            else if (this->isNull() && other.isNull())
                return true;

            String a(this->getFullPath());
            String b(other.getFullPath());
            return this->getFullPath() == other.getFullPath();
        }
        bool FileSystemEntry::operator!=(FileSystemEntry const& other) const
        {
            return !(*this == other);
        }

        void FileSystemEntry::assertValid() const
        {
            if (!this->isValid())
                throw Exception("FileSystemEntry is invalid");
        }

        FileSystemEntry::Type FileSystemEntry::getType() const
        {
            return this->type;
        }

        String FileSystemEntry::getFullPath() const
        {
            return Path::ToAbsolute(this->getPath());
        }
		String const& FileSystemEntry::getPath() const
		{
			return this->path;
		}
        Directory FileSystemEntry::getParent() const
        {
            this->assertValid();

            String fullPath = this->getFullPath();

#ifdef _WINDOWS
            //TODO: Verbessern; Mehrere Slashes/Backslashes, insbesodere vermischt, am Ende des Pfads machen alles putt
            String::size_type backPos = fullPath.find_last_of('\\');
            String::size_type slashPos = fullPath.find_last_of('/');
            if (backPos == String::npos || slashPos == fullPath.size() - 1)
                backPos = -1;
            if (slashPos == String::npos || slashPos == fullPath.size() - 1)
                slashPos = -1;

            String::size_type pos = max(backPos, slashPos);
            if (pos == -1)
                return Directory();
            else
                return Directory(fullPath.substr(0, pos));
#else
            String::size_type slashPos;
            while ((slashPos = fullPath.find_last_of('/')) == fullPath.size() - 1)
            	fullPath = fullPath.substr(0, slashPos);

            if (slashPos == String::npos)
            	return Directory();
            else
            	return Directory(fullPath.substr(0, slashPos));
#endif
        }

        bool FileSystemEntry::exists() const
        {
            this->assertValid();

            return FileSystemEntry::Exists(this->getPath());
        }

        bool FileSystemEntry::Exists(String const& path)
        {
#ifdef _WINDOWS
            DWORD attribs = GetFileAttributesA(path.data());
            return attribs != INVALID_FILE_ATTRIBUTES;
#else
            return getStats(path.data(), NULL);
#endif
        }
	}
}
