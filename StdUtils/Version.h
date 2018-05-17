#ifndef STDUTILS_VERSION_H
#define STDUTILS_VERSION_H

#define STDUTILS_MAJOR 1
#define STDUTILS_MINOR 1
#define STDUTILS_REVISION 0


#ifdef STDUTILS_USE_MAJOR
#if STDUTILS_USE_MAJOR != STDUTILS_MAJOR
#error "StdUtils major version differs from requested"
#endif
#endif

#ifdef STDUTILS_USE_MINOR
#if STDUTILS_USE_MINOR != STDUTILS_MINOR
#error "StdUtils minor version differs from requested"
#endif
#endif

#ifdef STDUTILS_USE_REVISION
#if STDUTILS_USE_REVISION != STDUTILS_REVISION
#error "StdUtils version differs from requested"
#endif
#endif


namespace StdUtils
{
	struct Version
	{
		static const int Major = STDUTILS_MAJOR;
		static const int Minor = STDUTILS_MINOR;
		static const int Revision = STDUTILS_REVISION;
	};
}

#endif