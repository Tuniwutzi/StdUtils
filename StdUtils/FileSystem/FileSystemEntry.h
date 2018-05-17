#ifndef STDUTILS_FILESYSTEMENTRY_H
#define STDUTILS_FILESYSTEMENTRY_H

#include <StdUtils/Base.h>

namespace StdUtils
{
	namespace FileSystem
	{
		class Directory;
		class FileSystemEntry
		{
		public:
			enum Type
			{
				FileEntry,
				DirectoryEntry
			};

		private:
			Type type;
			String path;

		protected:
			explicit FileSystemEntry(Type type, String const& path = "");

		public:
			virtual ~FileSystemEntry() {}

        public:
            bool operator==(FileSystemEntry const& orig) const;
            bool operator!=(FileSystemEntry const& orig) const;

        protected:
            void assertValid() const;

		public:
            Type getType() const;

            bool exists() const;

            String getFullPath() const;
			String const& getPath() const; //Gibt den Pfad zurueck, mit dem das Objekt erstellt wurde
			Directory getParent() const;

            virtual bool isNull() const = 0;
            virtual bool isValid() const = 0;

            virtual void create(bool createIntermediates = false) = 0;
			virtual void erase() = 0;
            virtual void copy(String const& to, bool overwrite = false) = 0;
			virtual void move(String const& to, bool overwrite = false) = 0;

        public:
            static bool Exists(String const& path);
		};
	}
}

#endif
