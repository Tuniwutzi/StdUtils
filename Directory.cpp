#include <StdUtils/FileSystem/Directory.h>


#ifdef _WINDOWS
#include <shellapi.h>
#else
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <StdUtils/Exceptions.h>
#include <StdUtils/FileSystem/File.h>
#include <StdUtils/FileSystem/Path.h>


using namespace std;


namespace StdUtils
{
	namespace FileSystem
	{
        //Helper
#ifdef _WINDOWS
        static void enumerateEntries(String const& path, String const& pattern, bool recursive, vector<File>* files, vector<Directory>* directories)
        {
            if (!files && !directories)
                return;

			WIN32_FIND_DATAA data;
			HANDLE fd = FindFirstFileA(Path::Combine(path, pattern).data(), &data);
			if (fd == INVALID_HANDLE_VALUE)
			{
				DWORD le = GetLastError();
				if (le != ERROR_FILE_NOT_FOUND) //TODO: FileNotFound? Nicht eher ERROR_NO_MORE_FILES?
					throw OSApiException("Error enumerating files", le);
			}

			if (fd != INVALID_HANDLE_VALUE)
			{
                if (String(data.cFileName) != "." && String(data.cFileName) != "..")
                {
                    String entryPath(Path::Combine(path, data.cFileName));
                    bool isDir = (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
				    if (isDir)
                    {
                        if (recursive || directories)
                        {
                            if (directories)
                                directories->push_back(Directory(entryPath));
                            if (recursive && pattern == "*")
                                enumerateEntries(entryPath, pattern, true, files, directories);
                        }
                    }
                    else if (files)
					    files->push_back(File(entryPath));
                }

				while (FindNextFileA(fd, &data) == TRUE)
				{
                    if (String(data.cFileName) != "." && String(data.cFileName) != "..")
                    {
                        String entryPath = Path::Combine(path, data.cFileName);
                        bool isDir = (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

				        if (isDir)
                        {
                            if (recursive || directories)
                            {
                                if (directories)
                                    directories->push_back(Directory(entryPath));
                                if (recursive && pattern == "*")
                                    enumerateEntries(entryPath, pattern, true, files, directories);
                            }
                        }
                        else if (files)
					        files->push_back(File(entryPath));
                    }
				}

				FindClose(fd);

				DWORD le = GetLastError();
				if (le != ERROR_NO_MORE_FILES)
					throw OSApiException("Error enumerating files", le);
			}

            //TODO: Rekursion mit Pattern effektiver
            if (recursive && pattern != "*")
            {
                vector<Directory> dirs;
                enumerateEntries(path, "*", true, NULL, &dirs);
                for (vector<Directory>::iterator it = dirs.begin(); it != dirs.end(); it++)
                    enumerateEntries(it->getPath(), pattern, true, files, directories);
            }
        }
#else
        //LINUX
        static bool fitsPattern(char const* fname, vector<string> const& patternParts)
        {
    		bool wildcarded = false;
    		string name(fname);
    		for (vector<string>::const_iterator it = patternParts.begin(); it != patternParts.end(); it++)
    		{
    			if (it->length() == 0)
    				wildcarded = true;
    			else
    			{
    				//TODO: Statt den namen neu zuzuweisen einen Index bei find uebergeben
    				string::size_type index = name.find(*it);
    				if (index != name.npos && (wildcarded || index == 0))
    				{
						name = name.substr(index + it->length());
						wildcarded = false;
    				}
    				else
    					return false;
    			}
    		}

    		return (wildcarded || name.length() == 0);
        }
        bool getStats(char const* path, struct stat* fi);
        static bool isFile(struct dirent* ent)
        {
#ifdef _DIRENT_HAVE_D_TYPE
			switch (ent->d_type)
			{
			case DT_REG: //Datei (Regular)
				return true;
				break;
			case DT_DIR:
				return false;
				break;
			default:
				throw Exception("Unexpected file entry type in directory");
			}
#else
			struct stat fi;
			if (getStats(ent->d_name, &fi))
			{
				if (S_ISREG(fi.st_mode))
					return true;
				else if (S_ISDIR(fi.st_mode))
					return false;
				else
					throw Exception("Unexpected file entry type in directory");
			}
			//else sollte nicht vorkommen; bedeutet file nicht gefunden
#endif
        }
        static void enumerateEntries(String const& path, String const& pattern, bool recursive, vector<File>* files, vector<Directory>* directories)
        {
        	if (!files && !directories)
        		return;

        	DIR* dir = opendir(path.data());
        	if (!dir)
        		throw OSApiException("Error reading directory", errno);

        	//Pattern analysieren
        	vector<string> patternParts;
        	string::size_type offset = 0;
        	string::size_type index;
			while ((index = pattern.find('*', offset)) != pattern.npos)
			{
				string before(pattern.substr(0, index));
				offset = index + 1;

				patternParts.push_back(before);
				patternParts.push_back(string());
			}
			if (patternParts.rbegin()->length() != 0 || (offset < pattern.length()))
				patternParts.push_back(pattern.substr(offset));

        	//Nicht readdir_r nutzen, unsichere Funktion
        	//readdir garantiert, dass der Buffer, der für dirent genutzt wird, pro DIR-stream eindeutig ist
        	//Also keine Probleme bei mehreren Threads, da ich hier immer einen neuen DIR-stream oeffne
        	struct dirent* ent;
        	while ((ent = readdir(dir)) != NULL)
        	{
        		//In jedem Fall erstmal den Typen checken, da in den meisten Systemen
        		//das feld d_type von dirent definiert ist.
        		//Daher ist Typcheck in den meisten Fällen billiger, als das Pattern.
        		bool file = isFile(ent);

        		if (file)
        		{
        			if (files && fitsPattern(ent->d_name, patternParts))
						files->push_back(File(Path::Combine(path, ent->d_name)));
        		}
        		else
        		{
        			string entryPath(Path::Combine(path, ent->d_name));
        			if (directories && fitsPattern(ent->d_name, patternParts))
    					directories->push_back(Directory(entryPath));

        			if (recursive)
        				enumerateEntries(entryPath, pattern, true, files, directories);
        		}
        	}
        	//TODO: So wie ich es sehe keine Moeglichkeit, auf Fehler zu pruefen
        	//Denn: readdir gibt NULL zurueck, bei Fehler und bei Ende des Streams
        	//Bei Fehler wird zusaetzlich die errno gesetzt, aber ich muesste wissen,
        	//ob die pro Thread eindeutig ist, nur dann koennte ich sie mir vorher
        	//merken und nachher kontrollieren.
        	//Selbst wenn ich das tue koennte vorher einfach genau der gleiche Fehler
        	//schon mal aufgetreten sein.

        	closedir(dir);
        }
#endif

        Directory::Directory()
            :FileSystemEntry(FileSystemEntry::DirectoryEntry), null(true)
        {
        }
		Directory::Directory(String const& path)
			:FileSystemEntry(FileSystemEntry::DirectoryEntry, path), null(false)
		{
		}

        bool Directory::isNull() const STDUTILS_OVERRIDE
        {
            return this->null;
        }

        bool Directory::isValid() const STDUTILS_OVERRIDE
        {
            //TODO: Cachen oder so? Wird haeufig abgefragt
#ifdef _WINDOWS
            if (!this->isNull())
            {
                DWORD attr = GetFileAttributesA(this->getPath().data());
                //gibt bei INVALID_... true zurück, da das bedeutet, das Verzeichnis existiert nicht.
                //Dann ist es aber trotzdem valide
                return (attr == INVALID_FILE_ATTRIBUTES) || (attr & FILE_ATTRIBUTE_DIRECTORY);
            }
            else
                return false;
#else
            if (!this->isNull())
            {
            	struct stat fi;

            	if (getStats(this->getPath().data(), &fi))
            		return S_ISDIR(fi.st_mode);
            	else
            		return true;
            }
            else
            	return false;
#endif
        }

        void Directory::create(bool createIntermediates)
        {
            this->assertValid();

            if (createIntermediates)
            {
                Directory parent = this->getParent();

                if (parent.isValid() && parent != *this && !parent.exists())
                    parent.create(true);
            }

#ifdef _WINDOWS
            if (!CreateDirectoryA(this->getPath().data(), NULL))
                throw OSApiException("Could not create directory", GetLastError());
#else
            //TODO: Permissions
            int rv = mkdir(this->getFullPath().data(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            if (rv)
            	throw OSApiException("Could not create directory", errno);
#endif
        }
        void Directory::erase()
        {
            this->assertValid();

#ifdef _WINDOWS
            if (!RemoveDirectoryA(this->getPath().data()))
                throw OSApiException("Could not remove directory", GetLastError());
#else
            int rv = rmdir(this->getFullPath().data());
            if (rv)
            	throw OSApiException("Could not erase directory", errno);
#endif
        }
        void Directory::move(String const& to, bool overwrite)
        {
            this->assertValid();

#ifdef _WINDOWS
            //TODO: Funktioniert, laut Doku, nicht, wenn Verzeichnisse (NICHT Dateien) auf unterschiedlichen "volumes" liegen
            if (!MoveFileExA(this->getPath().data(), to.data(), MOVEFILE_COPY_ALLOWED | (overwrite ? MOVEFILE_REPLACE_EXISTING : 0)))
				throw OSApiException("Error moving directory", GetLastError());
#else
            if (Directory::Exists(to))
            {
            	if (!overwrite)
            		throw Exception("Target directory exists");
            }
            else
            	Directory::Create(to, false);

            vector<File> files = this->enumerateFiles("*", false);
            vector<Directory> directories = this->enumerateDirectories("*", false);

            for (vector<File>::iterator it = files.begin(); it != files.end(); it++)
            	it->move(Path::Combine(to, Path::GetFileName(it->getPath(), true)), true);

            for (vector<Directory>::iterator it = directories.begin(); it != directories.end(); it++)
            	it->move(Path::Combine(to, Path::GetFileName(it->getPath(), true)), true);
#endif
        }
        void Directory::copy(String const& to, bool overwrite)
        {
            this->assertValid();

            //TODO: unter windows Evtl. SHFileOperation, sieht aber nicht zu sinnvoll aus
            if (this->exists())
            {
                Directory target(to);
                if (target.exists())
                {
                    if (!overwrite)
                        throw Exception("Target directory exists");
                }
                else
                    target.create();

                vector<Directory> dirs;
                vector<File> files;
                enumerateEntries(this->getPath(), "*", false,&files, &dirs);
                for (vector<File>::iterator it = files.begin(); it != files.end(); it++)
                    it->copy(Path::Combine(to, Path::GetFileName(it->getPath())), overwrite);
                for (vector<Directory>::iterator it = dirs.begin(); it != dirs.end(); it++)
                    it->copy(Path::Combine(to, Path::GetLastSegment(it->getPath())), overwrite);
            }
            else
                throw Exception("Source directory does not exist");
            //TODO: evtl unter linux Systemfunktion, sonst funktioniert hier auch der windows code
        }

        void Directory::eraseRecursive()
        {
            this->clear();
            this->erase();
        }
        void Directory::clear()
        {
            vector<File> files;
            vector<Directory> directories;
            enumerateEntries(this->getPath(), "*", false, &files, &directories);

            for (vector<File>::iterator it = files.begin(); it != files.end(); it++)
                it->erase();
            for (vector<Directory>::iterator it = directories.begin(); it != directories.end(); it++)
                it->eraseRecursive();
        }

        vector<SharedPointer<FileSystemEntry> > Directory::enumerateChildren(String const& pattern, bool recursive) const
        {
            vector<SharedPointer<FileSystemEntry> > rv;

            vector<File> files;
            vector<Directory> dirs;
            enumerateEntries(this->getPath(), pattern, recursive, &files, &dirs);

            for (vector<File>::iterator it = files.begin(); it != files.end(); it++)
                rv.push_back(SharedPointer<FileSystemEntry>(new File(*it)));
            for (vector<Directory>::iterator it = dirs.begin(); it != dirs.end(); it++)
                rv.push_back(SharedPointer<FileSystemEntry>(new Directory(*it)));

            return rv;
        }
		vector<File> Directory::enumerateFiles(String const& pattern, bool recursive) const
		{
            this->assertValid();

			vector<File> rv;
            enumerateEntries(this->getPath(), pattern, recursive, &rv, NULL);
            return rv;
		}
        vector<Directory> Directory::enumerateDirectories(String const& pattern, bool recursive) const
        {
            this->assertValid();

            vector<Directory> dirs;
            enumerateEntries(this->getPath(), pattern, recursive, NULL, &dirs);
            return dirs;
        }
		
		void Directory::getFile(String const& relativePath, File& buffer) const
		{
			buffer = File(Path::Combine(this->getPath(), relativePath));
		}
		File Directory::getFile(String const& relativePath) const
		{
			File rv;

			this->getFile(relativePath, rv);

			return rv;
		}

        Directory Directory::Create(String const& path, bool createIntermediates)
        {
            Directory dir(path);
            dir.create(createIntermediates);
            return dir;
        }
        Directory Directory::GetWorkingDirectory()
        {
            return Directory("");
        }
        void Directory::SetWorkingDirectory(Directory const& dir)
        {
#ifndef _WINDOWS
        	int rv = chdir(dir.getFullPath().data());
        	if (rv)
        		throw OSApiException("Could not change working directory", errno);
#else
        	//TODO?: Laut stackoverflow funktioniert chdir auch fuer windows; laut MSDN aber nur _chdir
            if (!SetCurrentDirectoryA(dir.getFullPath().data()))
                throw OSApiException("Could not set working directory", GetLastError());
#endif
        }
	}
}
