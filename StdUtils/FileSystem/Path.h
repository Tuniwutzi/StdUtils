#ifndef STDUTILS_PATH_H
#define STDUTILS_PATH_H


#include <vector>

#include <StdUtils/Base.h>


namespace StdUtils
{
	namespace FileSystem
	{
		//class PathBuilder
		//{
		//private:
		//	String path;

		//public:
		//	PathBuilder(String const& base);
		//	
		//public:
		//	PathBuilder& append(String const& part);

		//	String const& toString() const;
		//};
		class Path
		{
		private:
			Path();

		public:
			//static PathBuilder Build(String const& base);
			static String Combine(String const& pathA, String const& pathB);

            static bool IsAbsolute(String const& path);
            static String ToAbsolute(String const& path);
            static String GetRoot(String const& path); //Returns Partition on Windows (C:, D:, ...) and / on linux
            static std::vector<String> GetParts(String const& path);

			static String GetLastSegment(String const& path);
            static String GetFileName(String const& path, bool includeExtension = true);
			static String GetFileExtension(String const& path);
		};
	}
}

#endif