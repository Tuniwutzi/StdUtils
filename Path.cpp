#include <StdUtils/FileSystem/Path.h>

#ifdef _WINDOWS
#include <Shlwapi.h>
#else
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#endif

#include <StdUtils/Exceptions.h>


using namespace std;


namespace StdUtils
{
	namespace FileSystem
	{
		String Path::Combine(String const& pathA, String const& pathB)
		{
			String rv(pathA);

#ifdef _WINDOWS
            if (pathA.find_last_of('/') != pathA.size() -1 && pathA.find_last_of('\\') != pathA.size() - 1)
			{
				if (pathB.find('/') != 0 && pathB.find('\\') != 0)
				{
                    if (pathA.find('/') != String::npos)
						rv += "/";
					else
						rv += "\\";
				}
			}

			rv += pathB;
#else
			if (pathA.find_last_of('/') != pathA.size() - 1)
			{
				if (pathB.find('/') != 0)
					rv += '/';
			}
			else if (pathB.find('/') == 0)
				rv = rv.substr(0, rv.size() - 1);

			rv += pathB;
#endif

			return rv;
		}


        bool Path::IsAbsolute(String const& path)
        {
#ifdef _WINDOWS
            //if (path.size() >= 2)
            //{
            //    char second = path.at(1);
            //    return second == ':';
            //}
            //return false;
            return PathIsRelativeA(path.data()) == FALSE;
#else
            return path.find('/') == 0;
#endif
        }
        String Path::ToAbsolute(String const& path)
        {
#ifdef _WINDOWS
            char buffer[512];

            if (path.size() == 0)
            {
                DWORD chars = GetCurrentDirectoryA(sizeof(buffer), buffer);
                if (!chars)
                    throw OSApiException("Could not determine full path, could not get working directory", GetLastError());
            }
            else
            {
                if (!GetFullPathNameA(path.data(), sizeof(buffer), buffer, NULL)) //TODO: Wenn der Buffer zu klein ist bekomme ich die mindestgr��e des Buffers zur�ck, die gebraucht wird
                    throw OSApiException("Could not determine full path", GetLastError());
            }

            return String(buffer);

#else
            char buffer[PATH_MAX + 1];
            if (path.size() == 0)
            {
            	if (!getcwd(buffer, sizeof(buffer)))
            		throw OSApiException("Could not determine current working directory", errno);
            	else
            		return String(buffer);
            }
            else if (!Path::IsAbsolute(path))
            {
				if (!realpath(path.data(), buffer))
				{
					//TODO: File/Dir does not exist -> must build smallest possible absolute path manually
					//Can not just concatentate working directory and path, because FullPath is used for checking equality of File-instances.
					throw OSApiException("Error determining full path", errno);
				}
				else
					return String(buffer);
            }
            else
            	return path;
            //TODO: realpath (Aber: REQUIRES THE PATH TO EXIST; nicht wirklich dolle)
#endif
//            if (Path::IsAbsolute(this->getPath()))
//                return this->getPath();
//            else
//            {
//                String workingDirectory;
//#ifdef _WINDOWS
//                char buffer[MAX_PATH];
//                DWORD chars = GetCurrentDirectoryA(sizeof(buffer), buffer);
//                if (!chars)
//                    throw OSApiException("Could not get working directory", GetLastError());
//                else
//                    workingDirectory = buffer;
//#else
//                implement
//#endif
//
//                if (this->getPath().size() == 0) //Passiert nur bei Directories
//                    return workingDirectory;
//#ifdef _WINDOWS
//                else if (this->getPath().at(0) == '/' || this->getPath().at(0) == '\\')
//                {
//                    if (chars < 2)
//                        throw Exception("Unexpected exception in FileSystemEntry::getFullPath");
//                    return Path::Combine(workingDirectory.substr(0, 2), this->getPath());
//                }
//#else
//                    implement
//#endif
//                else
//                    return Path::Combine(workingDirectory, this->getPath());
//            }
        }
        String Path::GetRoot(String const& path)
        {
#ifdef _WINDOWS
            String abs = Path::ToAbsolute(path);
            vector<String> parts = Path::GetParts(path);
            return parts.at(0);
#else
            return "/";
#endif
        }

        vector<String> Path::GetParts(String const& path)
        {
            vector<String> rv;
            String::size_type len = 0;

            for (String::size_type i = 0; i < path.size(); i++)
            {
                char c = path.at(i);
#ifdef _WINDOWS
                if (c == '/' || c == '\\')
#else
                if (c == '/')
#endif
                {
                    rv.push_back(path.substr(len, i - len));
                    len = i + 1;
                }
            }

            if (len < path.size())
                rv.push_back(path.substr(len));

            return rv;
        }
        String Path::GetLastSegment(String const& path)
		{
#ifdef _WINDOWS
            String::size_type li = path.find_last_of('\\');
			if (li == String::npos)
			{
                li = path.find_last_of('/');
				if (li == String::npos)
					return path;
			}

			return path.substr(li + 1);
#else
			String::size_type li = path.find_last_of('/');
			if (li == String::npos)
				return path;
			else
				return path.substr(li + 1);
#endif
		}
        String Path::GetFileName(String const& path, bool includeExtension)
        {
            String fn = Path::GetLastSegment(path);
            if (!includeExtension)
            {
                String::size_type li = fn.find_last_of('.');
                if (li != String::npos)
                    fn = fn.substr(0, li);
            }
            return fn;
        }
		String Path::GetFileExtension(String const& path)
		{
			String fn = Path::GetLastSegment(path);
			String::size_type li = fn.find_last_of('.');
			if (li != String::npos)
				return fn.substr(li);
			else
				throw Exception("File has no extension");
		}
	}
}
