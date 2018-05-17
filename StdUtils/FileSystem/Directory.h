#ifndef STDUTILS_DIRECTORY_H
#define STDUTILS_DIRECTORY_H

#include <StdUtils/Base.h>
#include <StdUtils/SharedPointer.h>
#include <StdUtils/FileSystem/FileSystemEntry.h>

#include <vector>

namespace StdUtils
{
	namespace FileSystem
	{
		class File;
		class Directory : public FileSystemEntry
		{
        private:
            bool null;

		public:
            explicit Directory();
			explicit Directory(String const& path);

		public:
            bool isNull() const STDUTILS_OVERRIDE;
            /*
            False, wenn isNull() true, oder das Ziel von Pfad eine Datei ist.
            True, wenn isNull() false und das Ziel von Pfad nicht existiert oder ein Directory ist.
            => Wenn true, dann duerfen alle anderen Methoden aufgerufen werden.
            => Wenn false, koennen alle anderen Methoden pauschal exceptions werfen.
            */
            bool isValid() const STDUTILS_OVERRIDE;

            void create(bool createIntermediates = false) STDUTILS_OVERRIDE;
            void erase() STDUTILS_OVERRIDE;
            void copy(String const& to, bool overwrite = false) STDUTILS_OVERRIDE;
            void move(String const& to, bool overwrite = false) STDUTILS_OVERRIDE;

            void eraseRecursive();
            void clear();

			std::vector<SharedPointer<FileSystemEntry> > enumerateChildren(String const& searchPattern = "*", bool recursive = false) const;
			std::vector<File> enumerateFiles(String const& searchPattern = "*", bool recursive = false) const;
			std::vector<Directory> enumerateDirectories(String const& searchPattern = "*", bool recursive = false) const;

			void getFile(StdUtils::String const& relativeFilePath, StdUtils::FileSystem::File& buffer) const;
			StdUtils::FileSystem::File getFile(StdUtils::String const& relativeFilePath) const;

        public:
            static Directory Create(String const& path, bool createIntermediates = false);

            static Directory GetWorkingDirectory();
            static void SetWorkingDirectory(Directory const& dir);
		};
	}
}

#endif
