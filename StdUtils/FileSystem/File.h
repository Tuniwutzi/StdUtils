#ifndef STDUTILS_FILE_H
#define STDUTILS_FILE_H

#include <StdUtils/FileSystem/FileSystemEntry.h>

namespace StdUtils
{
	namespace FileSystem
	{
		class File : public FileSystemEntry
		{
		public:
			struct FileInfo
			{
				uint64_t Size;
			};

		public:
            explicit File();
			explicit File(String const& path);

		public:
            bool isNull() const STDUTILS_OVERRIDE;
            bool isValid() const STDUTILS_OVERRIDE;

            void create(bool createIntermediates = false) STDUTILS_OVERRIDE;
			void erase() STDUTILS_OVERRIDE;
            void copy(String const& to, bool overwrite = false) STDUTILS_OVERRIDE;
			void move(String const& to, bool overwrite = false) STDUTILS_OVERRIDE;

			std::ofstream openWrite(bool binary = false);
			std::ifstream openRead(bool binary = false);
			std::fstream open(bool binary = false);

			FileInfo getFileInfo() const;

        public:
            static File Create(String const& path, bool createIntermediates = false);
			//static File GetExecutable(); //Scheint nicht sinnvoll auf mehreren Plattformen machbar zu sein
		};
	}
}

#endif